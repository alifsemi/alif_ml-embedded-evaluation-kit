#!/usr/bin/env python3
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
"""
Functions for creating and interacting with Python virtual environments
"""
import os
import sys
import typing
import venv
from pathlib import Path

from .util import call_command


def set_up_python_venv(
        download_dir: Path,
        additional_requirements_file: Path = ""
) -> typing.Tuple[Path, str]:
    """
    Set up the Python environment with which to set up the resources

    @param download_dir:                    Path to the resources_downloaded directory
    @param additional_requirements_file:    Optional additional requirements file
    @return:                                Path to the venv Python binary + activate command
    """
    env_path = download_dir / 'env'

    venv_builder = venv.EnvBuilder(with_pip=True, upgrade_deps=True)
    venv_context = venv_builder.ensure_directories(env_dir=env_path)

    env_python = Path(venv_context.env_exe)

    if not env_python.is_file():
        # Create the virtual environment using current interpreter's venv
        # (not necessarily the system's Python3)
        venv_builder.create(env_dir=env_path)

    if sys.platform == "win32":
        env_activate = Path(f"{venv_context.bin_path}/activate.bat")
        env_activate_cmd = str(env_activate)
    else:
        env_activate = Path(f"{venv_context.bin_path}/activate")
        env_activate_cmd = f". {env_activate}"

    if not env_activate.is_file():
        venv_builder.install_scripts(venv_context, venv_context.bin_path)

    # Install additional requirements first, if a valid file has been provided
    if additional_requirements_file and os.path.isfile(additional_requirements_file):
        install_requirements(env_activate_cmd, additional_requirements_file)

    return env_path, env_activate_cmd


def is_pip_package_installed(package_name: str, env_activate_cmd: str) -> bool:
    """
    Check if a named package is installed in the Python environment
    :param package_name:        The name of the pip package as it appears in `pip freeze`
    :param env_activate_cmd:    The command for activating the Python virtual environment
    :return:                    True if the named package is installed, False otherwise
    """
    packages = call_command(f"{env_activate_cmd} && pip freeze").split("\n")
    return len([package for package in packages if package.startswith(package_name)]) > 0


def install_requirements(
        env_activate_cmd: str,
        requirements_file: Path,
        no_deps: bool = False
):
    """
    Install a requirements file for a specified Python environment
    :param env_activate_cmd:    Command to activate Python env
    :param requirements_file:   Path to the requirements file
    :param no_deps:             Determine whether the `--no-deps` flag is passed to
                                `pip -install`
    """
    no_deps_token = "--no-deps" if no_deps else ""
    call_command(
        f"{env_activate_cmd} "
        f"&& python -m pip install {no_deps_token} -r {requirements_file}"
    )


def install_pip_package_if_needed(
        package_name: str,
        env_activate_cmd: str,
        installed_package_name: typing.Optional[str] = None,
        environment=None,
        no_deps: bool = False,
):
    """
    Installs the specified pip package if it is not already installed in the
    Python virtual environment
    :param package_name:            The name of the package as passed to `pip install`
                                    (which could be a path, e.g. to a git repository)
    :param env_activate_cmd:        The command for activating the Python virtual environment
    :param installed_package_name:  The name of the pip package as it appears in `pip freeze`.
                                    If this is not passed, it is assumed to be the same as
                                    the `package_name` parameter.
    :param environment:             An optional dictionary of environment variables to set
                                    when installing the specified package.
    :param no_deps:                 Determine whether the `--no-deps` flag is passed to
                                    `pip -install`
    """
    if not is_pip_package_installed(
            installed_package_name if installed_package_name else package_name,
            env_activate_cmd
    ):
        no_deps_token = "--no-deps" if no_deps else ""
        call_command(
            f"{env_activate_cmd} "
            f"&& python -m pip install {package_name} {no_deps_token}",
            env=environment
        )
