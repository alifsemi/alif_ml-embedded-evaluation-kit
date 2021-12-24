# Arm® ML embedded evaluation kit

- [Arm® ML embedded evaluation kit](./documentation.md#arm_ml-embedded-evaluation-kit)
  - [Trademarks](./documentation.md#trademarks)
  - [Prerequisites](./documentation.md#prerequisites)
    - [Additional reading](./documentation.md#additional-reading)
  - [Repository structure](./documentation.md#repository-structure)
  - [Models and resources](./documentation.md#models-and-resources)
  - [Building](./documentation.md#building)
  - [Deployment](./documentation.md#deployment)
  - [Implementing custom ML application](./documentation.md#implementing-custom-ml-application)
  - [Testing and benchmarking](./documentation.md#testing-and-benchmarking)
  - [Memory Considerations](./documentation.md#memory-considerations)
  - [Troubleshooting](./documentation.md#troubleshooting)
  - [Appendix](./documentation.md#appendix)

## Trademarks

- Arm® and Cortex® are registered trademarks of Arm® Limited (or its subsidiaries) in the US and/or elsewhere.
- Arm® and Ethos™ are registered trademarks or trademarks of Arm® Limited (or its subsidiaries) in the US and/or
  elsewhere.
- Arm® and Corstone™ are registered trademarks or trademarks of Arm® Limited (or its subsidiaries) in the US and/or
  elsewhere.
- TensorFlow™, the TensorFlow logo, and any related marks are trademarks of Google Inc.

## Prerequisites

Before starting the setup process, please make sure that you have:

- A Linux x86_64 based machine, the Windows Subsystem for Linux is preferable.
  > **Note:** Currently, Windows is not supported as a build environment.

- At least one of the following toolchains:
  - GNU Arm Embedded toolchain (version 10.2.1 or above) -
  [GNU Arm Embedded toolchain downloads](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
  - Arm Compiler (version 6.15 or above) with a valid license -
  [Arm Compiler download Page](https://developer.arm.com/tools-and-software/embedded/arm-compiler/downloads)

- An Arm® MPS3 FPGA prototyping board and components for FPGA evaluation or a `Fixed Virtual Platform` binary:
  - An MPS3 board loaded with Arm® Corstone™-300 reference package (`AN552`) from:
    <https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/download-fpga-images>. You
    must have a USB connection between your machine and the MPS3 board - for UART menu and for deploying the
    application.
  - `Arm Corstone-300` based FVP for MPS3 is available from:
    <https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps>.

> **NOTE**: There are two Arm® Corstone™-300 implementations available for the MPS3 FPGA board - application
> notes `AN547` and `AN552`. We are aligned with the latest application note `AN552`. However, the application built
> for MPS3 target should work on both FPGA packages.

### Additional reading

This document contains information that is specific to Arm® Ethos™-U55 and Arm® Ethos™-U65 products. Please refer to the following documents
for additional information:

- ML platform overview: <https://mlplatform.org/>

- Arm® ML processors technical overview: <https://developer.arm.com/ip-products/processors/machine-learning>

- Arm® `Cortex-M55`® processor: <https://www.arm.com/products/silicon-ip-cpu/cortex-m/cortex-m55>

- ML processor, also referred to as a Neural Processing Unit (NPU) - Arm® `Ethos™-U55`:
    <https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u55>

- ML processor, also referred to as a Neural Processing Unit (NPU) - Arm® `Ethos™-U65`:
    <https://www.arm.com/products/silicon-ip-cpu/ethos/ethos-u65>

- Arm® MPS3 FPGA Prototyping Board:
    <https://developer.arm.com/tools-and-software/development-boards/fpga-prototyping-boards/mps3>

- Arm® ML-Zoo: <https://github.com/ARM-software/ML-zoo/>

- Arm® Ethos-U software: <https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u>

To access Arm documentation online, please visit: <http://developer.arm.com>

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
│          ├── include
│          ├── src
│          └── usecase.cmake
├── tests
│   └── ...
└── CMakeLists.txt
```

What these folders contain:

- `dependencies`: All the third-party dependencies for this project.

- `docs`: The documentation for this ML application.

- `model_conditioning_examples`: short example scripts that demonstrate some methods available in TensorFlow
    to condition your model in preparation for deployment on Arm Ethos NPU.

- `resources`: contains ML use-cases applications resources such as input data, label files, etc.

- `resources_downloaded`: created by `set_up_default_resources.py`, contains downloaded resources for ML use-cases
    applications such as models, test data, etc.

- `scripts`: Build and source generation scripts.

- `source`: C/C++ sources for the platform and ML applications.
  > **Note:** Common code related to the `Ethos-U` NPU software framework resides in *application* subfolder.

  The contents of the *application* subfolder is as follows:

  - `application`: All sources that form the *core* of the application. The `use-case` part of the sources depend on the
    sources themselves, such as:

    - `hal`: Contains Hardware Abstraction Layer (HAL) sources, providing a platform agnostic API to access hardware
      platform-specific functions.

    - `main`: Contains the main function and calls to platform initialization logic to set up things before launching
      the main loop. Also contains sources common to all use-case implementations.

    - `tensorflow-lite-micro`: Contains abstraction around TensorFlow Lite Micro API. This abstraction implements common
      functions to initialize a neural network model, run an inference, and access inference results.

  - `use_case`: Contains the ML use-case specific logic. Stored as a separate subfolder, it helps isolate the
    ML-specific application logic. With the assumption that the `application` performs the required setup for logic to
    run. It also makes it easier to add a new use-case block.

  - `tests`: Contains the x86 tests for the use-case applications.

The HAL has the following structure:

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

What these folders contain:

- The folders `include` and `hal.c` contain the HAL top-level platform API and data acquisition, data presentation, and
  timer interfaces.
    > **Note:** the files here and lower in the hierarchy have been written in C and this layer is a clean C/ + boundary
    > in the sources.

- `platforms/bare-metal/data_acquisition`\
  `platforms/bare-metal/data_presentation`\
  `platforms/bare-metal/timer`\
  `platforms/bare-metal/utils`:

  These folders contain the bare-metal HAL support layer and platform initialization helpers. Function calls are routed
  to platform-specific logic at this level. For example, for data presentation, an `lcd` module has been used. This
  `lcd` module wraps the LCD driver calls for the actual hardware (for example, MPS3).

- `platforms/bare-metal/bsp/bsp-packs`: The core low-level drivers (written in C) for the platform reside. For supplied
  examples, this happens to be an MPS3 board. However, support can be added here for other platforms. The functions
  defined in this space are wired to the higher-level functions under HAL and is like those in the
  `platforms/bare-metal/` level).

- `platforms/bare-metal/bsp/bsp-packs/mps3/include`\
  `platforms/bare-metal/bsp/bsp-packs/mps3`: Contains the peripheral (LCD, UART, and timer) drivers specific to MPS3
  board.

- `platforms/bare-metal/bsp/bsp-core`\
  `platforms/bare-metal/bsp/include`: Contains the BSP core sources common to all BSPs and includes a UART header.
  However, the implementation of this is platform-specific, while the API is common. Also "re-targets" the standard
  output and error streams to the UART block.

- `platforms/bare-metal/bsp/cmsis-device`: Contains the CMSIS template implementation for the CPU and also device
  initialization routines. It is also where the system interrupts are set up and the handlers are overridden. The main
  entry point of a bare-metal application most likely resides in this space. This entry point is responsible for the
  set-up before calling the user defined "main" function in the higher-level `application` logic.

- `platforms/bare-metal/bsp/mem_layout`: Contains the platform-specific linker scripts.

## Models and resources

The models used in the use-cases implemented in this project can be downloaded from: [Arm ML-Zoo](https://github.com/ARM-software/ML-zoo).

- [Mobilenet V2](https://github.com/ARM-software/ML-zoo/tree/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8)
- [MicroNet for Keyword Spotting](https://github.com/ARM-software/ML-zoo/tree/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8)
- [Wav2Letter](https://github.com/ARM-software/ML-zoo/tree/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8)
- [MicroNet for Anomaly Detection](https://github.com/ARM-software/ML-zoo/tree/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8)
- [MicroNet for Visual Wake Word](https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/vww4_128_128_INT8.tflite)
- [RNNoise](https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/rnnoise_INT8.tflite)

When using *Ethos-U* NPU backend, Vela compiler optimizes the the NN model. However, if not and it is supported by
TensorFlow Lite Micro, then it falls back on the CPU and execute.

![Vela compiler](./media/vela_flow.jpg)

The Vela compiler is a tool that can optimize a neural network model into a version that run on an embedded system
containing the *Ethos-U* NPU.

The optimized model contains custom operators for sub-graphs of the model that the *Ethos-U* NPU can accelerate. The
remaining layers that cannot be accelerated, are left unchanged, and are run on the CPU using optimized, `CMSIS-NN`, or
reference kernels provided by the inference engine.

For detailed information, see: [Optimize model with Vela compiler](./sections/building.md#Optimize-custom-model-with-Vela-compiler).

## Building

This section describes how to build the code sample applications from sources and includes illustrating the build
options and the process.

The project can be built for MPS3 FPGA and FVP emulating MPS3. Using default values for configuration parameters builds
executable models that support the *Ethos-U* NPU.

For further information, please see:

- [Building the ML embedded code sample applications from sources](./sections/building.md#building-the-ml-embedded-code-sample-applications-from-sources)
  - [Build prerequisites](./sections/building.md#build-prerequisites)
  - [Build options](./sections/building.md#build-options)
  - [Build process](./sections/building.md#build-process)
    - [Preparing build environment](./sections/building.md#preparing-build-environment)
      - [Fetching submodules](./sections/building.md#fetching-submodules)
      - [Fetching resource files](./sections/building.md#fetching-resource-files)
    - [Create a build directory](./sections/building.md#create-a-build-directory)
    - [Configuring the build for MPS3 SSE-300](./sections/building.md#configuring-the-build-for-mps3-sse_300)
      - [Using GNU Arm embedded toolchain](./sections/building.md#using-gnu-arm-embedded-toolchain)
      - [Using Arm Compiler](./sections/building.md#using-arm-compiler)
      - [Generating project for Arm Development Studio](./sections/building.md#generating-project-for-arm-development-studio)
      - [Working with model debugger from Arm Fast Model Tools](./sections/building.md#working-with-model-debugger-from-arm-fast-model-tools)
      - [Configuring with custom TPIP dependencies](./sections/building.md#configuring-with-custom-tpip-dependencies)
    - [Configuring native unit-test build](./sections/building.md#configuring-native-unit_test-build)
    - [Configuring the build for simple-platform](./sections/building.md#configuring-the-build-for-simple_platform)
    - [Building the configured project](./sections/building.md#building-the-configured-project)
  - [Building timing adapter with custom options](./sections/building.md#building-timing-adapter-with-custom-options)
  - [Add custom inputs](./sections/building.md#add-custom-inputs)
  - [Add custom model](./sections/building.md#add-custom-model)
  - [Optimize custom model with Vela compiler](./sections/building.md#optimize-custom-model-with-vela-compiler)
  - [Building for different Ethos-U NPU variants](./sections/building.md#building-for-different-ethos_u-npu-variants)
  - [Automatic file generation](./sections/building.md#automatic-file-generation)

## Deployment

This section describes how to deploy the code sample applications on the Fixed Virtual Platform (FVP) or the MPS3 board.

For further information, please see:

- [Deployment](./sections/deployment.md#deployment)
  - [Fixed Virtual Platform](./sections/deployment.md#fixed-virtual-platform)
    - [Setting up the MPS3 Corstone-300 FVP](./sections/deployment.md#setting-up-the-mps3-arm-corstone_300-fvp)
    - [Deploying on an FVP emulating MPS3](./sections/deployment.md#deploying-on-an-fvp-emulating-mps3)
  - [MPS3 board](./sections/deployment.md#mps3-board)
    - [Deployment on MPS3 board](./sections/deployment.md#deployment-on-mps3-board)

## Implementing custom ML application

This section describes how to implement a custom Machine Learning application running on a platform supported by the
repository, either an FVP or an MPS3 board.

Both the *Cortex-M55* CPU and *Ethos-U* NPU Code Samples software project offers a way to incorporate extra use-case
code into the existing infrastructure. It also provides a build system that automatically picks up added functionality
and produces corresponding executable for each use-case.

For further information, please see:

- [Implementing custom ML application](./sections/customizing.md#implementing-custom-ml-application)
  - [Software project description](./sections/customizing.md#software-project-description)
  - [Hardware Abstraction Layer API](./sections/customizing.md#hardware-abstraction-layer-api)
  - [Main loop function](./sections/customizing.md#main-loop-function)
  - [Application context](./sections/customizing.md#application-context)
  - [Profiler](./sections/customizing.md#profiler)
  - [NN Model API](./sections/customizing.md#nn-model-api)
  - [Adding custom ML use-case](./sections/customizing.md#adding-custom-ml-use_case)
  - [Implementing main loop](./sections/customizing.md#implementing-main-loop)
  - [Implementing custom NN model](./sections/customizing.md#implementing-custom-nn-model)
  - [Executing inference](./sections/customizing.md#executing-inference)
  - [Printing to console](./sections/customizing.md#printing-to-console)
  - [Reading user input from console](./sections/customizing.md#reading-user-input-from-console)
  - [Output to MPS3 LCD](./sections/customizing.md#output-to-mps3-lcd)
  - [Building custom use-case](./sections/customizing.md#building-custom-use_case)

## Testing and benchmarking

Please refer to: [Testing and benchmarking](./sections/testing_benchmarking.md#testing-and-benchmarking).

## Memory Considerations

Please refer to:

- [Memory considerations](./sections/memory_considerations.md#memory-considerations)
  - [Understanding memory usage from Vela output](./sections/memory_considerations.md#understanding-memory-usage-from-vela-output)
    - [Total SRAM used](./sections/memory_considerations.md#total-sram-used)
    - [Total Off-chip Flash used](./sections/memory_considerations.md#total-off_chip-flash-used)
  - [Memory mode configurations](./sections/memory_considerations.md#memory-mode-configurations)
  - [Tensor arena and neural network model memory placement](./sections/memory_considerations.md#tensor-arena-and-neural-network-model-memory-placement)
  - [Memory usage for ML use-cases](./sections/memory_considerations.md#memory-usage-for-ml-use_cases)
  - [Memory constraints](./sections/memory_considerations.md#memory-constraints)

## Troubleshooting

For further information, please see:

- [Troubleshooting](./sections/troubleshooting.md#troubleshooting)
  - [Inference results are incorrect for my custom files](./sections/troubleshooting.md#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](./sections/troubleshooting.md#the-application-does-not-work-with-my-custom-model)
  - [NPU configuration mismatch error when running inference](./sections/troubleshooting.md#npu-configuration-mismatch-error-when-running-inference)

## Appendix

Please refer to:

- [Appendix](./sections/appendix.md#appendix)
  - [Cortex-M55 Memory map overview](./sections/appendix.md#arm_cortex_m55-memory-map-overview-for-corstone_300-reference-design)

## FAQ

Please refer to: [FAQ](./sections/faq.md#faq)
