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
"""Python environment setup, installation helpers, and shared utilities."""
from mlek_tools.setup.install import install_executorch_project, setup_executorch, setup_vela
from mlek_tools.setup.logging_config import (
    LOG_LEVELS,
    ColourFormatter,
    LoggingOptions,
    configure_logging,
    parse_log_level,
)
from mlek_tools.setup.python_venv import PythonEnv, set_up_python_venv
from mlek_tools.setup.util import (
    call_command,
    download_file,
    get_sha256sum_for_file,
    remove_tree_dir,
    verify_file_sha256,
)

__all__ = [
    "PythonEnv",
    "ColourFormatter",
    "LoggingOptions",
    "LOG_LEVELS",
    "call_command",
    "configure_logging",
    "download_file",
    "get_sha256sum_for_file",
    "install_executorch_project",
    "remove_tree_dir",
    "set_up_python_venv",
    "setup_executorch",
    "setup_vela",
    "parse_log_level",
    "verify_file_sha256",
]
