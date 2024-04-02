# Alif Semiconductor Ensemble  ML Embedded Evaluation Kit Examples

This repo contains different ML models that can be built to run on the Alif Beta AI/ML AppKit development board. It is based on the ARM ML embedded evaluation Kit.

For instructions on setting up the build environment and creating loadable images, please refer to the PDF file in the root directory [AUGD0011 ML Embedded Evaluation Kit v3.0.pdf](https://github.com/alifsemi/ml-embedded-evaluation-kit_DEV/blob/alif_main/AUGD0011%20ML%20Embedded%20Evaluation%20Kit%20v3.0.pdf).

The instructions and build by default is for Generation 2 device AppKit. To build for Generation 1 devices please use the _gen1_ branch and make sure TARGET_REVISION=A.

By default the build is for AppKit; for any other board set the TARGET_BOARD apropriately.

__IMPORTANT NOTE:__
Due to syncing up with the latest from Upstream ARM ML Embedded Evaluation Kit, some dependencies have changed.
1. The dependency on Python has been changed from 3.9 to Python 3.10. This may require you clean up existing setup and re-install version 3.10 again.
2. The ARM Embedded Compiler version now needs to be version 6.19

The original README file content from the Arm ML embedded evaluation kit can be found in the root directory as ARM_Readme.md.
