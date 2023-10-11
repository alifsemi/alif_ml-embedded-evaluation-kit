# Alif Semiconductor Ensemble  ML Embedded Evaluation Kit Examples

This repo contains different ML models that can be built to run on the Alif Beta AI/ML AppKit development board. It is based on the ARM ML embedded evaluation Kit.

For instructions on setting up the build environment and creating loadable images, please refer to the PDF file in the root directory [AUGD0011 ML Embedded Evaluation Kit v2.0.pdf](https://github.com/alifsemi/ml-embedded-evaluation-kit_DEV/blob/alif_main/AUGD0011%20ML%20Embedded%20Evaluation%20Kit%20v2.0.pdf).

The instructions and build by default is for Generation 1 device AppKit. To build for Generation 2 devices use -DTARGET_REVISION=B option while configuring with CMake. By default the build is for DevKit; for any other board set the TARGET_BOARD apropriately.

The original README file content from the Arm ML embedded evaluation kit can be found in the root directory as ARM_Readme.md.
