# Anomaly Detection Code Sample

- [Anomaly Detection Code Sample](./ad.md#anomaly-detection-code-sample)
  - [Introduction](./ad.md#introduction)
    - [Preprocessing and feature extraction](./ad.md#preprocessing-and-feature-extraction)
    - [Postprocessing](./ad.md#postprocessing)
    - [Prerequisites](./ad.md#prerequisites)
  - [Building the code sample application from sources](./ad.md#building-the-code-sample-application-from-sources)
    - [Build options](./ad.md#build-options)
    - [Build process](./ad.md#build-process)
    - [Add custom input](./ad.md#add-custom-input)
    - [Add custom model](./ad.md#add-custom-model)
  - [Setting-up and running Ethos-U NPU Code Sample](./ad.md#setting-up-and-running-ethos_u-npu-code-sample)
    - [Setting up the Ethos-U NPU Fast Model](./ad.md#setting-up-the-ethos_u-npu-fast-model)
    - [Starting Fast Model simulation](./ad.md#starting-fast-model-simulation)
    - [Running Anomaly Detection](./ad.md#running-anomaly-detection)

## Introduction

This document describes the process of setting up and running the Arm® *Ethos™-U* NPU Anomaly Detection example.

Use-case code could be found in the following directory: [source/use_case/ad](../../source/use_case/ad).

### Preprocessing and feature extraction

The Anomaly Detection model that is used with the Code Samples and expects audio data to be preprocessed in a specific
way before performing an inference.

Therefore, this section provides an overview of the feature extraction process used.

First, the audio data is normalized to the range (`-1`, `1`).

Next, a window of 1024 audio samples is taken from the start of the audio clip. From these 1024 samples, we calculate 64
Log Mel Energies that form part of a Log Mel Spectrogram.

The window is shifted by 512 audio samples and another 64 Log Mel Energies are calculated. This is repeated until we
have 64 sets of Log Mel Energies.

This 64x64 matrix of values is then resized by a factor of two, resulting in a 32x32 matrix of values.

The average of the training dataset is then subtracted from this 32x32 matrix and an inference can now be performed.

We start this process again, but shift the start by 20\*512=10240 audio samples. This keeps repeating until enough
inferences have been performed to cover the whole audio clip.

### Postprocessing

Softmax is then applied to the result of each inference. Based on the machine ID of the wav clip being processed, we
look at a specific index in each output vector. An average of the negative value at this index across all the inferences
performed for the audio clip is taken.

If this average value is greater than a chosen threshold score, then the machine in the clip is not behaving
anomalously. If the score is lower than the threshold, then the machine in the clip is behaving anomalously.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, the Anomaly Detection use-case adds:

- `ad_MODEL_TFLITE_PATH` - Path to the NN model file in the `TFLite` format. The model is then processed and included in
  the application `axf` file. The default value points to one of the delivered set of models.

    Note that the parameters `ad_LABELS_TXT_FILE`, `TARGET_PLATFORM`, and `ETHOS_U_NPU_ENABLED` must be aligned with the
    chosen model. In other words:

  - If `ETHOS_U_NPU_ENABLED` is set to `On` or `1`, then the NN model is assumed to be optimized. The model naturally
    falls back to the Arm® *Cortex®-M* CPU if an unoptimized model is supplied.
  - if `ETHOS_U_NPU_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
    model in this case results in a runtime error.

- `ad_FILE_PATH`: Path to the directory containing audio files, or a path to single WAV file, to be used in the
  application. The default value points to the `resources/ad/samples` folder containing the delivered set of audio
  clips.

- `ad_AUDIO_RATE`: The input data sampling rate. Each audio file from `ad_FILE_PATH` is preprocessed during the build to
  match the NN model input requirements. The default value is `16000`.

- `ad_AUDIO_MONO`: If set to `ON`, then the audio data is converted to mono. The default value is `ON`.

- `ad_AUDIO_OFFSET`: begin loading the audio data, while starting from this offset amount, defined in seconds. The
  default value is set to `0`.

- `ad_AUDIO_DURATION`: Length of the audio data to be used in the application in seconds. Default is `0`, meaning that
  the whole audio file is used.

- `ad_AUDIO_MIN_SAMPLES`: Minimum number of samples required by the network model. If the audio clip is shorter than
  this number, then it is padded with zeros. The default value is `16000`.

- `ad_MODEL_SCORE_THRESHOLD`: Threshold value to be applied to average Softmax score over the clip, if larger than this
  value, then there is an anomaly.

- `ad_ACTIVATION_BUF_SZ`: The intermediate, or activation, buffer size reserved for the NN model. By default, it is set
  to 2MiB and is enough for most models.

In order to **ONLY** build the Anomaly Detection example application, add `-DUSE_CASE_BUILD=ad` to the `cmake` command
line that is specified in: [Building](../documentation.md#Building).

### Build process

> **Note:** This section describes the process for configuring the build for the `MPS3: SSE-300` for a different target
> platform. Additional information can be found at: [Building](../documentation.md#Building).

Create a build directory folder and then navigate inside using:

```commandline
mkdir build_ad && cd build_ad
```

On Linux, when providing only the mandatory arguments for CMake configuration, execute the following command to **only**
build the Anomaly Detection application to run on the *Ethos-U55* Fast Model:

```commandline
cmake ../ -DUSE_CASE_BUILD=ad
```

To configure a build that can be debugged using Arm DS, specify the build type as `Debug` and use the `Arm Compiler`
toolchain file, like so:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=ad
```

For additional information, please refer to:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple-platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Building for different Ethos-U NPU variants](../sections/building.md#building-for-different-ethos_u-npu-variants)

> **Note:** If re-building with changed parameters values, we recommend that you clean the build directory and then
> re-run the CMake command.

If the CMake command succeeded, build the application as follows:

```commandline
make -j4
```

To see compilation and link details, add `VERBOSE=1`.

Results of the build are placed under `build/bin` folder. For example:

```tree
bin
 ├── ethos-u-.axf
 ├── ethos-u-ad.htm
 ├── ethos-u-.map
 └── sectors
      ├── images.txt
      └── ad
          ├── ddr.bin
          └── itcm.bin
```

The bin folder contains the following files and folders:

- `ethos-u-ad.axf`: The built application binary for the Anomaly Detection use-case.

- `ethos-u-ad.map`: Information from building the application. For example, the libraries used, what was optimized, and
  the location of objects.

- `ethos-u-ad.htm`: Human readable file containing the call graph of application functions.

- `sectors/ad`: Folder containing the built application. is split into files for loading into different FPGA memory
  regions.

- `sectors/images.txt`: Tells the FPGA which memory regions to use for loading the binaries in the `sectors/..` folder.

### Add custom input

The application anomaly detection is set up to perform inferences on data found in the folder, or an individual file,
that is pointed to by the parameter `ad_FILE_PATH`.

To run the application with your own audio clips, first create a folder to hold them and then copy the custom clips into
the following folder:

```commandline
mkdir /tmp/custom_files

cp custom_id_00.wav /tmp/custom_files/
```

> **Note:** The data used for this example comes from
> [https://zenodo.org/record/3384388\#.X6GILFNKiqA](https://zenodo.org/record/3384388\#.X6GILFNKiqA) and the model
> included in this example is trained on the "Slider" part of the dataset.\
> The machine ID for the clip, so: `00`, `02`, `04`, `06`, comes from must be in the file name for the application to
> work.\
> The file name must have a pattern that matches. For example: `<any>_<text>_00_<here>.wav` if the audio was from machine
> ID `00`, or `<any>_<text>_02_<here>.wav` if it was from machine ID `02`, and so on.
>
> **Note:** Clean the build directory before re-running the CMake command.

Next, set `ad_FILE_PATH` to the location of the following folder when building:

```commandline
cmake .. \
    -Dad_FILE_PATH=/tmp/custom_files/ \
    -DUSE_CASE_BUILD=ad
```

The audio flies found in the `ad_FILE_PATH` folder are picked up and automatically converted to C++ files during the
CMake configuration stage. They are then compiled into the application during the build phase for performing inference
with.

The log from the configuration stage tells you what image directory path has been used:

```log
-- User option ad_FILE_PATH is set to /tmp/custom_files
```

After compiling, your custom inputs have now replaced the default ones in the application.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter ``ad_MODEL_TFLITE_PATH``.

> **Note:** If you want to run the model using an *Ethos-U*, ensure that your custom model has been successfully run
> through the Vela compiler *before* continuing. Please refer to this section for more help:
> [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

For example:

```commandline
cmake .. \
    -Dad_MODEL_TFLITE_PATH=<path/to/custom_ad_model_after_vela.tflite> \
    -DUSE_CASE_BUILD=ad
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `ad_MODEL_TFLITE_PATH` is converted to C++ files during the CMake configuration
stage and is then compiled into the application for performing inference with.

The log from the configuration stage tells you what model path has been used. For example:

```log
-- User option TARGET_PLATFORM is set to fastmodel
-- User option ad_MODEL_TFLITE_PATH is set to <path/to/custom_ad_model_after_vela.tflite>
...
-- Using <path/to/custom_ad_model_after_vela.tflite>
++ Converting custom_ad_model_after_vela.tflite to custom_ad_model_after_vela.tflite.cc
...
```

After compiling, your custom model has now replaced the default one in the application.

 >**Note:** To successfully run the model, the NPU must be enabled and the platform `TARGET_PLATFORM` is set to `mps3`
 >and `TARGET_SUBSYSTEM` is `SSE-300`.

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

> **Note:** The anomaly detection example does not come pre-built. Therefore, you must first follow the instructions in
> section three for building the application from source.

After building, and assuming the install location of the FVP was set to the `~/FVP_install_location`, the simulation can
be started by running:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 ./bin/ethos-u-ad.axf
```

A log output now appears on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This also launches a telnet window with the standard output of the sample application. It also includes error log
entries containing information about the pre-built application version, TensorFlow Lite Micro library version used, and
data types. The log also includes the input and output tensor sizes of the model compiled into the executable binary.

After the application has started, if `ad_FILE_PATH` points to a single file, or even a folder that contains a single
input file, then the inference starts immediately. If there are multiple inputs, it outputs a menu and then waits for
input from the user. For example:

```log
User input required
Enter option number from:

  1. Classify next audio signal
  2. Classify audio signal at chosen index
  3. Run classification on all audio signals
  4. Show NN model info
  5. List audio signals

  Choice:

```

What the preceding choices do:

1. Classify next audio clip: Runs a single inference on the next in line.

2. Classify audio clip at chosen index: Runs inference on the chosen audio clip.

    > **Note:** Please make sure to select audio clip index within the range of supplied audio clips during application
    > build. By default, the pre-built application has one file with index `0`.

3. Run ... on all: Triggers sequential inference executions on all built-in applications.

4. Show NN model info: Prints information about the model data type, input, and output, tensor sizes:

    ```log
    INFO - Model info:
    INFO - Model INPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 1024 bytes with dimensions
    INFO -    0:   1
    INFO -    1:  32
    INFO -    2:  32
    INFO -    3:   1
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.192437
    INFO - ZeroPoint[0] = 11
    INFO - Model OUTPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 8 bytes with dimensions
    INFO -    0:   1
    INFO -    1:   8
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.048891
    INFO - ZeroPoint[0] = -30
    INFO - Activation buffer (a.k.a tensor arena) size used: 321252
    INFO - Number of operators: 1
    INFO -  Operator 0: ethos-u
    ```

5. List: Prints a list of pair ... indexes. The original filenames are embedded in the application, like so:

    ```log
    INFO - List of Files:
    INFO - 0 =>; random_id_00_000000.wav
    ```

### Running Anomaly Detection

Please select the first menu option to execute the Anomaly Detection.

The following example illustrates the output of an application:

```log
INFO - Running inference on audio clip 4 => random_id_00_000000.wav
INFO - Inference 1/1
INFO - Average anomaly score is: -0.883147
INFO - Anomaly threshold is: -0.800000
INFO - Everything fine, no anomaly detected!
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 485194
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 138111
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 56245
INFO - NPU ACTIVE cycles: 967731
INFO - NPU IDLE cycles: 441
INFO - NPU TOTAL cycles: 968172
```

As multiple inferences must be run for one clip, it takes around a minute for all inferences to complete.

For the `random_id_00_000000.wav` clip, after averaging results across all inferences needed, the score is less than the
chosen anomaly threshold. Therefore, an anomaly was not detected with the machine in this clip.

The profiling section of the log shows that for each inference. For the last inference, the profiling reports:

- *Ethos-U* PMU report:

  - 968,172 total cycle: The number of NPU cycles

  - 967,731 active cycles: number of NPU cycles that were used for computation

  - 441 idle cycles: number of cycles for which the NPU was idle

  - 48,5194 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where
    *Ethos-U* NPU reads and writes to the computation buffers, activation buf, or tensor arenas.

  - 138,111 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 56,245 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where
   *Ethos-U* NPU reads the model. So, read-only.

- For FPGA platforms, a CPU cycle count can also be enabled. However, do not use cycle counters for FVP, as the CPU
  model is not cycle-approximate or cycle-accurate.
