# Keyword Spotting Code Sample

- [Keyword Spotting Code Sample](#keyword-spotting-code-sample)
  - [Introduction](#introduction)
    - [Preprocessing and feature extraction](#preprocessing-and-feature-extraction)
    - [Postprocessing](#postprocessing)
    - [Prerequisites](#prerequisites)
  - [Building the code sample application from sources](#building-the-code-sample-application-from-sources)
    - [Build options](#build-options)
    - [Build process](#build-process)
    - [Add custom input](#add-custom-input)
    - [Add custom model](#add-custom-model)
  - [Setting up and running Ethos-U55 code sample](#setting-up-and-running-ethos-u55-code-sample)
    - [Setting up the Ethos-U55 Fast Model](#setting-up-the-ethos-u55-fast-model)
    - [Starting Fast Model simulation](#starting-fast-model-simulation)
    - [Running Keyword Spotting](#running-keyword-spotting)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U55 Keyword Spotting
example.

Use case code could be found in [source/use_case/kws](../../source/use_case/kws]) directory.

### Preprocessing and feature extraction

The DS-CNN keyword spotting model that is supplied with the Code Samples expects audio data to be preprocessed in
a specific way before performing an inference. This section aims to provide an overview of the feature extraction
process used.

First the audio data is normalized to the range (-1, 1).

> **Note:** Mel-frequency cepstral coefficients (MFCCs) are a common feature extracted from audio data and can be used as
>input for machine learning tasks like keyword spotting and speech recognition.
>See source/application/main/include/Mfcc.hpp for implementation details.

Next, a window of 640 audio samples is taken from the start of the audio clip. From these 640 samples we calculate 10
MFCC features.

The whole window is shifted to the right by 320 audio samples and 10 new MFCC features are calculated. This process of
shifting and calculating is repeated until the end of the 16000 audio samples needed to perform an inference is reached.
In total this will be 49 windows that each have 10 MFCC features calculated for them, giving an input tensor of shape
49x10.

These extracted features are quantized, and an inference is performed.

![KWS preprocessing](../media/KWS_preprocessing.png)

If the audio clip is longer than 16000 audio samples then the initial starting position is offset by 16000/2 = 8000
audio samples. From this new starting point, MFCC features for the next 16000 audio samples are calculated and another
inference is performed (i.e. do an inference for samples 8000-24000).

> **Note:** Parameters of the MFCC feature extraction such as window size, stride, number of features etc. all depend on
>what was used during model training. These values are specific to each model and if you try a different keyword spotting
>model that uses MFCC input then values are likely to need changing to match the new model.
In addition, MFCC feature extraction methods can vary slightly with different normalization methods or scaling etc. being used.

### Postprocessing

After an inference is complete the highest probability detected word is output to console, providing its probability is
larger than a threshold value (default 0.9).

If multiple inferences are performed for an audio clip, then multiple results will be output.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, keyword spotting use case adds:

- `kws_MODEL_TFLITE_PATH` - Path to the NN model file in TFLite format. Model will be processed and included into the application axf file. The default value points to one of the delivered set of models. Note that the parameters `kws_LABELS_TXT_FILE`,`TARGET_PLATFORM` and `ETHOS_U55_ENABLED` should be aligned with the chosen model, i.e.:
  - if `ETHOS_U55_ENABLED` is set to `On` or `1`, the NN model is assumed to be optimized. The model will naturally fall back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized model in this case will result in a runtime error.

- `kws_FILE_PATH`: Path to the directory containing audio files, or a path to single WAV file, to be used in the application. The default value points
    to the resources/kws/samples folder containing the delivered set of audio clips.

- `kws_LABELS_TXT_FILE`: Path to the labels' text file. The file is used to map key word class index to the text
    label. The default value points to the delivered labels.txt file inside the delivery package.

- `kws_AUDIO_RATE`: Input data sampling rate. Each audio file from kws_FILE_PATH is preprocessed during the build to
    match NN model input requirements. Default value is 16000.

- `kws_AUDIO_MONO`: If set to ON the audio data will be converted to mono. Default is ON.

- `kws_AUDIO_OFFSET`: Start loading audio data starting from this offset (in seconds). Default value is 0.

- `kws_AUDIO_DURATION`: Length of the audio data to be used in the application in seconds. Default is 0 meaning the
    whole audio file will be taken.

- `kws_AUDIO_MIN_SAMPLES`: Minimum number of samples required by the network model. If the audio clip is shorter than
    this number, it is padded with zeros. Default value is 16000.

- `kws_MODEL_SCORE_THRESHOLD`: Threshold value [0.0, 1.0] that must be applied to the inference results for a
    label to be deemed valid. Default is 0.9

- `kws_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By default, it is set
    to 1MiB and should be enough for most models.

In order to build **ONLY** keyword spotting example application add to the `cmake` command line specified in [Building](../documentation.md#Building) `-DUSE_CASE_BUILD=kws`.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target platform see [Building](../documentation.md#Building) section.

In order to build **only** the keyword spotting example, create a build directory and
navigate inside, for example:

```commandline
mkdir build_kws && cd build_kws
```

On Linux, execute the following command to build Keyword Spotting application to run on the Ethos-U55 Fast Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake ../ -DUSE_CASE_BUILD=kws
```

To configure a build that can be debugged using Arm-DS, we can just specify
the build type as `Debug` and use the `Arm Compiler` toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=kws
```

Also see:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple_platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Working with model debugger from Arm FastModel Tools](../sections/building.md#working-with-model-debugger-from-arm-fastmodel-tools)

> **Note:** If re-building with changed parameters values, it is highly advised to clean the build directory and re-run the CMake command.

If the CMake command succeeded, build the application as follows:

```commandline
make -j4
```

Add VERBOSE=1 to see compilation and link details.

Results of the build will be placed under `build/bin` folder:

```tree
bin
 ├── ethos-u-kws.axf
 ├── ethos-u-kws.htm
 ├── ethos-u-kws.map
 └── sectors
      ├── images.txt
      └── kws
           ├── dram.bin
           └── itcm.bin
```

Where:

- `ethos-u-kws.axf`: The built application binary for the Keyword Spotting use case.

- `ethos-u-kws.map`: Information from building the application (e.g. libraries used, what was optimized, location of
    objects)

- `ethos-u-kws.htm`: Human readable file containing the call graph of application functions.

- `sectors/kws`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `sectors/images.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/\*\* folder.

### Add custom input

The application performs inference on audio data found in the folder, or an individual file, set by the CMake parameter `kws_FILE_PATH`.

To run the application with your own audio clips first create a folder to hold them and then copy the custom audio files
into this folder, for example:

```commandline
mkdir /tmp/custom_wavs

cp my_clip.wav /tmp/custom_wavs/
```

> **Note:** Clean the build directory before re-running the CMake command.

Next set `kws_FILE_PATH` to the location of this folder when building:

```commandline
cmake .. \
    -Dkws_FILE_PATH=/tmp/custom_wavs/ \
    -DUSE_CASE_BUILD=kws
```

The audio clips found in the `kws_FILE_PATH` folder will be picked up and automatically converted to C++ files during the
CMake configuration stage and then compiled into the application during the build phase for performing inference with.

The log from the configuration stage should tell you what audio clip directory path has been used:

```log
-- User option kws_FILE_PATH is set to /tmp/custom_wavs
-- Generating audio files from /tmp/custom_wavs
++ Converting my_clip.wav to my_clip.cc
++ Generating build/generated/kws/include/AudioClips.hpp
++ Generating build/generated/kws/src/AudioClips.cc
-- Defined build user options:
-- kws_FILE_PATH=/tmp/custom_wavs
```

After compiling, your custom inputs will have now replaced the default ones in the application.

> **Note:** The CMake parameter `kws_AUDIO_MIN_SAMPLES` determine the minimum number of input sample. When building the application,
if the size of the audio clips is less then `kws_AUDIO_MIN_SAMPLES` then it will be padded so that it does.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter `kws_MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using Ethos-U55, ensure your custom model has been run through the Vela compiler successfully before continuing. See [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

To run the application with a custom model you will need to provide a labels_<model_name>.txt file of labels
associated with the model. Each line of the file should correspond to one of the outputs in your model. See the provided
ds_cnn_labels.txt file for an example.

Then, you must set kws_MODEL_TFLITE_PATH to the location of the Vela processed model file and kws_LABELS_TXT_FILE
to the location of the associated labels file.

An example:

```commandline
cmake .. \
    -Dkws_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -Dkws_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DUSE_CASE_BUILD=kws
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `kws_MODEL_TFLITE_PATH` and labels text file pointed to by `kws_LABELS_TXT_FILE` will
be converted to C++ files during the CMake configuration stage and then compiled into the application for performing
inference with.

The log from the configuration stage should tell you what model path and labels file have been used:

```log
-- User option kws_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- User option kws_LABELS_TXT_FILE is set to <path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to\
custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build/generated/src/Labels.cc>
...
```

After compiling, your custom model will have now replaced the default one in the application.

## Setting up and running Ethos-U55 code sample

### Setting up the Ethos-U55 Fast Model

The FVP is available publicly from [Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For Ethos-U55 evaluation, please download the MPS3 version of the Arm® Corstone™-300 model that contains Ethos-U55 and
Cortex-M55. The model is currently only supported on Linux based machines. To install the FVP:

- Unpack the archive

- Run the install script in the extracted package

```commandline
./FVP_Corstone_SSE-300_Ethos-U55.sh
```

- Follow the instructions to install the FVP to your desired location

### Starting Fast Model simulation

Once completed the building step, application binary ethos-u-kws.axf can be found in the `build/bin` folder.
Assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be started by:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55
./bin/mps3-sse-300/ethos-u-kws.axf
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

After the application has started if `kws_FILE_PATH` pointed to a single file (or a folder containing a single input file)
the inference starts immediately. In case of multiple inputs choice, it outputs a menu and waits for the user input from telnet terminal:

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

1. “Classify next audio clip” menu option will run inference on the next in line voice clip from the collection of the
    compiled audio.

    > **Note:** Note that if the clip is over a certain length, the application will invoke multiple inference runs to cover the entire file.

2. “Classify audio clip at chosen index” menu option will run inference on the chosen audio clip.

    > **Note:** Please make sure to select audio clip index in the range of supplied audio clips during application build.
    By default, pre-built application has 4 files, indexes from 0 to 3.

3. “Run classification on all audio clips” menu option triggers sequential inference executions on all built-in voice
    samples.

4. “Show NN model info” menu option prints information about model data type, input and output tensor sizes:

    ```log
    INFO - uTFL version: 2.5.0
    INFO - Model info:
    INFO - Model INPUT tensors:
    INFO - 	tensor type is INT8
    INFO - 	tensor occupies 490 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1:   1
    INFO - 		2:  49
    INFO - 		3:  10
    INFO - Quant dimension: 0
    INFO - Scale[0] = 1.107164
    INFO - ZeroPoint[0] = 95
    INFO - Model OUTPUT tensors:
    INFO - 	tensor type is INT8
    INFO - 	tensor occupies 12 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1:  12
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.003906
    INFO - ZeroPoint[0] = -128
    INFO - Activation buffer (a.k.a tensor arena) size used: 72848
    INFO - Number of operators: 1
    INFO - 	Operator 0: ethos-u
    ```

5. “List audio clips” menu option prints a list of pair audio indexes - the original filenames embedded in the
    application:

    ```log
    [INFO] List of Files:
    [INFO] 0 => down.wav
    [INFO] 1 => right_left_up.wav
    [INFO] 2 => yes.wav
    [INFO] 3 => yes_no_go_stop.wav
    ```

### Running Keyword Spotting

Selecting the first option will run inference on the first file.

The following example illustrates application output for classification:

```logINFO - Running inference on audio clip 0 => down.wav
INFO - Inference 1/1
INFO - Final results:
INFO - Total number of inferences: 1
INFO - For timestamp: 0.000000 (inference #: 0); label: down, score: 0.996094; threshold: 0.900000
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 217385
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 82607
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 59608
INFO - NPU ACTIVE cycles: 680611
INFO - NPU IDLE cycles: 561
INFO - NPU total cycles: 681172
```

Each inference should take less than 30 seconds on most systems running Fast Model.
The profiling section of the log shows that for this inference:

- Ethos-U55's PMU report:

  - 681,172 total cycle: The number of NPU cycles

  - 680,611 active cycles: The number of NPU cycles that were used for computation

  - 561 idle cycles: number of cycles for which the NPU was idle

  - 217,385 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus.
    AXI0 is the bus where Ethos-U55 NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 82,607 write cycles: The number of AXI beats with write transactions to AXI0 bus.

  - 59,608 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus.
    AXI1 is the bus where Ethos-U55 NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
    the CPU model is not cycle-approximate or cycle-accurate.

The application prints the highest confidence score and the associated label from ds_cnn_labels.txt file.
