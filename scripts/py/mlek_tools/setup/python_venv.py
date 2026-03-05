#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2026 Arm Limited and/or its
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
PythonEnv dataclass and helper for creating Python virtual environments.
"""
import os
import subprocess
import sys
import typing
import venv
from dataclasses import dataclass
from pathlib import Path

from .util import call_command


@dataclass
class PythonEnv:
    """
    Represents a Python virtual environment.

    Provides methods for package installation and querying without requiring
    shell activation — all operations are performed by invoking the venv
    Python executable directly.
    """
    path: Path

    @property
    def python(self) -> Path:
        """Path to the Python interpreter inside this virtual environment."""
        if sys.platform == "win32":
            return self.path / "Scripts" / "python.exe"
        return self.path / "bin" / "python3"

    @property
    def bin_dir(self) -> Path:
        """Path to the bin (Scripts on Windows) directory of this virtual environment."""
        if sys.platform == "win32":
            return self.path / "Scripts"
        return self.path / "bin"

    def is_installed(self, package_name: str) -> bool:
        """
        Check if a package is installed using ``pip show``.

        :param package_name:    The package name to query.
        :return:                True if the package is installed, False otherwise.
        """
        result = subprocess.run(
            [str(self.python), "-m", "pip", "show", package_name],
            capture_output=True,
            check=False,
        )
        return result.returncode == 0

    def pip_install(
            self,
            package_spec: str,
            no_deps: bool = False,
            environment: typing.Optional[typing.Dict] = None,
    ) -> None:
        """
        Install a package or package spec into this virtual environment.

        :param package_spec:    Spec passed to ``pip install`` (name, path, URL, or
                                a string that includes extra index flags).
        :param no_deps:         Pass ``--no-deps`` to pip if True.
        :param environment:     Optional dict of extra environment variables for the process.
        """
        no_deps_token = "--no-deps" if no_deps else ""
        call_command(
            f'"{self.python}" -m pip install {no_deps_token} {package_spec}',
            env=environment,
        )

    def pip_install_if_needed(
            self,
            package_spec: str,
            installed_name: typing.Optional[str] = None,
            no_deps: bool = False,
            environment: typing.Optional[typing.Dict] = None,
    ) -> None:
        """
        Install a package only if it is not already present.

        :param package_spec:    Spec passed to ``pip install``.
        :param installed_name:  Name checked by ``pip show``; defaults to ``package_spec``.
                                Useful when ``package_spec`` is a URL or path.
        :param no_deps:         Pass ``--no-deps`` to pip if True.
        :param environment:     Optional dict of extra environment variables.
        """
        check_name = installed_name if installed_name else package_spec
        if not self.is_installed(check_name):
            self.pip_install(package_spec, no_deps=no_deps, environment=environment)

    def pip_install_requirements(
            self,
            requirements_file: Path,
            no_deps: bool = False,
    ) -> None:
        """
        Install packages from a requirements file.

        :param requirements_file:   Path to the requirements.txt file.
        :param no_deps:             Pass ``--no-deps`` to pip if True.
        """
        no_deps_token = "--no-deps" if no_deps else ""
        call_command(
            f'"{self.python}" -m pip install {no_deps_token} -r {requirements_file}'
        )


def set_up_python_venv(
        env_dir: Path,
        additional_requirements_file: typing.Optional[Path] = None,
) -> PythonEnv:
    """
    Create or reuse a Python virtual environment at ``env_dir``.

    :param env_dir:                         Path to the virtual environment directory.
    :param additional_requirements_file:    Optional requirements file to install into
                                            the venv after creation.
    :return:                                A :class:`PythonEnv` for the virtual environment.
    """
    venv_builder = venv.EnvBuilder(with_pip=True, upgrade_deps=True)
    venv_context = venv_builder.ensure_directories(env_dir=env_dir)

    if not Path(venv_context.env_exe).is_file():
        venv_builder.create(env_dir=env_dir)

    env_activate = Path(venv_context.bin_path) / (
        "activate.bat" if sys.platform == "win32" else "activate"
    )
    if not env_activate.is_file():
        venv_builder.install_scripts(venv_context, venv_context.bin_path)

    python_env = PythonEnv(path=env_dir)

    if additional_requirements_file and os.path.isfile(additional_requirements_file):
        python_env.pip_install_requirements(additional_requirements_file)

    return python_env
