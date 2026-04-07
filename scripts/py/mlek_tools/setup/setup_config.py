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
Setup config definitions
"""
# pylint: disable=line-too-long
import typing
from dataclasses import dataclass, field
from pathlib import Path

from .python_venv import PythonEnv
from .util import HttpHeadersType


@dataclass(frozen=True)
# Configuration container; multiple flags are intentional.
# pylint: disable=too-many-instance-attributes
class SetupConfig:
    """
    Configuration for resource-setup behaviour.
    """
    run_vela_on_models: bool = False
    use_case_names: typing.List[str] = ()
    check_clean_folder: bool = False
    set_up_executorch: bool = True
    set_up_tensorflow: bool = True
    parallel: int = 1
    http_headers: HttpHeadersType = field(default_factory=dict)
    executorch_excluded_npu_processor_ids: typing.Tuple[str, ...] = ()


@dataclass(frozen=True)
class OptimizationConfig:
    """
    Configuration for Vela optimisation behaviour.
    """
    additional_npu_config_names: typing.List[str] = ()
    arena_cache_size: int = 0


@dataclass(frozen=True)
class PathsConfig:
    """
    Paths used by the resource setup process.
    """
    additional_requirements_file: typing.Optional[Path] = None
    use_case_resources_files: typing.List[Path] = field(default_factory=list)
    downloads_dir: typing.Optional[Path] = None
    executorch_path: typing.Optional[Path] = None
    vela_config_file: typing.Optional[Path] = None
    venv_dir: typing.Optional[Path] = None
    metadata_file: typing.Optional[Path] = None


@dataclass(frozen=True)
class VelaConfig:
    """
    Configuration for Vela installation and invocation.
    """
    version: str
    url: str
    install_from_source: bool = False


class SetupContext:
    """
    Hold mutable state during setup execution.
    """

    def __init__(self,
                 setup_config: SetupConfig,
                 optimization_config: OptimizationConfig,
                 paths_config: PathsConfig,
                 ):
        self._setup_config = setup_config
        self._optimization_config = optimization_config
        self._paths_config = paths_config

        self.python_env: typing.Optional[PythonEnv] = None

    @property
    def setup_config(self):
        """
        Return the setup configuration.

        :returns:    The setup configuration.
        """
        return self._setup_config

    @property
    def optimization_config(self):
        """
        Return the optimisation configuration.

        :returns:    The optimisation configuration.
        """
        return self._optimization_config

    @property
    def paths_config(self):
        """
        Return the paths configuration.

        :returns:    The paths configuration.
        """
        return self._paths_config
