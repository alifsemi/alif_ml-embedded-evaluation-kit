#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2025 Arm Limited and/or its
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
import typing
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class SetupConfig:
    """
    Configuration for setup behaviour.

    Attributes:
        run_vela_on_models (bool)           :   Whether to run Vela on the downloaded models
        use_case_names (list)               :   List of names of use cases to set up resources for
                                                (default is all).
        check_clean_folder (bool)           :   Indicates whether the resources folder needs to
                                                be checked for updates and cleaned.
        set_up_executorch (bool)            :   Indicates whether to set up ExecuTorch
        set_up_tensorflow (bool)            :   Indicates whether to set up ExecuTorch
        parallel (int)                      :   Number of threads to use for downloads
                                                and model optimisation
    """
    run_vela_on_models: bool = False
    use_case_names: typing.List[str] = ()
    check_clean_folder: bool = False
    set_up_executorch: bool = True
    set_up_tensorflow: bool = True
    parallel: int = 1


@dataclass(frozen=True)
class OptimizationConfig:
    """
    Configuration for Vela optimization.

    Attributes:
        additional_npu_config_names (list)  :   List of strings of Ethos-U NPU configs.
        arena_cache_size (int)              :   Specifies arena cache size in bytes. If a value
                                                greater than 0 is provided, this will be taken
                                                as the cache size. If 0, the default values, as per
                                                the NPU config requirements, are used.
    """
    additional_npu_config_names: typing.List[str] = ()
    arena_cache_size: int = 0


@dataclass(frozen=True)
class PathsConfig:
    """
    Configuration of paths to resources used by the setup process.

    Attributes:
        additional_requirements_file (str)  :   Path to a requirements.txt file if
                                                additional packages need to be
                                                installed.
        use_case_resources_file (Path)      :   Path to a JSON file containing the use case
                                                metadata resources.

        downloads_dir (Path)                :  Path to store model resources files.
        executorch_path (Path)              :  Path to ExecuTorch repository
    """
    additional_requirements_file: Path = ""
    use_case_resources_file: Path = ""
    downloads_dir: Path = ""
    executorch_path: Path = ""


class SetupContext:
    """
    Used to manage state during the setup progress
    """

    def __init__(self,
                 setup_config: SetupConfig,
                 optimization_config: OptimizationConfig,
                 paths_config: PathsConfig,
                 ):
        self._setup_config = setup_config
        self._optimization_config = optimization_config
        self._paths_config = paths_config

        self.env_path: Path = Path("/")
        self.env_activate_cmd: str = ""

    @property
    def setup_config(self):
        """
        Get the setup config
        :return:    The setup config
        """
        return self._setup_config

    @property
    def optimization_config(self):
        """
        Get the optimization config
        :return:    The optimization config
        """
        return self._optimization_config

    @property
    def paths_config(self):
        """
        Get the paths config
        :return:    The paths config
        """
        return self._paths_config
