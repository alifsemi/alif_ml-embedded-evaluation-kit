# Building the ML embedded code sample applications from sources

- [Building the ML embedded code sample applications from sources](./building.md#building-the-ml-embedded-code-sample-applications-from-sources)
  - [Build prerequisites](./building.md#build-prerequisites)
    - [Third-party build prerequisites](./building.md#third-party-build-prerequisites)
  - [Build options](./building.md#build-options)
  - [Build process](./building.md#build-process)
    - [Preparing build environment](./building.md#preparing-build-environment)
      - [Fetching submodules](./building.md#fetching-submodules)
      - [Fetching resource files](./building.md#fetching-resource-files)
    - [Building for default configuration](./building.md#building-for-default-configuration)
    - [Create a build directory](./building.md#create-a-build-directory)
    - [Configuring the build for MPS3 SSE-300](./building.md#configuring-the-build-for-mps3-sse_300)
      - [Using GNU Arm Embedded toolchain](./building.md#using-gnu-arm-embedded-toolchain)
      - [Using Arm Compiler](./building.md#using-arm-compiler)
      - [Generating project for Arm Development Studio](./building.md#generating-project-for-arm-development-studio)
      - [Configuring with custom TPIP dependencies](./building.md#configuring-with-custom-tpip-dependencies)
    - [Configuring the build for MPS3 SSE-310](./building.md#configuring-the-build-for-mps3-sse_310)
    - [Configuring native unit-test build](./building.md#configuring-native-unit_test-build)
    - [Configuring the build for simple-platform](./building.md#configuring-the-build-for-simple_platform)
    - [Building with CMakePresets](./building.md#building-with-cmakepresets)
    - [Building the configured project](./building.md#building-the-configured-project)
  - [Building timing adapter with custom options](./timing_adapters.md#building-timing-adapter-with-custom-options)
  - [Add custom inputs](./building.md#add-custom-inputs)
  - [Add custom model](./building.md#add-custom-model)
  - [Optimize custom model with Vela compiler](./building.md#optimize-custom-model-with-vela-compiler)
  - [Building for different Ethos-U NPU variants](./building.md#building-for-different-ethos_u-npu-variants)
  - [Automatic file generation](./building.md#automatic-file-generation)

This section assumes that you are using an **x86_64 Linux** build machine.

## Build prerequisites

Before proceeding, it is *essential* to ensure that the following prerequisites have been fulfilled:

- GNU Arm embedded toolchain 10.2.1 (or higher) or the Arm Compiler version 6.16, or higher, is installed and available
  on the path. Test the compiler by running:

    ```commandline
    armclang -v
    ```

    ```log
    Product: ARM Compiler 6.16 Professional
    Component: ARM Compiler 6.16
    ```

> **Note:** To compile for Arm® Cortex™-M85 CPU (default CPU for Arm® Corstone-310), 6.18 is the minimum version required.

  Alternatively, use:

    ```commandline
    arm-none-eabi-gcc --version
    ```

    ```log
    arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10-2020-q4-major) 10.2.1 20201103 (release)
    Copyright (C) 2020 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    ```

> **Note:** If required, add the compiler to the path (can be added to ~/.bashrc to be set permanently):
>
> `export PATH=/path/to/armclang/bin:$PATH` OR `export PATH=/path/to/gcc-arm-none-eabi-toolchain/bin:$PATH`

- If you are using the proprietary Arm Compiler, ensure that the compiler license has been correctly configured.

- CMake version 3.21.0 or above is installed and available on the path. Test CMake by running:

    ```commandline
    cmake --version
    ```

    ```log
    cmake version 3.22.4
    ```

> **Note:** Required version of CMake is also installed in the Python3 virtual environment created by
> `setup_default_resources.py` script. See [Fetching resource files](./building.md#fetching-resource-files) section.
>
> To add CMake to the PATH on Ubuntu for example, use: `export PATH=/absolute/path/for/cmake/:${PATH}`
> Once `setup_default_resources.py` has been executed, activating the virtual environment will enable you
> to use that CMake. Alternatively, from the root of the repository, you could use:
>
> `export PATH=$(readlink -e resources_downloaded/env/bin):${PATH}`

- Python 3.7 or above is installed. Check your current installed version of Python by running:

    ```commandline
    python3 --version
    ```

    ```log
    Python 3.7.13
    ```

  > **Note:** If you have an older version of Python installed (< 3.7) see the
  > [Troubleshooting](./troubleshooting.md#how-to-update-python3-package-to-newer-version)
  > for instruction on how to update it.

- The build system creates a Python virtual environment during the build process. Please make sure that Python virtual
  environment module is installed by running:

    ```commandline
    python3 -m venv
    ```

  > **Note:** If you are using an Arm Virtual Hardware instance then Python virtual environment is not installed by default.
  > You will need to install it yourself by running the following command:
  >   ```commandline
  >    sudo apt install python3.8-venv
  >    ```

- The build system uses external Python libraries during the building process. Please make sure that the latest pip and
  libsndfile versions are installed.

  ```commandline
  pip3 --version
  ```

  ```log
  pip 9.0.1 from /usr/lib/python3/dist-packages (python 3.7)
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

- Access to the internet to download the third-party dependencies, specifically: TensorFlow Lite Micro, Arm®
  *Ethos™-U55* NPU driver, and CMSIS. Instructions for downloading these are listed under:
  [preparing build environment](./building.md#preparing-build-environment).

> **NOTE**: A Docker image built from the `Dockerfile` provided will have all the above packages installed and
> available for use. The GNU Embedded toolchain is available by default and the environment of the image will
> have certain helper variables set for running the FVP within a non-interactive terminal. Run `env` to see the
> list.

### Third-party build prerequisites

- The following software is needed by TensorFlow Lite Micro.

  - xxd
  - unzip
  - Python Pillow

> **Note:** Due to the fast paced nature of development, this list might not be exhaustive.
Please refer to Tensorflow Lite Micro documentation for more info.

## Build options

The project build system allows you to specify custom neural network models (in the `.tflite` format) for each use-case
along with the network inputs.

It also builds TensorFlow Lite for Microcontrollers library, Arm® *Ethos™-U* NPU driver library, and the CMSIS-DSP library
from sources.

The build script is parameterized to support different options (see [common_user_options.cmake](../../scripts/cmake/common_user_options.cmake)).
Default values for these parameters configure the build for all use-cases to be executed on an MPS3 FPGA or the Fixed Virtual
Platform (FVP) implementation of the Arm® *Corstone™-300* design.

The build parameters are:

- `TARGET_PLATFORM`: The target platform to execute the application on:
  - `mps3` (default)
  - `native`
  - `simple_platform`

- `TARGET_SUBSYSTEM`: The target platform subsystem. Specifies the design implementation for the deployment target. For
  both, the MPS3 FVP and the MPS3 FPGA, this must be left to the default value of SSE-300:
  - `sse-300` (default - [Arm® Corstone™-300](https://developer.arm.com/ip-products/subsystem/corstone/corstone-300))
  - `sse-310` (The FVP is available via Arm Virtual Hardware (AVH) hosted on AWS)

- `CMAKE_TOOLCHAIN_FILE`: This built-in CMake parameter can be used to override the default toolchain file used for the
  build. All the valid toolchain files are located in the scripts directory. For example, see:
  [bare-metal-gcc.cmake](../../scripts/cmake/toolchains/bare-metal-gcc.cmake).

- `TENSORFLOW_SRC_PATH`: the path to the root of the TensorFlow directory. The default value points to the
  `dependencies/tensorflow` git submodule. Repository is hosted here: [tensorflow](https://github.com/tensorflow/tensorflow)

- `ETHOS_U_NPU_DRIVER_SRC_PATH`: The path to the *Ethos-U* NPU core driver sources. The default value points to the
  `dependencies/core-driver` git submodule. Repository is hosted here:
  [ethos-u-core-driver](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-core-driver).

- `CMSIS_SRC_PATH`: The path to the CMSIS sources to be used to build TensorFlow Lite Micro library. This parameter is
  optional and is only valid for Arm® *Cortex®-M* CPU targeted configurations. The default value points to the
  `dependencies/cmsis` git submodule. Repository is hosted here: [CMSIS-5](https://github.com/ARM-software/CMSIS_5.git)

- `ETHOS_U_NPU_ENABLED`: Sets whether the use of *Ethos-U* NPU is available for the deployment target. By default, this
  is set and therefore application is built with *Ethos-U* NPU supported.

- `ETHOS_U_NPU_ID`: The *Ethos-U* NPU processor:
  - `U55` (default)
  - `U65`

- `ETHOS_U_NPU_MEMORY_MODE`:  The *Ethos-U* NPU memory mode:
  - `Shared_Sram` (default for *Ethos-U55* NPU)
  - `Dedicated_Sram` (default for *Ethos-U65* NPU)
  - `Sram_Only`

  > **Note:** The `Shared_Sram` memory mode is available on both *Ethos-U55* and *Ethos-U65* NPU, `Dedicated_Sram` only
  > for *Ethos-U65* NPU and `Sram_Only` only for *Ethos-U55* NPU.

- `ETHOS_U_NPU_CONFIG_ID`: This parameter is set by default based on the value of `ETHOS_U_NPU_ID`.
  For Ethos-U55, it defaults to the `H128` indicating that the Ethos-U55 128 MAC optimised model
  should be used. For Ethos-U65, it defaults to `Y256` instead. However, the user can override these
  defaults to a configuration ID from `H32`, `H64`, `H256` and `Y512`.

  > **Note:** This ID is only used to choose which tflite file path is to be used by the CMake
  > configuration for all the use cases. If the user has overridden use-case specific model path
  > parameter `ETHOS_U_NPU_CONFIG_ID` parameter will become irrelevant for that use-case. Also, the
  > model files for the chosen `ETHOS_U_NPU_CONFIG_ID` are expected to exist in the default locations.
  > See [Fetching resource files](./building.md#fetching-resource-files) for details on how to do this for your
  > chosen configuration.

- `ETHOS_U_NPU_CACHE_SIZE`: The *Ethos-U* NPU cache size used if the *Ethos-U* NPU processor selected with the option
  `ETHOS_U_NPU_ID` is `U65`. Default value is 393216 (see [default_vela.ini](../../scripts/vela/default_vela.ini) ).

- `CPU_PROFILE_ENABLED`: Sets whether profiling information for the CPU core should be displayed. By default, this is
  set to false, but can be turned on for FPGA targets. The FVP and the CPU core cycle counts are **not** meaningful and
  are not to be used.

- `LOG_LEVEL`: Sets the verbosity level for the output of the application over `UART`, or `stdout`. Valid values are:
  `LOG_LEVEL_TRACE`, `LOG_LEVEL_DEBUG`, `LOG_LEVEL_INFO`, `LOG_LEVEL_WARN`, and `LOG_LEVEL_ERROR`. The default is set
  to: `LOG_LEVEL_INFO`.

- `<use_case>_MODEL_TFLITE_PATH`: The path to the model file that is processed and is included into the application
  `axf` file. The default value points to one of the delivered set of models. Make sure that the model chosen is aligned
  with the `ETHOS_U_NPU_ENABLED` setting.

  - When using the *Ethos-U* NPU backend, the NN model is assumed to be optimized by Vela compiler. However, even if
    not, if it is supported by TensorFlow Lite Micro, it falls back on the CPU and execute.

  - When use of the *Ethos-U* NPU is disabled, and if a Vela optimized model is provided, then the application reports
    a failure at runtime.

- `USE_CASE_BUILD`: Specifies the list of applications to build. By default, the build system scans sources to identify
  available ML applications and produces executables for all detected use-cases. This parameter can accept single value,
  for example: `USE_CASE_BUILD=img_class`, or multiple values. For example: `USE_CASE_BUILD="img_class;kws"`.

- `ETHOS_U_NPU_TIMING_ADAPTER_SRC_PATH`: The path to timing adapter sources. The default value points to the
  `timing_adapter` dependencies folder.

- `ETHOS_U_NPU_TIMING_ADAPTER_ENABLED`: Specifies if the *Ethos-U* timing adapter is enabled.

  > **NOTE**: This setting is set to ON for all platforms if `ETHOS_U_NPU_ENABLED` is set. However, it is overridden
  > to OFF for Arm® Corstone-310 implementation.

- `TA_CONFIG_FILE`: The path to the CMake configuration file that contains the timing adapter parameters. Used only if
  the timing adapter build is enabled. Default for Ethos-U55 NPU is
  [ta_config_u55_high_end.cmake](../../scripts/timing_adapter/ta_config_u55_high_end.cmake),
  for Ethos-U65 NPU is [ta_config_u55_high_end.cmake](../../scripts/timing_adapter/ta_config_u55_high_end.cmake).

- `TENSORFLOW_LITE_MICRO_CLEAN_BUILD`: Optional parameter to enable, or disable, "cleaning" prior to building for the
  TensorFlow Lite Micro library. Enabled by default.

- `TENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS`: Optional parameter to enable wiping out `TPIP` downloads from TensorFlow
  source tree prior to each build. Disabled by default.

For details on the specific use-case build options, follow the instructions in the use-case specific documentation.

Also, when setting any of the CMake configuration parameters that expect a directory, or file, path, **use absolute
paths instead of relative paths**.

## Build process

The build process uses three major steps:

1. Prepare the build environment by downloading third-party sources required, see
   [Preparing build environment](./building.md#preparing-build-environment).

2. Configure the build for the platform chosen. This stage includes:
    - CMake options configuration
    - When `<use_case>_MODEL_TFLITE_PATH` build options are not provided, the default neural network models can be
      downloaded from [Arm ML-Zoo](https://github.com/ARM-software/ML-zoo). For native builds, the network input and
      output data for tests are downloaded.
    - Some files such as neural network models, network inputs, and output labels are automatically converted into C/C++
      arrays, see: [Automatic file generation](./building.md#automatic-file-generation).

3. Build the application.
   Application and third-party libraries are now built. For further information, see:
   [Building the configured project](./building.md#building-the-configured-project).

### Preparing build environment

#### Fetching submodules

Certain third-party sources are required to be present on the development machine to allow the example sources in this
repository to link against.

1. [TensorFlow Lite Micro repository](https://github.com/tensorflow/tensorflow)
2. [Ethos-U NPU core driver repository](https://review.mlplatform.org/admin/repos/ml/ethos-u/ethos-u-core-driver)
3. [CMSIS-5](https://github.com/ARM-software/CMSIS_5.git)
4. [Ethos-U NPU core driver repository](https://review.mlplatform.org/admin/repos/ml/ethos-u/ethos-u-core-platform)

> **Note:** If you are using non git project sources, run `python3 ./download_dependencies.py` and ignore further git
> instructions. Proceed to [Fetching resource files](./building.md#fetching-resource-files) section.

To pull the submodules:

```sh
git submodule update --init
```

This downloads all of the required components and places them in a tree, like so:

```tree
dependencies
    ├── cmsis
    ├── core-driver
    ├── core-platform
    └── tensorflow
```

> **Note:** The default source paths for the `TPIP` sources assume the above directory structure. However, all of the
> relevant paths can be overridden by CMake configuration arguments `TENSORFLOW_SRC_PATH` `ETHOS_U_NPU_DRIVER_SRC_PATH`,
> and `CMSIS_SRC_PATH`.

#### Fetching resource files

Every ML use-case example in this repository also depends on external neural network models. To download these, run the
following command from the root of the repository:

```sh
python3 ./set_up_default_resources.py
```

This fetches every model into the `resources_downloaded` directory. It also optimizes the models using the Vela compiler
for the default 128 MACs configuration of the Arm® *Ethos™-U55* NPU and for the default 256 MACs configuration of the
Arm® *Ethos™-U65* NPU.

> **Note:** This script requires Python version 3.7 or higher. Please make sure all [build prerequisites](./building.md#build-prerequisites)
> are satisfied.
>
> **Note:** This script also installs required version of CMake into the virtual environment, which can be used by activating it.

Additional command line arguments supported by this script are:

- `--additional-ethos-u-config-name`: if you need to optimize the models for a different Ethos-U configuration,
  you can pass a list of additional configurations for Vela compiler. For example, to optimize the downloaded models
  for *Ethos™-U55* 32 MAC configuration and *Ethos™-U65* 512 MAC configuration, you can choose to run:

  ```sh
  python3 ./set_up_default_resources.py \
    --additional-ethos-u-config-name ethos-u55-32 \
    --additional-ethos-u-config-name ethos-u65-512
  ```

  > **Note:** As the argument name suggests, the configuration names are **in addition to** the default ones: `ethos-u55-128`
  > and `ethosu-u65-256`.

- `--arena-cache-size`: the size of the arena cache memory area, in bytes.
  The default value is:
  - the internal SRAM size for Corstone-300 implementation on MPS3 specified by AN552,
  when optimizing for the default 128 MACs configuration of the Arm® *Ethos™-U55* NPU.
  - the default value specified in the Vela configuration file [default_vela.ini](../../scripts/vela/default_vela.ini),
  when optimizing for the default 256 MACs configuration of the Arm® *Ethos™-U65* NPU.

### Building for default configuration

A helper script `build_default.py` is provided to configure and build all the applications. It configures the project
with default settings i.e., for `mps3` target, `sse-300` subsystem and *Ethos-U55* timing-adapter settings.
Under the hood, it invokes all the necessary
CMake commands that are described in the next sections.

If using the `Arm GNU embedded toolchain`, execute:

```commandline
./build_default.py
```

If using the `Arm Compiler`, execute:

```commandline
./build_default.py --toolchain arm
```

Additional command line arguments supported by this script are:

- `--skip-download`: Do not download resources: models and test vectors
- `--skip-vela`: Do not run Vela optimizer on downloaded models.
- `--npu-config-name`: Arm Ethos-U configuration to build for. The default value is
    `ethos-u55-128`. Valid values are:
  - `ethos-u55-32`
  - `ethos-u55-64`
  - `ethos-u55-128`
  - `ethos-u55-256`
  - `ethos-u65-256`
  - `ethos-u65-512`
- `--make-jobs`: Specifies the number of concurrent jobs to use for compilation.
  The default value is equal to the number of cores in the system.
  Lowering this value can be useful in case of limited resources.
- `--make-verbose`: Make the compile process verbose. This is equal to run ```make VERBOSE=1```.

To build for *Ethos™-U55* 32 MAC configuration, using `Arm Compiler`, run:

```commandline
./build_default.py --npu-config-name ethos-u55-32 --toolchain arm
```

### Create a build directory

To configure the build project manually, create a build directory in the root of the project and navigate inside:

```commandline
mkdir build && cd build
```

### Configuring the build for MPS3 SSE-300

#### Using GNU Arm Embedded toolchain

On Linux, if using `Arm GNU embedded toolchain`, execute the following command to build the application to run on the
Arm® *Ethos™-U* NPU when providing only the mandatory arguments for CMake configuration:

```commandline
cmake ../
```

The preceding command builds for the default target platform `mps3`, the default subsystem `sse-300`, using the
default toolchain file for the target as `bare-metal-gcc` and the default *Ethos-U55* timing adapter settings.
This is equivalent to running:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DTA_CONFIG_FILE=scripts/cmake/timing_adapter/ta_config_u55_high_end.cmake
```

#### Using Arm Compiler

If using `Arm Compiler` to set the compiler and platform-specific parameters, the toolchain option
`CMAKE_TOOLCHAIN_FILE` can be used to point to the `ARMClang` CMake file, like so:

```commandline
cmake ../ -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

To configure a build that can be debugged using Arm Development Studio, specify the build type as `Debug`. For example:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug
```

#### Generating project for Arm Development Studio

To import the project into Arm Development Studio, add the Eclipse project generator and `CMAKE_ECLIPSE_VERSION` in the
CMake command.

It is advisable that the build directory is one level up relative to the source directory. When the build has been
generated, you must follow the Import wizard in Arm Development Studio and import the existing project into the
workspace.

You can then compile and debug the project using Arm Development Studio. Note that the following command is executed one
level up from the source directory:

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

#### Configuring with custom TPIP dependencies

If the TensorFlow source tree is not in its default expected location, set the path using `TENSORFLOW_SRC_PATH`.
Similarly, if the *Ethos-U* NPU driver and `CMSIS` are not in the default location, then use
`ETHOS_U_NPU_DRIVER_SRC_PATH` and `CMSIS_SRC_PATH` to configure their location.

For example:

```commandline
cmake .. \
    -DTENSORFLOW_SRC_PATH=/my/custom/location/tensorflow \
    -DETHOS_U_NPU_DRIVER_SRC_PATH=/my/custom/location/core-driver \
    -DCMSIS_SRC_PATH=/my/custom/location/cmsis
```

> **Note:** If re-building with changed parameters values, we recommend that you clean the build directory and re-run
> the CMake command.


### Configuring the build for MPS3 SSE-310

On Linux, execute the following command to build the application for target platform `mps3` and subsystem `sse-310`,
using the default toolchain file for the target as `bare-metal-gcc` and the default *Ethos-U55* timing adapter settings.
This is equivalent to running:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-gcc.cmake \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-310 \
    -DTA_CONFIG_FILE=scripts/cmake/timing_adapter/ta_config_u55_high_end.cmake
```

> **Note:** Only *Ethos-U55* timing adapter settings can be used.
> *Ethos-U65* is not supported for this subsystem.

### Configuring native unit-test build

```commandline
cmake ../ -DTARGET_PLATFORM=native
```

Results of the build are placed under the `build/bin/` folder. For example:

```tree
bin
├── arm_ml_embedded_evaluation_kit-<usecase1>-tests
├── arm_ml_embedded_evaluation_kit-<usecase2>-tests
├── ethos-u-<usecase1>
└── ethos-u-<usecase1>
```

### Configuring the build for simple-platform

```commandline
cmake ../ -DTARGET_PLATFORM=simple_platform
```

Again, if using `Arm Compiler`, use:

```commandline
cmake .. \
    -DTARGET_PLATFORM=simple_platform \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

### Building with CMakePresets

If you are using CMake version 3.21 or above, then an alternative method of building is by using CMakePresets.
This can be done by calling the following command:
```commandline
cmake --preset <platform>
cmake --build --preset <platform>
```
where platform is one of:
```commandline
native
mps3-gcc
mps3-clang
simple-gcc
simple-clang
```
This will automatically configure and build the evaluation-kit into a corresponding folder.
You can still pass in build flags as usual, for example:
```commandline
cmake --preset mps3-gcc -DUSE_CASE_BUILD=inference_runner
cmake --build --preset mps3-gcc
```
Alternatively, you can create a CMakeUserPresets.json in the root of the project with personal user settings, for example:
```commandline
{
  "version": 3,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 21,
    "patch": 0
  },
  "configurePresets": [
    {
      "name": "my-config",
      "displayName": "my-config",
      "inherits": ["mps3-300-gcc"],
      "cacheVariables": {
        "LOG_LEVEL": {
          "type": "STRING",
          "value": "LOG_LEVEL_DEBUG"
        }
      }
    }
  ],
  "buildPresets": [
    {
      "name": "mps3-300-gcc-custom",
      "displayName": "mps-300-gcc-custom",
      "configurePreset": "my-config",
      "targets": [
        "ethos-u-object_detection",
        "ethos-u-inference_runner"],
      "jobs": 12
    }
  ]
}
```

### Building the configured project

If the CMake command succeeds, build the application as follows:

```commandline
make -j4
```

To see compilation and link details, add `VERBOSE=1`.

Results of the build are placed underÂ `build/bin`Â folder, for example:

```tree
bin
 ├── ethos-u-<use_case_name>.axf
 ├── ethos-u-<use_case_name>.htm
 ├── ethos-u-<use_case_name>.map
 └── sectors
        ├── images.txt
        └── <use_case>
                ├── ddr.bin
                └── itcm.bin
```

Where for each implemented use-case under the `source/use-case` directory, the following build artifacts are created:

- `ethos-u-<use-case name>.axf`: The built application binary for an ML use-case.

- `ethos-u-<use-case name>.map`: Information from building the application. For example: Libraries used, what was
  optimized, and location of objects).

- `ethos-u-<use-case name>.htm`: Human readable file containing the call graph of application functions.

- `sectors/<use-case>`: Folder containing the built application. Split into files for loading into different FPGA
  memory regions.

- `images.txt`: Tells the FPGA which memory regions to use for loading the binaries in the `sectors/..`
  folder.

> **Note:**  For the specific use-case commands, refer to the relative section in the use-case documentation.

## Add custom inputs

The application performs inference on input data found in the folder set by the CMake parameters, for more information
see section 3.3 in the specific use-case documentation.

## Add custom model

The application performs inference using the model pointed to by the CMake parameter `MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using *Ethos-U* NPU, ensure that your custom model has been run through the
> Vela compiler successfully before continuing.

To run the application with a custom model, you must provide a `labels_<model_name>.txt` file of labels that are
associated with the model.

Each line of the file should correspond to one of the outputs in your model. See the provided
`labels_mobilenet_v2_1.0_224.txt` file in the `img_class` use-case for an example.

Then, you must set `<use_case>_MODEL_TFLITE_PATH` to the location of the Vela processed model file and
`<use_case>_LABELS_TXT_FILE` to the location of the associated labels file (if necessary), like so:

```commandline
cmake .. \
    -D<use_case>_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -D<use_case>_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DTARGET_PLATFORM=mps3 \
    -DTARGET_SUBSYSTEM=sse-300 \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

> **Note:** For the specific use-case command, refer to the relative section in the use-case documentation.
>
> **Note:** Clean the build directory before re-running the CMake command.

The TensorFlow Lite for Microcontrollers model pointed to by `<use_case>_MODEL_TFLITE_PATH` and the labels text file
pointed to by `<use_case>_LABELS_TXT_FILE` are converted to C++ files during the CMake configuration stage. They are
then compiled into the application for performing inference with.

The log from the configuration stage tells you what model path and labels file have been used. For example:

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

After compiling, your custom model has now replaced the default one in the application.

## Optimize custom model with Vela compiler

> **Note:** This tool is not available within this project. It is a Python tool available from
> <https://pypi.org/project/ethos-u-vela/>.
> The source code is hosted on <https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/>.

> **Note:** Using the 22.05 versions of software dependencies will require Vela to be at least version 3.4.0
> or you may encounter issues when trying to run applications on different variants of Ethos-U.

The Vela compiler is a tool that can optimize a neural network model into a version that can run on an embedded system
containing an *Ethos-U* NPU.

The optimized model contains custom operators for sub-graphs of the model that can be accelerated by the *Ethos-U*
NPU. The remaining layers that cannot be accelerated, are left unchanged and are run on the CPU using optimized, or
`CMSIS-NN`, or reference kernels that are provided by the inference engine.

After the compilation, the optimized model can only be executed on a system using an *Ethos-U* NPU.

> **Note:** The NN model provided during the build and compiled into the application executable binary defines whether
the CPU or NPU is used to execute workloads. If an unoptimized model is used, then inference runs on the *Cortex-M* CPU.

The Vela compiler accepts parameters to influence a model optimization. The model provided within this project has been
optimized with the following parameters:

```commandline
vela \
    --accelerator-config=ethos-u55-128 \
    --optimise Performance \
    --config my_vela_cfg.ini \
    --memory-mode Shared_Sram \
    --system-config Ethos_U55_High_End_Embedded \
    <model>.tflite
```

The Vela command contains the following:

- `--accelerator-config`: Specifies the accelerator configuration to use between `ethos-u55-256`, `ethos-u55-128`,
  `ethos-u55-64`, `ethos-u55-32`, `ethos-u65-256`, and `ethos-u65-512`.
- `--optimise`: Sets the optimisation strategy to Performance or Size. The Size strategy results in a model minimising
  the SRAM usage whereas the Performance strategy optimises the neural network for maximal performance.
  Note that if using the Performance strategy, you can also pass the `--arena-cache-size` option to Vela.
- `--config`: Specifies the path to the Vela configuration file. The format of the file is a Python ConfigParser `.ini`
    file. An example can be found in the `dependencies` folder [default_vela.ini](../../scripts/vela/default_vela.ini).
- `--memory-mode`: Selects the memory mode to use as specified in the Vela configuration file.
- `--system-config`: Selects the system configuration to use as specified in the Vela configuration file:
  `Ethos_U55_High_End_Embedded`for *Ethos-U55* and `Ethos_U65_High_End` for *Ethos-U65*.

Vela compiler accepts `.tflite` file as input and saves optimized network model as a `.tflite` file.

Using `--show-cpu-operations` and `--show-subgraph-io-summary` shows all the operations that fall back to the CPU. And
includes a summary of all the subgraphs and their inputs and outputs.

To see Vela helper for all the parameters use: `vela --help`.

> **Note:** By default, use of the *Ethos-U* NPU is enabled in the CMake configuration. This can be changed by passing
> `-DETHOS_U_NPU_ENABLED`.

## Building for different Ethos-U NPU variants

The building process described in the previous paragraphs assumes building for the default *Ethos-U55* NPU with 128 MACs,
using the *Ethos-U55* High End timing adapter system configuration.

To build for a different *Ethos-U* NPU variant:

- Optimize the model with Vela compiler with the correct parameters. See [Optimize custom model with Vela compiler](./building.md#optimize-custom-model-with-vela-compiler).
- Use the correct `ETHOS_U_NPU_ID`: `U55` for *Ethos-U55* NPU, `U65` for *Ethos-U65* NPU.
- Use the Vela model as custom model in the building command. See [Add custom model](./building.md#add-custom-model)
- Use the correct timing adapter settings configuration. See [Building timing adapter with custom options](./building.md#building-timing-adapter-with-custom-options)

For example, when building for *Ethos-U65* High End system configuration and 512 MACs/cc, the Vela command will be:

```commandline
vela \
    <model_file>.tflite \
    --accelerator-config ethos-u65-512 \
    --optimise Performance \
    --memory-mode=Dedicated_Sram \
    --system-config=Ethos_U65_High_End \
    --config=../scripts/vela/default_vela.ini
```

And the cmake command:

```commandline
cmake .. \
    -DETHOS_U_NPU_ID=U65 \
    -D<use_case>_MODEL_TFLITE_PATH=<path/to/ethos_u65_vela_model.tflite>
```

## Automatic file generation

As mentioned in the previous sections, some files such as neural network models, network inputs, and output labels are
automatically converted into C/C++ arrays during the CMake project configuration stage.

Also, some code is generated to allow access to these arrays.

For example:

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

In particular, the building options pointing to the input files `<use_case>_FILE_PATH`, the model
`<use_case>_MODEL_TFLITE_PATH`, and labels text file `<use_case>_LABELS_TXT_FILE` are used by Python scripts in order to
generate not only the converted array files, but also some headers with utility functions.

For example, the generated utility functions for image classification are:

- `build/generated/include/InputFiles.hpp`

    ```C++
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

    ```C++
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

These headers are generated using Python templates, that are located in `scripts/py/templates/*.template`:

```tree
scripts
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

Based on the type of use-case, the correct conversion is called in the use-case cmake file. Or, audio or image
respectively, for voice, or vision use-cases.

For example, the generations call for image classification, `source/use_case/img_class/usecase.cmake`, looks like:

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

> **Note:** When required, for models and labels conversion, it is possible to add extra parameters such as extra code
> to put in `<model>.cc` file or namespaces.
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

After the build, the files generated in the build folder are:

```tree
build/generated/
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

The next section of the documentation details: [Deployment](deployment.md).
