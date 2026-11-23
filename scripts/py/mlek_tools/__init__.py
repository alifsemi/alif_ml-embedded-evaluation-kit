#  SPDX-FileCopyrightText:  Copyright 2026 Arm Limited and/or
#  its affiliates <open-source-office@arm.com>
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
"""Python tools for ML model resource setup and code generation."""
# Register the package alias before absolute mlek_tools imports below. This supports
# loading through the source-tree compatibility path, scripts.py.mlek_tools.
# pylint: disable=wrong-import-position
import sys

sys.modules.setdefault("mlek_tools", sys.modules[__name__])

from mlek_tools.config import (
    CopFormat,
    DownloadConfig,
    ExecuTorchConfig,
    MLFramework,
    MemoryMode,
    Npu,
    NpuConfig,
    iter_npu_configs,
    OptimizerConfig,
    PathsConfig,
    parse_ml_framework,
    TfliteConfig,
    supported_npu_config_names,
)
from mlek_tools.config import __all__ as _config_exports
from mlek_tools.orchestrate import set_up_resources
from mlek_tools.setup.python_venv import PythonEnv, set_up_python_venv
from mlek_tools.setup.util import (
    call_command,
    download_file,
    get_sha256sum_for_file,
    remove_tree_dir,
    verify_file_sha256,
)
from mlek_tools.use_case.model import ExecuTorchResource, UseCase, load_use_case_resources

__all__ = _config_exports + [
    "set_up_resources",
    "PythonEnv",
    "set_up_python_venv",
    "call_command",
    "download_file",
    "get_sha256sum_for_file",
    "remove_tree_dir",
    "verify_file_sha256",
    "ExecuTorchResource",
    "UseCase",
    "load_use_case_resources",
]
