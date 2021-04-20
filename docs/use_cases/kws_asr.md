# Keyword Spotting and Automatic Speech Recognition Code Sample

- [Introduction](#introduction)
  - [Prerequisites](#prerequisites)
- [Building the code sample application from sources](#building-the-code-sample-application-from-sources)
  - [Build options](#build-options)
  - [Build process](#build-process)
  - [Add custom input](#add-custom-input)
  - [Add custom model](#add-custom-model)
- [Setting-up and running Ethos-U55 Code Samples](#setting-up-and-running-ethos-u55-code-samples)
  - [Setting up the Ethos-U55 Fast Model](#setting-up-the-ethos-u55-fast-model)
  - [Starting Fast Model simulation](#starting-fast-model-simulation)
  - [Running Keyword Spotting and Automatic Speech Recognition](#running-keyword-spotting-and-automatic-speech-recognition)
- [Keyword Spotting and Automatic Speech Recognition processing information](#keyword-spotting-and-automatic-speech-recognition-processing-information)
  - [Preprocessing and feature extraction](#preprocessing-and-feature-extraction)
    - [Keyword Spotting Preprocessing](#keyword-spotting-preprocessing)
    - [Automatic Speech Recognition Preprocessing](#automatic-speech-recognition-preprocessing)
  - [Postprocessing](#postprocessing)

## Introduction

This document describes the process of setting up and running an example of sequential execution of the Keyword Spotting
and Automatic Speech Recognition models on Cortex-M CPU and Ethos-U NPU.

The Keyword Spotting and Automatic Speech Recognition example demonstrates how to run multiple models sequentially. A
Keyword Spotting model is first run on the CPU and if a set keyword is detected then an Automatic Speech Recognition
model is run on Ethos-U55 on the remaining audio.
Tensor arena memory region is reused between models to optimise application memory footprint.

"Yes" key word is used to trigger full command recognition following the key word.
Use case code could be found in [source/use_case/kws_asr](../../source/use_case/kws_asr]) directory.

### Preprocessing and feature extraction

In this use-case there are 2 different models being used with different requirements for preprocessing. As such each
preprocessing process is detailed below. Note that Automatic Speech Recognition only occurs if a keyword is detected in
the audio clip.

By default the KWS model is run purely on CPU and not on the Ethos-U55.

#### Keyword Spotting Preprocessing

The DS-CNN keyword spotting model that is used with the Code Samples expects audio data to be preprocessed in
a specific way before performing an inference. This section aims to provide an overview of the feature extraction
process used.

First the audio data is normalized to the range (-1, 1).

> **Note:** Mel-frequency cepstral coefficients (MFCCs) are a common feature extracted from audio data and can be used as input for machine learning tasks like keyword spotting and speech recognition. See source/application/main/include/Mfcc.hpp for implementation details.

Next, a window of 640 audio samples is taken from the start of the audio clip. From these 640 samples we calculate 10
MFCC features.

The whole window is shifted to the right by 320 audio samples and 10 new MFCC features are calculated. This process of
shifting and calculating is repeated until the end of the 16000 audio samples needed to perform an inference is reached.
In total this will be 49 windows that each have 10 MFCC features calculated for them, giving an input tensor of shape
49x10.

These extracted features are quantized, and an inference is performed.

If the audio clip is longer than 16000 audio samples then the initial starting position is offset by 16000/2 = 8000
audio samples. From this new starting point, MFCC features for the next 16000 audio samples are calculated and another
inference is performed (i.e. do an inference for samples 8000-24000).

> **Note:** Parameters of the MFCC feature extraction such as window size, stride, number of features etc. all depend on what was used during model training. These values are specific to each model and if you try a different keyword spotting model that uses MFCC input then values are likely to need changing to match the new model.

In addition, MFCC feature extraction methods can vary slightly with different normalization methods or scaling etc. being used.

#### Automatic Speech Recognition Preprocessing

The wav2letter automatic speech recognition model that is used with the Code Samples expects audio data to be
preprocessed in a specific way before performing an inference. This section aims to provide an overview of the feature
extraction process used.

First the audio data is normalized to the range (-1, 1).

> **Note:** Mel-frequency cepstral coefficients (MFCCs) are a common feature extracted from audio data and can be used as input for machine learning tasks like keyword spotting and speech recognition. See source/application/main/include/Mfcc.hpp for implementation details.

Next, a window of 512 audio samples is taken from the start of the audio clip. From these 512 samples we calculate 13
MFCC features.

The whole window is shifted to the right by 160 audio samples and 13 new MFCC features are calculated. This process of
shifting and calculating is repeated until enough audio samples to perform an inference have been processed. In total
this will be 296 windows that each have 13 MFCC features calculated for them.

After extracting MFCC features the first and second order derivatives of these features with respect to time are
calculated. These derivative features are then standardized and concatenated with the MFCC features (which also get
standardized). At this point the input tensor will have a shape of 296x39.

These extracted features are quantized, and an inference is performed.

For longer audio clips where multiple inferences need to be performed, then the initial starting position is offset by
(100\*160) = 16000 audio samples. From this new starting point, MFCC and derivative features are calculated as before
until there is enough to perform another inference. Padding can be used if there are not enough audio samples for at
least 1 inference. This step is repeated until the whole audio clip has been processed. If there are not enough audio
samples for a final complete inference the MFCC features will be padded by repeating the last calculated feature until
an inference can be performed.

> **Note:** Parameters of the MFCC feature extraction such as window size, stride, number of features etc. all depend on what was used during model training. These values are specific to each model. If you switch to a different ASR model than the one supplied, then the feature extraction process could be completely different to the one currently implemented.

The amount of audio samples we offset by for long audio clips is specific to the included wav2letter model.

### Postprocessing

If a keyword is detected then the ASR process is run and the raw output of that inference needs to be postprocessed to
get a usable result.

The raw output from the model is a tensor of shape 148x29 where each row is a probability distribution over the possible
29 characters that can appear at each of the 148 time steps.

This wav2letter model is trained using context windows, this means that only certain parts of the output are usable
depending on the bit of the audio clip that is currently being processed.

If this is the first inference and multiple inferences are required, then ignore the final 49 rows of the output.
Similarly, if this is the final inference from multiple inferences then ignore the first 49 rows of the output. Finally,
if this inference is not the last or first inference then ignore the first and last 49 rows of the model output.

> **Note:** If the audio clip is small enough then the whole of the model output is usable and there is no need to throw away any of the output before continuing.

Once any rows have been removed the final processing can be done. To process the output, first the letter with the
highest probability at each time step is found. Next, any letters that are repeated multiple times in a row are removed
(e.g. [t, t, t, o, p, p] becomes [t, o, p]). Finally, the 29^th^ blank token letter is removed from the output.

For the final output, the result from all inferences are combined before decoding. What you are left with is then
displayed to the console.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, Keyword Spotting and Automatic Speech
Recognition use case adds:

- `kws_asr_MODEL_TFLITE_PATH_ASR` and `kws_asr_MODEL_TFLITE_PATH_KWS`: Path to the NN model files in TFLite format.
    Models will be processed and included into the application axf file. The default value points to one of the delivered set of models.
    Note that the parameters `kws_asr_LABELS_TXT_FILE_KWS`, `kws_asr_LABELS_TXT_FILE_ASR`,`TARGET_PLATFORM` and `ETHOS_U55_ENABLED`
    should be aligned with the chosen model, i.e:
  - if `ETHOS_U55_ENABLED` is set to `On` or `1`, the NN model is assumed to be optimized. The model will naturally fall back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized model in this case will result in a runtime error.

- `kws_asr_FILE_PATH`: Path to the directory containing audio files, or a path to single WAV file, to be used in the application. The default value
    points to the resources/kws_asr/samples folder containing the delivered set of audio clips.

- `kws_asr_LABELS_TXT_FILE_KWS` and `kws_asr_LABELS_TXT_FILE_ASR`: Path respectively to keyword spotting labels' and the automatic speech
    recognition labels' text files. The file is used to map
    letter class index to the text label. The default value points to the delivered labels.txt file inside the delivery
    package.

- `kws_asr_AUDIO_RATE`: Input data sampling rate. Each audio file from kws_asr_FILE_PATH is preprocessed during the
    build to match NN model input requirements. Default value is 16000.

- `kws_asr_AUDIO_MONO`: If set to ON the audio data will be converted to mono. Default is ON.

- `kws_asr_AUDIO_OFFSET`: Start loading audio data starting from this offset (in seconds). Default value is 0.

- `kws_asr_AUDIO_DURATION`: Length of the audio data to be used in the application in seconds. Default is 0 meaning
    the whole audio file will be taken.

- `kws_asr_AUDIO_MIN_SAMPLES`: Minimum number of samples required by the network model. If the audio clip is shorter
    than this number, it is padded with zeros. Default value is 16000.

- `kws_asr_MODEL_SCORE_THRESHOLD_KWS`: Threshold value that must be applied to the keyword spotting inference
    results for a label to be deemed valid. Default is 0.9.

- `kws_asr_MODEL_SCORE_THRESHOLD_ASR`: Threshold value that must be applied to the automatic speech recognition
    inference results for a label to be deemed valid. Default is 0.5.

- `kws_asr_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By default, it is
    set to 2MiB and should be enough for most models.

In order to build **ONLY** Keyword Spotting and Automatic Speech
Recognition example application add to the `cmake` command line specified in [Building](../documentation.md#Building) `-DUSE_CASE_BUILD=kws_asr`.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target platform see [Building](../documentation.md#Building).

Create a build directory and navigate inside:

```commandline
mkdir build_kws_asr && cd build_kws_asr
```

On Linux, execute the following command to build the application to run on the Ethos-U55 Fast Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=./scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=kws_asr ..
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
    -DUSE_CASE_BUILD=kws_asr ..
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
    -DUSE_CASE_BUILD=kws_asr ..
```

> **Note:** If building for different Ethos-U55 configurations, see [Configuring build for different Arm Ethos-U55 configurations](../sections/building.md#Configuring-build-for-different-Arm-Ethos-U55-configurations):

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
    -DUSE_CASE_BUILD=kws_asr ..
```

Also, `CMSIS_SRC_PATH` parameter can be used to override the CMSIS sources used for compilation used by TensorFlow by default. For example, to use the CMSIS sources fetched by the ethos-u helper script, we can use:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DTENSORFLOW_SRC_PATH=../ethos-u/core_software/tensorflow \
    -DETHOS_U55_DRIVER_SRC_PATH=../ethos-u/core_software/core_driver \
    -DCMSIS_SRC_PATH=../ethos-u/core_software/cmsis \
    -DUSE_CASE_BUILD=kws_asr ..
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
 ├── ethos-u-kws_asr.axf
 ├── ethos-u-kws_asr.htm
 ├── ethos-u-kws_asr.map
 ├── images-kws_asr.txt
 └── sectors
      └── kws_asr
           ├── dram.bin
           └── itcm.bin
```

Where:

- `ethos-u-kws_asr.axf`: The built application binary for the Keyword Spotting and Automatic Speech Recognition use
    case.

- `ethos-u-kws_asr.map`: Information from building the application (e.g. libraries used, what was optimized, location
    of objects)

- `ethos-u-kws_asr.htm`: Human readable file containing the call graph of application functions.

- `sectors/`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `Images-kws_asr.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/** folder.

### Add custom input

The application performs inference on data found in the folder set by the CMake parameter `kws_asr_FILE_PATH`.

To run the application with your own audio clips first create a folder to hold them and then copy the custom files into
this folder:

```commandline
mkdir /tmp/custom_files

cp custom_audio1.wav /tmp/custom_files/
```

> **Note:** Clean the build directory before re-running the CMake command.

Next set `kws_asr_FILE_PATH` to the location of this folder when building:

```commandline
cmake \
    -Dkws_asr_FILE_PATH=/tmp/custom_files/ \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=kws_asr- ..
```

The files found in the `kws_asr_FILE_PATH` folder will be picked up and automatically converted to C++ files during the
CMake configuration stage and then compiled into the application during the build phase for performing inference with.

The log from the configuration stage should tell you what directory path has been used:

```log
-- User option kws_asr_FILE_PATH is set to /tmp/custom_files
```

After compiling, your custom inputs will have now replaced the default ones in the application.

### Add custom model

The application performs KWS inference using the model pointed to by the CMake parameter `kws_asr_MODEL_TFLITE_PATH_KWS` and
ASR inference using the model pointed to by the CMake parameter `kws_asr_MODEL_TFLITE_PATH_ASR`.

This section assumes you wish to change the existing ASR model to a custom one. If instead you wish to change the KWS
model then the instructions will be the same except ASR will change to KWS.

> **Note:** If you want to run the model using Ethos-U55, ensure your custom model has been run through the Vela compiler successfully before continuing. See [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

To run the application with a custom model you will need to provide a labels_<model_name>.txt file of labels
associated with the model. Each line of the file should correspond to one of the outputs in your model. See the provided
labels_wav2letter.txt file for an example.

Then, you must set `kws_asr_MODEL_TFLITE_PATH_ASR` to the location of the Vela processed model file and
`kws_asr_LABELS_TXT_FILE_ASR` to the location of the associated labels file.

An example:

```commandline
cmake \
    -Dkws_asr_MODEL_TFLITE_PATH_ASR=<path/to/custom_asr_model_after_vela.tflite> \
    -Dkws_asr_LABELS_TXT_FILE_ASR=<path/to/labels_custom_model.txt> \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=kws_asr ..
```

For Windows, add `-G "MinGW Makefiles"` to the CMake command.

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model files pointed to by `kws_asr_MODEL_TFLITE_PATH_KWS` and `kws_asr_MODEL_TFLITE_PATH_ASR`, labels text files pointed to by `kws_asr_LABELS_TXT_FILE_KWS` and `kws_asr_LABELS_TXT_FILE_ASR`
will be converted to C++ files during the CMake configuration stage and then compiled into the application for
performing inference with.

The log from the configuration stage should tell you what model path and labels file have been used:

```log
-- User option TARGET_PLATFORM is set to mps3
-- User option kws_asr_MODEL_TFLITE_PATH_ASR is set to <path/to/custom_asr_model_after_vela.tflite>
...
-- User option kws_asr_LABELS_TXT_FILE_ASR is set to <path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_asr_model_after_vela.tflite>
++ Converting custom_asr_model_after_vela.tflite to\
custom_asr_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to Labels_wav2letter
...
```

After compiling, your custom model will have now replaced the default one in the application.

## Setting-up and running Ethos-U55 Code Samples

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

Once completed the building step, application binary ethos-u-kws_asr.axf can be found in the `build/bin` folder.
Assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be started by:

```commandline
$ ~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55
./bin/mps3-sse-300/ethos-u-kws_asr.axf
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

After the application has started if `kws_asr_FILE_PATH` pointed to a single file (or a folder containing a single input file)
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

1. “Classify next audio clip” menu option will run single inference on the next included file.

2. “Classify audio clip at chosen index” menu option will run inference on the chosen audio clip.

    > **Note:** Please make sure to select audio clip index in the range of supplied audio clips during application build.

3. “Run ... on all” menu option triggers sequential inference executions on all built-in files.

4. “Show NN model info” menu option prints information about model data type, input and output tensor sizes:

    ```log
    INFO - uTFL version: 2.5.0
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
    INFO - Activation buffer (a.k.a tensor arena) size used: 123616
    INFO - Number of operators: 16
    INFO - 	Operator 0: RESHAPE
    INFO - 	Operator 1: CONV_2D
    INFO - 	Operator 2: DEPTHWISE_CONV_2D
    INFO - 	Operator 3: CONV_2D
    INFO - 	Operator 4: DEPTHWISE_CONV_2D
    INFO - 	Operator 5: CONV_2D
    INFO - 	Operator 6: DEPTHWISE_CONV_2D
    INFO - 	Operator 7: CONV_2D
    INFO - 	Operator 8: DEPTHWISE_CONV_2D
    INFO - 	Operator 9: CONV_2D
    INFO - 	Operator 10: DEPTHWISE_CONV_2D
    INFO - 	Operator 11: CONV_2D
    INFO - 	Operator 12: AVERAGE_POOL_2D
    INFO - 	Operator 13: RESHAPE
    INFO - 	Operator 14: FULLY_CONNECTED
    INFO - 	Operator 15: SOFTMAX
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
    INFO - Activation buffer (a.k.a tensor arena) size used: 809808
    INFO - Number of operators: 1
    INFO - 	Operator 0: ethos-u
    ```

5. “List” menu option prints a list of pair ... indexes - the original filenames embedded in the application:

    ```log
    [INFO] List of Files:
    [INFO] 0 => yes_no_go_stop.wav
    ```

### Running Keyword Spotting and Automatic Speech Recognition

Please select the first menu option to execute Keyword Spotting and Automatic Speech Recognition.

The following example illustrates application output:

```log
INFO - KWS audio data window size 16000
INFO - Running KWS inference on audio clip 0 => yes_no_go_stop.wav
INFO - Inference 1/7
INFO - For timestamp: 0.000000 (inference #: 0); threshold: 0.900000
INFO -          label @ 0: yes, score: 0.996094
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 217385
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 82607
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 59608
INFO - NPU ACTIVE cycles: 680611
INFO - NPU IDLE cycles: 561
INFO - NPU total cycles: 681172
INFO - Keyword spotted
INFO - Inference 1/2
INFO - Inference 2/2
INFO - Result for inf 0: no gow
INFO - Result for inf 1:  stoppe
INFO - Final result: no gow stoppe
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 13520864
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 2841970
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 2717670
INFO - NPU ACTIVE cycles: 28909309
INFO - NPU IDLE cycles: 863
INFO - NPU total cycles: 28910172
```

It could take several minutes to complete one inference run (average time is 2-3 minutes).

Using the input “yes_no_go_stop.wav”, the log shows inference results for the KWS operation first, detecting the
trigger word “yes“ with the stated probability score (in this case 0.99). After this, the ASR inference is run,
printing the words recognized from the input sample.

The profiling section of the log shows that for the ASR inference:

- Ethos-U55's PMU report:

  - 28,910,172 total cycle: The number of NPU cycles

  - 28,909,309 active cycles: number of NPU cycles that were used for computation

  - 863 idle cycles: number of cycles for which the NPU was idle

  - 13,520,864 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus.
    AXI0 is the bus where Ethos-U55 NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 2,841,970 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 2,717,670 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus.
    AXI1 is the bus where Ethos-U55 NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
the CPU model is not cycle-approximate or cycle-accurate.

    Note that in this example the KWS inference does not use the Ethos-U55 and is run purely on CPU, therefore 0 Active
    NPU cycles is shown.
