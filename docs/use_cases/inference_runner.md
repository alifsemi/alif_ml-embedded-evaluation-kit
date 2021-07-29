# Inference Runner Code Sample

- [Inference Runner Code Sample](#inference-runner-code-sample)
  - [Introduction](#introduction)
    - [Prerequisites](#prerequisites)
  - [Building the Code Samples application from sources](#building-the-code-samples-application-from-sources)
    - [Build options](#build-options)
    - [Build process](#build-process)
    - [Add custom model](#add-custom-model)
  - [Setting up and running Ethos-U55 code sample](#setting-up-and-running-ethos-u55-code-sample)
    - [Setting up the Ethos-U55 Fast Model](#setting-up-the-ethos-u55-fast-model)
    - [Starting Fast Model simulation](#starting-fast-model-simulation)
    - [Running Inference Runner](#running-inference-runner)

## Introduction

This document describes the process of setting up and running the Arm® *Ethos™-U55* NPU Inference Runner.

The inference runner is intended for quickly checking profiling results for any wanted network, providing it has been
processed by the Vela compiler.

A simple model is provided with the Inference Runner as an example. However, we expect you to replace this model with
one that you must profile.

For further details, refer to: [Add custom model](./inference_runner.md#Add-custom-model).

The inference runner populates all input tensors for the provided model with randomly generated data and an inference is
then performed. Profiling results are then displayed in the console.

The example use-case code can be found in the following directory:
[source/use_case/inference_runner](../../source/use_case/inference_runner).

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the Code Samples application from sources

### Build options

In addition to the already specified build option in the main documentation, the Inference Runner use-case adds the
following:

- `inference_runner_MODEL_TFLITE_PATH` - The path to the NN model file in the `TFLite` format. The model is then
  processed and included in the application `axf` file. The default value points to one of the delivered set of models.

  Note that the parameters `TARGET_PLATFORM` and `ETHOS_U55_ENABLED` must be aligned with the chosen model. In other
  words:

  - If `ETHOS_U55_ENABLED` is set to `On` or `1`, then the NN model is assumed to be optimized. The model naturally
    falls back to the Arm® *Cortex®-M* CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
    model in this case results in a runtime error.

- `inference_runner_ACTIVATION_BUF_SZ`: The intermediate, or activation, buffer size reserved for the NN model. By
  default, it is set to 2MiB and is enough for most models.

To build **ONLY** the Inference Runner example application, add `-DUSE_CASE_BUILD=inferece_runner` to the `cmake`
command line, as specified in: [Building](../documentation.md#Building).

### Build process

> **Note:** This section describes the process for configuring the build for the *MPS3: SSE-300*. To build for a
> different target platform, please refer to: [Building](../documentation.md#Building).

Create a build directory and navigate inside, like so:

```commandline
mkdir build_inference_runner && cd build_inference_runner
```

On Linux, when providing only the mandatory arguments for the CMake configuration, execute the following command to
build **only** Image Classification application to run on the *Ethos-U55* Fast Model:

```commandline
cmake ../ -DUSE_CASE_BUILD=inference_runner
```

To configure a build that can be debugged using Arm DS specify the build type as `Debug` and then use the `Arm Compiler`
toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=inference_runner
```

For further information, please refer to:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple_platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Working with model debugger from Arm Fast Model Tools](../sections/building.md#working-with-model-debugger-from-arm-fast-model-tools)

> **Note:** If re-building with changed parameters values, we recommend that you clean the build directory and re-run
> the CMake command.

If the CMake command succeeds, build the application as follows:

```commandline
make -j4
```

To see compilation and link details, add `VERBOSE=1`.

Results of the build are placed under the `build/bin` folder, like so:

```tree
bin
 ├── ethos-u-inference_runner.axf
 ├── ethos-u-inference_runner.htm
 ├── ethos-u-inference_runner.map
 └── sectors
      └── inference_runner
        ├── ddr.bin
        └── itcm.bin
```

The `bin` folder contains the following files:

- `ethos-u-inference_runner.axf`: The built application binary for the Inference Runner use-case.

- `ethos-u-inference_runner.map`: Information from building the application. For example: The libraries used, what was
  optimized, and the location of objects.

- `ethos-u-inference_runner.htm`: Human readable file containing the call graph of application functions.

- `sectors/inference_runner`: Folder containing the built application. It is split into files for loading into different FPGA memory
  regions.

- `sectors/images.txt`: Tells the FPGA which memory regions to use for loading the binaries in the `sectors/..`
  folder.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter
`inference_runner_MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using an *Ethos-U55*, ensure that your custom model has been successfully run
> through the Vela compiler *before* continuing.

For further information: [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

Then, you must set `inference_runner_MODEL_TFLITE_PATH` to the location of the Vela processed model file.

An example:

```commandline
cmake .. \
  -Dinference_runner_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
  -DUSE_CASE_BUILD=inference_runner
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `inference_runner_MODEL_TFLITE_PATH` is converted to C++ files during the CMake
configuration stage. It is then compiled into the application for performing inference with.

The log from the configuration stage tells you what model path and labels file have been used, for example:

```stdout
-- User option inference_runner_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to\
custom_model_after_vela.tflite.cc
...
```

After compiling, your custom model has now replaced the default one in the application.

## Setting up and running Ethos-U55 code sample

### Setting up the Ethos-U55 Fast Model

The FVP is available publicly from
[Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For the *Ethos-U55* evaluation, please download the MPS3 version of the Arm® *Corstone™-300* model that contains both
the *Ethos-U55* and *Cortex-M55*. The model is currently only supported on Linux-based machines.

To install the FVP:

- Unpack the archive.

- Run the install script in the extracted package:

```commandline
./FVP_Corstone_SSE-300_Ethos-U55.sh
```

- Follow the instructions to install the FVP to the required location.

### Starting Fast Model simulation

Once completed the building step, the application binary `ethos-u-infernce_runner.axf` can be found in the `build/bin`
folder.

Assuming that the install location of the FVP was set to `~/FVP_install_location`, then the simulation can be started by
using:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55
./bin/mps3-sse-300/ethos-u-inference_runner.axf
```

A log output appears on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This also launches a telnet window with the standard output of the sample application. It also includes error log
entries containing information about the pre-built application version, TensorFlow Lite Micro library version used, and
data types. The log also includes the input and output tensor sizes of the model compiled into the executable binary.

### Running Inference Runner

After the application has started the inference starts immediately and it outputs the results on the telnet terminal.

The following example illustrates application output:

```log
INFO - Final results:
INFO - Profile for Inference :
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 9332
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 3248
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 2219
INFO - NPU ACTIVE cycles: 33145
INFO - NPU IDLE cycles: 1033
INFO - NPU TOTAL cycles: 34178
```

After running an inference on randomly generated data, the output of the log shows the profiling results that for this
inference. For example:

- *Ethos-U55* PMU report:

  - 34,178 total cycle: The number of NPU cycles.

  - 33,145 active cycles: The number of NPU cycles that were used for computation.

  - 1,033 idle cycles: The number of cycles for which the NPU was idle.

  - 9,332 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where the
    *Ethos-U55* NPU reads and writes to the computation buffers, activation buf, or tensor arenas.

  - 3,248 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 2,219 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where the
    *Ethos-U55* NPU reads the model. So, read-only.

- For FPGA platforms, a CPU cycle count can also be enabled. However, do not use cycle counters for FVP, as the CPU
  model is not cycle-approximate or cycle-accurate.
