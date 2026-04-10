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
Arm Ethos-U NPU validation utilities for TFLite models.

This module parses TFLite flatbuffers directly using the Vela schema.
The validation logic is purely offline and does not require running the model.
"""
import struct
from pathlib import Path
from typing import Optional, Tuple

from mlek_tools.validation.npu_validation import (
    NpuValidationArgs,
    normalize_memory_mode,
    parse_accelerator_config,
)


def _load_tflite_schema():
    """
    Load the TFLite flatbuffer schema from ethos-u-vela.

    Returns:
        (Model, TensorType): schema classes used to parse a tflite file.
    """
    # Import inside the function so this module stays importable even if
    # the Vela package is not installed (the caller can handle the error).
    try:
        from ethosu.vela.tflite.Model import Model  # type: ignore  # pylint: disable=import-error,import-outside-toplevel
        from ethosu.vela.tflite.TensorType import TensorType  # type: ignore  # pylint: disable=import-error,import-outside-toplevel
        return Model, TensorType
    except Exception as exc:  # pylint: disable=broad-except
        raise RuntimeError(
            "Failed to import ethosu.vela.tflite schema. "
            "Ensure the Python virtual environment includes ethos-u-vela."
        ) from exc



def _tflite_has_ethos_u_op(model) -> bool:
    """
    Detect Arm Ethos-U NPU usage by checking for the custom "ethos-u" op.
    """
    # Ethos-U compilation introduces a custom op with code "ethos-u".
    opcodes_len = model.OperatorCodesLength()
    for i in range(opcodes_len):
        opcode = model.OperatorCodes(i)
        custom_code = opcode.CustomCode()
        if not custom_code:
            continue
        if isinstance(custom_code, bytes):
            custom_code = custom_code.decode("utf-8", "ignore")
        if custom_code.lower() == "ethos-u":
            return True
    return False


def _tflite_infer_memory_mode(model) -> Optional[str]:
    """
    Infer memory mode from scratch/scratch_fast tensor sizes.

    Returns:
        "shared" if sizes match, "dedicated" if they differ, None if not found.
    """
    if model.SubgraphsLength() == 0:
        return None
    # We only need the first subgraph for the scratch tensor heuristic.
    subgraph = model.Subgraphs(0)
    scratch_sizes = []
    scratch_fast_sizes = []
    for i in range(subgraph.TensorsLength()):
        tensor = subgraph.Tensors(i)
        name = tensor.Name()
        if name is None:
            continue
        if isinstance(name, bytes):
            name = name.decode("utf-8", "ignore")
        name = name.lower()
        # Vela uses a 1D scratch tensor; assume Shape(0) encodes the size.
        if tensor.ShapeLength() < 1:
            continue
        size = tensor.Shape(0)
        if size <= 0:
            continue
        # Vela names scratch buffers consistently; match by substring.
        if "scratch_fast" in name:
            scratch_fast_sizes.append(size)
        elif "scratch" in name:
            scratch_sizes.append(size)

    if not scratch_sizes or not scratch_fast_sizes:
        return None

    # Use the max size to be robust to multiple scratch buffers.
    scratch = max(scratch_sizes)
    scratch_fast = max(scratch_fast_sizes)
    if scratch == scratch_fast:
        return "shared"
    return "dedicated"


def _tflite_detect_cop1_config(model) -> Tuple[Optional[str], Optional[int]]:
    """
    Extract Arm Ethos-U NPU product and MACs from the COP1 driver payload.

    Returns:
        (NPU ID string, MACs) or (None, None) if payload not found.
    """
    try:
        # Vela exposes a single register layout module; config_r includes a
        # "product" field that distinguishes U55/U65/U85.
        from ethosu.vela.ethos_u55_regs.ethos_u55_regs import config_r  # type: ignore  # pylint: disable=import-error,import-outside-toplevel
    except Exception:  # pylint: disable=broad-except
        return None, None

    # Product field values are defined by Vela; map to user-facing IDs.
    npu_product_map = {0: "U55", 1: "U65", 2: "U85"}
    for i in range(model.BuffersLength()):
        buf = model.Buffers(i)
        if buf.DataLength() < 12:
            continue
        # COP1 payload starts with a 12-byte header:
        # 0..3 "COP1", 8..11 config register with product + macs bits.
        data = bytes(buf.DataAsNumpy()[:12])
        if data[:4] != b"COP1":
            continue
        cr = config_r()
        cr.word = struct.unpack_from("<I", data, 8)[0]
        product = npu_product_map.get(cr.get_product())
        macs = 1 << cr.get_macs_per_cc()
        return product, macs
    return None, None


def validate_tflite_model(model_path: Path, args: NpuValidationArgs) -> None:
    """
    Validate Arm Ethos-U NPU settings for a TFLite model.

    Checks:
    - Presence of Arm Ethos-U NPU custom op
    - COP1 payload for NPU product + MACs (config)
    - scratch/scratch_fast sizes for memory mode
    """
    # If no NPU args are supplied, skip validation entirely.
    if args.memory_mode is None and args.config is None:
        return

    model_cls, _ = _load_tflite_schema()

    # Read the flatbuffer once; everything else is derived from it.
    buf = model_path.read_bytes()
    model = model_cls.GetRootAs(buf, 0)

    # If Arm Ethos-U NPU args are supplied, require the Arm Ethos-U NPU op.
    if not _tflite_has_ethos_u_op(model):
        raise ValueError(
            f"{model_path} is not Arm Ethos-U NPU optimized but NPU args were provided."
        )

    # Validate against COP1 header to avoid relying on filenames.
    detected_npu_id, detected_macs = _tflite_detect_cop1_config(model)
    if detected_npu_id is None or detected_macs is None:
        raise ValueError(
            f"{model_path} is missing COP1 payload; unable to validate NPU config."
        )

    if args.memory_mode:
        # Memory mode is inferred from scratch/scratch_fast tensor sizes.
        inferred = _tflite_infer_memory_mode(model)
        if inferred is None:
            print(
                "warning: unable to infer Arm Ethos-U NPU memory mode from "
                f"{model_path}; expected scratch and scratch_fast tensors."
            )
        else:
            requested = normalize_memory_mode(args.memory_mode)
            if inferred != requested:
                inferred_mode = (
                    "Dedicated_Sram" if inferred == "dedicated" else "Shared_Sram"
                )
                print(
                    "warning: NPU memory mode mismatch for "
                    f"{model_path}: requested {args.memory_mode} but model looks like "
                    f"{inferred_mode}."
                )

    if args.config:
        # Config is validated from COP1 product + MACs.
        expected_npu_id, expected_macs = parse_accelerator_config(args.config)
        if expected_npu_id is None or expected_macs is None:
            raise ValueError(
                f"Unsupported Arm Ethos-U NPU config '{args.config}'. "
                "Expected ethos-u55-128 style."
            )
        if detected_npu_id != expected_npu_id:
            raise ValueError(
                f"NPU config mismatch for {model_path}: "
                f"requested {expected_npu_id} but model is {detected_npu_id}."
            )
        if detected_macs != expected_macs:
            raise ValueError(
                f"NPU MACs mismatch for {model_path}: "
                f"requested {expected_macs} but model is {detected_macs}."
            )
