# Alif Semiconductor Ensemble  ML Embedded Evaluation Kit Examples

This repo contains different ML models that can be built to run on the Alif Beta AI/ML AppKit development board. It is based on the ARM ML embedded evaluation Kit.

For instructions on setting up the build environment and creating loadable images, please refer to the [ML Embedded Evaluation Kit document](ML_Embedded_Evaluation_Kit.md) file in the root directory.

The instructions and build by default is for Generation 2 device AppKit.

By default the build is for AppKit; for any other board set the `TARGET_BOARD` apropriately.
The default camera for the build is `MT9M114`. If you have `ARX3A0` camera, set the `ENSEMBLE_CAMERA_MODULE` accordingly (`-DENSEMBLE_CAMERA_MODULE=ARX3A0`).

Supported camera modules:
- MT9M114 **This is the default camera**
- ARX3A0 **This camera module has also been shipped with Alif Semiconductor kits**

__IMPORTANT NOTE:__
Due to syncing up with the latest from Upstream ARM ML Embedded Evaluation Kit, some dependencies have changed.
1. The dependency on Python has been changed from 3.9 to Python 3.10. This may require you clean up existing setup and re-install version 3.10 again.
2. The ARM Embedded Compiler version now needs to be version 6.19 or newer

The original README file content from the Arm ML embedded evaluation kit can be found in the root directory as ARM_Readme.md.
