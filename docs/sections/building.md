# Building the ML embedded code sample applications from sources

## Contents

- [Building the ML embedded code sample applications from sources](#building-the-ml-embedded-code-sample-applications-from-sources)
  - [Contents](#contents)
  - [Build prerequisites](#build-prerequisites)
  - [Build options](#build-options)
  - [Build process](#build-process)
    - [Preparing build environment](#preparing-build-environment)
      - [Fetching submodules](#fetching-submodules)
      - [Fetching resource files](#fetching-resource-files)
    - [Create a build directory](#create-a-build-directory)
    - [Configuring the build for MPS3: SSE-300](#configuring-the-build-for-mps3-sse-300)
      - [Using GNU Arm Embedded Toolchain](#using-gnu-arm-embedded-toolchain)
      - [Using Arm Compiler](#using-arm-compiler)
      - [Generating project for Arm Development Studio](#generating-project-for-arm-development-studio)
      - [Working with model debugger from Arm FastModel Tools](#working-with-model-debugger-from-arm-fastmodel-tools)
      - [Configuring with custom TPIP dependencies](#configuring-with-custom-tpip-dependencies)
    - [Configuring native unit-test build](#configuring-native-unit-test-build)
    - [Configuring the build for simple_platform](#configuring-the-build-for-simple_platform)
    - [Building the configured project](#building-the-configured-project)
  - [Building timing adapter with custom options](#building-timing-adapter-with-custom-options)
  - [Add custom inputs](#add-custom-inputs)
  - [Add custom model](#add-custom-model)
  - [Optimize custom model with Vela compiler](#optimize-custom-model-with-vela-compiler)
  - [Automatic file generation](#automatic-file-generation)

This section assumes the use of an **x86 Linux** build machine.

## Build prerequisites

Before proceeding, please, make sure that the following prerequisites
are fulfilled:

- GNU Arm embedded toolchain 10.2.1 (or higher) or the Arm Compiler version 6.14 (or higher)
    is installed and available on the path.

    Test the compiler by running:

    ```commandline
    armclang -v
    ```

    ```log
    Product: ARM Compiler 6.14 Professional
    Component: ARM Compiler 6.14
    ```

    Alternatively,

    ```commandline
    arm-none-eabi-gcc --version
    ```
    ```log
    arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10-2020-q4-major) 10.2.1 20201103 (release)
    Copyright (C) 2020 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    ```

    > **Note:** Add compiler to the path, if needed:
    >
    > `export PATH=/path/to/armclang/bin:$PATH`
    >           OR
    > `export PATH=/path/to/gcc-arm-none-eabi-toolchain/bin:$PATH`

- Compiler license, if using the proprietary Arm Compiler, is configured correctly.

- CMake version 3.15 or above is installed and available on the path.
    Test CMake by running:

    ```commandline
    cmake --version
    ```

    ```log
    cmake version 3.16.2
    ```

    > **Note:** Add cmake to the path, if needed:
    >
    > `export PATH=/path/to/cmake/bin:$PATH`

- Python 3.6 or above is installed. Test python version by running:

    ```commandline
    python3 --version
    ```

    ```log
    Python 3.6.8
    ```

- Build system will create python virtual environment during the build
    process. Please make sure that python virtual environment module is
    installed:

    ```commandline
    python3 -m venv
    ```

- Make

    ```commandline
    make --version
    ```

    ```log
    GNU Make 4.1

    ...
    ```

    > **Note:** Add it to the path environment variable, if needed.

- Access to the Internet to download the third party dependencies, specifically: TensorFlow Lite Micro, Arm® Ethos™-U55 NPU
driver and CMSIS. Instructions for downloading these are listed under [preparing build environment](#preparing-build-environment).

## Build options

The project build system allows user to specify custom neural network
models (in `.tflite` format) for each use case along with the network
inputs. It also builds TensorFlow Lite for Microcontrollers library,
Arm® Ethos™-U55 driver library, and CMSIS-DSP library from sources.

The build script is parameterized to support different options. Default
values for build parameters will build the applications for all use cases
for Arm® Corstone™-300 design that can execute on an MPS3 FPGA or the FVP.

The build parameters are:

- `TARGET_PLATFORM`: Target platform to execute application:
  - `mps3` (default)
  - `native`
  - `simple_platform`

- `TARGET_SUBSYSTEM`: Platform target subsystem; this specifies the
    design implementation for the deployment target. For both, the MPS3
    FVP and the MPS3 FPGA, this should be left to the default value of
    SSE-300:
  - `sse-300` (default - [Arm® Corstone™-300](https://developer.arm.com/ip-products/subsystem/corstone/corstone-300))

- `CMAKE_TOOLCHAIN_FILE`: This built-in CMake parameter can be used to override the
    default toolchain file used for the build. All the valid toolchain files are in the
    scripts directory. For example, see [bare-metal-gcc.cmake](../../scripts/cmake/toolchains/bare-metal-gcc.cmake).

- `TENSORFLOW_SRC_PATH`: Path to the root of the TensorFlow directory.
    The default value points to the TensorFlow submodule in the
    [ethos-u](https://git.mlplatform.org/ml/ethos-u/ethos-u.git/about/) `dependencies` folder.

- `ETHOS_U55_DRIVER_SRC_PATH`: Path to the Ethos-U55 NPU core driver sources.
    The default value points to the core_driver submodule in the
    [ethos-u](https://git.mlplatform.org/ml/ethos-u/ethos-u.git/about/) `dependencies` folder.

- `CMSIS_SRC_PATH`: Path to the CMSIS sources to be used to build TensorFlow
    Lite Micro library. This parameters is optional and valid only for
    Arm® Cortex®-M CPU targeted configurations. The default value points to the CMSIS submodule in the
    [ethos-u](https://git.mlplatform.org/ml/ethos-u/ethos-u.git/about/) `dependencies` folder.

- `ETHOS_U55_ENABLED`: Sets whether the use of Ethos-U55 NPU is available for
    the deployment target. By default, this is set and therefore
    application is built with Ethos-U55 NPU supported.

- `CPU_PROFILE_ENABLED`: Sets whether profiling information for the CPU
    core should be displayed. By default, this is set to false, but can
    be turned on for FPGA targets. The the FVP, the CPU core's cycle
    counts are not meaningful and should not be used.

- `LOG_LEVEL`: Sets the verbosity level for the application's output
    over UART/stdout. Valid values are `LOG_LEVEL_TRACE`, `LOG_LEVEL_DEBUG`,
    `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN` and `LOG_LEVEL_ERROR`. By default, it
    is set to `LOG_LEVEL_INFO`.

- `<use_case>_MODEL_TFLITE_PATH`: Path to the model file that will be
    processed and included into the application axf file. The default
    value points to one of the delivered set of models. Make sure the
    model chosen is aligned with the `ETHOS_U55_ENABLED` setting.

  - When using Ethos-U55 NPU backend, the NN model is assumed to be
    optimized by Vela compiler.
    However, even if not, it will fall back on the CPU and execute,
    if supported by TensorFlow Lite Micro.

  - When use of Ethos-U55 NPU is disabled, and if a Vela optimized model
    is provided, the application will report a failure at runtime.

- `USE_CASE_BUILD`: specifies the list of applications to build. By
    default, the build system scans sources to identify available ML
    applications and produces executables for all detected use-cases.
    This parameter can accept single value, for example,
    `USE_CASE_BUILD=img_class` or multiple values, for example,
    `USE_CASE_BUILD="img_class;kws"`.

- `ETHOS_U55_TIMING_ADAPTER_SRC_PATH`: Path to timing adapter sources.
    The default value points to the `timing_adapter` dependencies folder.

- `TA_CONFIG_FILE`: Path to the CMake configuration file containing the
    timing adapter parameters. Used only if the timing adapter build is
    enabled.

- `TENSORFLOW_LITE_MICRO_CLEAN_BUILD`: Optional parameter to enable/disable
    "cleaning" prior to building for the TensorFlow Lite Micro library.
    It is enabled by default.

- `TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS`: Optional parameter to enable wiping
    out TPIP downloads from TensorFlow source tree prior to each build.
    It is disabled by default.

- `ARMCLANG_DEBUG_DWARF_LEVEL`: When the CMake build type is specified as `Debug`
    and when armclang toolchain is being used to build for a Cortex-M CPU target,
    this optional argument can be set to specify the DWARF format.
    By default, this is set to 4 and is synonymous with passing `-g`
    flag to the compiler. This is compatible with Arm-DS and other tools
    which can interpret the latest DWARF format. To allow debugging using
    the Model Debugger from Arm FastModel Tools Suite, this argument can be used
    to pass DWARF format version as "3". Note: this option is only available
    when CMake project is configured with `-DCMAKE_BUILD_TYPE=Debug` argument.
    Also, the same dwarf format is used for building TensorFlow Lite Micro library.

> **Note:** For details on the specific use case build options, follow the
> instructions in the use-case specific documentation.
> Also, when setting any of the CMake configuration parameters that expect a directory/file path , it is advised
>to **use absolute paths instead of relative paths**.

## Build process

The build process can summarized in three major steps:

- Prepare the build environment by downloading third party sources required, see
[Preparing build environment](#preparing-build-environment).

- Configure the build for the platform chosen.
This stage includes:
  - CMake options configuration
  - When `<use_case>_MODEL_TFLITE_PATH` build options aren't provided, defaults neural network models are be downloaded
from [Arm ML-Zoo](https://github.com/ARM-software/ML-zoo/). In case of native build, network's input and output data
for tests are downloaded.
  - Some files such as neural network models, network's inputs and output labels are automatically converted
    into C/C++ arrays, see [Automatic file generation](#automatic-file-generation).

- Build the application.\
During this stage application and third party libraries are built see [Building the configured project](#building-the-configured-project).

### Preparing build environment

#### Fetching submodules

Certain third party sources are required to be present on the development machine to allow the example sources in this
repository to link against.

1. [TensorFlow Lite Micro repository](https://github.com/tensorflow/tensorflow)
2. [Ethos-U55 NPU core driver repository](https://review.mlplatform.org/admin/repos/ml/ethos-u/ethos-u-core-driver)
3. [CMSIS-5](https://github.com/ARM-software/CMSIS_5.git)

These are part of the [ethos-u repository](https://git.mlplatform.org/ml/ethos-u/ethos-u.git/about/) and set as
submodules of this project.

To pull the submodules:

```sh
git submodule update --init
```

This will download all the required components and place them in a tree like:

```tree
dependencies
 └── ethos-u
     ├── cmsis
     ├── core_driver
     ├── tensorflow
     └── ...
```

> **NOTE**: The default source paths for the TPIP sources assume the above directory structure, but all of the relevant
>paths can be overridden by CMake configuration arguments `TENSORFLOW_SRC_PATH`, `ETHOS_U55_DRIVER_SRC_PATH`,
>and `CMSIS_SRC_PATH`.


#### Fetching resource files

All the ML use case examples in this repository also depend on external neural
network models. To download these, run the following command from the root of
the repository:

```sh
python3 ./set_up_default_resources.py
```

This will fetch all the models into `resources_downloaded` directory. It will
also optimize the models using the Vela compiler for default 128 MAC configuration
of Arm® Ethos™-U55 NPU.

### Create a build directory

Create a build directory in the root of the project and navigate inside:

```commandline
mkdir build && cd build
```

### Configuring the build for MPS3: SSE-300

#### Using GNU Arm Embedded Toolchain

On Linux, if using `Arm GNU embedded toolchain`, execute the following command
to build the application to run on the Arm® Ethos™-U55 NPU when providing only
the mandatory arguments for CMake configuration:


```commandline
cmake ../
```

The above command will build for the default target platform `mps3`, the default subsystem
`sse-300`, and using the default toolchain file for the target as `bare-metal-gcc.` This is
equivalent to:


```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300
```

#### Using Arm Compiler

If using `Arm Compiler` instead, the toolchain option `CMAKE_TOOLCHAIN_FILE` can be used to
point to the ARMClang CMake file instead to set the compiler and platform specific parameters.

```commandline
cmake ../ -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

To configure a build that can be debugged using Arm Development Studio, we can just specify
the build type as `Debug`:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug
```

#### Generating project for Arm Development Studio

To be able to import the project in Arm Development Studio, add the Eclipse project generator and CMAKE_ECLIPSE_VERSION in the CMake command. It is advisable that the build directory is one level up relative to the source directory. When the build has been generated, you need to follow the Import wizard in Arm Development Studio and import the existing project into the workspace. You can then compile and debug the project using Arm Development Studio. Note that the below command is executed one level up from the source directory.

```commandline
cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -G "Eclipse CDT4 - Unix Makefiles" \
    -DCMAKE_ECLIPSE_VERSION=4.15 \
    ml-embedded-evaluation-kit
```

#### Working with model debugger from Arm FastModel Tools

To configure a build that can be debugged using a tool that only supports
DWARF format 3 (Modeldebugger for example), we can use:

```commandline
cmake .. \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DARMCLANG_DEBUG_DWARF_LEVEL=3
```

#### Configuring with custom TPIP dependencies

If the TensorFlow source tree is not in its default expected location, set the path
using `TENSORFLOW_SRC_PATH`. Similarly, if the Ethos-U55 NPU driver and CMSIS are
not in the default location, `ETHOS_U55_DRIVER_SRC_PATH` and `CMSIS_SRC_PATH` can be
used to configure their location. For example:

```commandline
cmake .. \
    -DTENSORFLOW_SRC_PATH=/my/custom/location/tensorflow \
    -DETHOS_U55_DRIVER_SRC_PATH=/my/custom/location/core_driver \
    -DCMSIS_SRC_PATH=/my/custom/location/cmsis
```

> **Note:** If re-building with changed parameters values, it is
highly advised to clean the build directory and re-run the CMake command.

### Configuring native unit-test build

```commandline
cmake ../ -DTARGET_PLATFORM=native
```

Results of the build will be placed under `build/bin/` folder:

```tree
 bin
  |- dev_ethosu_eval-tests
  |_ ethos-u
```

### Configuring the build for simple_platform

```commandline
cmake ../ -DTARGET_PLATFORM=simple_platform
```

Again, if using `Arm Compiler`, use:

```commandline
cmake .. \
    -DTARGET_PLATFORM=simple_platform \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

### Building the configured project

If the CMake command succeeds, build the application as follows:

```commandline
make -j4
```

Add `VERBOSE=1` to see compilation and link details.

Results of the build will be placed under `build/bin` folder, an
example:

```tree
bin
 ├── ethos-u-<use_case_name>.axf
 ├── ethos-u-<use_case_name>.htm
 ├── ethos-u-<use_case_name>.map
 ├── images-<use_case_name>.txt
 └── sectors
        └── <use_case>
                ├── dram.bin
                └── itcm.bin
```

Where for each implemented use-case under the `source/use-case` directory,
the following build artefacts will be created:

- `ethos-u-<use case name>.axf`: The built application binary for a ML
    use case.

- `ethos-u-<use case name>.map`: Information from building the
    application (e.g. libraries used, what was optimized, location of
    objects).

- `ethos-u-<use case name>.htm`: Human readable file containing the
    call graph of application functions.

- `sectors/`: Folder containing the built application, split into files
    for loading into different FPGA memory regions.

- `images-<use case name>.txt`: Tells the FPGA which memory regions to
    use for loading the binaries in sectors/** folder.

> **Note:**  For the specific use case commands see the relative section
in the use case documentation.

## Building timing adapter with custom options

The sources also contains the configuration for a timing adapter utility
for the Ethos-U55 NPU driver. The timing adapter allows the platform to simulate user
provided memory bandwidth and latency constraints.

The timing adapter driver aims to control the behavior of two AXI buses
used by Ethos-U55 NPU. One is for SRAM memory region and the other is for
flash or DRAM. The SRAM is where intermediate buffers are expected to be
allocated and therefore, this region can serve frequent R/W traffic
generated by computation operations while executing a neural network
inference. The flash or DDR is where we expect to store the model
weights and therefore, this bus would typically be used only for R/O
traffic.

It is used for MPS3 FPGA as well as for Fast Model environment.

The CMake build framework allows the parameters to control the behavior
of each bus with following parameters:

- `MAXR`: Maximum number of pending read operations allowed. 0 is
    inferred as infinite, and the default value is 4.

- `MAXW`: Maximum number of pending write operations allowed. 0 is
    inferred as infinite, and the default value is 4.

- `MAXRW`: Maximum number of pending read+write operations allowed. 0 is
    inferred as infinite, and the default value is 8.

- `RLATENCY`: Minimum latency, in cycle counts, for a read operation.
    This is the duration between ARVALID and RVALID signals. The default
    value is 50.

- `WLATENCY`: Minimum latency, in cycle counts, for a write operation.
    This is the duration between WVALID + WLAST and BVALID being
    de-asserted. The default value is 50.

- `PULSE_ON`: Number of cycles during which addresses are let through.
    The default value is 5100.

- `PULSE_OFF`: Number of cycles during which addresses are blocked. The
    default value is 5100.

- `BWCAP`: Maximum number of 64-bit words transferred per pulse cycle. A
    pulse cycle is PULSE_ON + PULSE_OFF. 0 is inferred as infinite, and
    the default value is 625.

- `MODE`: Timing adapter operation mode. Default value is 0

  - Bit 0: 0=simple; 1=latency-deadline QoS throttling of read vs.
        write

  - Bit 1: 1=enable random AR reordering (0=default),

  - Bit 2: 1=enable random R reordering (0=default),

  - Bit 3: 1=enable random B reordering (0=default)

For timing adapter's CMake build configuration, the SRAM AXI is assigned
index 0 and the flash/DRAM AXI bus has index 1. To change the bus
parameter for the build a "***TA_\<index>_**"* prefix should be added
to the above. For example, **TA0_MAXR=10** will set the SRAM AXI bus's
maximum pending reads to 10.

As an example, if we have the following parameters for flash/DRAM
region:

- `TA1_MAXR` = "2"

- `TA1_MAXW` = "0"

- `TA1_MAXRW` = "0"

- `TA1_RLATENCY` = "64"

- `TA1_WLATENCY` = "32"

- `TA1_PULSE_ON` = "320"

- `TA1_PULSE_OFF` = "80"

- `TA1_BWCAP` = "50"

For a clock rate of 500MHz, this would translate to:

- The maximum duty cycle for any operation is:\
![Maximum duty cycle formula](../media/F1.png)

- Maximum bit rate for this bus (64-bit wide) is:\
![Maximum bit rate formula](../media/F2.png)

- With a read latency of 64 cycles, and maximum pending reads as 2,
    each read could be a maximum of 64 or 128 bytes, as defined for
    Ethos-U55 NPU\'s AXI bus\'s attribute.

    The bandwidth is calculated solely by read parameters ![Bandwidth formula](
        ../media/F3.png)

    This is higher than the overall bandwidth dictated by the bus parameters
    of \
    ![Overall bandwidth formula](../media/F4.png)

This suggests that the read operation is limited only by the overall bus
bandwidth.

Timing adapter requires recompilation to change parameters. Default timing
adapter configuration file pointed to by `TA_CONFIG_FILE` build parameter is
located in the scripts/cmake folder and contains all options for AXI0 and
AXI1 described above.

An example of scripts/cmake/ta_config.cmake:

```cmake
# Timing adapter options
set(TA_INTERACTIVE OFF)

# Timing adapter settings for AXI0
set(TA0_MAXR "8")
set(TA0_MAXW "8")
set(TA0_MAXRW "0")
set(TA0_RLATENCY "32")
set(TA0_WLATENCY "32")
set(TA0_PULSE_ON "3999")
set(TA0_PULSE_OFF "1")
set(TA0_BWCAP "4000")
...
```

An example of the build with custom timing adapter configuration:

```commandline
cmake .. -DTA_CONFIG_FILE=scripts/cmake/my_ta_config.cmake
```

## Add custom inputs

The application performs inference on input data found in the folder set
by the CMake parameters, for more information see the 3.3 section in the
specific use case documentation.

## Add custom model

The application performs inference using the model pointed to by the
CMake parameter `MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using Ethos-U55 NPU, ensure your custom
model has been run through the Vela compiler successfully before continuing.

To run the application with a custom model you will need to provide a
labels_<model_name>.txt file of labels associated with the model.
Each line of the file should correspond to one of the outputs in your
model. See the provided labels_mobilenet_v2_1.0_224.txt file in the
img_class use case for an example.

Then, you must set `<use_case>_MODEL_TFLITE_PATH` to the location of
the Vela processed model file and `<use_case>_LABELS_TXT_FILE` to the
location of the associated labels file:

```commandline
cmake .. \
    -D<use_case>_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -D<use_case>_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

> **Note:** For the specific use case command see the relative section in the use case documentation.
> **Note:** Clean the build directory before re-running the CMake command.

The TensorFlow Lite for Microcontrollers model pointed to by `<use_case>_MODEL_TFLITE_PATH` and
labels text file pointed to by `<use_case>_LABELS_TXT_FILE` will be
converted to C++ files during the CMake configuration stage and then
compiled into the application for performing inference with.

The log from the configuration stage should tell you what model path and
labels file have been used:

```log
-- User option TARGET_PLATFORM is set to mps3
-- User option <use_case>_MODEL_TFLITE_PATH is set to
<path/to/custom_model_after_vela.tflite>
...
-- User option <use_case>_LABELS_TXT_FILE is set to
<path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build>/generated/include/Labels.hpp and <path/to/build>/generated/src/Labels.cc
...
```

After compiling, your custom model will have now replaced the default
one in the application.

## Optimize custom model with Vela compiler

> **Note:** This tool is not available within this project.
It is a python tool available from <https://pypi.org/project/ethos-u-vela/>.
The source code is hosted on <https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/>.

The Vela compiler is a tool that can optimize a neural network model
into a version that can run on an embedded system containing Ethos-U55 NPU.

The optimized model will contain custom operators for sub-graphs of the
model that can be accelerated by Ethos-U55 NPU, the remaining layers that
cannot be accelerated are left unchanged and will run on the CPU using
optimized (CMSIS-NN) or reference kernels provided by the inference
engine.

After the compilation, the optimized model can only be executed on a
system with Ethos-U55 NPU.

> **Note:** The NN model provided during the build and compiled into the application
executable binary defines whether CPU or NPU is used to execute workloads.
If unoptimized model is used, then inference will run on Cortex-M CPU.

Vela compiler accepts parameters to influence a model optimization. The
model provided within this project has been optimized with
the following parameters:

```commandline
vela \
    --accelerator-config=ethos-u55-128 \
    --block-config-limit=0 \
    --config my_vela_cfg.ini \
    --memory-mode Shared_Sram \
    --system-config Ethos_U55_High_End_Embedded \
    <model>.tflite
```

Where:

- `--accelerator-config`: Specify the accelerator configuration to use
    between ethos-u55-256, ethos-u55-128, ethos-u55-64 and ethos-u55-32.
- `--block-config-limit`: Limit block config search space, use zero for
    unlimited.
- `--config`: Specifies the path to the Vela configuration file. The format of the file is a Python ConfigParser .ini file.
    An example can be found in the `dependencies` folder [vela.ini](../../scripts/vela/vela.ini).
- `--memory-mode`: Selects the memory mode to use as specified in the Vela configuration file.
- `--system-config`:Selects the system configuration to use as specified in the Vela configuration file.

Vela compiler accepts `.tflite` file as input and saves optimized network
model as a `.tflite` file.

Using `--show-cpu-operations` and `--show-subgraph-io-summary` will show
all the operations that fall back to the CPU and a summary of all the
subgraphs and their inputs and outputs.

To see Vela helper for all the parameters use: `vela --help`.

Please, get in touch with your Arm representative to request access to
Vela Compiler documentation for more details.

> **Note:** By default, use of the Ethos-U55 NPU is enabled in the CMake configuration.
This could be changed by passing `-DETHOS_U55_ENABLED`.


## Automatic file generation

As mentioned in the previous sections, some files such as neural network
models, network's inputs, and output labels are automatically converted
into C/C++ arrays during the CMake project configuration stage.
Additionally, some code is generated to allow access to these arrays.

An example:

```log
-- Building use-cases: img_class.
-- Found sources for use-case img_class
-- User option img_class_FILE_PATH is set to /tmp/samples
-- User option img_class_IMAGE_SIZE is set to 224
-- User option img_class_LABELS_TXT_FILE is set to /tmp/labels/labels_model.txt
-- Generating image files from /tmp/samples
++ Converting cat.bmp to cat.cc
++ Converting dog.bmp to dog.cc
-- Skipping file /tmp/samples/files.md due to unsupported image format.
++ Converting kimono.bmp to kimono.cc
++ Converting tiger.bmp to tiger.cc
++ Generating /tmp/build/generated/img_class/include/InputFiles.hpp
-- Generating labels file from /tmp/labels/labels_model.txt
-- writing to /tmp/build/generated/img_class/include/Labels.hpp and /tmp/build/generated/img_class/src/Labels.cc
-- User option img_class_ACTIVATION_BUF_SZ is set to 0x00200000
-- User option img_class_MODEL_TFLITE_PATH is set to /tmp/models/model.tflite
-- Using /tmp/models/model.tflite
++ Converting model.tflite to    model.tflite.cc
...
```

In particular, the building options pointing to the input files `<use_case>_FILE_PATH`,
the model `<use_case>_MODEL_TFLITE_PATH` and labels text file `<use_case>_LABELS_TXT_FILE`
are used by python scripts in order to generate not only the converted array files,
but also some headers with utility functions.

For example, the generated utility functions for image classification are:

- `build/generated/include/InputFiles.hpp`

```c++
#ifndef GENERATED_IMAGES_H
#define GENERATED_IMAGES_H

#include <cstdint>

#define NUMBER_OF_FILES  (2U)
#define IMAGE_DATA_SIZE  (150528U)

extern const uint8_t im0[IMAGE_DATA_SIZE];
extern const uint8_t im1[IMAGE_DATA_SIZE];

const char* get_filename(const uint32_t idx);
const uint8_t* get_img_array(const uint32_t idx);

#endif /* GENERATED_IMAGES_H */
```

- `build/generated/src/InputFiles.cc`

```c++
#include "InputFiles.hpp"

static const char *img_filenames[] = {
    "img1.bmp",
    "img2.bmp",
};

static const uint8_t *img_arrays[] = {
    im0,
    im1
};

const char* get_filename(const uint32_t idx)
{
    if (idx < NUMBER_OF_FILES) {
        return img_filenames[idx];
    }
    return nullptr;
}

const uint8_t* get_img_array(const uint32_t idx)
{
    if (idx < NUMBER_OF_FILES) {
        return img_arrays[idx];
    }
    return nullptr;
}
```

These headers are generated using python templates, that are in `scripts/py/templates/*.template`.

```tree
scripts/
├── cmake
│   ├── ...
│   ├── subsystem-profiles
│   │   └── corstone-sse-300.cmake
│   ├── templates
│   │   ├── mem_regions.h.template
│   │   ├── peripheral_irqs.h.template
│   │   └── peripheral_memmap.h.template
│   └── ...
└── py
    ├── <generation scripts>
    ├── requirements.txt
    └── templates
        ├── audio.cc.template
        ├── AudioClips.cc.template
        ├── AudioClips.hpp.template
        ├── default.hpp.template
        ├── header_template.txt
        ├── image.cc.template
        ├── Images.cc.template
        ├── Images.hpp.template
        ├── Labels.cc.template
        ├── Labels.hpp.template
        ├── testdata.cc.template
        ├── TestData.cc.template
        ├── TestData.hpp.template
        └── tflite.cc.template
```

Based on the type of use case the correct conversion is called in the use case cmake file
(audio or image respectively for voice or vision use cases).
For example, the generations call for image classification (`source/use_case/img_class/usecase.cmake`):

```c++
# Generate input files
generate_images_code("${${use_case}_FILE_PATH}"
                     ${SRC_GEN_DIR}
                     ${INC_GEN_DIR}
                     "${${use_case}_IMAGE_SIZE}")

# Generate labels file
set(${use_case}_LABELS_CPP_FILE Labels)
generate_labels_code(
    INPUT           "${${use_case}_LABELS_TXT_FILE}"
    DESTINATION_SRC ${SRC_GEN_DIR}
    DESTINATION_HDR ${INC_GEN_DIR}
    OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE}"
)

...

# Generate model file
generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
    DESTINATION ${SRC_GEN_DIR}
)
```

> **Note:** When required, for models and labels conversion it's possible to add extra parameters such
> as extra code to put in `<model>.cc` file or namespaces.
>
> ```c++
> set(${use_case}_LABELS_CPP_FILE Labels)
> generate_labels_code(
>     INPUT           "${${use_case}_LABELS_TXT_FILE}"
>     DESTINATION_SRC ${SRC_GEN_DIR}
>     DESTINATION_HDR ${INC_GEN_DIR}
>     OUTPUT_FILENAME "${${use_case}_LABELS_CPP_FILE}"
>     NAMESPACE       "namespace1" "namespace2"
> )
>
> ...
>
> set(EXTRA_MODEL_CODE
>     "/* Model parameters for ${use_case} */"
>     "extern const int   g_myvariable2     = value1"
>     "extern const int   g_myvariable2     = value2"
> )
>
> generate_tflite_code(
>     MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
>     DESTINATION ${SRC_GEN_DIR}
>     EXPRESSIONS ${EXTRA_MODEL_CODE}
>     NAMESPACE   "namespace1" "namespace2"
> )
> ```

In addition to input file conversions, the correct platform/system profile is selected
(in `scripts/cmake/subsystem-profiles/*.cmake`) based on `TARGET_SUBSYSTEM` build option
and the variables set are used to generate memory region sizes, base addresses and IRQ numbers,
respectively used to generate mem_region.h, peripheral_irqs.h and peripheral_memmap.h headers.
Templates from `scripts/cmake/templates/*.template` are used to generate the header files.

After the build, the files generated in the build folder are:

```tree
build/generated/
├── bsp
│   ├── mem_regions.h
│   ├── peripheral_irqs.h
│   └── peripheral_memmap.h
├── <use_case_name1>
│   ├── include
│   │   ├── InputFiles.hpp
│   │   └── Labels.hpp
│   └── src
│       ├── <uc1_input_file1>.cc
│       ├── <uc1_input_file2>.cc
│       ├── InputFiles.cc
│       ├── Labels.cc
│       └── <uc1_model_name>.tflite.cc
└──  <use_case_name2>
    ├── include
    │   ├── InputFiles.hpp
    │   └── Labels.hpp
    └── src
        ├── <uc2_input_file1>.cc
        ├── <uc2_input_file2>.cc
        ├── InputFiles.cc
        ├── Labels.cc
        └── <uc2_model_name>.tflite.cc
```

Next section of the documentation: [Deployment](../documentation.md#Deployment).
