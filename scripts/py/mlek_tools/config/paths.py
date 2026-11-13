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
"""Framework-agnostic project path configuration."""
import typing
from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class PathsConfig:
    """Filesystem paths that are independent of which ML framework is active.

    Framework-specific paths (Vela config file, ExecuTorch source tree) live in
    :class:`~mlek_tools.config.optimizer.OptimizerConfig` and
    :class:`~mlek_tools.config.executorch.ExecuTorchConfig` respectively.
    """
    requirements_files: typing.List[Path] = field(default_factory=list)
    use_case_resources_files: typing.List[Path] = field(default_factory=list)
    downloads_dir: typing.Optional[Path] = None
    venv_dir: typing.Optional[Path] = None
    metadata_file: typing.Optional[Path] = None
