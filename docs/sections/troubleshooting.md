# Troubleshooting

- [Troubleshooting](./troubleshooting.md#troubleshooting)
  - [Inference results are incorrect for my custom files](./troubleshooting.md#inference-results-are-incorrect-for-my-custom-files)
  - [The application does not work with my custom model](./troubleshooting.md#the-application-does-not-work-with-my-custom-model)
  - [NPU configuration mismatch error when running inference](./troubleshooting.md#npu-configuration-mismatch-error-when-running-inference)
  - [Errors when cloning the repository](./troubleshooting.md#errors-when-cloning-the-repository)
  - [Problem installing Vela](./troubleshooting.md#problem-installing-vela)
  - [No matching distribution found for ethos-u-vela==3.4.0](./troubleshooting.md#no-matching-distribution-found-for-ethos_u_vela)
    - [How to update Python3 package to 3.7 version](./troubleshooting.md#how-to-update-python3-package-to-newer-version)
  - [Error trying to build on Arm Virtual Hardware](./troubleshooting.md#error-trying-to-build-on-arm-virtual-hardware)

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

Vela 3.4.0 increases Python requirement to at least version 3.7, if not installed on your system the following error will occur:

```log
python3 -m pip install ethos-u-vela==3.4.0
ERROR: Could not find a version that satisfies the requirement ethos-u-vela==3.4.0 (from versions: 0.1.0, 1.0.0, 1.1.0, 1.2.0, 2.0.0, 2.0.1, 2.1.1, 3.0.0, 3.1.0, 3.2.0)
ERROR: No matching distribution found for ethos-u-vela==3.4.0
```

Ensure that the minimum Python 3.7 requirement is installed and it's the default version.
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

2. Install the Python 3.7 packages necessary on the system:

   ```commandline
   sudo apt-get install python3.7 python3.7-venv libpython3.7 libpython3.7-dev
   ```

3. Update the `python3` alternatives (set as 1 your previous version displayed at step 1):

   ```commandline
   sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.6 1
   sudo update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.7 2
   ```

4. At the prompt, update the configuration by selecting Python3.7 as the chosen default alternative:

   ```commandline
   sudo update-alternatives --config python3
   ```

5. Python3.7 is now set as default you can check it by running:

   ```commandline
   python3 --version
   ```

   ```log
   Python 3.7.0
   ```

> **Note:** After updating to from Python3.6 Python3.7 it may happen that the `gnome-terminal` or the relative
> shortcuts doesn't work anymore.
> If when opening it from XTerm with `gnome-terminal` the following error appear:
>
> ```log
> Traceback (most recent call last):
>   File "/usr/bin/gnome-terminal", line 9, in <module>
>     from gi.repository import GLib, Gio
>   File "/usr/lib/python3/dist-packages/gi/__init__.py", line 42, in <module>
>     from . import _gi
> ImportError: cannot import name '_gi' from partially initialized module 'gi' (most likely due to a circular import)
> (/usr/lib/python3/dist-packages/gi/> __init__.py)
> ```
>
> Modify the `gnome-terminal` script located in `/usr/bin/` and changing the environment (first line of the script)
> from `#!/usr/bin/python3` to `#!/usr/bin/python3.6`.

Next section of the documentation: [Appendix](appendix.md).

## Error trying to build on Arm Virtual Hardware

If trying to build on Arm Virtual Hardware and you encounter an error similar to the following:

```log
The virtual environment was not created successfully because ensurepip is not
available.  On Debian/Ubuntu systems, you need to install the python3-venv
package using the following command.

    apt install python3.8-venv

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
sudo apt install python3.8-venv
rm -r resources_downloaded
```

You can then try rebuilding again e.g.

```commandline
python3 ./build_default.py
```
and the error should be fixed.