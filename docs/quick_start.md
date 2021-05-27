# Quick start example ML application

This is a quick start guide that shows you how to run the keyword spotting example application.

The aim of this quick start guide is to enable you to run an application quickly on the Fixed Virtual Platform (FVP).
This documentation assumes that your Arm® *Ethos™-U55* NPU is configured to use 128 Multiply-Accumulate units, and is
sharing SRAM with the Arm® *Cortex®-M55*.

To get started quickly, please follow these steps:

1. First, verify that you have installed [the required prerequisites](sections/building.md#Build-prerequisites).

2. Clone the *Ethos-U55* evaluation kit repository:

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
    embedded system that contains an *Ethos-U55* NPU.

    It is worth noting that to take full advantage of the capabilities of the NPU, the neural network operators must be
    [supported by Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/+/HEAD/SUPPORTED_OPS.md).

    ```commandline
    ./build_default.py
    ```

    > **Note** The preceding command assumes you are using the GNU Arm Embedded toolchain. If you are using the Arm
    > Compiler instead, you can override the default selection by executing:

    ```commandline
    ./build_default.py --toolchain arm
    ```

5. Launch the project as explained in the following section: [Deployments](sections/deployment.md#Deployment). In quick
   start guide, we use the keyword spotting application and the FVP.

    Point the generated `bin/ethos-u-kws.axf` file, from step four, to the FVP you downloaded when installing the
    prerequisites. Now use the following command:

    ```commandline
    <path_to_FVP>/FVP_Corstone_SSE-300_Ethos-U55 -a ./bin/ethos-u-kws.axf
    ```

6. A telnet window is launched through where you can interact with the application and obtain performance figures.

> **Note:** Executing the `build_default.py` script is equivalent to running the following commands:

```commandline
mkdir resources_downloaded && cd resources_downloaded
python3 -m venv env
env/bin/python3 -m pip install --upgrade pip
env/bin/python3 -m pip install --upgrade setuptools
env/bin/python3 -m pip install ethos-u-vela==2.1.1
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
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8/mobilenet_v2_1.0_224_quantized_1_default_1.tflite \
    --output ./resources_downloaded/img_class/mobilenet_v2_1.0_224_quantized_1_default_1.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8/testing_input/input/0.npy \
    --output ./resources_downloaded/img_class/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8/testing_output/output/0.npy \
    --output ./resources_downloaded/img_class/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite \
    --output ./resources_downloaded/kws/ds_cnn_clustered_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_input/input_2/0.npy \
    --output ./resources_downloaded/kws/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_output/Identity/0.npy \
    --output ./resources_downloaded/kws/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/wav2letter_pruned_int8.tflite \
    --output ./resources_downloaded/kws_asr/wav2letter_pruned_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_input/input_2_int8/0.npy \
    --output ./resources_downloaded/kws_asr/asr/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_output/Identity_int8/0.npy \
    --output ./resources_downloaded/kws_asr/asr/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite \
    --output ./resources_downloaded/kws_asr/ds_cnn_clustered_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_input/input_2/0.npy \
    --output ./resources_downloaded/kws_asr/kws/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_output/Identity/0.npy \
    --output ./resources_downloaded/kws_asr/kws/ofm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/dnn_small/tflite_int8/dnn_s_quantized.tflite \
    --output ./resources_downloaded/inference_runner/dnn_s_quantized.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws/ds_cnn_clustered_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/kws
mv resources_downloaded/kws/ds_cnn_clustered_int8_vela.tflite resources_downloaded/kws/ds_cnn_clustered_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws_asr/wav2letter_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/kws_asr
mv resources_downloaded/kws_asr/wav2letter_int8_vela.tflite resources_downloaded/kws_asr/wav2letter_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/kws_asr/ds_cnn_clustered_int8.tflite -\
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/kws_asr
mv resources_downloaded/kws_asr/ds_cnn_clustered_int8_vela.tflite resources_downloaded/kws_asr/ds_cnn_clustered_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/inference_runner/dnn_s_quantized.tflite -\
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/inference_runner
mv resources_downloaded/inference_runner/dnn_s_quantized_vela.tflite resources_downloaded/inference_runner/dnn_s_quantized_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/img_class/mobilenet_v2_1.0_224_quantized_1_default_1.tflite \
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/img_class
mv resources_downloaded/img_class/mobilenet_v2_1.0_224_quantized_1_default_1_vela.tflite resources_downloaded/img_class/mobilenet_v2_1.0_224_quantized_1_default_1_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/asr/wav2letter_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/asr
mv resources_downloaded/asr/wav2letter_int8_vela.tflite resources_downloaded/asr/wav2letter_int8_vela_H128.tflite

. resources_downloaded/env/bin/activate && vela resources_downloaded/ad/ad_medium_int8.tflite \
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 --config scripts/vela/default_vela.ini \
    --memory-mode=Shared_Sram \
    --system-config=Ethos_U55_High_End_Embedded \
    --output-dir=resources_downloaded/ad
mv resources_downloaded/ad/ad_medium_int8_vela.tflite resources_downloaded/ad/ad_medium_int8_vela_H128.tflite

mkdir cmake-build-mps3-sse-300-gnu-release and cd cmake-build-mps3-sse-300-gnu-release

cmake .. \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake
```

> **Note:** If you want to change the application, then, instead of using the `build_default` Python script, follow the
> approach defined in [documentation.md](./documentation.md). For example, if you wanted to modify the number of
> MAC units of the Ethos-U, or running a custom neural network.
