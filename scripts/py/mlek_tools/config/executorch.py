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
"""ExecuTorch framework configuration."""
import typing
from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class ExecuTorchConfig:
    """ExecuTorch framework: installation settings and whether to run lowering.

    Optimization parameters (Vela config file, arena cache size, etc.) live in
    :class:`~mlek_tools.config.optimizer.OptimizerConfig` so they are shared
    with the TFLM path.
    """
    enabled: bool = True
    run_lowering: bool = False
    executorch_path: typing.Optional[Path] = None
    cmsis_nn_path: typing.Optional[Path] = None
    excluded_npu_processor_ids: typing.Tuple[str, ...] = field(default_factory=tuple)
