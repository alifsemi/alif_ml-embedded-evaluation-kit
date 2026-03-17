# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [26.03]

### Added
- MLEK Zephyr module to allow platform-agnostic library to be used in Zephyr-based projects
- Linker script override options at global and use-case level
- Configure-time and runtime detection of the model's intended target to highlight any mismatches with build parameters
- Improved visual output for the ExecuTorch Conformer ASR use case
- AGENTS.md to support coding agents
- Pre-commit configuration

### Changed
- Updated to 26.02 NPU components and dependencies (core-driver, core-platform, Vela 5.0.0, CMSIS-6, CMSIS-DSP, CMSIS-NN, TensorFlow Lite Micro and ExecuTorch 1.1.0)
- Refactored the source and CMake layout to separate application and library code to better-support inclusion in external projects
- Refactored Python tooling into an installable `mlek_tools` package
- Improved detection and reporting of ExecuTorch model memory mode, memory use and ops
- Improved error handling when ExecuTorch model memory requirements exceed limits

### Fixed
- Replaced the Yolo-Fastest model download URL with a more suitable alternative

## [25.12]

### Added
- Support for ExecuTorch framework alongside TensorFlow Lite Micro
- ASR use case updated to allow building with Conformer model via ExecuTorch
- Image Classification use case support for ExecuTorch version of MobileNet V2 and DeiT tiny
- Multi-threaded model download and optimisation in set up script

### Changed
- Updated to 25.08 NPU components and dependencies (core-driver, core-platform, Vela 4.4.1, CMSIS-6, CMSIS-DSP, CMSIS-NN, TensorFlow Lite Micro and ExecuTorch 1.0.0)

## [25.05]

### Added
- Support for LLVM-based Arm Toolchain for Embedded (ATfE) versions 19.1.5, 20.0.0 and 20.1.0
- Interactive mode option to allow user control when running use cases

### Changed
- Updated bare-metal toolchain files to always add function and data sections for both toolchains
- Updated CMSIS-DSP build to only build required sources by default
- General documentation updates: fixed broken links, added macOS support commands for FPGA deployment, updated URLs to GitLab

## [25.02]

### Changed
- Updated to 25.02 NPU components and dependencies (core-driver, core-platform, Vela 4.2.0, CMSIS-6, CMSIS-DSP, CMSIS-NN and TensorFlow Lite Micro)
- Minor update to generic BSP with `CMSIS_device_header` definition preferred to `CPU_HEADER_FILE`
- Removed Arm Virtual Hardware references from documentation

### Fixed
- CMSIS-NN usage in TensorFlow Lite Micro (bug introduced in Sep 2024)

## [24.11]

### Added
- Support for Arm Corstone-320 FVP, including Arm Ethos-U85 NPU
- HAL APIs for obtaining camera and audio data for use case apps
- Support for Arm Virtual Streaming Interface (VSI) FVP feature as camera and audio data source

### Changed
- Updated to 24.11 NPU components and dependencies (core-driver, core-platform, Vela 4.1.0, CMSIS-6, CMSIS-DSP, CMSIS-NN and TensorFlow Lite Micro)
- Moved to C11 and C++17 standards

### Removed
- Dropped CMSIS-5 support; now using CMSIS-6 exclusively

## [24.08]

### Added
- BatchMatMul to all-ops resolver
- Setup script now allows download location to be specified

### Changed
- Updated to 24.08 NPU components and dependencies (core-driver, core-platform, Vela 4.0.0, CMSIS, CMSIS-NN and TensorFlow Lite Micro)
- Test file generation supports types other than int8

## [24.05]

### Added
- Support for Arm Corstone-315 targets

### Changed
- Updated to 24.05 NPU components and dependencies (core-driver, core-platform, Vela 3.12.0, CMSIS, CMSIS-NN and TensorFlow Lite Micro)
- CMSIS pack refresh (version from 23.02 added)
- CMake user configuration options refactored
- Dockerfile updates including FVP versions downloaded
- Minor improvements to ease integration for cases where this project is a dependency

## [23.11]

### Added
- Support for Arm Cortex-M85 with GCC

### Changed
- Updated to 23.11 NPU components and dependencies (core-driver, core-platform, Vela 3.10.0, CMSIS, CMSIS-NN and TensorFlow Lite Micro)
- Minimum Python version increased to 3.10
- Updated Python files to conform to Pylint

## [23.08]

### Changed
- Updated to 23.08 NPU components and dependencies (core-driver, core-platform, Vela 3.9.0, CMSIS, CMSIS-NN and TensorFlow Lite Micro)
- Updated Python package requirements for compatibility
- Minimum Armclang version increased to 6.19

## [23.05]

### Changed
- Updated to 23.05 NPU components and dependencies (core-driver, core-platform, Vela 3.8.0, CMSIS, CMSIS-NN and TensorFlow Lite Micro)
- Minor updates to CMSIS-pack dependencies
- Updated Python package requirements for compatibility
- Minimum Python version increased to 3.9

## [23.02]

### Changed
- Updated to 23.02 NPU components and dependencies (core-driver, core-platform, Vela 3.7.0, CMSIS, CMSIS-DSP, CMSIS-NN and TensorFlow Lite Micro)
- Improvement to PMU counters: reducing cache maintenance burden for Arm Cortex-M55 CPUs when NPU is used
- Inclusive language updates
- Documentation updates including links to AN555 resources
- Updates to Python requirements file for virtual environment

## [22.11]

### Added
- Formal support for CMake presets
- Support for Arm Compiler 6.19
- Published CMSIS-Pack under `resources`

### Changed
- Updated to 22.11 NPU components and dependencies (core-driver, core-platform, Vela 3.6.0, CMSIS, TensorFlow Lite Micro)
- CMSIS-NN added as a new dependency (moved out of core CMSIS repository)
- Updates for Arm Corstone-310 platform

### Fixed
- Bug fix for building with NPU disabled

## [22.08]

### Added
- Formal support for Arm Corstone-310 Arm Virtual Hardware and FPGA implementations
- CMake presets
- Provision to allow platform-agnostic use case API to be packaged as CMSIS pack

### Changed
- Updated to 22.08 NPU components and dependencies (core-driver, core-platform, Vela 3.5.0, CMSIS, TensorFlow Lite Micro)

### Fixed
- Arm Ethos-U65 NPU default AXI burst length

## [22.05]

### Added
- Support for Arm Cortex-M85 (Armclang 6.18 required)

### Changed
- Updated to 22.05 NPU components and dependencies (core-driver, core-platform, Vela 3.4.0, CMSIS, TensorFlow Lite Micro)
- Restructured repository sources to allow generation of CMSIS packs
- Minimum Python version reduced to 3.7 (aligned with Vela 3.4.0)
- Minimum CMake version increased to 3.21.0

### Fixed
- Various bug fixes

## [22.02]

### Added
- Object Detection use case

### Changed
- Updated to 22.02 NPU components and dependencies (core-driver, core-platform, Vela 3.3.0, CMSIS, TensorFlow Lite Micro)
- Replaced DSCNN with MicroNet for KWS and KWS_ASR use cases
- Minimum CMake version increased to 3.15.6, Armclang to 6.16, Python to 3.8
- Initial restructuring of repository sources

### Fixed
- Various minor bug fixes

## [21.11]

### Added
- Support for different memory modes: Shared_Sram, Dedicated_Sram and Sram_Only
- Noise Reduction use case
- Dynamic load support for FVP for inference runner use case

### Changed
- Updated to 21.11 NPU components and dependencies (core-software, core-driver, Vela 3.2.0, CMSIS, TensorFlow Lite Micro)
- Updated support for Arm GNU Embedded Toolchain 10.3-2021.07 and Arm Compiler 6.17
- Changes to support AN552 design (new Arm Corstone-300 implementation)

## [21.08]

### Added
- Visual Wake Word use case

### Changed
- Updated to 21.05 NPU components (core-software, core-driver, Vela 3.0.0)
- TensorFlow submodule changed to https://github.com/tensorflow/tflite-micro
- Image classification model changed (from uint8 to int8 datatype)
- Added support for Corstone-300 + Arm Ethos-U65

## [21.05]

### Added
- Script to download and optimize default models
- Script to run default build flow
- Model for Anomaly Detection use case
- Support for build with Arm GNU Embedded Toolchain (10.2.1)

### Deprecated
- Support for target subsystem SSE-200

## [21.03]

### Added
- Simple platform support
- Model conditioning examples
- Tests for native platform
- Anomaly detection use case

### Changed
- Build changed to use sources of the dependency libraries

## [20.11]

### Added
- SSE-200 and SSE-300 system support
- Models with multiple output tensors support
- Generic inference runner use case
- ASR triggered by KWS in combined use case (kws_asr)

### Changed
- Build cmake parameters changed: TARGET_SUBSYSTEM added, TARGET_PLATFORM accepted values changed

### Removed
- Support for simple fixed virtual platform for Arm Ethos-U55 and Arm Cortex-M55

## [20.09]

### Added
- Speech recognition use case

### Changed
- Updated to TensorFlow Lite Micro version > 2.3.0
- Updated NPU Fastmodel version to r0p2-00eac0-rc4

## [20.08]

### Added
- Keyword spotting use case
- Person detection use case

### Known Issues
- Telnet connection to FastModel environment may stop responding after some period of inactivity

## [20.05]

### Added
- MPS3 FPGA build support
- Configurable timing adaptor
- Active and Idle cycle counts for NPU and CPU profiling report
- Windows support for build scripts

### Changed
- FastModel environment built with FastModel Tools v11.10.22
- Source code structure and build scripts refactored to support multiple ML use cases
- Using EAC NPU software model and drivers

### Known Issues
- Telnet connection to FastModel environment may stop responding after some period of inactivity
