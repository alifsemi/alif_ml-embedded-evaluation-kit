# Quick start example ML application

This is a quick start guide that will show you how to run the keyword spotting example application.
The aim of this quick start guide is to enable you to run an application quickly on the Fixed Virtual Platform.
The assumption we are making is that your Arm® Ethos™-U55 NPU is configured to use 128 Multiply-Accumulate units,
is using a shared SRAM with the Arm® Cortex®-M55.  

1. Verify you have installed [the required prerequisites](sections/building.md#Build-prerequisites).

2. Clone the Ethos-U55 evaluation kit repository.

    ```commandline
    git clone "https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit"
    cd ml-embedded-evaluation-kit
    ```

3. Pull all the external dependencies with the commands below:

    ```commandline
    git submodule update --init
    ```

4. Next, you can use the `build_default` python script to get the default neural network models, compile them with
    Vela and build the project.
    [Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela) is an open-source python tool converting
    TensorFlow Lite for Microcontrollers neural network model into an optimized model that can run on an embedded system
    containing an Ethos-U55 NPU. It is worth noting that in order to take full advantage of the capabilities of the NPU, the
    neural network operators should be [supported by Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/+/HEAD/SUPPORTED_OPS.md).

    ```commandline
    python3 build_default.py
    ```

5. Launch the project as explained [here](sections/deployment.md#Deployment). For the purpose of this quick start guide,
    we'll use the keyword spotting application and the Fixed Virtual Platform.
    Point the generated `bin/ethos-u-kws.axf` file in stage 4 to the FVP that you have downloaded when
    installing the prerequisites.

    ```commandline
    <path_to_FVP>/FVP_Corstone_SSE-300_Ethos-U55 -a ./bin/ethos-u-kws.axf
    ```

6. A telnet window is launched through which you can interact with the application and obtain performance figures.

> **Note:**: Execution of the build_default.py script is equivalent to running the following commands:

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
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/wav2letter_int8.tflite \
    --output ./resources_downloaded/asr/wav2letter_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_input/input_2_int8/0.npy \
    --output ./resources_downloaded/asr/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_output/Identity_int8/0.npy \
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
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/wav2letter_int8.tflite \
    --output ./resources_downloaded/kws_asr/wav2letter_int8.tflite
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_input/input_2_int8/0.npy \
    --output ./resources_downloaded/kws_asr/asr/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_input/input_2_int8/0.npy
    --output ./resources_downloaded/kws_asr/asr/ifm0.npy
curl -L https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_output/Identity_int8/0.npy \
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

> **Note:** If you want to make changes to the application (for example modifying the number of MAC units of the Ethos-U or running a custom neural network),
> you should follow the approach defined in [documentation.md](../docs/documentation.md) instead of using the `build_default` python script.
