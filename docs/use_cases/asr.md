# Automatic Speech Recognition Code Sample

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
  - [Running Automatic Speech Recognition](#running-automatic-speech-recognition)
- [Automatic Speech Recognition processing information](#automatic-speech-recognition-processing-information)
  - [Preprocessing and feature extraction](#preprocessing-and-feature-extraction)
  - [Postprocessing](#postprocessing)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U55 Automatic Speech Recognition example.

Use case code could be found in [source/use_case/asr](../../source/use_case/asr]) directory.

### Preprocessing and feature extraction

The wav2letter automatic speech recognition model that is used with the Code Samples expects audio data to be
preprocessed in a specific way before performing an inference. This section aims to provide an overview of the feature
extraction process used.

First the audio data is normalized to the range (-1, 1).

> **Note:** Mel-frequency cepstral coefficients (MFCCs) are a common feature extracted from audio data and can be used as
>input for machine learning tasks like keyword spotting and speech recognition. See source/application/main/include/Mfcc.hpp
>for implementation details.

Next, a window of 512 audio samples is taken from the start of the audio clip. From these 512 samples we calculate 13
MFCC features.

The whole window is shifted to the right by 160 audio samples and 13 new MFCC features are calculated. This process of
shifting and calculating is repeated until enough audio samples to perform an inference have been processed. In total
this will be 296 windows that each have 13 MFCC features calculated for them.

After extracting MFCC features the first and second order derivatives of these features with respect to time are
calculated. These derivative features are then standardized and concatenated with the MFCC features (which also get
standardized). At this point the input tensor will have a shape of 296x39.

These extracted features are quantized, and an inference is performed.

![ASR preprocessing](../media/ASR_preprocessing.png)

For longer audio clips where multiple inferences need to be performed, then the initial starting position is offset by
(100*160) = 16000 audio samples. From this new starting point, MFCC and derivative features are calculated as before
until there is enough to perform another inference. Padding can be used if there are not enough audio samples for at
least 1 inference. This step is repeated until the whole audio clip has been processed. If there are not enough audio
samples for a final complete inference the MFCC features will be padded by repeating the last calculated feature until
an inference can be performed.

> **Note:** Parameters of the MFCC feature extraction such as window size, stride, number of features etc. all depend on
>what was used during model training. These values are specific to each model. If you switch to a different ASR model
>than the one supplied, then the feature extraction process could be completely different to the one currently implemented.

The amount of audio samples we offset by for long audio clips is specific to the included wav2letter model.

### Postprocessing

After performing an inference, the raw output need to be postprocessed to get a usable result.

The raw output from the model is a tensor of shape 148x29 where each row is a probability distribution over the possible
29 characters that can appear at each of the 148 time steps.

This wav2letter model is trained using context windows, this means that only certain parts of the output are usable
depending on the bit of the audio clip that is currently being processed.

If this is the first inference and multiple inferences are required, then ignore the final 49 rows of the output.
Similarly, if this is the final inference from multiple inferences then ignore the first 49 rows of the output. Finally,
if this inference is not the last or first inference then ignore the first and last 49 rows of the model output.

> **Note:** If the audio clip is small enough then the whole of the model output is usable and there is no need to throw
>away any of the output before continuing.

Once any rows have been removed the final processing can be done. To process the output, first the letter with the
highest probability at each time step is found. Next, any letters that are repeated multiple times in a row are removed
(e.g. [t, t, t, o, p, p] becomes [t, o, p]). Finally, the 29th blank token letter is removed from the output.

For the final output, the result from all inferences are combined before decoding. What you are left with is then
displayed to the console.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, Automatic Speech Recognition use case
adds:

- `asr_MODEL_TFLITE_PATH` - Path to the NN model file in TFLite format. Model will be processed and included into the
application axf file. The default value points to one of the delivered set of models. Note that the parameters
`asr_LABELS_TXT_FILE`,`TARGET_PLATFORM` and `ETHOS_U55_ENABLED` should be aligned with the chosen model, i.e.:
  - if `ETHOS_U55_ENABLED` is set to `On` or `1`, the NN model is assumed to be optimized. The model will naturally
fall back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
model in this case will result in a runtime error.

- `asr_FILE_PATH`:  Path to the directory containing audio files, or a path to single WAV file, to be used in the
    application. The default value points
    to the resources/asr/samples folder containing the delivered set of audio clips.

- `asr_LABELS_TXT_FILE`: Path to the labels' text file. The file is used to map letter class index to the text label.
    The default value points to the delivered labels.txt file inside the delivery package.

- `asr_AUDIO_RATE`: Input data sampling rate. Each audio file from asr_FILE_PATH is preprocessed during the build to
    match NN model input requirements. Default value is 16000.

- `asr_AUDIO_MONO`: If set to ON the audio data will be converted to mono. Default is ON.

- `asr_AUDIO_OFFSET`: Start loading audio data starting from this offset (in seconds). Default value is 0.

- `asr_AUDIO_DURATION`: Length of the audio data to be used in the application in seconds. Default is 0 meaning the
    whole audio file will be taken.

- `asr_AUDIO_MIN_SAMPLES`: Minimum number of samples required by the network model. If the audio clip is shorter than
    this number, it is padded with zeros. Default value is 16000.

- `asr_MODEL_SCORE_THRESHOLD`: Threshold value that must be applied to the inference results for a label to be
    deemed valid. Default is 0.5.

- `asr_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By default, it is set
    to 2MiB and should be enough for most models.

In order to build **ONLY** automatic speech recognition example application add to the `cmake` command line specified in
[Building](../documentation.md#Building) `-DUSE_CASE_BUILD=asr`.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target
>platform see [Building](../documentation.md#Building) section.

In order to build **only** the automatic speech recognition example, create a build directory and navigate inside:

```commandline
mkdir build_asr && cd build_asr
```

On Linux, execute the following command to build **only** Automatic Speech Recognition application to run on the
Ethos-U55 Fast Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=./scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=asr ..
```

For Windows, add `-G "MinGW Makefiles"`:

```commandline
cmake \
    -G "MinGW Makefiles" \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=./scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=asr ..
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
    -DUSE_CASE_BUILD=asr ..
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
    -DUSE_CASE_BUILD=asr ..
```

> **Note:** If building for different Ethos-U55 configurations, see
>[Configuring build for different Arm Ethos-U55 configurations](../sections/building.md#Configuring-build-for-different-Arm-Ethos-U55-configurations):

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
    -DUSE_CASE_BUILD=asr ..
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
    -DUSE_CASE_BUILD=asr ..
```

> **Note:** If re-building with changed parameters values, it is highly advised to clean the build directory and re-run
>the CMake command.

If the CMake command succeeded, build the application as follows:

```commandline
make -j4
```

For Windows, use `mingw32-make`.

Add `VERBOSE=1` to see compilation and link details.

Results of the build will be placed under `build/bin` folder:

```tree
bin
 ├── ethos-u-asr.axf
 ├── ethos-u-asr.htm
 ├── ethos-u-asr.map
 ├── images-asr.txt
 └── sectors
      └── asr
          ├── dram.bin
          └── itcm.bin
```

Where:

- `ethos-u-asr.axf`: The built application binary for the Automatic Speech Recognition use case.

- `ethos-u-asr.map`: Information from building the application (e.g. libraries used, what was optimized, location of
    objects)

- `ethos-u-asr.htm`: Human readable file containing the call graph of application functions.

- `sectors/`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `Images-asr.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/** folder.

### Add custom input

The application performs inference on audio data found in the folder, or an individual file, set by the CMake parameter
`asr_FILE_PATH`.

To run the application with your own audio clips first create a folder to hold them and then copy the custom audio clips
into this folder:

```commandline
mkdir /tmp/custom_wavs

cp my_clip.wav /tmp/custom_wavs/
```

> **Note:** Clean the build directory before re-running the CMake command.

Next set `asr_FILE_PATH` to the location of this folder when building:

```commandline
cmake \
    -Dasr_FILE_PATH=/tmp/custom_wavs/ \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DUSE_CASE_BUILD=asr \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake ..
```

For Windows, add `-G "MinGW Makefiles"` to the CMake command.

The audio clips found in the `asr_FILE_PATH` folder will be picked up and automatically converted to C++ files during the
CMake configuration stage and then compiled into the application during the build phase for performing inference with.

The log from the configuration stage should tell you what audio clip directory path has been used:

```log
-- User option asr_FILE_PATH is set to /tmp/custom_wavs
-- Generating audio files from /tmp/custom_wavs
++ Converting my_clip.wav to my_clip.cc
++ Generating build/generated/asr/include/InputFiles.hpp
++ Generating build/generated/asr/src/InputFiles.cc
-- Defined build user options:
-- asr_FILE_PATH=/tmp/custom_wavs
```

After compiling, your custom inputs will have now replaced the default ones in the application.

> **Note:** The CMake parameter asr_AUDIO_MIN_SAMPLES determine the minimum number of input sample. When building the
>application, if the size of the audio clips is less then asr_AUDIO_MIN_SAMPLES then it will be padded so that it does.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter MODEL_TFLITE_PATH.

> **Note:** If you want to run the model using Ethos-U55, ensure your custom model has been run through the Vela
>compiler successfully before continuing. See [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

To run the application with a custom model you will need to provide a labels_<model_name>.txt file of labels
associated with the model. Each line of the file should correspond to one of the outputs in your model. See the provided
labels_wav2letter.txt file for an example.

Then, you must set `asr_MODEL_TFLITE_PATH` to the location of the Vela processed model file and `asr_LABELS_TXT_FILE`to
the location of the associated labels file.

An example:

```commandline
cmake \
    -Dasr_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -Dasr_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake ..
```

For Windows, add `-G "MinGW Makefiles"` to the CMake command.

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `asr_MODEL_TFLITE_PATH` and labels text file pointed to by `asr_LABELS_TXT_FILE`
will be converted to C++ files during the CMake configuration stage and then compiled into the application for performing
inference with.

The log from the configuration stage should tell you what model path and labels file have been used:

```log
-- User option TARGET_PLATFORM is set to mps3
-- User option asr_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- User option asr_LABELS_TXT_FILE is set to <path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to\
custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build/generated/src/Labels.cc>
...
```

After compiling, your custom model will have now replaced the default one in the application.

## Setting-up and running Ethos-U55 Code Sample

### Setting up the Ethos-U55 Fast Model

The FVP is available publicly from [Arm Ecosystem FVP downloads
](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For Ethos-U55 evaluation, please download the MPS3 version of the Arm® Corstone™-300 model that contains Ethos-U55 and
Cortex-M55. The model is currently only supported on Linux based machines. To install the FVP:

- Unpack the archive

- Run the install script in the extracted package

```commandline
./FVP_Corstone_SSE-300_Ethos-U55.sh
```

- Follow the instructions to install the FVP to your desired location

### Starting Fast Model simulation

Once completed the building step, application binary ethos-u-asr.axf can be found in the `build/bin` folder.
Assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be started by:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55
./bin/mps3-sse-300/ethos-u-asr.axf
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

After the application has started if `asr_FILE_PATH` pointed to a single file (or a folder containing a single input file)
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

1. “Classify next audio clip” menu option will run inference on the next in line voice clip from the collection of the
    compiled audio.

    > **Note:** Note that if the clip is over a certain length, the application will invoke multiple inference runs to
    >cover the entire file.

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
    INFO - 	tensor occupies 11544 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1: 296
    INFO - 		2:  39
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.110316
    INFO - ZeroPoint[0] = -11
    INFO - Model OUTPUT tensors:
    INFO - 	tensor type is INT8
    INFO - 	tensor occupies 4292 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1:   1
    INFO - 		2: 148
    INFO - 		3:  29
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.003906
    INFO - ZeroPoint[0] = -128
    INFO - Activation buffer (a.k.a tensor arena) size used: 783168
    INFO - Number of operators: 1
    INFO - 	Operator 0: ethos-u
    ```

5. “List” menu option prints a list of pair audio clip indexes - the original filenames embedded in the application:

    ```log
    INFO - List of Files:
    INFO - 0 => anotherdoor.wav
    INFO - 1 => anotherengineer.wav
    INFO - 2 => itellyou.wav
    INFO - 3 => testingroutine.wav
    ```

### Running Automatic Speech Recognition

Please select the first menu option to execute Automatic Speech Recognition.

The following example illustrates application output:

```log
INFO - Running inference on audio clip 0 => another_door.wav
INFO - Inference 1/2
INFO - Inference 2/2
INFO - Final results:
INFO - Total number of inferences: 2
INFO - For timestamp: 0.000000 (inference #: 0); label: and he walked immediately out of th
INFO - For timestamp: 0.000000 (inference #: 1); label: e apartment by another door
INFO - Complete recognition: and he walked immediately out of the apartment by another door
INFO - Profile for Inference :
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED cycles: 6564262
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN cycles: 928889
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED cycles: 841712
INFO - NPU ACTIVE cycles: 28450696
INFO - NPU IDLE cycles: 476
INFO - NPU total cycles: 28451172
```

It could take several minutes to complete each inference (average time is 5-7 minutes), and on this audio clip multiple
inferences were required to cover the whole clip.

The profiling section of the log shows that for the first inference:

- Ethos-U55's PMU report:

  - 28,451,172 total cycle: The number of NPU cycles

  - 28,450,696 active cycles: number of NPU cycles that were used for computation

  - 476 idle cycles: number of cycles for which the NPU was idle

  - 6,564,262 AXI0 read cycles: The number of cycles the NPU spends to execute AXI0 read transactions.
    AXI0 is the bus where Ethos-U55 NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 928,889 AXI0 write cycles: The number of cycles the NPU spends to execute AXI0 write transactions.

  - 841,712 AXI1 read cycles: The number of cycles the NPU spends to execute AXI1 read transactions.
    AXI1 is the bus where Ethos-U55 NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
    the CPU model is not cycle-approximate or cycle-accurate.

The application prints the decoded output from each of the inference runs as well as the final combined result.
