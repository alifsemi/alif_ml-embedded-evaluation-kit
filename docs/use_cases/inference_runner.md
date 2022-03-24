# Inference Runner Code Sample

- [Inference Runner Code Sample](./inference_runner.md#inference-runner-code-sample)
  - [Introduction](./inference_runner.md#introduction)
    - [Prerequisites](./inference_runner.md#prerequisites)
  - [Building the Code Samples application from sources](./inference_runner.md#building-the-code-samples-application-from-sources)
    - [Build options](./inference_runner.md#build-options)
    - [Build process](./inference_runner.md#build-process)
    - [Add custom model](./inference_runner.md#add-custom-model)
  - [Setting up and running Ethos-U NPU code sample](./inference_runner.md#setting-up-and-running-ethos_u-npu-code-sample)
    - [Setting up the Ethos-U NPU Fast Model](./inference_runner.md#setting-up-the-ethos_u-npu-fast-model)
    - [Starting Fast Model simulation](./inference_runner.md#starting-fast-model-simulation)
    - [Running Inference Runner](./inference_runner.md#running-inference-runner)
    - [Building with dynamic model load capability](./inference_runner.md#building-with-dynamic-model-load-capability)
    - [Running the FVP with dynamic model loading](./inference_runner.md#running-the-fvp-with-dynamic-model-loading)

## Introduction

This document describes the process of setting up and running the Arm® *Ethos™-U* NPU Inference Runner.

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

  Note that the parameters `TARGET_PLATFORM` and `ETHOS_U_NPU_ENABLED` must be aligned with the chosen model. In other
  words:

  - If `ETHOS_U_NPU_ENABLED` is set to `On` or `1`, then the NN model is assumed to be optimized. The model naturally
    falls back to the Arm® *Cortex®-M* CPU if an unoptimized model is supplied.
  - if `ETHOS_U_NPU_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
    model in this case results in a runtime error.

- `inference_runner_ACTIVATION_BUF_SZ`: The intermediate, or activation, buffer size reserved for the NN model. By
  default, it is set to 2MiB and is enough for most models.

- `inference_runner_DYNAMIC_MEM_LOAD_ENABLED`: This can be set to ON or OFF, to allow dynamic model load capability for use with MPS3 FVPs. See section [Building with dynamic model load capability](./inference_runner.md#building-with-dynamic-model-load-capability) below for more details.

To build **ONLY** the Inference Runner example application, add `-DUSE_CASE_BUILD=inference_runner` to the `cmake`
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
- [Configuring the build for simple-platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Building for different Ethos-U NPU variants](../sections/building.md#building-for-different-ethos_u-npu-variants)

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

> **Note:** If you want to run the model using an *Ethos-U*, ensure that your custom model has been successfully run
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

Once completed the building step, the application binary `ethos-u-inference_runner.axf` can be found in the `build/bin`
folder.

Assuming that the install location of the FVP was set to `~/FVP_install_location`, then the simulation can be started by
using:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 -a ./bin/mps3-sse-300/ethos-u-inference_runner.axf
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

- *Ethos-U* PMU report:

  - 34,178 total cycle: The number of NPU cycles.

  - 33,145 active cycles: The number of NPU cycles that were used for computation.

  - 1,033 idle cycles: The number of cycles for which the NPU was idle.

  - 9,332 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where the
    *Ethos-U* NPU reads and writes to the computation buffers, activation buf, or tensor arenas.

  - 3,248 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 2,219 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where the
    *Ethos-U* NPU reads the model. So, read-only.

- For FPGA platforms, a CPU cycle count can also be enabled. However, do not use cycle counters for FVP, as the CPU
  model is not cycle-approximate or cycle-accurate.

### Building with dynamic model load capability

It is possible to build the inference runner application, targeting only the FVP environment, that allows
loading of the TFLite model file at runtime. In this build configuration, the model TFLite file is not
baked into the application but the application expects this model binary to be loaded at a specific address
by an external agent. This loading capability also extends to the input data for the model.

This feature depends on these addresses to be specified in target platform's CMake description and, by
default, is available for use on the MPS3 FVP platform.

> **Note:**: The application built with this support will not work on the FPGA. This capability is only
> provided for use with the FVP, to make it easier to try different ML workloads without having to build
> the applications with different TFLite files baked into the application statically.
> Also, this feature is not available for `native` target.

The parameter `inference_runner_DYNAMIC_MEM_LOAD_ENABLED` should be set to ON in the CMake configuration
command to enable this feature. For example, from a freshly created build directory, run:

```commandline
cmake .. \
  -Dinference_runner_DYNAMIC_MEM_LOAD_ENABLED=ON \
  -DUSE_CASE_BUILD=inference_runner
```

Once the configuration completes, running:

```commandline
make -j
```

will build the application that will expect the neural network model and the IFM to be loaded into
specific addresses. The default values for these addresses are defined in the use-case CMake file.

### Running the FVP with dynamic model loading

If the application has been built with dynamic loading capability, as described in the previous section,
the FVP can be invoked with command line parameters that will load specific data into memory. For example,
the command below loads a custom model at address `0x90000000`, a custom input from address `0x92000000`
and when the FVP exits, it dumps a file named `output.bin` with the output tensors consolidated into a
binary blob.

> **Note:** The CMake profile for the target should also give an indication of the maximum sizes for
> each of the regions. This is also mentioned in the linker scripts for the same target. For MPS3,
> the model size can be a maximum of 32MiB. The IFM and OFM spaces are both reserved as 16MiB sections.

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 \
  -a ./bin/ethos-u-inference_runner.axf \
  --data /path/to/custom-model.tflite@0x90000000 \
  --data /path/to/custom-ifm.bin@0x92000000 \
  --dump cpu0=/path/to/output.bin@Memory:0x93000000,1024
```

The above command will dump a 1KiB (1024 bytes) file with output tensors as a binary blob after it
has consumed the model and IFM data provided by the file paths specified and the inference is
executed successfully.
If the size of the output tensors is unknown before running the FVP, it can be run without the `--dump`
parameter to check the size of the output first by looking at the application log. Alternatively, a
size of 16MiB will dump the whole reserved section for the OFM to a file.

> **Note:**: When there are multiple input tensors, the application is set up to iterate over all of
> them and populate each of them, in sequence, with the required amount of data. The sequence in which
> these tensors are populated is governed by the index assigned to them within the TensorFlow Lite Micro
> framework. So, the input binary blob should be a consolidated file containing data for all the input
> tensors. The same packing is used for output binary dumps.
