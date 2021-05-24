# Arm® ML embedded evaluation kit

- [Arm® ML embedded evaluation kit](#arm_ml-embedded-evaluation-kit)
  - [Trademarks](#trademarks)
  - [Prerequisites](#prerequisites)
    - [Additional reading](#additional-reading)
  - [Repository structure](#repository-structure)
  - [Models and resources](#models-and-resources)
  - [Building](#building)
  - [Deployment](#deployment)
  - [Implementing custom ML application](#implementing-custom-ml-application)
  - [Testing and benchmarking](#testing-and-benchmarking)
  - [Memory Considerations](#memory-considerations)
  - [Troubleshooting](#troubleshooting)
  - [Appendix](#appendix)

## Trademarks

- Arm® and Cortex® are registered trademarks of Arm® Limited (or its subsidiaries) in the US and/or elsewhere.
- Arm® and Ethos™ are registered trademarks or trademarks of Arm® Limited (or its subsidiaries) in the US and/or elsewhere.
- Arm® and Corstone™ are registered trademarks or trademarks of Arm® Limited (or its subsidiaries) in the US and/or elsewhere.
- TensorFlow™, the TensorFlow logo and any related marks are trademarks of Google Inc.

## Prerequisites

Before starting the setup process, please make sure that you have:

- Linux x86_64 based machine or Windows Subsystem for Linux is preferable.
  Unfortunately, Windows is not supported as a build environment yet.

- At least one of the following toolchains:
  - GNU Arm Embedded Toolchain (version 10.2.1 or above) - [GNU Arm Embedded Toolchain Downloads](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
  - Arm Compiler (version 6.14 or above) with a valid license - [Arm Compiler Download Page](https://developer.arm.com/tools-and-software/embedded/arm-compiler/downloads/)

- An Arm® MPS3 FPGA prototyping board and components for FPGA evaluation or a `Fixed Virtual Platform` binary:
  - An MPS3 board loaded with  Arm® Corstone™-300 reference package (`AN547`) from:
    <https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/download-fpga-images>.
    You would also need to have a USB connection between your machine and the MPS3 board - for UART menu and for
    deploying the application.
  - `Arm Corstone-300` based FVP for MPS3 is available from: <https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps>.

### Additional reading

This document contains information that is specific to Arm® Ethos™-U55 products.
See the following documents for other relevant information:

- ML platform overview: <https://mlplatform.org/>

- Arm® ML processors technical overview: <https://developer.arm.com/ip-products/processors/machine-learning>

- Arm® Cortex-M55® processor: <https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m55>

- ML processor, also referred to as a Neural Processing Unit (NPU) - Arm® Ethos™-U55:
    <https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u55>

- Arm® MPS3 FPGA Prototyping Board:
    <https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/mps3>

- Arm® ML-Zoo: <https://github.com/ARM-software/ML-zoo/>

See <http://developer.arm.com> for access to Arm documentation.

## Repository structure

The repository has the following structure:

```tree
.
├── dependencies
├── docs
├── model_conditioning_examples
├── resources
├── /resources_downloaded/
├── scripts
│   └── ...
├── source
│   ├── application
│   │ ├── hal
│   │ ├── main
│   │ └── tensorflow-lite-micro
│   └── use_case
│     └── <usecase_name>
│                ├── include
│                ├── src
│                └── usecase.cmake
├── tests
│   └── ...
└── CMakeLists.txt
```

Where:

- `dependencies`: contains all the third party dependencies for this project.

- `docs`: contains the documentation for this ML applications.

- `model_conditioning_examples`: contains short example scripts that demonstrate some methods available in TensorFlow 
    to condition your model in preparation for deployment on Arm Ethos NPU.

- `resources`: contains ML use cases applications resources such as input data, label files, etc.

- `resources_downloaded`: created by `set_up_default_resources.py`, contains downloaded resources for ML use cases 
    applications such as models, test data, etc.

- `scripts`: contains build related and source generation scripts.

- `source`: contains C/C++ sources for the platform and ML applications.
    Common code related to the Ethos-U55 NPU software
    framework resides in *application* sub-folder with the following
    structure:

  - `application`: contains all the sources that form the *core* of the application.
    The `use case` part of the sources depend on sources here.

    - `hal`: contains hardware abstraction layer sources providing a
        platform agnostic API to access hardware platform specific functions.

    - `main`: contains the main function and calls to platform initialization
          logic to set things up before launching the main loop.
          It also contains sources common to all use case implementations.

    - `tensorflow-lite-micro`: contains abstraction around TensorFlow Lite Micro API
          implementing common functions to initialize a neural network model, run an inference, and
          access inference results.

  - `use_case`: contains the ML use-case specific logic. Having this as a separate sub-folder isolates ML specific
    application logic with the assumption that the `application` will do all the required set up for logic here to run.
    It also makes it easier to add a new use case block.

  - `tests`: contains the x86 tests for the use case applications.

Hardware abstraction layer has the following structure:

```tree
hal
├── hal.c
├── include
│   └── ...
└── platforms
    ├── bare-metal
    │   ├── bsp
    │   │   ├── bsp-core
    │   │   │   └── include
    │   │   ├── bsp-packs
    │   │   │   └── mps3
    │   │   ├── cmsis-device
    │   │   ├── include
    │   │   └── mem_layout
    │   ├── data_acquisition
    │   ├── data_presentation
    │   │   ├── data_psn.c
    │   │   └── lcd
    │   │       └── include
    │   ├── images
    │   ├── timer
    │   └── utils
    └── native
```

- `include` and `hal.c`: contains the hardware abstraction layer (HAL) top level platform API and data acquisition, data
presentation and timer interfaces.
    > Note: the files here and lower in the hierarchy have been written in
    C and this layer is a clean C/C++ boundary in the sources.

- `platforms/bare-metal/data_acquisition`\
`platforms/bare-metal/data_presentation`\
`platforms/bare-metal/timer`\
`platforms/bare-metal/utils`: contains bare metal HAL support layer and platform initialisation helpers. Function calls
  are routed to platform specific logic at this level. For example, for data presentation, an `lcd` module has been used.
  This wraps the LCD driver calls for the actual hardware (for example MPS3).

- `platforms/bare-metal/bsp/bsp-packs`: contains the core low-level drivers (written in C) for the platform reside.
  For supplied examples this happens to be an MPS3 board, but support could be added here for other platforms too.
  The functions defined in this space are wired to the higher level functions under HAL (as those in `platforms/bare-metal/` level).

- `platforms/bare-metal/bsp/bsp-packs/mps3/include`\
`platforms/bare-metal/bsp/bsp-packs/mps3`: contains the peripheral (LCD, UART and timer) drivers specific to MPS3 board.

- `platforms/bare-metal/bsp/bsp-core`\
`platforms/bare-metal/bsp/include`: contains the BSP core sources common to all BSPs. These include a UART header
  (only the implementation of this is platform specific, but the API is common) and "re-targeting" of the standard output
  and error streams to the UART block.

- `platforms/bare-metal/bsp/cmsis-device`: contains the CMSIS template implementation for the CPU and also device
  initialisation routines. It is also where the system interrupts are set up and handlers are overridden.
  The main entry point of a bare metal application will most likely reside in this space. This entry point is
  responsible for setting up before calling the user defined "main" function in the higher level `application` logic.

- `platforms/bare-metal/bsp/mem_layout`: contains the platform specific linker scripts.

## Models and resources

The models used in the use cases implemented in this project can be downloaded
from [Arm ML-Zoo](https://github.com/ARM-software/ML-zoo/).

- [Mobilenet V2](https://github.com/ARM-software/ML-zoo/blob/master/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8).
- [DS-CNN](https://github.com/ARM-software/ML-zoo/blob/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8).
- [Wav2Letter](https://github.com/ARM-software/ML-zoo/tree/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8).
- [Anomaly Detection](https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/ad_medium_int8.tflite).

When using Ethos-U55 NPU backend, the NN model is assumed to be optimized by Vela compiler.
However, even if not, it will fall back on the CPU and execute, if supported by TensorFlow Lite Micro.

![Vela compiler](./media/vela_flow.jpg)

The Vela compiler is a tool that can optimize a neural network model
into a version that can run on an embedded system containing Ethos-U55 NPU.

The optimized model will contain custom operators for sub-graphs of the
model that can be accelerated by Ethos-U55 NPU, the remaining layers that
cannot be accelerated are left unchanged and will run on the CPU using
optimized (CMSIS-NN) or reference kernels provided by the inference
engine.

For detailed information see [Optimize model with Vela compiler](./sections/building.md#Optimize-custom-model-with-Vela-compiler).

## Building

This section describes how to build the code sample applications from sources - illustrating the build
options and the process.

The project can be built for MPS3 FPGA and FVP emulating MPS3. Default values for configuration parameters
will build executable models with Ethos-U55 NPU support.
See:

- [Building the ML embedded code sample applications from sources](./sections/building.md#building-the-ml-embedded-code-sample-applications-from-sources)
  - [Build prerequisites](./sections/building.md#build-prerequisites)
  - [Build options](./sections/building.md#build-options)
  - [Build process](./sections/building.md#build-process)
    - [Preparing build environment](./sections/building.md#preparing-build-environment)
      - [Fetching submodules](./sections/building.md#fetching-submodules)
      - [Fetching resource files](./sections/building.md#fetching-resource-files)
    - [Create a build directory](./sections/building.md#create-a-build-directory)
    - [Configuring the build for MPS3 SSE-300](./sections/building.md#configuring-the-build-for-mps3-sse-300)
      - [Using GNU Arm Embedded Toolchain](./sections/building.md#using-gnu-arm-embedded-toolchain)
      - [Using Arm Compiler](./sections/building.md#using-arm-compiler)
      - [Generating project for Arm Development Studio](./sections/building.md#generating-project-for-arm-development-studio)
      - [Working with model debugger from Arm FastModel Tools](./sections/building.md#working-with-model-debugger-from-arm-fastmodel-tools)
      - [Configuring with custom TPIP dependencies](./sections/building.md#configuring-with-custom-tpip-dependencies)
    - [Configuring native unit-test build](./sections/building.md#configuring-native-unit-test-build)
    - [Configuring the build for simple_platform](./sections/building.md#configuring-the-build-for-simple_platform)
    - [Building the configured project](./sections/building.md#building-the-configured-project)
  - [Building timing adapter with custom options](./sections/building.md#building-timing-adapter-with-custom-options)
  - [Add custom inputs](./sections/building.md#add-custom-inputs)
  - [Add custom model](./sections/building.md#add-custom-model)
  - [Optimize custom model with Vela compiler](./sections/building.md#optimize-custom-model-with-vela-compiler)
  - [Automatic file generation](./sections/building.md#automatic-file-generation)

## Deployment

This section describes how to deploy the code sample applications on the Fixed Virtual Platform or the MPS3 board.
See:

- [Deployment](./sections/deployment.md)
  - [Fixed Virtual Platform](./sections/deployment.md#fixed-virtual-platform)
    - [Setting up the MPS3 Corstone-300 FVP](./sections/deployment.md#setting-up-the-mps3-arm-corstone-300-fvp)
    - [Deploying on an FVP emulating MPS3](./sections/deployment.md#deploying-on-an-fvp-emulating-mps3)
  - [MPS3 board](./sections/deployment.md#mps3-board)
    - [Deployment on MPS3 board](./sections/deployment.md#deployment-on-mps3-board)

## Implementing custom ML application

This section describes how to implement a custom Machine Learning application running
on a platform supported by the repository (Fixed Virtual Platform or an MPS3 board).

Cortex-M55 CPU and Ethos-U55 NPU Code Samples software project offers a simple way to incorporate additional
use-case code into the existing infrastructure and provides a build
system that automatically picks up added functionality and produces
corresponding executable for each use-case.

See:

- [Implementing custom ML application](./sections/customizing.md)
  - [Software project description](./sections/customizing.md#software-project-description)
  - [HAL API](./sections/customizing.md#hal-api)
  - [Main loop function](./sections/customizing.md#main-loop-function)
  - [Application context](./sections/customizing.md#application-context)
  - [Profiler](./sections/customizing.md#profiler)
  - [NN Model API](./sections/customizing.md#nn-model-api)
  - [Adding custom ML use-case](./sections/customizing.md#adding-custom-ml-use-case)
  - [Implementing main loop](./sections/customizing.md#implementing-main-loop)
  - [Implementing custom NN model](./sections/customizing.md#implementing-custom-nn-model)
  - [Executing inference](./sections/customizing.md#executing-inference)
  - [Printing to console](./sections/customizing.md#printing-to-console)
  - [Reading user input from console](./sections/customizing.md#reading-user-input-from-console)
  - [Output to MPS3 LCD](./sections/customizing.md#output-to-mps3-lcd)
  - [Building custom use-case](./sections/customizing.md#building-custom-use-case)

## Testing and benchmarking

See [Testing and benchmarking](./sections/testing_benchmarking.md).

## Memory Considerations

See [Memory considerations](./sections/memory_considerations.md)

## Troubleshooting

See:

- [Troubleshooting](./sections/troubleshooting.md)
  - [Inference results are incorrect for my custom files](./sections/troubleshooting.md#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](./sections/troubleshooting.md#the-application-does-not-work-with-my-custom-model)

## Appendix

See:

- [Appendix](./sections/appendix.md)
  - [Cortex-M55 Memory map overview](./sections/appendix.md#arm_cortex_m55-memory-map-overview-for-corstone_300-reference-design)
