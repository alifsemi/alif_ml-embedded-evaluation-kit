# Troubleshooting

- [Troubleshooting](#troubleshooting)
  - [Inference results are incorrect for my custom files](#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](#the-application-does-not-work-with-my-custom-model)
  - [NPU configuration mismatch error when running inference](#npu-configuration-mismatch-error-when-running-inference)

## Inference results are incorrect for my custom files

Ensure that the files you are using match the requirements of the model you are using. Ensure that cmake parameters are
set accordingly. More information on these cmake parameters is detailed in their separate sections.

> **Note:** Preprocessing of the files can also affect the inference result, such as the rescaling and padding
> operations performed for image classification.

## The application does not work with my custom model

Ensure that your model is in a fully quantized `.tflite` file format, either `uint8` or `int8`, and that it has
successfully been run through the Vela compiler.

Also, please check that the cmake parameters used match the input requirements of your new model.

> **Note:** The Vela tool is not available within this software project. It is a separate Python tool that is available
> from: <https://pypi.org/project/ethos-u-vela/>. The source code is hosted on
> <https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/>.

## NPU configuration mismatch error when running inference

If you see the following error message when an inference is executed:

```commandline
NPU config mismatch: npu.macs_per_cc=8 optimizer.macs_per_cc=7
NPU config mismatch: npu.shram_size=48 optimizer.shram_size=24
ERROR - Invoke failed.
ERROR - Inference failed.
```

It shows that the configuration of the Vela compiled `.tflite` file doesn't match the number of MAC units on the FVP.

The Vela configuration parameter `accelerator-config` used for producing the .`tflite` file that is used
while building the application should match the MAC configuration that the FVP is emulating.
For example, if the `accelerator-config` from the Vela command was `ethos-u55-128`, the FVP should be emulating the
128 MAC configuration of the Ethos-U55 block(default FVP configuration). If the `accelerator-config` used was
`ethos-u55-256`, the FVP must be executed with additional command line parameter to instruct it to emulate the
256 MAC configuration instead.

The [deploying on an FVP emulating MPS3](./deployment.md#deploying-on-an-fvp-emulating-mps3) page provides guidance
on how to instruct the FVP to change the number of MAC units.

Note that when the FVP is launched and the application starts executing, various parameters about the system are
logged over UART. These include the MAC/cc configuration of the FVP.

```log
INFO - MPS3 core clock has been set to: 32000000Hz
INFO - CPU ID: 0x410fd220
INFO - CPU: Cortex-M55 r0p0
INFO - Ethos-U55 device initialised
INFO - Ethos-U55 version info:
INFO -  Arch:       v1.0.6
INFO -  Driver:     v0.16.0
INFO -  MACs/cc:    128
INFO -  Cmd stream: v0
INFO -  SHRAM size: 24
INFO - Arm Corstone-300 (SSE-300) platform initialised
INFO - ARM ML Embedded Evaluation Kit for MPS3 FPGA and FastModel
INFO - Target system design: Arm Corstone-300 (SSE-300)
```

Next section of the documentation: [Appendix](appendix.md).
