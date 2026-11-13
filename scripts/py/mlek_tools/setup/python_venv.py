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
import shlex
import subprocess
import sys
import typing
import venv
from dataclasses import dataclass
from pathlib import Path

from mlek_tools.setup.util import call_command

PipInstallArg = typing.Union[str, Path]
PipInstallArgs = typing.Union[PipInstallArg, typing.Iterable[PipInstallArg]]


def _normalise_pip_args(pip_args: PipInstallArgs) -> typing.List[str]:
    """
    Convert pip install arguments into command-line tokens.

    :param pip_args:   A single pip argument or an iterable of pip arguments.
    :return:           Command-line tokens for ``pip install``.
    """
    if isinstance(pip_args, (str, Path)):
        return [str(pip_args)]
    return [str(pip_arg) for pip_arg in pip_args]


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
            package_specs: PipInstallArgs = (),
            requirements_files: PipInstallArgs = (),
            environment: typing.Optional[typing.Mapping[str, str]] = None,
    ):
        """
        Install packages into this virtual environment.

        :param package_specs:           Specs passed to ``pip install``. For pip options
                                        that take a value, pass the option and its value
                                        as separate entries.
        :param requirements_files:      Requirements files passed to ``pip install`` as
                                        ``-r`` arguments.
        :param environment:             Extra environment variables for the process.
        :raises ValueError:             If no package specs or requirements files are
                                        supplied.
        """
        package_args = _normalise_pip_args(package_specs)
        requirements_args = _normalise_pip_args(requirements_files)
        if not package_args and not requirements_args:
            raise ValueError("No package specs or requirements files supplied.")

        command = [str(self.python), "-m", "pip", "install"]
        for requirements_file in requirements_args:
            command.extend(["-r", requirements_file])
        command.extend(package_args)

        process_environment = os.environ.copy()
        if environment is not None:
            process_environment.update(environment)
        call_command(shlex.join(command), env=process_environment)

    def pip_install_if_needed(
            self,
            installed_names: typing.Union[str, typing.Iterable[str]],
            **pip_install_kwargs: typing.Any,
    ):
        """
        Install packages only if one or more checked packages are not present.

        :param installed_names:      Names checked by ``pip show``.
        :param pip_install_kwargs:   Keyword arguments forwarded to
                                     :meth:`pip_install`.
        :raises ValueError:          If no installed package names are supplied.
        """
        if isinstance(installed_names, str):
            check_names = [installed_names]
        else:
            check_names = list(installed_names)
        if not check_names:
            raise ValueError("At least one installed package name must be supplied.")

        missing_names = [
            check_name for check_name in check_names
            if not self.is_installed(check_name)
        ]
        if missing_names:
            self.pip_install(**pip_install_kwargs)


def set_up_python_venv(
        env_dir: Path,
        requirements_files: typing.Iterable[Path] = (),
) -> PythonEnv:
    """
    Create or reuse a Python virtual environment at ``env_dir``.

    :param env_dir:             Path to the virtual environment directory.
    :param requirements_files:  Requirements files to install into the venv after
                                creation.
    :return:                    A :class:`PythonEnv` for the virtual environment.
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

    existing_requirements_files = [
        requirements_file for requirements_file in requirements_files
        if Path(requirements_file).is_file()
    ]
    if existing_requirements_files:
        python_env.pip_install(requirements_files=existing_requirements_files)

    return python_env
