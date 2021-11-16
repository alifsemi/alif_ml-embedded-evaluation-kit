# Troubleshooting

- [Troubleshooting](./troubleshooting.md#troubleshooting)
  - [Inference results are incorrect for my custom files](./troubleshooting.md#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](./troubleshooting.md#the-application-does-not-work-with-my-custom-model)
  - [NPU configuration mismatch error when running inference](./troubleshooting.md#npu-configuration-mismatch-error-when-running-inference)
  - [Problem installing Vela](./troubleshooting.md#problem-installing-vela)

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

It shows that the configuration of the Vela compiled `.tflite` file doesn't match the number of MACs units on the FVP.

The Vela configuration parameter `accelerator-config` used for producing the .`tflite` file that is used
while building the application should match the MACs configuration that the FVP is emulating.
For example, if the `accelerator-config` from the Vela command was `ethos-u55-128`, the FVP should be emulating the
128 MACs configuration of the Ethos-U55 block(default FVP configuration). If the `accelerator-config` used was
`ethos-u55-256`, the FVP must be executed with additional command line parameter to instruct it to emulate the
256 MACs configuration instead.

The [deploying on an FVP emulating MPS3](./deployment.md#deploying-on-an-fvp-emulating-mps3) page provides guidance
on how to instruct the FVP to change the number of MACs units.

Note that when the FVP is launched and the application starts executing, various parameters about the system are
logged over UART. These include the MACs/cc configuration of the FVP.

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

## Problem installing Vela

During Vela installation, part of the package is compiled and requires libpython3.
If the system lacks this dependency the following error will occur:

```shell
 x86_64-linux-gnu-gcc -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -DNPY_NO_DEPRECATED_API=NPY_1_9_API_VERSION -I/venv/include -I/usr/include/python3.8 -I/venv/lib/python3.8/site-packages/numpy/core/include -c ethosu/mlw_codec/mlw_codecmodule.c -o build/temp.linux-x86_64-3.8/ethosu/mlw_codec/mlw_codecmodule.o
    ethosu/mlw_codec/mlw_codecmodule.c:20:10: fatal error: Python.h: No such file or directory
       20 | #include <Python.h>
          |          ^~~~~~~~~~
    compilation terminated.
    error: command 'x86_64-linux-gnu-gcc' failed with exit status 1
    ----------------------------------------
ERROR: Command errored out with exit status 1: /venv/bin/python -u -c 'import sys, setuptools, tokenize; sys.argv[0] = '"'"'/tmp/pip-install-4rmoh7va/ethos-u-vela/setup.py'"'"'; __file__='"'"'/tmp/pip-install-4rmoh7va/ethos-u-vela/setup.py'"'"';f=getattr(tokenize, '"'"'open'"'"', open)(__file__);code=f.read().replace('"'"'\r\n'"'"', '"'"'\n'"'"');f.close();exec(compile(code, __file__, '"'"'exec'"'"'))' install --record /tmp/pip-record-jidxiokn/install-record.txt --single-version-externally-managed --compile --install-headers /venv/include/site/python3.8/ethos-u-vela Check the logs for full command output.
```

To solve this issue install libpython3 on the system.

Next section of the documentation: [Appendix](appendix.md).