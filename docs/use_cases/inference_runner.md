# Inference Runner Code Sample

- [Introduction](#introduction)
  - [Prerequisites](#prerequisites)
- [Building the Code Samples application from sources](#building-the-code-samples-application-from-sources)
  - [Build options](#build-options)
  - [Build process](#build-process)
  - [Add custom model](#add-custom-model)
- [Setting-up and running Ethos-U55 code sample](#setting-up-and-running-ethos-u55-code-sample)
  - [Setting up the Ethos-U55 Fast Model](#setting-up-the-ethos-u55-fast-model)
  - [Starting Fast Model simulation](#starting-fast-model-simulation)
  - [Running Inference Runner](#running-inference-runner)
- [Inference Runner processing information](#inference-runner-processing-information)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U55 NPU Inference Runner.
The inference runner is intended for quickly checking profiling results for any desired network, providing it has been
processed by the Vela compiler.

A simple model is provided with the Inference Runner as an example, but it is expected that the user will replace this
model with one they wish to profile, see [Add custom model](./inference_runner.md#Add-custom-model) for more details.

The inference runner is intended for quickly checking profiling results for any desired network
providing it has been processed by the Vela compiler.

The inference runner will populate all input tensors for the provided model with randomly generated data and an
inference is then performed. Profiling results are then displayed in the console.

Use case code could be found in [source/use_case/inference_runner](../../source/use_case/inference_runner]) directory.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the Code Samples application from sources

### Build options

In addition to the already specified build option in the main documentation, the Inference Runner use case adds:

- `inference_runner_MODEL_TFLITE_PATH` - Path to the NN model file in TFLite format. Model will be processed and
  included into the application axf file. The default value points to one of the delivered set of models.
  Note that the parameters `TARGET_PLATFORM` and `ETHOS_U55_ENABLED` should be aligned with the chosen model, i.e.:
  - if `ETHOS_U55_ENABLED` is set to `On` or `1`, the NN model is assumed to be optimized. The model will naturally
    all back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized model
    in this case will result in a runtime error.

- `inference_runner_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By
    default, it is set to 2MiB and should be enough for most models.

In order to build **ONLY** Inference Runner example application add to the `cmake` command line specified in
[Building](../documentation.md#Building) `-DUSE_CASE_BUILD=inferece_runner`.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target platform
>see [Building](../documentation.md#Building) section.

Create a build directory and navigate inside:

```commandline
mkdir build_inference_runner && cd build_inference_runner
```

On Linux, execute the following command to build **only** Inference Runner application to run on the Ethos-U55 Fast
Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=./scripts/cmake/bare-metal-toolchain.cmake \
    -DUSE_CASE_BUILD=inference_runner ..
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
    -DUSE_CASE_BUILD=inference_runner ..
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
    -DUSE_CASE_BUILD=inference_runner ..
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
    -DUSE_CASE_BUILD=inference_runner ..
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
    -DUSE_CASE_BUILD=inference_runner ..
```

> **Note:** If re-building with changed parameters values, it is highly advised to clean the build directory and re-run
>the CMake command.

If the CMake command succeeded, build the application as follows:

```commandline
make -j4
```

Add VERBOSE=1 to see compilation and link details.

Results of the build will be placed under `build/bin` folder:

```tree
bin
 ├── ethos-u-inference_runner.axf
 ├── ethos-u-inference_runner.htm
 ├── ethos-u-inference_runner.map
 ├── images-inference_runner.txt
 └── sectors
      ├── kws
      │ └── ...
      └── img_class
        ├── dram.bin
        └── itcm.bin
```

Where:

- `ethos-u-inference_runner.axf`: The built application binary for the Inference Runner use case.

- `ethos-u-inference_runner.map`: Information from building the application (e.g. libraries used, what was optimized,
    location of objects)

- `ethos-u-inference_runner.htm`: Human readable file containing the call graph of application functions.

- `sectors/`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `Images-inference_runner.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/**
    folder.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter `inference_runner_MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using Ethos-U55, ensure your custom model has been run through the Vela compiler
>successfully before continuing. See [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

Then, you must set `inference_runner_MODEL_TFLITE_PATH` to the location of the Vela processed model file.

An example:

```commandline
cmake \
  -Dinference_runner_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
  -DTARGET_PLATFORM=mps3 \
  -DTARGET_SUBSYSTEM=sse-300 \
  -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/bare-metal-toolchain.cmake ..
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `inference_runner_MODEL_TFLITE_PATH` will be converted to C++ files during the CMake
configuration stage and then compiled into the application for performing inference with.

The log from the configuration stage should tell you what model path has been used:

```stdout
-- User option inference_runner_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to\
custom_model_after_vela.tflite.cc
...
```

After compiling, your custom model will have now replaced the default one in the application.

## Setting-up and running Ethos-U55 code sample

### Setting up the Ethos-U55 Fast Model

The FVP is available publicly from
[Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For Ethos-U55 evaluation, please download the MPS3 version of the Arm® Corstone™-300 model that contains Ethos-U55 and
Cortex-M55. The model is currently only supported on Linux based machines. To install the FVP:

- Unpack the archive

- Run the install script in the extracted package

```commandline
./FVP_Corstone_SSE-300_Ethos-U55.sh
```

- Follow the instructions to install the FVP to your desired location

### Starting Fast Model simulation

Once completed the building step, application binary ethos-u-infernce_runner.axf can be found in the `build/bin` folder.
Assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be started by:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55
./bin/mps3-sse-300/ethos-u-inference_runner.axf
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
INFO - NPU total cycles: 34178
```

After running an inference on randomly generated data, the output of the log shows the profiling results that for this
inference:

- Ethos-U55's PMU report:

  - 34,178 total cycle: The number of NPU cycles

  - 33,145 active cycles: number of NPU cycles that were used for computation

  - 1,033 idle cycles: number of cycles for which the NPU was idle

  - 9,332 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus.
    AXI0 is the bus where Ethos-U55 NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 3,248 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 2,219 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus.
    AXI1 is the bus where Ethos-U55 NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
    the CPU model is not cycle-approximate or cycle-accurate.
