# Noise Reduction Code Sample

- [Noise Reduction Code Sample](./noise_reduction.md#noise-reduction-code-sample)
  - [Introduction](./noise_reduction.md#introduction)
  - [How the default neural network model works](./noise_reduction.md#how-the-default-neural-network-model-works)
  - [Post-processing](./noise_reduction.md#post_processing)
    - [Dumping of memory contents from the Fixed Virtual Platform](./noise_reduction.md#dumping-of-memory-contents-from-the-fixed-virtual-platform)
    - [Dumping post processed results for all inferences](./noise_reduction.md#dumping-post_processed-results-for-all-inferences)
  - [Prerequisites](./noise_reduction.md#prerequisites)
  - [Building the code sample application from sources](./noise_reduction.md#building-the-code-sample-application-from-sources)
    - [Build options](./noise_reduction.md#build-options)
    - [Build process](./noise_reduction.md#build-process)
    - [Add custom input](./noise_reduction.md#add-custom-input)
    - [Add custom model](./noise_reduction.md#add-custom-model)
  - [Setting up and running Ethos-U NPU code sample](./noise_reduction.md#setting-up-and-running-ethos_u-npu-code-sample)
    - [Setting up the Ethos-U NPU Fast Model](./noise_reduction.md#setting-up-the-ethos_u-npu-fast-model)
    - [Starting Fast Model simulation](./noise_reduction.md#starting-fast-model-simulation)
    - [Running Noise Reduction](./noise_reduction.md#running-noise-reduction)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U NPU Noise Reduction
example.

Use case code is stored in the following directory: [source/use_case/noise_reduction](../../source/use_case/noise_reduction).

## How the default neural network model works

Instead of replicating a "noisy audio in" and "clean audio out" problem, a simpler version is
defined. We use different frequency bands for the audio (22 in the original paper
[RNNoise: Learning Noise Suppression](https://jmvalin.ca/demo/rnnoise/)). It is based on a scale like the "Mel scale"
or "Bark scale" and calculates the energies for each band. Using this type of scale, the bands get
divided up and the result is based on what is important to the human ear.

When we have a noisy audio clip, the model takes the energy levels of these different bands as
input. The model then tries to predict a value (called a gain), to apply to each frequency band. It
is expected that applying this gain to each band brings the audio back to what a "clean" audio
sample would have been like. It is like a 22-band equalizer, where we quickly adjust the level of
each band so that the noise is removed. However, the signal, or speech, still passes through.

In addition to the 22 band values calculated, the input features also include:

- First and second derivatives of the first 6 coefficients,
- The pitch period (1/frequency),
- The pitch gain for six bands,
- A value used to detect if speech is occurring.

This provides 42 feature inputs, `22 + 6 + 6 + 1 + 6 + 1 = 42`, and the model produces `22` (gain
values) outputs.

> **Note:** The model also has a second output that predicts if speech is occurring in the given
> sample.

The pre-processing works in a windowed fashion, on 20ms of the audio clip at a time, and the stride
is 10ms. So, for example, if we provide one second of audio this gives us `1000ms/10ms = 100` windows of
features and, therefore, an input shape of `100x42` to the model. The output shape of the model is
then `100x22`, representing the gain values to apply to each of the 100 windows.

These output gain values can then be applied to each corresponding window of the noisy audio clip,
producing a cleaner output.

For more information please refer to the original paper:
[A Hybrid DSP/Deep Learning Approach to Real-Time Full-Band Speech Enhancement](https://arxiv.org/pdf/1709.08243.pdf)

## Post-processing

After each inference the output of the model is passed to post-processing code which uses the gain values the model
produced to generate audio with the noise removed from it.

For you to verify the outputs of the model after post-processing, you will have manually use an [offline script](../../scripts/py/rnnoise_dump_extractor.py)
to convert the post-processed outputs into a wav file.
This offline script takes a dump file as the input and saves the denoised WAV file to disk. The following is an example
of how to call the script from the command line after running the use-case and
[selecting to dump memory contents](./noise_reduction.md#dumping-post_processed-results-for-all-inferences).

```commandline
python scripts/py/rnnoise_dump_extractor.py --dump_file <path_to_dump_file.bin> --output_dir <path_to_output_folder>
```

The application for this use case has been written to dump the post-processed output to the address pointed to by
the CMake parameter `noise_reduction_MEM_DUMP_BASE_ADDR`. The default value is set to `0x80000000`.

### Dumping of memory contents from the Fixed Virtual Platform

The fixed virtual platform supports dumping of memory contents to a file. This can be done by
specifying command-line arguments when starting the FVP executable. For example, the argument:

```commandline
$ FVP_Corstone_SSE-300_Ethos-U55 -a ./bin/ethos-u-noise_reduction.axf \
    --dump cpu0=output.bin@Memory:0x80000000,0x100000
```

Dumps 1 MiB worth of data from address `0x80000000` to the file `output.bin`.

### Dumping post-processed results for all inferences

The Noise Reduction application uses the memory address specified by
`noise_reduction_MEM_DUMP_BASE_ADDR` as a buffer to store post-processed results from all inferences.
The maximum size of this buffer is set by the parameter
`noise_reduction_MEM_DUMP_LEN` which defaults to 1 MiB.

Logging information is generated for every inference run performed. Each line corresponds to the post-processed
result of that inference being written to a certain location in memory.

For example:

```log
INFO - Audio Clip dump header info (20 bytes) written to 0x80000000
INFO - Inference 1/136
INFO - Copied 960 bytes to 0x80000014
...
INFO - Inference 136/136
INFO - Copied 960 bytes to 0x8001fa54
```

In the preceding output we can see that it starts at the default address of
`0x80000000` where some header information is dumped. Then, after the first inference 960 bytes
(480 INT16 values) are written to the first address after the dumped header `0x80000014`.
Each inference afterward will then write another 960 bytes to the next address and so on until all inferences
are complete.

When consolidating all inference outputs for an entire audio clip, the application output should report:

```log
INFO - Output memory dump of 130580 bytes written at address 0x80000000
```

The application output log states that there are 130580 bytes worth of valid data ready to be read
from `0x80000000`. If the FVP was started with the `--dump` option, then the output file is created
when the FVP instance exits.

## Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, keyword spotting use
case adds:

- `noise_reduction_MODEL_TFLITE_PATH` - The path to the NN model file in *TFLite* format. The model
  is processed and is included in the application axf file. The default value points to one of the
  delivered set of models. Note that the parameter
  `ETHOS_U_NPU_ENABLED` must be aligned with the chosen model. Therefore:
  - if `ETHOS_U_NPU_ENABLED` is set to `On` or `1`, we assume that the NN model is optimized. The
    model naturally falls back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U_NPU_ENABLED` is set to `Off` or `0`, then we assume that the NN model is unoptimized.
    In this case, supplying an optimized model results in a runtime error.

- `noise_reduction_FILE_PATH`: The path to the directory containing WAV files, or a path to single
  WAV file, to be used in the application. The default value points to the
  `resources/noise_reduction/samples` folder containing the delivered set of audio clips.

- `noise_reduction_AUDIO_RATE`: The input data sampling rate. Each audio file from `noise_reduction_FILE_PATH` is
  preprocessed during the build to match the NN model input requirements. The default value is `48000`.

- `noise_reduction_AUDIO_MONO`: If set to `ON`, then the audio data is converted to mono. The default value is `ON`.

- `noise_reduction_AUDIO_OFFSET`: Begins loading audio data and starts from this specified offset, defined in seconds.
  The default value is set to `0`.

- `noise_reduction_AUDIO_DURATION`: The length of the audio data to be used in the application in seconds.
  The default is `0`, meaning that the whole audio file is used.

- `noise_reduction_AUDIO_MIN_SAMPLES`: Minimum number of samples required by the network model. If the audio clip is shorter than
  this number, then it is padded with zeros. The default value is `480`.

- `noise_reduction_ACTIVATION_BUF_SZ`: The intermediate, or activation, buffer size reserved for the
  neural network model. By default, it is set to 2MiB.

To **ONLY** build a `noise_reduction` example application, add `-DUSE_CASE_BUILD=noise_reduction`
  (as specified in [Building](../documentation.md#Building) to the `cmake` command line).

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300`. To
> configure a different target platform, please see the [Building](../documentation.md#Building)
> section.

To **only** build the `noise_reduction` example, create a build directory, and then navigate inside.
For example:

```commandline
mkdir build_noise_reduction && cd build_noise_reduction
```

On Linux, when providing only the mandatory arguments for CMake configuration, use the following
command to build the Noise Reduction application to run on the *Ethos-U55* Fast Model:

```commandline
cmake ../ -DUSE_CASE_BUILD=noise_reduction
```

To configure a build that can be debugged using Arm DS, we specify the build type as `Debug` and use
the `Arm Compiler` toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=noise_reduction
```

For more notes, please refer to:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple-platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Building for different Ethos-U variants](../sections/building.md#building-for-different-ethos_u-npu-variants)

> **Note:** If you are rebuilding with changed parameters values, it is highly advised that you
> clean the build directory and rerun the CMake command.

If the CMake command is successful, then build the application as follows:

```commandline
make -j4
```

> **Note:** To see compilation and link details, add `VERBOSE=1`.

The build results are placed under the `build/bin` folder. For example:

```tree
bin
 ├── ethos-u-noise_reduction.axf
 ├── ethos-u-noise_reduction.htm
 ├── ethos-u-noise_reduction.map
 ├── images-noise_reduction.txt
 └── sectors
      └── noise_reduction
           ├── dram.bin
           └── itcm.bin
```

Based on the preceding output, the files contain the following information:

- `ethos-u-noise_reduction.axf`: The built application binary for the noise reduction use case.

- `ethos-u-noise_reduction.map`: Information from building the application (for example. The
  libraries used, what was optimized, and location of objects).

- `ethos-u-noise_reduction.htm`: A human readable file containing the call graph of application
  functions.

- `sectors/`: This folder contains the built application, which is split into files for loading into
  different FPGA memory regions.

- `Images-noise_reduction.txt`: Tells the FPGA which memory regions to use for loading the binaries
  in the `sectors/...` folder.

### Add custom input

To run with inputs different to the ones supplied, the parameter `noise_reduction_FILE_PATH` can be
pointed to a WAV file, or a directory containing WAV files. Once you have a directory with WAV files,
run the following command:

```commandline
cmake .. \
    -DUSE_CASE_BUILD=noise_reduction \
    -Dnoise_reduction_FILE_PATH=/path/to/custom/wav_files
```

### Add custom model

The application performs inference using the model pointed to by the CMake parameter
`noise_reduction_MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using *Ethos-U* ensure that your custom model has been
> run through the Vela compiler successfully before continuing.

For further information: [Optimize model with Vela compiler](../sections/building.md#optimize-custom-model-with-vela-compiler).

An example:

```commandline
cmake .. \
    -Dnoise_reduction_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -DUSE_CASE_BUILD=noise_reduction
```

> **Note** Changing the neural network model often also requires the pre-processing implementation
> to be changed. Please refer to:
> [How the default neural network model works](./noise_reduction.md#how-the-default-neural-network-model-works).
>
> **Note:** Before re-running the CMake command, clean the build directory.

The `.tflite` model file, which is pointed to by `noise_reduction_MODEL_TFLITE_PATH`, is converted
to C++ files during the CMake configuration stage. It is then compiled into the application for
performing inference with.

To see which model path was used, inspect the configuration stage log:

```log
-- User option noise_reduction_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build/generated/src/Labels.cc>
...
```

After compiling, your custom model replaces the default one in the application.

## Setting up and running Ethos-U NPU code sample

### Setting up the Ethos-U NPU Fast Model

The FVP is available publicly from [Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For the *Ethos-U* evaluation, please download the MPS3 based version of the Arm® *Corstone™-300* model that contains *Cortex-M55*
and offers a choice of the *Ethos-U55* and *Ethos-U65* processors.

To install the FVP:

- Unpack the archive,

- Run the install script in the extracted package:

```commandline
$./FVP_Corstone_SSE-300.sh
```

- Follow the instructions to install the FVP to your required location.

### Starting Fast Model simulation

Once the building step has completed, the application binary `ethos-u-noise_reduction.axf` can be
found in the `build/bin` folder. Assuming the install location of the FVP was set to
`~/FVP_install_location`, start the simulation with the following command:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 ./bin/mps3-sse-300/ethos-u-noise_reduction.axf
```

A log output then appears on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This also launches a telnet window with the standard output of the sample application. It also
includes error log entries containing information about the pre-built application version,
TensorFlow Lite Micro library version used, and the data type. As well as the input and output
tensor sizes of the model that was compiled into the executable binary.

After the application has started, if `noise_reduction_FILE_PATH` pointed to a single file (or a
folder containing a single input file), then the inference starts immediately. If multiple inputs
are chosen, then a menu is output and waits for the user input from telnet terminal.

For example:

```log
User input required
Enter option number from:

  1. Run noise reduction on the next WAV
  2. Run noise reduction on a WAV at chosen index
  3. Run noise reduction on all WAVs
  4. Show NN model info
  5. List audio clips

Choice:
```

1. “Run noise reduction on the next WAV”: Runs processing and inference on the next in line WAV file.

    > **Note:** Depending on the size of the input WAV file, multiple inferences can be invoked.

2. “Run noise reduction on a WAV at chosen index”: Runs processing and inference on the WAV file
   corresponding to the chosen index.

    > **Note:** Select the index in the range of supplied WAVs during application build. By default,
    the pre-built application has three files and indexes from 0-2.

3. “Run noise reduction on all WAVs”: Triggers sequential processing and inference executions on
   all baked-in WAV files.

4. “Show NN model info”: Prints information about the model data type, including the input and
   output tensor sizes. For example:

    ```log
    INFO - Model info:
    INFO - Model INPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 42 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   1
    INFO -          2:  42
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.221501
    INFO - ZeroPoint[0] = 14
    INFO -  tensor type is INT8
    INFO -  tensor occupies 24 bytes with dimensions
    INFO -          0:   1
    INFO -          1:  24
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.007843
    INFO - ZeroPoint[0] = -1
    INFO -  tensor type is INT8
    INFO -  tensor occupies 48 bytes with dimensions
    INFO -          0:   1
    INFO -          1:  48
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.047942
    INFO - ZeroPoint[0] = -128
    INFO -  tensor type is INT8
    INFO -  tensor occupies 96 bytes with dimensions
    INFO -          0:   1
    INFO -          1:  96
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.007843
    INFO - ZeroPoint[0] = -1
    INFO - Model OUTPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 96 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   1
    INFO -          2:  96
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.007843
    INFO - ZeroPoint[0] = -1
    INFO -  tensor type is INT8
    INFO -  tensor occupies 22 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   1
    INFO -          2:  22
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.003906
    INFO - ZeroPoint[0] = -128
    INFO -  tensor type is INT8
    INFO -  tensor occupies 48 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   1
    INFO -          2:  48
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.047942
    INFO - ZeroPoint[0] = -128
    INFO -  tensor type is INT8
    INFO -  tensor occupies 24 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   1
    INFO -          2:  24
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.007843
    INFO - ZeroPoint[0] = -1
    INFO -  tensor type is INT8
    INFO -  tensor occupies 1 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   1
    INFO -          2:   1
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.003906
    INFO - ZeroPoint[0] = -128
    INFO - Activation buffer (a.k.a tensor arena) size used: 1940
    INFO - Number of operators: 1
    INFO -  Operator 0: ethos-u
    INFO - Use of Arm uNPU is enabled
    ```

5. “List audio clips”: Prints a list of pair audio indexes. The original filenames are embedded in
    the application. For example:

    ```log
    INFO - List of Files:
    INFO -  0 => p232_113.wav
    INFO -  1 => p232_208.wav
    INFO -  2 => p257_031.wav
    ```

### Running Noise Reduction

Selecting the first option runs inference on the first file.

The following example illustrates an application output:

```log
INFO - Audio Clip dump header info (20 bytes) written to 0x80000000
INFO - Inference 1/136
INFO - Copied 960 bytes to 0x80000014
INFO - Inference 2/136
INFO - Copied 960 bytes to 0x800003d4
...
INFO - Inference 136/136
INFO - Copied 960 bytes to 0x8001fa54
INFO - Output memory dump of 130580 bytes written at address 0x80000000
INFO - Final results:
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 530
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 376
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 13911
INFO - NPU ACTIVE cycles: 103870
INFO - NPU IDLE cycles: 643
INFO - NPU TOTAL cycles: 104514
```

> **Note:** When running Fast Model, each inference can take several seconds on most systems.

Each inference dumps the post processed output to memory. For further information, please refer to:
[Dumping post processed results for all inferences](./noise_reduction.md#dumping-post_processed-results-for-all-inferences).

The profiling section of the log shows that for this inference:

- *Ethos-U* NPU PMU report for each inference:

  - 104514: The total number of NPU cycles.

  - 103870: How many NPU cycles were used for computation.

  - 643: How many cycles the NPU was idle for.

  - 530: The number of AXI beats with read transactions from AXI0 bus.
    > **Note:** The AXI0 is the bus where the *Ethos-U* NPU reads and writes to the computation
    > buffers, or the activation buf or tensor arenas.

  - 370: The number of AXI beats with write transactions to the AXI0 bus.

  - 13911: The number of AXI beats with read transactions from AXI1 bus.
    > **Note:** The AXI1 is the bus where *Ethos-U* NPU reads the model, which is read-only.

- For FPGA platforms, the CPU cycle count can also be enabled. However, for FVP, do not use the CPU
  cycle counters as the CPU model is not cycle-approximate or cycle-accurate.
