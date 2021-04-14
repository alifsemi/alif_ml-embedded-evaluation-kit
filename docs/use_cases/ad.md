# Anomaly Detection Code Sample

  - [Introduction](#introduction)
    - [Prerequisites](#prerequisites)
  - [Building the code sample application from sources](#building-the-code-sample-application-from-sources)
    - [Build options](#build-options)
    - [Build process](#build-process)
    - [Add custom input](#add-custom-input)
    - [Add custom model](#add-custom-model)
  - [Setting-up and running Ethos-U55 Code Sample](#setting-up-and-running-ethos-u55-code-sample)
    - [Setting up the Ethos-U55 Fast Model](#setting-up-the-ethos-u55-fast-model)
    - [Starting Fast Model simulation](#starting-fast-model-simulation)
    - [Running Anomaly Detection](#running-anomaly-detection)
  - [Anomaly Detection processing information](#anomaly-detection-processing-information)
    - [Preprocessing and feature extraction](#preprocessing-and-feature-extraction)
    - [Postprocessing](#postprocessing)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U55 Anomaly Detection example.

Use case code could be found in [source/use_case/ad](../../source/use_case/ad]) directory.

### Preprocessing and feature extraction

The Anomaly Detection model that is used with the Code Samples expects audio data to be preprocessed
in a specific way before performing an inference. This section aims to provide an overview of the feature extraction
process used.

First the audio data is normalized to the range (-1, 1).

Next, a window of 1024 audio samples are taken from the start of the audio clip. From these 1024 samples we calculate 64
Log Mel Energies that form part of a Log Mel Spectrogram.

The window is shifted by 512 audio samples and another 64 Log Mel Energies are calculated. This is repeated until we
have 64 sets of Log Mel Energies.

This 64x64 matrix of values is resized by a factor of 2 resulting in a 32x32 matrix of values.

The average of the training dataset is subtracted from this 32x32 matrix and an inference can then be performed.

We start this process again but shifting the start by 20\*512=10240 audio samples. This keeps repeating until enough
inferences have been performed to cover the whole audio clip.

### Postprocessing

Softmax is applied to the result of each inference. Based on the machine ID of the wav clip being processed we look at a
specific index in each output vector. An average of the negative value at this index across all the inferences performed
for the audio clip is taken. If this average value is greater than a chosen threshold score, then the machine in the
clip is not behaving anomalously. If the score is lower than the threshold then the machine in the clip is behaving
anomalously.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, Anomaly Detection use case adds:

- `ad_MODEL_TFLITE_PATH` - Path to the NN model file in TFLite format. Model will be processed and included into
the application axf
    file. The default value points to one of the delivered set of models. Note that the parameters `ad_LABELS_TXT_FILE`,
    `TARGET_PLATFORM` and `ETHOS_U55_ENABLED` should be aligned with the chosen model, i.e.:
  - if `ETHOS_U55_ENABLED` is set to `On` or `1`, the NN model is assumed to be optimized. The model will naturally fall
back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
model in this case will result in a runtime error.

- `ad_FILE_PATH`: Path to the directory containing audio files, or a path to single WAV file, to be used in the
    application. The default value points to the resources/ad/samples folder containing the delivered set of audio clips.

- `ad_AUDIO_RATE`: Input data sampling rate. Each audio file from ad_FILE_PATH is preprocessed during the build to match
NN model input requirements.
    Default value is 16000.

- `ad_AUDIO_MONO`: If set to ON the audio data will be converted to mono. Default is ON.

- `ad_AUDIO_OFFSET`: Start loading audio data starting from this offset (in seconds). Default value is 0.

- `ad_AUDIO_DURATION`: Length of the audio data to be used in the application in seconds. Default is 0 meaning the
    whole audio file will be taken.

- `ad_AUDIO_MIN_SAMPLES`: Minimum number of samples required by the network model. If the audio clip is shorter than
    this number, it is padded with zeros. Default value is 16000.

- `ad_MODEL_SCORE_THRESHOLD`: Threshold value to be applied to average softmax score over the clip, if larger than this
score we have an anomaly.

- `ad_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By default, it is set to
    2MiB and should be enough for most models.

In order to build **ONLY** Anomaly Detection example application add to the `cmake` command line specified in [Building](../documentation.md#Building) `-DUSE_CASE_BUILD=ad`.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target
>platform see [Building](../documentation.md#Building).

Create a build directory folder and navigate inside:

```commandline
mkdir build_ad && cd build_ad
```

On Linux, execute the following command to build **only** Anomaly Detection application to run on the Ethos-U55 Fast Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=./scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=ad ..
```

For Windows, add `-G "MinGW Makefiles"`:

```commandline
cmake \
    -G "MinGW Makefiles" \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=./scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=ad ..
```

Toolchain option `CMAKE_TOOLCHAIN_FILE` points to the toolchain specific file to set the compiler and platform specific
parameters.

To configure a build that can be debugged using Arm-DS, we can just specify
the build type as `Debug`:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=ad ..
```

To configure a build that can be debugged using a tool that only supports
DWARF format 3 (Modeldebugger for example), we can use:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DARMCLANG_DEBUG_DWARF_LEVEL=3 \
    -DUSE_CASE_BUILD=ad ..
```

> **Note:** If building for different Ethos-U55 configurations, see
[Configuring build for different Arm Ethos-U55 configurations](../sections/building.md#Configuring-build-for-different-Arm-Ethos-U55-configurations):

If the TensorFlow source tree is not in its default expected location,
set the path using `TENSORFLOW_SRC_PATH`.
Similarly, if the Ethos-U55 driver is not in the default location,
`ETHOS_U55_DRIVER_SRC_PATH` can be used to configure the location. For example:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DTENSORFLOW_SRC_PATH=/my/custom/location/tensorflow \
    -DETHOS_U55_DRIVER_SRC_PATH=/my/custom/location/core_driver \
    -DUSE_CASE_BUILD=ad ..
```

Also, `CMSIS_SRC_PATH` parameter can be used to override the CMSIS sources used for compilation used by TensorFlow by
default. For example, to use the CMSIS sources fetched by the ethos-u helper script, we can use:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DTENSORFLOW_SRC_PATH=../ethos-u/core_software/tensorflow \
    -DETHOS_U55_DRIVER_SRC_PATH=../ethos-u/core_software/core_driver \
    -DCMSIS_SRC_PATH=../ethos-u/core_software/cmsis \
    -DUSE_CASE_BUILD=ad ..
```

> **Note:** If re-building with changed parameters values, it is highly advised to clean the build directory and re-run the CMake command.

If the CMake command succeeded, build the application as follows:

```commandline
make -j4
```

For Windows, use `mingw32-make`.

Add VERBOSE=1 to see compilation and link details.

Results of the build will be placed under `build/bin` folder:

```tree
bin
 ├── ethos-u-.axf
 ├── ethos-u-ad.htm
 ├── ethos-u-.map
 ├── images-ad.txt
 └── sectors
      └── ad
          ├── dram.bin
          └── itcm.bin
```

Where:

- `ethos-u-ad.axf`: The built application binary for the Anomaly Detection use case.

- `ethos-u-ad.map`: Information from building the application (e.g. libraries used, what was optimized, location of
    objects)

- `ethos-u-ad.htm`: Human readable file containing the call graph of application functions.

- `sectors/`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `Images-ad.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/\*\* folder.

### Add custom input

The application anomaly detection on audio data found in the folder, or an individual file, set by the CMake parameter
``ad_FILE_PATH``.

To run the application with your own audio clips first create a folder to hold them and then copy the custom clips into
this folder:

```commandline
mkdir /tmp/custom_files

cp custom_id_00.wav /tmp/custom_files/
```

> **Note:** The data used for this example comes from
[https://zenodo.org/record/3384388\#.X6GILFNKiqA](https://zenodo.org/record/3384388\#.X6GILFNKiqA)
and the model included in this example is trained on the ‘Slider’ part of the dataset.
The machine ID (00, 02, 04, 06) the clip comes from must be in the file name for the application to work.
The file name should have a pattern that matches
e.g. `<any>_<text>_00_<here>.wav` if the audio was from machine ID 00
or `<any>_<text>_02_<here>.wav` if it was from machine ID 02 etc.
>
> **Note:** Clean the build directory before re-running the CMake command.

Next set ad_FILE_PATH to the location of this folder when building:

```commandline
cmake \
    -Dad_FILE_PATH=/tmp/custom_files/ \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=ad ..
```

For Windows, add `-G "MinGW Makefiles"` to the CMake command.

The images found in the _DIR folder will be picked up and automatically converted to C++ files during the CMake
configuration stage and then compiled into the application during the build phase for performing inference with.

The log from the configuration stage should tell you what image directory path has been used:

```log
-- User option ad_FILE_PATH is set to /tmp/custom_files
```

After compiling, your custom inputs will have now replaced the default ones in the application.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter ``ad_MODEL_TFLITE_PATH``.

> **Note:** If you want to run the model using Ethos-U55, ensure your custom model has been run through the Vela compiler
>successfully before continuing. See [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

An example:

```commandline
cmake \
    -Dad_MODEL_TFLITE_PATH=<path/to/custom_ad_model_after_vela.tflite> \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=ad ..
```

For Windows, add `-G "MinGW Makefiles"` to the CMake command.

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `ad_MODEL_TFLITE_PATH` will be converted
to C++ files during the CMake configuration
stage and then compiled into the application for performing inference with.

The log from the configuration stage should tell you what model path has been used:

```log
-- User option TARGET_PLATFORM is set to fastmodel
-- User option ad_MODEL_TFLITE_PATH is set to <path/to/custom_ad_model_after_vela.tflite>
...
-- Using <path/to/custom_ad_model_after_vela.tflite>
++ Converting custom_ad_model_after_vela.tflite to custom_ad_model_after_vela.tflite.cc
...
```

After compiling, your custom model will have now replaced the default one in the application.

 >**Note:** In order to successfully run the model, the NPU needs to be enabled and
 the platform `TARGET_PLATFORM` is set to mps3 and TARGET_SUBSYSTEM is SSE-200 or SSE-300.

## Setting-up and running Ethos-U55 Code Sample

### Setting up the Ethos-U55 Fast Model

The FVP is available publicly from [Arm Ecosystem FVP downloads
](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For Ethos-U55 evaluation, please download the MPS3 version of the Arm® Corstone™-300 model that contains Ethos-U55 and
Cortex-M55. The model is currently only supported on Linux based machines. To install the FVP:

- Unpack the archive

- Run the install script in the extracted package

```commandline
.FVP_Corstone_SSE-300_Ethos-U55.sh
```

- Follow the instructions to install the FVP to your desired location

### Starting Fast Model simulation

> **Note:** The anomaly detection example does not come pre-built. You will first need to follow the instructions in
>section 3 for building the application from source.

After building, and assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be
started by:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 ./bin/ethos-u-ad.axf
```

A log output should appear on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This will also launch a telnet window with the sample application's standard output and error log entries containing
information about the pre-built application version, TensorFlow Lite Micro library version used, data type as well as
the input and output tensor sizes of the model compiled into the executable binary.

After the application has started if `ad_FILE_PATH` pointed to a single file (or a folder containing a single input file)
the inference starts immediately. In case of multiple inputs choice, it outputs a menu and waits for the user input from
telnet terminal:

```log
User input required
Enter option number from:

1. Classify next audio clip
2. Classify audio clip at chosen index
3. Run classification on all audio clips
4. Show NN model info
5. List audio clips

Choice:

```

1. “Classify next audio clip” menu option will run single inference on the next in line.

2. “Classify audio clip at chosen index” menu option will run inference on the chosen audio clip.

    > **Note:** Please make sure to select audio clip index in the range of supplied audio clips during application build.
    By default, pre-built application has 4 files, indexes from 0 to 3.

3. “Run ... on all” menu option triggers sequential inference executions on all built-in .

4. “Show NN model info” menu option prints information about model data type, input and output tensor sizes:

    ```log
    INFO - uTFL version: 2.5.0
    INFO - Model info:
    INFO - Model INPUT tensors:
    INFO - 	tensor type is INT8
    INFO - 	tensor occupies 1024 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1:  32
    INFO - 		2:  32
    INFO - 		3:   1
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.192437
    INFO - ZeroPoint[0] = 11
    INFO - Model OUTPUT tensors:
    INFO - 	tensor type is INT8
    INFO - 	tensor occupies 8 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1:   8
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.048891
    INFO - ZeroPoint[0] = -30
    INFO - Activation buffer (a.k.a tensor arena) size used: 198016
    INFO - Number of operators: 1
    INFO - 	Operator 0: ethos-u
    ```

5. “List” menu option prints a list of pair ... indexes - the original filenames embedded in the application:

    ```log
    INFO - List of Files:
    INFO - 0 =>; anomaly_id_00_00000000.wav
    INFO - 1 =>; anomaly_id_02_00000076.wav
    INFO - 2 =>; normal_id_00_00000004.wav
    INFO - 3 =>; normal_id_02_00000001.wav
    ```

### Running Anomaly Detection

Please select the first menu option to execute Anomaly Detection.

The following example illustrates application output:

```log
INFO - Running inference on audio clip 0 => anomaly_id_00_00000000.wav
INFO - Inference 1/13
INFO - Inference 2/13
INFO - Inference 3/13
INFO - Inference 4/13
INFO - Inference 5/13
INFO - Inference 6/13
INFO - Inference 7/13
INFO - Inference 8/13
INFO - Inference 9/13
INFO - Inference 10/13
INFO - Inference 11/13
INFO - Inference 12/13
INFO - Inference 13/13
INFO - Average anomaly score is: -0.024493
Anomaly threshold is: -0.800000
Anomaly detected!
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 628122
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 135087
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 62870
INFO - NPU ACTIVE cycles: 1081007
INFO - NPU IDLE cycles: 626
INFO - NPU total cycles: 1081634
```

As multiple inferences have to be run for one clip it will take around a minute or so for all inferences to complete.

For the anomaly_id_00_00000000.wav clip, after averaging results across all inferences the score is greater than the
chosen anomaly threshold so an anomaly was detected with the machine in this clip.

The profiling section of the log shows that for each inference. For the last inference the profiling reports:

- Ethos-U55's PMU report:

  - 1,081,634 total cycle: The number of NPU cycles

  - 1,081,007 active cycles: number of NPU cycles that were used for computation

  - 626 idle cycles: number of cycles for which the NPU was idle

  - 628,122 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus.
    AXI0 is the bus where Ethos-U55 NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 135,087 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 62,870 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus.
    AXI1 is the bus where Ethos-U55 NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
    the CPU model is not cycle-approximate or cycle-accurate.
