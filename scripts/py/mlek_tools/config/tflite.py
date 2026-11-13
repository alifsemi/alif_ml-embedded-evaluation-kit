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
"""TFLite Micro framework configuration."""
from dataclasses import dataclass


@dataclass(frozen=True)
class TfliteConfig:
    """TFLite Micro framework: Vela installation settings and whether to run optimisation.

    Optimization parameters (arena cache size, config file, etc.) live in
    :class:`~mlek_tools.config.optimizer.OptimizerConfig` so they are shared
    with the ExecuTorch path.
    """
    enabled: bool = True
    run_vela: bool = False
    vela_version: str = ""
    vela_url: str = ""
    vela_install_from_source: bool = False
