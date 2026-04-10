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
"""Orchestration for Python environment setup, model resource downloads, and optimisation."""
from .npu_config import NpuConfig, NpuConfigs, valid_npu_configs, get_default_npu_config_from_name
from .python_venv import PythonEnv, set_up_python_venv
from .resources import set_up_resources
from .setup_config import SetupConfig, OptimizationConfig, PathsConfig, VelaConfig, SetupContext
from .use_case import UseCase, ExecutorchResource, load_use_case_resources
from .util import call_command, download_file, get_md5sum_for_file, remove_tree_dir

__all__ = [
    "NpuConfig", "NpuConfigs", "valid_npu_configs", "get_default_npu_config_from_name",
    "PythonEnv", "set_up_python_venv",
    "set_up_resources",
    "SetupConfig", "OptimizationConfig", "PathsConfig", "VelaConfig", "SetupContext",
    "UseCase", "ExecutorchResource", "load_use_case_resources",
    "call_command", "download_file", "get_md5sum_for_file", "remove_tree_dir",
]
