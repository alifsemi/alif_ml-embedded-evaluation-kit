# Alif Semiconductor ML Embedded Evaluation Kit Examples

Built on the Arm ML Embedded Evaluation Kit, this repository provides a collection of ML models that can be compiled and run on Alif development boards.

For instructions on setting up the build environment and creating loadable images, please refer to the [ML Embedded Evaluation Kit document](ML_Embedded_Evaluation_Kit.md) file in the root directory.

The instructions and build by default is for Generation 2 device AppKit.

test CI pr from public repo!

By default the build is for AppKit; for any other board set the `TARGET_BOARD` apropriately.
The default camera for the build is `MT9M114`. If you have `ARX3A0` camera, set the `ALIF_CAMERA_MODULE` accordingly (`-DALIF_CAMERA_MODULE=ARX3A0`).

Supported camera modules:
- MT9M114 **This is the default camera**
- ARX3A0
- OV5675

The original README file content from the Arm ML embedded evaluation kit can be found in the root directory as ARM_Readme.md.
