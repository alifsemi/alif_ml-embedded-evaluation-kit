# Quick start example ML application

This is a quick start guide that will show you how to run the keyword spotting example application. The aim of this guide
is to illustrate the flow of running an application on the evaluation kit rather than showing the keyword spotting
functionality or performance. All use cases in the evaluation kit follow the steps.

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

4. Next, you would need to get a neural network model. For the purpose of this quick start guide, we'll use the
    `ds_cnn_clustered_int8` keyword spotting model from the [Arm public model zoo](https://github.com/ARM-software/ML-zoo)
    and the principle remains the same for all of the other use cases. Download the `ds_cnn_large_int8.tflite` model
    file with the curl command below:

    ```commandline
    curl -L https://github.com/ARM-software/ML-zoo/blob/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite?raw=true --output ds_cnn_clustered_int8.tflite
    ```

5. [Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela) is an open-source python tool converting
    TensorFlow Lite for Microcontrollers neural network model into an optimized model that can run on an embedded system
    containing an Ethos-U55 NPU. It is worth noting that in order to take full advantage of the capabilities of the NPU, the
    neural network operators should be [supported by Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/+/HEAD/SUPPORTED_OPS.md).
    In this step, you will compile the model with Vela.

    For this step, you need to ensure you have [correctly installed the Vela package](https://pypi.org/project/ethos-u-vela/):

    ```commandline
    python3 -m venv env
    source ./env/bin/activate
    pip install --upgrade pip
    pip install --upgrade setuptools
    pip install ethos-u-vela
    ```

    In the command below, we specify that we are using the Arm® Ethos™-U55 NPU with a 128 Multiply-Accumulate units
    (MAC units) configured for a High End Embedded use case. The [building section](sections/building.md#Optimize-custom-model-with-Vela-compiler)
    has more detailed explanation about Vela usage.

    ```commandline
    vela ds_cnn_clustered_int8.tflite \
        --accelerator-config=ethos-u55-128 \
        --block-config-limit=0 \
        --config scripts/vela/vela.ini \
        --memory-mode Shared_Sram \
        --system-config Ethos_U55_High_End_Embedded
    ```

    An optimized model file for Ethos-U55 is generated in a folder named `output`.

6. Create a `build` folder in the root level of the evaluation kit.

    ```commandline
    mkdir build && cd build
    ```

7. Build the makefiles with `CMake` as shown in the command below. The [build process section](sections/building.md#Build-process)
    gives an in-depth explanation about the meaning of every parameter. For the time being, note that we point the Vela
    optimized model from stage 5 in the `-Dkws_MODEL_TFLITE_PATH` parameter.

    ```commandline
    cmake \
        -DUSE_CASE_BUILD=kws \
        -Dkws_MODEL_TFLITE_PATH=output/ds_cnn_clustered_int8_vela.tflite \
        ..
    ```

8. Compile the project with a `make`. Details about this stage can be found in the [building part of the documentation](sections/building.md#Building-the-configured-project).

    ```commandline
    make -j4
    ```

9. Launch the project as explained [here](sections/deployment.md#Deployment). In this quick-start guide, we'll use the Fixed
    Virtual Platform. Point the generated `bin/ethos-u-kws.axf` file in stage 8 to the FVP that you have downloaded when
    installing the prerequisites.

    ```commandline
    <path_to_FVP>/FVP_Corstone_SSE-300_Ethos-U55 -a ./bin/ethos-u-kws.axf
    ```

10. A telnet window is launched through which you can interact with the application and obtain performance figures.
