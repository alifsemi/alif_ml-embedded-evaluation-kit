# Implementing Pre- and Post-Processing for a Custom ML Model

Depending on your custom model you may need to pre-process the raw sensor data before pushing it to model graph input tensor. Also you need to know what kind of data and in which format the model outputs in order to make the output human-readable or otherwise meaningful.
This guide briefly explains how to write custom pre-processing and post-processing classes for a model that is not part of the existing use cases in [alifsemi/alif_ml-embedded-evaluation-kit](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit). For creating the full use-case refer to [`docs/sections/customizing.md`](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/blob/main/docs/sections/customizing.md).

---

## The Base Interfaces

All pre- and post-processing classes inherit from two abstract base classes in [`BaseProcessing.hpp`](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/blob/304a0c18542f3ef497d9023b0dc3e374c8c18e08/source/application/api/common/include/BaseProcessing.hpp):

```c++
class BasePreProcess {
public:
    virtual ~BasePreProcess() = default;
    virtual bool DoPreProcess(const void* input, size_t inputSize) = 0;
};

class BasePostProcess {
public:
    virtual ~BasePostProcess() = default;
    virtual bool DoPostProcess() = 0;
};
```

Your new classes should override `DoPreProcess` and `DoPostProcess`. Place them under `source/lib/mlek/use_case/<your_use_case>/` with headers in `include/` and sources in `src/`.

---

## Pre-Processing

Pre-processing transforms raw input (camera frame, audio buffer, sensor data) and writes it into the model's **input tensor**.

1. **Validate** the input pointer and size.
2. **Transform** raw data to match what the model expects (resize, normalize, extract features, MFCC, MEL, ...).
3. **Handle the tensor data type** — check `m_inputTensor->Type()` and convert accordingly:
   - `INT8`: convert uint8 → int8 (subtract 128), or quantize float → int8
   - `UINT8`: memcpy directly if data is already uint8
   - `FP32`: normalize (e.g. divide by 255, apply mean/stddev)
4. **Write** into the input tensor buffer via `m_inputTensor->GetData<T>()`.

### Example: Image Classification

The existing [`ImgClassPreProcess`](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/blob/304a0c18542f3ef497d9023b0dc3e374c8c18e08/source/application/api/use_case/img_class/src/ImgClassProcessing.cc#L36-L72) demonstrates all three data-type paths:
Typically TFLM uses the INT8 or UINT8 and FP32 is used by ExecuTorch model.

```c++
bool ImgClassPreProcess::DoPreProcess(const void* data, size_t inputSize)
{
    auto src = static_cast<const uint8_t*>(data);
    switch (this->m_inputTensor->Type()) {
    case fwk::iface::TensorType::INT8:
        image::ConvertUint8ToInt8(this->m_inputTensor->GetData<int8_t>(),
                                  src, this->m_inputTensor->GetNumElements(),
                                  this->m_inputTensor->Layout());
        break;
    case fwk::iface::TensorType::UINT8:
        std::memcpy(this->m_inputTensor->GetData(), src, inputSize);
        break;
    case fwk::iface::TensorType::FP32:
        return image::ConvertUint8ToFp32<kNumChannels>(
            this->m_inputTensor->GetData<float>(), src,
            this->m_inputTensor->GetNumElements(),
            this->m_inputTensor->Layout(), this->m_mean, this->m_stddev);
    default:
        return false;
    }
    return true;
}
```

### Pre-Processing Checklist

| Concern | What to check |
|---------|---------------|
| **Data type match** | Does raw data type match `m_inputTensor->Type()`? Convert if not. |
| **Normalization** | Replicate the exact normalization used during training (mean/stddev, [0,1], [-1,1]). |
| **Quantization** | For quantized models: `int_val = (float_val / scale) + zero_point`. Use `tensor->GetQuantParams()`. |
| **Tensor layout** | Check `m_inputTensor->Layout()` for NHWC vs NCHW and arrange data accordingly. |
| **Spatial dimensions** | Resize/crop input if it doesn't match the model's expected size. |
| **Feature extraction** | Audio models may need MFCC or Mel spectrogram computation before writing to the tensor. |

---

## Post-Processing

Post-processing reads the model's **output tensor(s)** after inference and produces meaningful results.

1. **Read** output tensor data via `m_outputTensor->GetData<T>()`.
2. **Dequantize** if the model is quantized: `float_val = (int8_val - zero_point) * scale`.
3. **Decode** the raw output into your domain (class labels, bounding boxes, text tokens, anomaly scores).
4. **Filter** results (confidence threshold, NMS, etc.).
5. **Populate** the results container.

### Reference: Object Detection

The existing [`DetectorPostProcess`](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/blob/304a0c18542f3ef497d9023b0dc3e374c8c18e08/source/application/api/use_case/object_detection/src/DetectorPostProcessing.cc) shows a complete pipeline: dequantize → decode boxes with anchors → threshold filter → NMS → gather results.

### Post-Processing Checklist

| Concern | What to check |
|---------|---------------|
| **Dequantization** | For int8 outputs: `float_val = (int8_val - zero_point) * scale` |
| **Softmax** | Is it baked into the model or must you apply it in post-processing? |
| **Top-N / argmax** | Classification: pick highest-scoring classes |
| **Box decoding** | Detection: decode anchors, convert center↔corner format |
| **NMS** | Filter overlapping detections — reuse `image::CalculateNMS()` from the kit |
| **Sequence decoding** | ASR/NLP: CTC or beam-search decoding |
| **Multiple outputs** | Use `model.GetOutputTensor(idx)` for multi-head models |

---

## Existing Use Cases as References

| Domain | Pre/Post-processing files | Key pattern |
|--------|---------------------------|-------------|
| **Image Classification** | `source/lib/mlek/use_case/img_class/` | Data type switching, per-channel normalization |
| **Object Detection** | `source/lib/mlek/use_case/object_detection/` | Anchor decoding, NMS, multi-tensor output |
| **Keyword Spotting** | `source/lib/mlek/use_case/kws/` | MFCC feature extraction |
| **Speech Recognition** | `source/lib/mlek/use_case/asr/` | Sliding window, CTC decoding |
| **Anomaly Detection** | `source/lib/mlek/use_case/ad/` | Scalar output, threshold comparison |
| **Visual Wake Word** | `source/lib/mlek/use_case/vww/` | Binary classification |
| **Noise Reduction** | `source/lib/mlek/use_case/noise_reduction/` | Audio frame processing |

---

## Using Pre/Post-Processing from Main Loop

Once your classes are implemented, instantiate them in your use-case handler:

```c++
auto inputTensor  = model.GetInputTensor(0);
auto outputTensor = model.GetOutputTensor(0);

MyPreProcess preProcess(inputTensor /*, your params */);
std::vector<MyResult> results;
MyPostProcess postProcess(outputTensor, results /*, your params */);

/* Run pipeline */
preProcess.DoPreProcess(rawData, rawDataSize);
model.RunInference();
postProcess.DoPostProcess();
```

---

## Porting Python Post-Processing to C/C++ — YOLOv8 Example

When bringing an external model into the mlek, you'll often start from a **Python implementation**. This section uses YOLOv8 as an example of how to translate Python post-processing into C++.

### YOLOv8 Python Post-Processing

A standard YOLOv8 detection pipeline (from the [Ultralytics repository](https://github.com/ultralytics/ultralytics)):

```python
import numpy as np

def yolov8_postprocess(output, conf_threshold=0.25, iou_threshold=0.45,
                       input_shape=(640, 640), orig_shape=(480, 640)):
    """
    Args:
        output: Raw model output, shape [1, 84, 8400] for 80-class COCO.
                84 = 4 (cx, cy, w, h) + 80 (class scores)
                8400 = 80x80 + 40x40 + 20x20 grid cells
    """
    # Step 1: Transpose to [8400, 84]
    predictions = np.squeeze(output).T

    # Step 2: Split boxes and scores
    boxes = predictions[:, :4]          # cx, cy, w, h
    class_scores = predictions[:, 4:]   # NO separate objectness in YOLOv8

    # Step 3: Max class score per detection
    max_scores = np.max(class_scores, axis=1)
    class_ids = np.argmax(class_scores, axis=1)

    # Step 4: Confidence filter
    mask = max_scores > conf_threshold
    boxes, scores, class_ids = boxes[mask], max_scores[mask], class_ids[mask]

    # Step 5: Center → corner format
    x1 = boxes[:, 0] - boxes[:, 2] / 2
    y1 = boxes[:, 1] - boxes[:, 3] / 2
    x2 = boxes[:, 0] + boxes[:, 2] / 2
    y2 = boxes[:, 1] + boxes[:, 3] / 2

    # Step 6: Scale to original image
    scale_x = orig_shape[1] / input_shape[1]
    scale_y = orig_shape[0] / input_shape[0]
    x1 *= scale_x;  x2 *= scale_x
    y1 *= scale_y;  y2 *= scale_y

    # Step 7: Clip to image boundaries
    x1 = np.clip(x1, 0, orig_shape[1])
    y1 = np.clip(y1, 0, orig_shape[0])
    x2 = np.clip(x2, 0, orig_shape[1])
    y2 = np.clip(y2, 0, orig_shape[0])

    # Step 8: NMS
    indices = nms(np.stack([x1, y1, x2, y2], axis=1), scores, iou_threshold)

    # Step 9: Gather results
    return [[x1[i], y1[i], x2[i], y2[i], scores[i], class_ids[i]] for i in indices]


def nms(boxes, scores, iou_threshold):
    order = scores.argsort()[::-1]
    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)
        if order.size == 1:
            break
        xx1 = np.maximum(boxes[i, 0], boxes[order[1:], 0])
        yy1 = np.maximum(boxes[i, 1], boxes[order[1:], 1])
        xx2 = np.minimum(boxes[i, 2], boxes[order[1:], 2])
        yy2 = np.minimum(boxes[i, 3], boxes[order[1:], 3])
        intersection = np.maximum(0, xx2 - xx1) * np.maximum(0, yy2 - yy1)
        area_i = (boxes[i, 2] - boxes[i, 0]) * (boxes[i, 3] - boxes[i, 1])
        area_rest = (boxes[order[1:], 2] - boxes[order[1:], 0]) * \
                    (boxes[order[1:], 3] - boxes[order[1:], 1])
        iou = intersection / (area_i + area_rest - intersection + 1e-6)
        order = order[np.where(iou <= iou_threshold)[0] + 1]
    return keep
```

### Mapping Python to C++ in mlek

| Step | Python | C++ equivalent | Notes |
|------|--------|-----------------------|-------|
| Read output | `output = model(input)` | `model.GetOutputTensor(0)` | Returns `shared_ptr<TensorIface>` |
| Dequantize | Implicit (float) | `(int8_val - zeroPoint) * scale` | Use `tensor->GetQuantParams()` |
| `np.squeeze().T` | Transpose | Swap index strides: `data[feat * numDets + det]` | Don't allocate a copy |
| `np.max(axis=1)` | Vectorized max | `for` loop tracking max | Combine with `argmax` in one pass |
| `mask = scores > thr` | Boolean mask | `if (score > thr) continue;` | Filter early to save memory |
| Center → corner | `x1 = cx - w/2` | Same scalar arithmetic | Already in `DetectorPostProcessing.cc` |
| `np.clip()` | Clip | `if (x < 0) x = 0;` | Already in `DetectorPostProcessing.cc` |
| NMS | `nms()` | `image::CalculateNMS()` | **Already in the kit** (`ImageUtils.cc`) |
| IoU | `compute_iou()` | `image::CalculateBoxIOU()` | **Already in the kit** |
| Sigmoid | `1/(1+exp(-x))` | `math::MathUtils::SigmoidF32()` | **Already in the kit** |

### Approach for the C++ Port

Create a `Yolov8PostProcess` class inheriting `BasePostProcess`, following the same pattern as the existing [`DetectorPostProcess`](https://github.com/alifsemi/alif_ml-embedded-evaluation-kit/blob/304a0c18542f3ef497d9023b0dc3e374c8c18e08/source/application/api/use_case/object_detection/src/DetectorPostProcessing.cc). The key differences from the kit's existing YOLO-Fastest implementation:

1. **Single output tensor** — YOLOv8 produces `[1, 84, 8400]` instead of multiple branch outputs.
2. **No objectness score** — YOLOv8 is anchor-free; class scores are the final confidences directly.
3. **No sigmoid on output** — Scores are already post-sigmoid. Don't apply it again.
4. **Transposed layout** — Index as `data[feature_idx * num_detections + det_idx]` instead of allocating a transposed copy.
5. **Direct pixel coordinates** — Boxes are in model-input pixel coordinates, not normalized. Scale directly to original image size without anchor/grid decoding.

### Porting Tips — Python to Embedded C/C++

#### Replacing NumPy Operations

| Python (NumPy) | C++ Equivalent | Notes |
|----------------|----------------|--------|
| `np.squeeze(output).T` | Swap index strides in flat array | Don't allocate a transposed copy |
| `np.max(arr, axis=1)` | `for` loop tracking `maxVal` | Avoid intermediate arrays |
| `np.argmax(arr, axis=1)` | `for` loop tracking `maxIdx` | Combine with max in a single pass |
| `mask = scores > thr` / `arr[mask]` | `if`/`continue` in a loop | Filter in-place |
| `np.clip(x, lo, hi)` | `std::max(lo, std::min(x, hi))` | mlek uses the `if`-pattern |
| `arr.argsort()[::-1]` | `std::forward_list::sort()` with comparator | See `CalculateNMS()` |
| `np.exp(x)` | `std::exp(x)` | `<cmath>` |
| `np.stack([a,b], axis=1)` | Store in struct fields (`Box{x,y,w,h}`) | mlek has `image::Box` |

#### Memory Management

Avoid large memory allocations from stack. Use static memory allocation when applicable and reuse when possible.

| Python habit | Embedded alternative |
|-------------|---------------------|
| `results.append()` | `std::vector` with `.reserve()`, or `std::forward_list` |
| Multiple intermediate array copies | Process in-place, single-pass algorithms |
| Large float32 tensors | Keep int8, dequantize only the values you need |
| `dict` / nested `list` | Plain `struct` arrays or `std::array` |

#### Quantization Handling

```c++
// C++:     val = (int8_val - zero_point) * scale  (manual dequantize)
auto qp = tensor->GetQuantParams();
float val = (static_cast<float>(data[i]) - qp.offset) * qp.scale;
```

#### Reusable Kit Utilities

Check these before writing your own:

| Function | Location | Python equivalent |
|----------|----------|-------------------|
| `image::CalculateNMS()` | `ImageUtils.cc` | `nms()` / `torchvision.ops.nms()` |
| `image::CalculateBoxIOU()` | `ImageUtils.cc` | IoU computation |
| `math::MathUtils::SigmoidF32()` | `PlatformMath.hpp` | `1/(1+exp(-x))` |
| `image::ConvertUint8ToInt8()` | `ImageUtils.cc` | `arr.astype(np.int8) - 128` |
| `image::ConvertUint8ToFp32()` | `ImageUtils.cc` | `arr.astype(np.float32) / 255` |

#### Debugging

1. **Validate numerically.** Run Python (on host OS) and C++ (host OS and target )on the same test input; compare values.
2. **Profile.** Use the kit's `Profiler` around your post-processing to find bottlenecks.
3. **Print intermediates.** Use `trace()` or `debug()` from `log_macros.h` during development.

#### YOLOv8 vs. mlek's YOLO-Fastest

| Feature | YOLOv8 | Kit's YOLO-Fastest |
|---------|--------|--------------------|
| Output shape | `[1, 84, 8400]` (single tensor) | Multi-branch (2 tensors) |
| Objectness score | **No** | Yes |
| Sigmoid on scores | **Not needed** | Required |
| Anchor-based | **No** (anchor-free) | Yes |
| Box format | cx, cy, w, h (pixel coords) | Encoded with anchors + grid |

Always check the model's export code to confirm the exact output format when porting to C++
