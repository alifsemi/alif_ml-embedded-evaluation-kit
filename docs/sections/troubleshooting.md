# Troubleshooting

- [Troubleshooting](./troubleshooting.md#troubleshooting)
  - [Inference results are incorrect for my custom files](./troubleshooting.md#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](./troubleshooting.md#the-application-does-not-work-with-my-custom-model)
  - [NPU configuration mismatch error when running inference](./troubleshooting.md#npu-configuration-mismatch-error-when-running-inference)
  - [Errors when cloning the repository](./troubleshooting.md#errors-when-cloning-the-repository)
  - [Problem installing Vela](./troubleshooting.md#problem-installing-vela)
  - [No matching distribution found for ethos-u-vela==3.9.0](./troubleshooting.md#no-matching-distribution-found-for-ethos_u_vela)
    - [How to update Python3 package to 3.9 version](./troubleshooting.md#how-to-update-python3-package-to-newer-version)
  - [Error trying to build on Arm Virtual Hardware](./troubleshooting.md#error-trying-to-build-on-arm-virtual-hardware)
  - [Internal Compiler Error](./troubleshooting.md#internal-compiler-error)
  - [Build issues with WSL2](./troubleshooting.md#build-issues-with-wsl2)

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
128 MACs configuration of the Ethos™-U55 block(default FVP configuration). If the `accelerator-config` used was
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
INFO - Ethos-U device initialised
INFO - Ethos-U version info:
INFO -  Arch:       v1.0.6
INFO -  Driver:     v0.16.0
INFO -  MACs/cc:    128
INFO -  Cmd stream: v0
INFO - Target system design: Arm Corstone-300 - AN552
INFO - Arm Corstone-300 - AN552 platform initialised
INFO - ARM ML Embedded Evaluation Kit
```

## Errors when cloning the repository

If you see following errors when cloning the repository:

- ```log
   fatal: unable to access 'https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit/':
   server certificate verification failed. CAfile: /etc/ssl/certs/ca-certificates.crt CRLfile: none
  ```

  We suggest to update to the latest common CA certificates:

  ```commandline
  sudo apt-get update
  sudo apt-get install ca-certificates
  ```

- ```log
  fatal: unable to access 'https://review.mlplatform.org/ml/ethos-u/ml-embedded-evaluation-kit/':
  error:06FFF089:digital envelope routines:CRYPTO_internal:bad key length
  ```

  We suggest to export the `CURL_SSL_BACKEND` variable as `secure-transport`:

  ```commandline
  export CURL_SSL_BACKEND="secure-transport"
  ```

## Problem installing Vela

During Vela installation, part of the package is compiled and requires libpython3.
If the system lacks this dependency the following error will occur:

```log
 x86_64-linux-gnu-gcc -pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O2 -Wall -g
  -fstack-protector-strong -Wformat -Werror=format-security -g -fwrapv -O2 -g -fstack-protector-strong
  -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIC -DNPY_NO_DEPRECATED_API=NPY_1_9_API_VERSION
  -I/venv/include -I/usr/include/python3.7 -I/venv/lib/python3.7/site-packages/numpy/core/include
  -c ethosu/mlw_codec/mlw_codecmodule.c -o build/temp.linux-x86_64-3.7/ethosu/mlw_codec/mlw_codecmodule.o
    ethosu/mlw_codec/mlw_codecmodule.c:20:10: fatal error: Python.h: No such file or directory
       20 | #include <Python.h>
          |          ^~~~~~~~~~
    compilation terminated.
    error: command 'x86_64-linux-gnu-gcc' failed with exit status 1
    ----------------------------------------
ERROR: Command errored out with exit status 1: /venv/bin/python -u -c
'import sys, setuptools, tokenize; sys.argv[0] = '"'"'/tmp/pip-install-4rmoh7va/ethos-u-vela/setup.py'"'"'
; __file__='"'"'/tmp/pip-install-4rmoh7va/ethos-u-vela/setup.py'"'"';f=getattr(tokenize, '"'"'open'"'"', open)
(__file__);code=f.read().replace('"'"'\r\n'"'"', '"'"'\n'"'"');f.close();exec(compile(code, __file__, '"'"'exec'"'"'))'
install --record /tmp/pip-record-jidxiokn/install-record.txt --single-version-externally-managed
--compile --install-headers
/venv/include/site/python3.7/ethos-u-vela Check the logs for full command output.
```

To solve this issue install libpython3 on the system.

## No matching distribution found for ethos-u-vela

Vela 3.9.0 increases Python requirement to at least version 3.9, if not installed on your system the following error will occur:

```log
python3 -m pip install ethos-u-vela==3.9.0
ERROR: Could not find a version that satisfies the requirement ethos-u-vela==3.9.0 (from versions: 0.1.0, 1.0.0, 1.1.0, 1.2.0, 2.0.0, 2.0.1, 2.1.1, 3.0.0, 3.1.0, 3.2.0)
ERROR: No matching distribution found for ethos-u-vela==3.9.0
```

Ensure that the minimum Python 3.9 requirement is installed, and it's the default version.
Check your current installed version of Python by running:

```commandline
python3 --version
```

### How to update Python3 package to newer version

1. Check your current installed version of Python by running:

   ```commandline
   python3 --version
   ```

   For example:

   ```log
   Python 3.6.9
   ```

2. Install the Python 3.9 packages necessary on the system:

   ```commandline
   sudo apt-get install python3.9 python3.9-venv libpython3.9 libpython3.9-dev
   ```

3. Explicitly specify this Python when executing the set-up Python scripts. For example:

    ```commandline
    python3.9 ./set_up_default_resources.py
    ```
> **Note:**: We do not recommend updating the Python version system-wide as it might break
> various desktop utilities for your OS distribution. However, if you are using a container
> or any other sandboxed environment, you can choose to update it using the following steps.
>
> * Update the `python3` alternatives (set as 1 your previous version displayed at step 1):
>
>   ```commandline
>   sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.6 1
>   sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.9 2
>   ```
>
> * At the prompt, update the configuration by selecting Python3.9 as the chosen default alternative:
>
>   ```commandline
>   sudo update-alternatives --config python3
>   ```
> * Python3.9 is now set as default, you can check it by running:
>
>   ```commandline
>   python3 --version
>   ```
>
>   ```log
>   Python 3.9.0
>   ```

Next section of the documentation: [Appendix](appendix.md).

## Error trying to build on Arm Virtual Hardware

If trying to build on Arm Virtual Hardware and you encounter an error similar to the following:

```log
The virtual environment was not created successfully because ensurepip is not
available.  On Debian/Ubuntu systems, you need to install the python3-venv
package using the following command.

    apt install python3.9-venv

You may need to use sudo with that command.  After installing the python3-venv
package, recreate your virtual environment.

Failing command: ['/home/test/ml-embedded-evaluation-kit/resources_downloaded/env/bin/python3', '-Im', 'ensurepip', '--upgrade', '--default-pip']


Traceback (most recent call last):
  File "./build_default.py", line 184, in <module>
    run(
  File "./build_default.py", line 89, in run
    (download_dir, env_path) = set_up_resources(
  File "/home/test/ml-embedded-evaluation-kit/set_up_default_resources.py", line 439, in set_up_resources
    call_command(command)
  File "/home/test/ml-embedded-evaluation-kit/set_up_default_resources.py", line 276, in call_command
    proc.check_returncode()
  File "/usr/lib/python3.8/subprocess.py", line 448, in check_returncode
    raise CalledProcessError(self.returncode, self.args, self.stdout,
subprocess.CalledProcessError: Command 'python3 -m venv env' returned non-zero exit status 1.
```

You can fix this error by installing Python virtual environment and removing the corrupted resources_downloaded folder.

```commandline
rm -r resources_downloaded
```

Follow the instructions to [update Python3 package to 3.9 version](./troubleshooting.md#how-to-update-python3-package-to-newer-version)
before attempting a rebuild with:

```commandline
python3 ./build_default.py
```
and the error should be fixed.

## Internal Compiler Error

There is a known issue with the Arm GNU toolchain version 12.2 (release December 22, 2022).
Compiler from this toolchain throws up this error:

```
during RTL pass: combine
/home/user/ml-embedded-evaluation-kit/dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_softmax_s8.c: In function 'arm_exp_on_negative_values_mve_32x4':
/home/user/ml-embedded-evaluation-kit/dependencies/cmsis-nn/Source/SoftmaxFunctions/arm_softmax_s8.c:74:1: internal compiler error: in trunc_int_for_mode, at explow.cc:59
   74 | }
      | ^
0x7f0343d9b082 __libc_start_main
	../csu/libc-start.c:308
Please submit a full bug report, with preprocessed source (by using -freport-bug).
Please include the complete backtrace with any bug report.
See <https://bugs.linaro.org/> for instructions.
```

It has been worked around by a CMSIS-NN patch from Dec 23, 2023. Make sure you are on a later commit of CMSIS-NN.

See
- [GCC patches: PR107987](https://gcc.gnu.org/pipermail/gcc-patches/2022-December/607963.html)
- [CMSIS-NN issue 13](https://github.com/ARM-software/CMSIS-NN/issues/13)
- [Workaround for GNU Toolchain 12.x bug ](https://github.com/ARM-software/CMSIS-NN/commit/245089501eef18e8b638865c5afd6cdf2d03386f)

## Build issues with WSL2

Builds using Windows® Subsystem For Linux (WSL2) can run into issues if their
environment's PATH variable has paths containing unescaped space characters.
The error might look like:

```
****Building TensorFlow Lite Micro library… /bin/sh: 1: Syntax error: “(” unexpected
 make[2]: *** [CMakeFiles/tensorflow_build.dir/build.make:71: CMakeFiles/tensorflow_build] Error 2
 make[1]: *** [CMakeFiles/Makefile2:537: CMakeFiles/tensorflow_build.dir/all] Error 2
 make[1]: *** Waiting for unfinished jobs…
```

The error here was caused by `C:/Program Files (x86)/` being part of the PATH.
To resolve this issue, remove any paths with spaces. Alternatively, if these
paths are required, escape them using `\` or enclose them with quotes.

Another example of a similar issue: [Discourse issue 171: Build error in makefile](https://discuss.mlplatform.org/t/build-error-in-makefile/171).
