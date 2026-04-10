# Image Classification Code Sample

- [Image Classification Code Sample](./img_class.md#image-classification-code-sample)
  - [Introduction](./img_class.md#introduction)
    - [Prerequisites](./img_class.md#prerequisites)
  - [Building the code sample application from sources](./img_class.md#building-the-code-sample-application-from-sources)
    - [Build options](./img_class.md#build-options)
    - [Build process](./img_class.md#build-process)
    - [Add custom input](./img_class.md#add-custom-input)
    - [Add custom model](./img_class.md#add-custom-model)
  - [Setting up and running Ethos-U NPU code sample](./img_class.md#setting-up-and-running-ethos_u-npu-code-sample)
    - [Setting up the Ethos-U NPU Fast Model](./img_class.md#setting-up-the-ethos_u-npu-fast-model)
    - [Starting Fast Model simulation](./img_class.md#starting-fast-model-simulation)
    - [Running Image Classification](./img_class.md#running-image-classification)

## Introduction

This document describes the process of setting up and running the Arm® *Ethos™-U* NPU Image Classification example.

This use-case example solves the classical computer vision problem of image classification. The ML sample works with
the *MobileNet v2* model with TensorFlow Lite Micro, or either the *MobileNet v2* or *DeiT tiny* model with ExecuTorch.
These models were both trained with the *ImageNet* dataset, and the overall pipeline is the same for both models.

Use-case code could be found in the following directory: [source/app/use_case/img_class](../../source/app/use_case/img_class).

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, the Image Classification use-case
specifies:

- `img_class_MODEL_PATH` - The path to the NN model file in `TFLite` (for TensorFlow Lite Micro) or
  `PTE` (for ExecuTorch) format. The model is then processed and included in the application `axf` file.
  The default value points to one of the delivered set of models.

  When using ExecuTorch, there are two included models: *MobileNet V2* (default) and *DeiT tiny*.
  To use *DeiT tiny*, use this argument to reference the generated `PTE` model for your target platform
  under the `resources_downloaded/img_class/` directory.

    Note that the parameters `img_class_LABELS_TXT_FILE`,`TARGET_PLATFORM`, and `ETHOS_U_NPU_ENABLED` must be aligned with
    the chosen model. In other words:

  - If `ETHOS_U_NPU_ENABLED` is set to `On` or `1`, then the NN model is assumed to be optimized. The model naturally
    falls back to the Arm® *Cortex®-M* CPU if an unoptimized model is supplied.
  - if `ETHOS_U_NPU_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
    model in this case results in a runtime error.

- `img_class_NORM_MEAN` and `img_class_NORM_STD` - These options are available
  when [building with ExecuTorch](../sections/building.md#build-options) and should be used together.
  They specify the normalisation mean and standard deviation values for each of the three input channels,
  and are used for converting uint8 image data to float32 which is the input type for the ExecuTorch models
  provided with this use case.

  The default values correspond to the *MobileNet V2* ExecuTorch model.
  To use *DeiT tiny*, these options must be provided with values specific to that model:
    - *MobileNet V2* (default):
      - `-Dimg_class_NORM_MEAN="0.485, 0.456, 0.406"`
      - `-Dimg_class_NORM_STD="0.229, 0.224, 0.225"`
    - *DeiT tiny*:
      - `-Dimg_class_NORM_MEAN="0.5, 0.5, 0.5"`
      - `-Dimg_class_NORM_STD="0.5, 0.5, 0.5"`

- `img_class_FILE_PATH`: The path to the directory containing the images, or a path to a single image file, that is to
   be used in the application. The default value points to the `resources/img_class/samples` folder containing the
   delivered set of images.

    For further information, please refer to: [Add custom input data section](./img_class.md#add-custom-input).

- `img_class_IMAGE_SIZE`: The NN model requires input images to be of a specific size. This parameter defines the size
  of the image side in pixels. Images are considered squared. The default value is `224`, which is what the supplied
  *MobilenetV2-1.0* model expects.

- `img_class_LABELS_TXT_FILE`: The path to the text file for the label. The file is used to map a classified class index
  to the text label. The default value points to the delivered `labels.txt` file inside the delivery package. Change
  this parameter to point to the custom labels file to map custom NN model output correctly.

  For the included `TFlite` version of *MobileNet V2*, this argument should reference the `labels_mobilenet_v2_1.0_224.txt` file.
  For either of the included ExecuTorch models (*MobileNet V2* or *DeiT tiny*), this argument should reference
  the `labels_mobilenet_v2_1.IMAGENET1K_V2.txt` file.

- `img_class_ACTIVATION_BUF_SZ`: The intermediate, or activation, buffer size reserved for the NN model. By default, it
  is set to 2MiB and is enough for most models.

- `USE_CASE_BUILD`: is set to `img_class` to only build this example.

To build **ONLY** the Image Classification example application, add `-DUSE_CASE_BUILD=img_class` to the `cmake` command
line, as specified in: [Building](../documentation.md#Building).

### Build process

> **Note:** This section describes the process for configuring the build for the *MPS3: SSE-300*. To build for a
> different target platform, please refer to: [Building](../documentation.md#Building).

Create a build directory and navigate inside, like so:

```commandline
mkdir build_img_class && cd build_img_class
```

On Linux, when providing only the mandatory arguments for the CMake configuration, execute the following command to
build **only** Image Classification application to run on the *Ethos-U55* Fast Model:

```commandline
cmake ../ -DUSE_CASE_BUILD=img_class
```

To configure a build that can be debugged using Arm DS specify the build type as `Debug` and then use the `Arm Compiler`
toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=img_class
```

For further information, please refer to:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple-platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Building for different Ethos-U NPU variants](../sections/building.md#building-for-different-ethos_u-npu-variants)

> **Note:** If re-building with changed parameters values, we recommend that you clean the build directory and re-run
> the CMake command.

If the CMake command succeeds, build the application as follows:

```commandline
make -j4
```

To see compilation and link details, add `VERBOSE=1`.

Results of the build are placed under the `build/bin` folder, like so:

```tree
bin
 ├── mlek_img_class.axf
 ├── mlek_img_class.htm
 ├── mlek_img_class.map
 └── sectors
      ├── images.txt
      └── img_class
           ├── ddr.bin
           └── itcm.bin
```

The `bin` folder contains the following files:

- `mlek_img_class.axf`: The built application binary for the Image Classification use-case.

- `mlek_img_class.map`: Information from building the application. For example: The libraries used, what was
  optimized, and the location of objects.

- `mlek_img_class.htm`: Human readable file containing the call graph of application functions.

- `sectors/img_class`: Folder containing the built application. It is split into files for loading into different FPGA memory
  regions.

- `sectors/images.txt`: Tells the FPGA which memory regions to use for loading the binaries in the `sectors/..`
  folder.

### Add custom input

The application anomaly detection is set up to perform inferences on data found in the folder, or an individual file,
that is pointed to by the parameter `img_class_FILE_PATH`.

To run the application with your own images, first create a folder to hold them and then copy the custom images into the
following folder:

```commandline
mkdir /tmp/custom_images

cp custom_image1.bmp /tmp/custom_images/
```

> **Note:** Clean the build directory before re-running the CMake command.

Next, set `img_class_FILE_PATH` to the location of this folder when building:

```commandline
cmake .. \
    -Dimg_class_FILE_PATH=/tmp/custom_images/ \
    -DUSE_CASE_BUILD=img_class
```

The images found in the `img_class_FILE_PATH` folder are picked up and automatically converted to C++ files during the
CMake configuration stage. They are then compiled into the application during the build phase for performing inference
with.

The log from the configuration stage tells you what image directory path has been used:

```log
-- User option img_class_FILE_PATH is set to /tmp/custom_images
-- User option img_class_IMAGE_SIZE is set to 224
...
-- Generating image files from /tmp/custom_images
++ Converting custom_image1.bmp to custom_image1.cc
...
-- Defined build user options:
...
-- img_class_FILE_PATH=/tmp/custom_images
-- img_class_IMAGE_SIZE=224
```

After compiling, your custom images have now replaced the default ones in the application.

> **Note:** The CMake parameter `IMAGE_SIZE` must match the model input size. When building the application, if the size
of any image does not match `IMAGE_SIZE`, then it is rescaled and padded so that it does.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter `MODEL_PATH`.

> **Note:** If you want to run the model using an *Ethos-U*, ensure that your custom model has been successfully run
> through the Vela compiler *before* continuing.

For further information: [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

To run the application with a custom model, you must provide a `labels_<model_name>.txt` file of labels that are
associated with the model. Each line of the file must correspond to one of the outputs in your model.

Refer to the provided `labels_mobilenet_v2_1.0_224.txt` and `labels_mobilenet_v2_1.IMAGENET1K_V2.txt` files for examples.

Then, you must set `img_class_MODEL_PATH` to the location of the Vela processed model file and
`img_class_LABELS_TXT_FILE` to the location of the associated labels file.

For example, a TensorFlow Lite Micro based build tree can be generated by:

```commandline
cmake .. \
    -Dimg_class_MODEL_PATH=<path/to/custom_model_after_vela.tflite> \
    -Dimg_class_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DUSE_CASE_BUILD=img_class
```

Or for ExecuTorch, it should be:

```commandline
cmake .. \
    -DML_FRAMEWORK=ExecuTorch \
    -Dimg_class_MODEL_PATH=<path/to/custom_model_from_arm_aot_compiler.pte> \
    -Dimg_class_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DUSE_CASE_BUILD=img_class
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` or `.pte` model file pointed to by `img_class_MODEL_PATH`, and the labels text file pointed to by
`img_class_LABELS_TXT_FILE` are converted to C++ files during the CMake configuration stage. They are then compiled into
the application for performing inference with.

The log from the configuration stage tells you what model path and labels file have been used, for example:

```log
-- User option img_class_MODEL_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- User option img_class_LABELS_TXT_FILE is set to <path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to\
custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build/generated/src/Labels.cc>
...
```

After compiling, your custom model has now replaced the default one in the application.

## Setting up and running Ethos-U NPU code sample

### Setting up the Ethos-U NPU Fast Model

The FVP is available publicly from
[Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For the *Ethos-U* evaluation, please download the MPS3 based version of the Arm® *Corstone™-300* model that contains *Cortex-M55*
and offers a choice of the *Ethos-U55* and *Ethos-U65* processors.

To install the FVP:

- Unpack the archive.

- Run the install script in the extracted package:

```commandline
./FVP_Corstone_SSE-300.sh
```

- Follow the instructions to install the FVP to the required location.

### Starting Fast Model simulation

The pre-built application binary `mlek_img_class.axf` can be found in the `bin/mps3-sse-300` folder of the delivery
package.

Assuming that the install location of the FVP was set to `~/FVP_install_location`, then the simulation can be started by
using:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 ./bin/mps3-sse-300/mlek_img_class.axf
```

A log output appears on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This also launches a telnet window with the standard output of the sample application. It also includes error log
entries containing information about the pre-built application version, TensorFlow Lite Micro library version used, and
data types. The log also includes the input and output tensor sizes of the model compiled into the executable binary.

### Running Image Classification

Please select the first menu option to execute Image Classification.

The following example illustrates an application output for classification:

```log
INFO - Running inference on image 0 => cat.bmp
INFO - Final results:
INFO - Total number of inferences: 1
INFO - 0) 282 (0.753906) -> tabby, tabby cat
INFO - 1) 286 (0.148438) -> Egyptian cat
INFO - 2) 283 (0.062500) -> tiger cat
INFO - 3) 458 (0.003906) -> bow tie, bow-tie, bowtie
INFO - 4) 288 (0.003906) -> lynx, catamount
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 2468259
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 1151319
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 432351
INFO - NPU ACTIVE cycles: 7345741
INFO - NPU IDLE cycles: 431
INFO - NPU TOTAL cycles: 7346172
```

It can take several minutes to complete one inference run. The average time is around 2-3 minutes.

The log shows the inference results for `image 0`, so `0` - `index`, that corresponds to `cat.bmp` in the sample image
resource folder.

The profiling section of the log shows that for this inference:

- *Ethos-U* PMU report:

  - 7,346,172 total cycle: The number of NPU cycles.

  - 7,345,741 active cycles: The number of NPU cycles that were used for computation.

  - 413 idle cycles: The number of cycles for which the NPU was idle.

  - 2,468,259 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where the
    *Ethos-U* NPU reads and writes to the computation buffers, activation buf, or tensor arenas.

  - 1,151,319 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 432,351 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where the
    *Ethos-U* NPU reads the model. So, read-only.

- For FPGA platforms, a CPU cycle count can also be enabled. However, do not use cycle counters for FVP, as the CPU
  model is not cycle-approximate or cycle-accurate

The application prints the top five classes with indexes, a confidence score, and labels from the associated
labels file (e.g. `labels_mobilenet_v2_1.0_224.txt`). The FVP window also shows the output on its LCD section.
