# Quick start example ML application

This is a quick start guide that shows you how to run the keyword spotting example application.

The aim of this quick start guide is to enable you to run an application quickly on the Fixed Virtual Platform (FVP).
This documentation assumes that you are using an Arm® *Ethos™-U55* NPU, it is configured to use 128 Multiply-Accumulate units, and is
sharing SRAM with the Arm® *Cortex®-M55*.

The FVP is also available via Arm Virtual Hardware (AVH) hosted on AWS. This quick start guide can also be executed on an AVH instance
but requires creation of an AWS account and basic configuration outlined [here](./sections/arm_virtual_hardware.md#getting-started).

To get started quickly, please follow these steps:

1. First, verify that you have installed all of [the required prerequisites](sections/building.md#build-prerequisites).

2. Clone the *Ethos-U* evaluation kit repository:

    ```commandline
    git clone "https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit"
    cd ml-embedded-evaluation-kit
    ```

3. Pull all the external dependencies with the following command:

    ```commandline
    git submodule update --init
    ```

4. Next, you can use the `build_default` Python script to get the default neural network models, compile them with Vela,
    and then build the project.

    [Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela) is an open-source Python tool. Vela
    converts a TensorFlow Lite for Microcontrollers neural network model into an optimized model that can run on an
    embedded system that contains an *Ethos-U* NPU.

    It is worth noting that to take full advantage of the capabilities of the NPU, the neural network operators must be
    [supported by Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/+/HEAD/SUPPORTED_OPS.md).

    ```commandline
    python3 ./build_default.py
    ```

    > **Note** The preceding command assumes you are using the GNU Arm Embedded toolchain. If you are using the Arm
    > Compiler instead, you can override the default selection by executing:

    ```commandline
    python3 ./build_default.py --toolchain arm
    ```

5. Launch the project as explained in the following section: [Deployments](sections/deployment.md#deployment). In quick
   start guide, we use the keyword spotting application and the FVP.

    Change directory to generated cmake build folder which contains the .axf file output from step 4 in it's `bin`
    subdirectory. Launch the application by passing the axf to the FVP you downloaded when installing the prerequisites.
    Alternatively, from the root directory add `<cmake-build-your_config>` to the path to the axf and use one of the
    following commands:

    ```commandline
   From auto-generated (or custom) build directory:
   <path_to_FVP>/FVP_Corstone_SSE-300_Ethos-U55 -a ./bin/ethos-u-kws.axf
   From root directory:
   <path_to_FVP>/FVP_Corstone_SSE-300_Ethos-U55 -a <cmake-build-your_config>/bin/ethos-u-kws.axf
    ```

6. A telnet window is launched through which you can interact with the application and obtain performance figures.

> **Note:** Executing the `build_default.py` script is equivalent to running the following commands:

```commandline
mkdir resources_downloaded && cd resources_downloaded
python3 -m venv env
env/bin/python3 -m pip install --upgrade pip
env/bin/python3 -m pip install --upgrade setuptools
env/bin/python3 -m pip install ethos-u-vela==3.4.0
cd ..

curl -L https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/ad_medium_int8.tflite \
    --output resources_downloaded/ad/ad_medium_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/testing_input/input/0.npy \
    --output ./resources_downloaded/ad/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/testing_output/Identity/0.npy \
    --output ./resources_downloaded/ad/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/wav2letter_pruned_int8.tflite \
    --output ./resources_downloaded/asr/wav2letter_pruned_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_input/input_2_int8/0.npy \
    --output ./resources_downloaded/asr/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_output/Identity_int8/0.npy \
    --output ./resources_downloaded/asr/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/mobilenet_v2_1.0_224_INT8.tflite \
    --output ./resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/testing_input/tfl.quantize/0.npy \
    --output ./resources_downloaded/img_class/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/testing_output/MobilenetV2/Predictions/Reshape_11/0.npy \
    --output ./resources_downloaded/img_class/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/kws_micronet_m.tflite \
    --output ./resources_downloaded/kws/kws_micronet_m.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/testing_input/input/0.npy \
    --output ./resources_downloaded/kws/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/testing_output/Identity/0.npy \
    --output ./resources_downloaded/kws/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/wav2letter_pruned_int8.tflite \
    --output ./resources_downloaded/kws_asr/wav2letter_pruned_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_input/input_2_int8/0.npy \
    --output ./resources_downloaded/kws_asr/asr/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_output/Identity_int8/0.npy \
    --output ./resources_downloaded/kws_asr/asr/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/kws_micronet_m.tflite \
    --output ./resources_downloaded/kws_asr/kws_micronet_m.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/testing_input/input/0.npy \
    --output ./resources_downloaded/kws_asr/kws/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/testing_output/Identity/0.npy \
    --output ./resources_downloaded/kws_asr/kws/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/rnnoise_INT8.tflite \
    --output ./resources_downloaded/noise_reduction/rnnoise_INT8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/main_input_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/vad_gru_prev_state_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ifm1.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/noise_gru_prev_state_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ifm2.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/denoise_gru_prev_state_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ifm3.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_1_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ofm1.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_2_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ofm2.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_3_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ofm3.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_4_int8/0.npy \
    --output ./resources_downloaded/noise_reduction/ofm4.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/dnn_small/tflite_int8/dnn_s_quantized.tflite \
    --output ./resources_downloaded/inference_runner/dnn_s_quantized.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/vww4_128_128_INT8.tflite \
    --output ./resources_downloaded/vww/vww4_128_128_INT8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/testing_input/input/0.npy \
    --output ./resources_downloaded/vww/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/testing_output/Identity/0.npy \
    --output ./resources_downloaded/vww/ofm0.npy

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws/kws_micronet_m.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/kws \
    --arena-cache-size=2097152
mv resources_downloaded/kws/kws_micronet_m.tflite resources_downloaded/kws/kws_micronet_m_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws/kws_micronet_m.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/kws
mv resources_downloaded/kws/kws_micronet_vela.tflite resources_downloaded/kws/kws_micronet_m_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws_asr/wav2letter_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/kws_asr \
    --arena-cache-size=2097152
mv resources_downloaded/kws_asr/wav2letter_int8_vela.tflite resources_downloaded/kws_asr/wav2letter_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws_asr/wav2letter_int8.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/kws_asr
mv resources_downloaded/kws_asr/wav2letter_int8_vela.tflite resources_downloaded/kws_asr/wav2letter_int8_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws_asr/kws_micronet_m.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/kws_asr \
    --arena-cache-size=2097152
mv resources_downloaded/kws_asr/kws_micronet_m.tflite_vela.tflite resources_downloaded/kws_asr/kws_micronet_m.tflite_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws_asr/kws_micronet_m.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/kws_asr
mv resources_downloaded/kws_asr/kws_micronet_m.tflite_vela.tflite resources_downloaded/kws_asr/kws_micronet_m.tflite_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/inference_runner/dnn_s_quantized.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/inference_runner \
    --arena-cache-size=2097152
mv resources_downloaded/inference_runner/dnn_s_quantized_vela.tflite resources_downloaded/inference_runner/dnn_s_quantized_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/inference_runner/dnn_s_quantized.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/inference_runner
mv resources_downloaded/inference_runner/dnn_s_quantized_vela.tflite resources_downloaded/inference_runner/dnn_s_quantized_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/img_class \
    --arena-cache-size=2097152
mv resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8_vela.tflite resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/img_class
mv resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8_vela.tflite resources_downloaded/img_class/mobilenet_v2_1.0_224_INT8_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/asr/wav2letter_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/asr \
    --arena-cache-size=2097152
mv resources_downloaded/asr/wav2letter_int8_vela.tflite resources_downloaded/asr/wav2letter_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/asr/wav2letter_int8.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/asr
mv resources_downloaded/asr/wav2letter_int8_vela.tflite resources_downloaded/asr/wav2letter_int8_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/ad/ad_medium_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/ad \
    --arena-cache-size=2097152
mv resources_downloaded/ad/ad_medium_int8_vela.tflite resources_downloaded/ad/ad_medium_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/ad/ad_medium_int8.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/ad
mv resources_downloaded/ad/ad_medium_int8_vela.tflite resources_downloaded/ad/ad_medium_int8_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/vww/vww4_128_128_INT8.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/ad \
    --arena-cache-size=2097152
mv resources_downloaded/vww/vww4_128_128_INT8_vela.tflite resources_downloaded/vww/vww4_128_128_INT8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/vww/vww4_128_128_INT8.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/ad
mv resources_downloaded/vww/vww4_128_128_INT8_vela.tflite resources_downloaded/vww/vww4_128_128_INT8_vela_Y256.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/noise_reduction/rnnoise_INT8.tflite \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/ad \
    --arena-cache-size=2097152
mv resources_downloaded/noise_reduction/rnnoise_INT8_vela.tflite resources_downloaded/noise_reduction/rnnoise_INT8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/noise_reduction/rnnoise_INT8.tflite \
    --accelerator-config=ethos-u65-256 \
    --optimise Performance --config scripts/vela/default_vela.ini \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --output-dir=resources_downloaded/ad
mv resources_downloaded/noise_reduction/rnnoise_INT8_vela.tflite resources_downloaded/noise_reduction/rnnoise_INT8_vela_Y256.tflite

mkdir cmake-build-mps3-sse-300-gnu-release and cd cmake-build-mps3-sse-300-gnu-release

cmake .. \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake
```

> **Note:** If you want to change the application, then, instead of using the `build_default` Python script, follow the
> approach defined in [documentation.md](./documentation.md#arm_ml-embedded-evaluation-kit). For example, if you wanted to modify the number of
> MACs units of the Ethos-U, or running a custom neural network.
