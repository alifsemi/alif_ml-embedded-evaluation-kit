#  SPDX-FileCopyrightText:  Copyright 2025-2026 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
"""Framework installation helpers: Vela and ExecuTorch."""
import os
import sys

from mlek_tools.config.executorch import ExecuTorchConfig
from mlek_tools.config.tflite import TfliteConfig
from mlek_tools.setup.python_venv import PythonEnv
from mlek_tools.setup.util import call_command
from mlek_tools.use_case.model import ExecuTorchResource


def setup_vela(python_env: PythonEnv, tflite_config: TfliteConfig, parallel: int = 1) -> None:
    """Install Vela into *python_env* as specified by *tflite_config*.

    :param python_env:      The Python virtual environment.
    :param tflite_config:   TFLite framework config carrying Vela version and URL.
    :param parallel:        Build parallelism (used only when installing from source).
    """
    if tflite_config.vela_install_from_source:
        python_env.pip_install(
            package_specs=f"git+{tflite_config.vela_url}@{tflite_config.vela_version}",
            environment={"CMAKE_BUILD_PARALLEL_LEVEL": str(parallel)},
        )
    else:
        python_env.pip_install(
            package_specs=f"ethos-u-vela=={tflite_config.vela_version}",
        )


def install_executorch(python_env: PythonEnv, executorch_config: ExecuTorchConfig) -> None:
    """Install ExecuTorch Python bindings into *python_env*.

    :param python_env:          The Python virtual environment.
    :param executorch_config:   ExecuTorch framework config carrying the source path.
    :raises NotADirectoryError: If ``executorch_config.executorch_path`` is invalid.
    :raises EnvironmentError:   If called on an unsupported platform.
    """
    if sys.platform not in ['linux', 'darwin']:
        raise EnvironmentError(f'{sys.platform} does not support ExecuTorch set up.')

    executorch_path = executorch_config.executorch_path
    if not executorch_path or not executorch_path.is_dir():
        raise NotADirectoryError(f'Invalid ExecuTorch directory: {executorch_path}')

    install_script = executorch_path / 'install_executorch.sh'
    env = {**os.environ, 'PATH': f"{python_env.bin_dir}:{os.environ.get('PATH', '')}"}
    call_command(
        command=f'{install_script} --clean && {install_script}',
        env=env,
    )


def setup_executorch(python_env: PythonEnv, executorch_config: ExecuTorchConfig) -> None:
    """Install ExecuTorch into *python_env* if not already present."""
    if not python_env.is_installed("executorch"):
        install_executorch(python_env, executorch_config)

    python_env.pip_install_if_needed(
        package_specs=executorch_config.cmsis_nn_path,
        installed_names="cmsis-nn",
    )


def install_executorch_project(
        python_env: PythonEnv,
        executorch_resource: ExecuTorchResource,
) -> None:
    """Install pip dependencies for a local ExecuTorch project resource.

    :param python_env:          The Python virtual environment.
    :param executorch_resource: Resource whose ``requirements_path`` to install.
    :raises FileNotFoundError:  If the requirements file does not exist.
    """
    if executorch_resource.requirements_path:
        if executorch_resource.requirements_path.exists():
            python_env.pip_install(
                requirements_files=executorch_resource.requirements_path
            )
        else:
            raise FileNotFoundError(
                f"Requirements file does not exist: {executorch_resource.requirements_path}"
            )
