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
"""
Shared types and helpers for Arm Ethos-U NPU validation.

This module is intentionally small and dependency-free so that other
validators can import it without pulling in Vela or ExecuTorch.
"""
from dataclasses import dataclass
import re
from typing import Optional, Tuple


@dataclass(frozen=True)
class NpuValidationArgs:
    """
    Arguments required for Arm Ethos-U NPU validation.

    If any field is None, that aspect of validation is skipped.
    """
    # Vela memory mode; set to None to skip memory-mode validation.
    memory_mode: Optional[str] = None
    # Arm Ethos-U NPU accelerator config (e.g. ethos-u55-128); None to skip.
    config: Optional[str] = None


def normalize_memory_mode(value: str) -> str:
    """
    Normalize Vela memory mode strings to comparison keys.

    Returns:
        "shared" for Shared_Sram or Sram_Only; "dedicated" for Dedicated_Sram.
    """
    # Normalize to lowercase for comparison while preserving the original for errors.
    lowered = value.strip().lower()
    if lowered in {"shared_sram", "sram_only"}:
        return "shared"
    if lowered == "dedicated_sram":
        return "dedicated"
    raise ValueError(
        f"Unsupported Arm Ethos-U NPU memory mode '{value}'. "
        "Expected one of: Shared_Sram, Sram_Only, Dedicated_Sram."
    )


def normalize_ethos_u_config(value: str) -> str:
    """
    Normalize Arm Ethos-U NPU config strings.

    Accepts short forms like H128/Y256/Z256 and expands to ethos-uNNN-MACS.
    """
    # Accept full strings (ethos-u55-128) and compact ones (H128/Y256/Z256).
    lowered = value.strip().lower()
    if lowered.startswith("ethos-u"):
        return lowered
    match = re.fullmatch(r"([hyz])([0-9]+)", lowered)
    if match:
        prefix_map = {"h": "ethos-u55", "y": "ethos-u65", "z": "ethos-u85"}
        return f"{prefix_map[match.group(1)]}-{match.group(2)}"
    return lowered


def parse_accelerator_config(value: Optional[str]) -> Tuple[Optional[str], Optional[int]]:
    """
    Parse an Arm Ethos-U NPU config string into (NPU ID, MACs).

    Example:
        ethos-u55-128 -> ("U55", 128)
    """
    if not value:
        return None, None
    normalized = normalize_ethos_u_config(value)
    match = re.fullmatch(r"ethos-u(55|65|85)-([0-9]+)", normalized)
    if match:
        return f"U{match.group(1)}", int(match.group(2))
    return None, None
