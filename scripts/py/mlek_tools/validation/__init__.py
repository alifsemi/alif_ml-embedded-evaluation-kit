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
"""Model validation utilities for Arm Ethos-U NPU."""
from mlek_tools.validation.npu_validation import (
    NpuValidationArgs,
    normalize_memory_mode,
    normalize_ethos_u_config,
    parse_accelerator_config,
)
from mlek_tools.validation.tflite_validator import validate_tflite_model
from mlek_tools.validation.pte_validator import validate_pte_model
from mlek_tools.validation.pte_ops_dump import dump_pte_ops, extract_vela_args, has_ethos_u_delegate

__all__ = [
    "NpuValidationArgs",
    "normalize_memory_mode",
    "normalize_ethos_u_config",
    "parse_accelerator_config",
    "validate_tflite_model",
    "validate_pte_model",
    "dump_pte_ops",
    "extract_vela_args",
    "has_ethos_u_delegate",
]
