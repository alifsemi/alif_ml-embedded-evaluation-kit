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
Arm Ethos-U NPU validation utilities for PTE models.

This module inspects ExecuTorch PTE files offline, using embedded
delegate metadata and serialized Vela command-line flags.
"""
from pathlib import Path

from mlek_tools.validation.npu_validation import (
    NpuValidationArgs,
    normalize_ethos_u_config,
    normalize_memory_mode,
)
from mlek_tools.validation.pte_ops_dump import (
    dump_pte_ops,
    extract_vela_args,
    has_ethos_u_delegate,
)


def validate_pte_model(model_path: Path, args: NpuValidationArgs) -> None:
    """
    Validate Arm Ethos-U NPU settings for a PTE model.

    Checks:
    - Arm Ethos-U NPU delegate presence
    - Vela flags embedded in the PTE file
    """
    # Emit ops/delegate summary first so it is available even if validation fails.
    dump_pte_ops(model_path)

    # Read the PTE once; all checks operate on this buffer.
    buf = model_path.read_bytes()

    # Vela command-line flags are embedded verbatim in the PTE file.
    vela_mem_mode, vela_config = extract_vela_args(buf)
    has_delegate = has_ethos_u_delegate(buf)

    # Vela flags are already emitted by dump_pte_ops.

    # If no NPU args are supplied, skip validation entirely.
    if args.memory_mode is None and args.config is None:
        return

    # If Arm Ethos-U NPU args are supplied, require Arm Ethos-U NPU delegation.
    if not has_delegate:
        raise ValueError(
            f"{model_path} is not Arm Ethos-U NPU optimized but NPU args were provided."
        )

    if args.memory_mode:
        if not vela_mem_mode:
            raise ValueError(
                f"Unable to find --memory-mode in {model_path} Vela command line."
            )
        requested = normalize_memory_mode(args.memory_mode)
        inferred = normalize_memory_mode(vela_mem_mode)
        if requested != inferred:
            raise ValueError(
                f"NPU memory mode mismatch for {model_path}: "
                f"requested {args.memory_mode} but model was compiled with {vela_mem_mode}."
            )

    if args.config:
        if not vela_config:
            raise ValueError(
                f"Unable to find --accelerator-config in {model_path} Vela command line."
            )
        requested_config = normalize_ethos_u_config(args.config)
        inferred_config = normalize_ethos_u_config(vela_config)
        if requested_config != inferred_config:
            raise ValueError(
                f"NPU config mismatch for {model_path}: "
                f"requested {args.config} but model was compiled with {vela_config}."
            )
