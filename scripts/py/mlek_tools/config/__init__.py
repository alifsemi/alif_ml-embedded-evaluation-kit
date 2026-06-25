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
"""Configuration dataclasses for mlek_tools."""
from mlek_tools.config.download import DownloadConfig
from mlek_tools.config.executorch import ExecuTorchConfig
from mlek_tools.config.npu import (
    NpuConfig,
    NpuConfigs,
    get_default_npu_config_from_name,
    valid_npu_configs,
)
from mlek_tools.config.optimizer import CopFormat, OptimizerConfig
from mlek_tools.config.paths import PathsConfig
from mlek_tools.config.tflite import TfliteConfig

__all__ = [
    "DownloadConfig",
    "ExecuTorchConfig",
    "NpuConfig",
    "NpuConfigs",
    "CopFormat",
    "OptimizerConfig",
    "PathsConfig",
    "TfliteConfig",
    "get_default_npu_config_from_name",
    "valid_npu_configs",
]
