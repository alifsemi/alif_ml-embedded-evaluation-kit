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
"""Sidecar metadata helpers for optimized model outputs."""
import dataclasses
import hashlib
import json
import typing
from pathlib import Path

from mlek_tools.config.npu import NpuConfig
from mlek_tools.setup.util import get_sha256sum_for_file

_OPTIMIZATION_METADATA_SUFFIX = ".mlek_metadata.json"


def get_optimization_metadata_path(output_path: Path) -> Path:
    """
    Return the sidecar metadata path for an optimized model output.

    :param output_path: Optimized model output path.
    :return:            Metadata sidecar path.
    """
    return output_path.with_suffix(output_path.suffix + _OPTIMIZATION_METADATA_SUFFIX)


def get_optimization_input_hash(input_model: typing.Union[str, Path]) -> str:
    """
    Return a stable hash for an optimization input.

    :param input_model: Model path or built-in model identifier.
    :return:            SHA-256 hash of the model file or identifier.
    """
    input_path = Path(input_model)
    if input_path.is_file():
        return get_sha256sum_for_file(input_path)

    return hashlib.sha256(str(input_model).encode("utf8")).hexdigest()


def add_entrypoint_metadata(
        metadata: typing.Dict,
        lowering_entrypoint: typing.Optional[typing.Union[str, Path]],
):
    """
    Add lowering entry point identity and file hash to metadata.

    :param metadata:             Metadata dictionary to update.
    :param lowering_entrypoint:  Script path or module entry point.
    """
    if lowering_entrypoint is None:
        return

    metadata["lowering_entrypoint"] = str(lowering_entrypoint)
    lowering_entrypoint_path = Path(lowering_entrypoint)
    if lowering_entrypoint_path.is_file():
        metadata["lowering_entrypoint_hash"] = get_sha256sum_for_file(
            lowering_entrypoint_path
        )


def add_lowering_options_metadata(
        metadata: typing.Dict,
        lowering_options: typing.Optional[typing.Dict[str, typing.Any]],
):
    """
    Add ExecuTorch lowering mode options to metadata.

    :param metadata:         Metadata dictionary to update.
    :param lowering_options: Lowering mode options, if any.
    """
    if lowering_options is not None:
        metadata["lowering_options"] = lowering_options


def build_optimization_metadata(
        input_model: typing.Union[str, Path],
        npu_config: typing.Optional[NpuConfig],
        vela_version: str,
        vela_flags: typing.List[str],
        lowering_entrypoint: typing.Optional[typing.Union[str, Path]] = None,
) -> typing.Dict:
    """
    Build metadata used to decide whether an optimized output is current.

    :param input_model:          Input model path or model identifier.
    :param npu_config:          NPU configuration, or None for native TOSA outputs.
    :param vela_version:        Vela version used for optimization.
    :param vela_flags:          Effective Vela flags used for optimization.
    :param lowering_entrypoint: Optional ExecuTorch lowering script or module.
    :return:                    Optimization metadata.
    """
    metadata = {
        "input": str(input_model),
        "input_hash": get_optimization_input_hash(input_model),
        "npu_config": dataclasses.asdict(npu_config) if npu_config is not None else None,
        "ethosu_vela_version": vela_version,
        "vela_flags": vela_flags,
    }
    add_entrypoint_metadata(metadata, lowering_entrypoint)
    return metadata


def write_optimization_metadata(output_path: Path, metadata: typing.Dict):
    """
    Write sidecar metadata for an optimized output.

    :param output_path: Optimized model output path.
    :param metadata:    Metadata to write.
    """
    with open(get_optimization_metadata_path(output_path), "w",
              encoding="utf8") as metadata_file:
        json.dump(metadata, metadata_file, indent=4, default=str)


def output_matches_optimization_metadata(output_path: Path, metadata: typing.Dict) -> bool:
    """
    Return whether an optimized output and sidecar metadata are current.

    :param output_path: Optimized model output path.
    :param metadata:    Expected optimization metadata.
    :return:            True if both output and sidecar metadata match.
    """
    metadata_path = get_optimization_metadata_path(output_path)
    if not output_path.is_file() or not metadata_path.is_file():
        return False

    try:
        with open(metadata_path, encoding="utf8") as metadata_file:
            return json.load(metadata_file) == metadata
    except (OSError, json.JSONDecodeError):
        return False
