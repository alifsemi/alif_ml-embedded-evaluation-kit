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
"""TFLite Micro model optimization via Vela."""
import concurrent.futures
import fnmatch
import itertools
import logging
import os
import shlex
import tempfile
import typing
from pathlib import Path

from mlek_tools.config.npu import NpuConfig
from mlek_tools.config.optimizer import OptimizerConfig
from mlek_tools.optimize.metadata import (
    build_optimization_metadata,
    output_matches_optimization_metadata,
    write_optimization_metadata,
)
from mlek_tools.setup.python_venv import PythonEnv
from mlek_tools.setup.util import call_command


def find_unoptimized_tflite_files(download_dir: Path) -> typing.List[Path]:
    """Return all ``.tflite`` files under *download_dir* that have not been Vela-optimized."""
    return [
        Path(dirpath) / f
        for dirpath, _, files in os.walk(download_dir)
        for f in fnmatch.filter(files, "*.tflite")
        if "vela" not in f
    ]


def run_vela(
        config: NpuConfig,
        python_env: PythonEnv,
        model: Path,
        optimizer_config: OptimizerConfig,
        output_dir: Path,
        vela_version: str,
) -> bool:
    """Run Vela on *model* for the given *config*.

    :param config:              NPU hardware descriptor.
    :param python_env:          The Python virtual environment containing Vela.
    :param model:               Path to the unoptimized ``.tflite`` model.
    :param optimizer_config:    MLEK optimization settings (config file, arena size, …).
    :param output_dir:          Directory where the optimized model is written.
    :param vela_version:        Vela version used for optimization metadata.
    :return:                    ``True`` if optimization was skipped.
    """
    # pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
    new_suffix = f"{config.vela_output_suffix}.tflite"
    optimized_model_path = output_dir / (model.stem + new_suffix)
    vela_flags = optimizer_config.to_vela_extra_flags(config.memory_mode)
    optimization_metadata = build_optimization_metadata(
        model,
        config,
        vela_version,
        vela_flags,
    )
    work_parent_dir = output_dir / f"{config.config_name}_{config.memory_mode}"

    if output_matches_optimization_metadata(optimized_model_path, optimization_metadata):
        logging.info("File %s exists, skipping optimisation.", optimized_model_path)
        try:
            work_parent_dir.rmdir()
        except OSError:
            pass
        return True

    if optimized_model_path.is_file():
        optimized_model_path.unlink()

    work_parent_dir.mkdir(parents=True, exist_ok=True)
    vela_exe = python_env.bin_dir / "vela"

    with tempfile.TemporaryDirectory(
            prefix=f"{model.stem}_",
            dir=work_parent_dir,
    ) as work_dir_name:
        work_dir = Path(work_dir_name)
        vela_args = " ".join(
            shlex.quote(arg)
            for arg in optimizer_config.to_vela_cli_args(config.memory_mode)
        )
        vela_command = (
            f'"{vela_exe}" {shlex.quote(str(model))} '
            + f"--accelerator-config={config.config_name} "
            + "--optimise Performance "
            + f"--memory-mode={config.memory_mode} "
            + f"--system-config={config.system_config} "
            + f"--output-dir={shlex.quote(str(work_dir))} "
            + vela_args
        )

        call_command(vela_command, buffer_logs=True)

        for vela_output in work_dir.glob("*"):
            new_file_name = (
                f"{vela_output.stem}_{config.filename_suffix}"
                f"{vela_output.suffix}"
            )
            logging.info("Renaming %s to %s.", vela_output.name, new_file_name)
            vela_output.rename(output_dir / new_file_name)
    try:
        work_parent_dir.rmdir()
    except OSError:
        pass
    write_optimization_metadata(optimized_model_path, optimization_metadata)

    return False


def submit_tflite_optimizations(
        executor: concurrent.futures.ThreadPoolExecutor,
        unoptimized_models: typing.List[Path],
        npu_configs: typing.List[NpuConfig],
        python_env: PythonEnv,
        optimizer_config: OptimizerConfig,
        vela_version: str,
) -> typing.List[concurrent.futures.Future]:
    """Submit Vela optimization tasks to *executor* for all model × NPU-config pairs.

    :return: List of futures; each resolves to ``True`` if the run was skipped.
    """
    # pylint: disable=too-many-arguments,too-many-positional-arguments
    return [
        executor.submit(
            run_vela,
            npu_config,
            python_env,
            model_path,
            optimizer_config,
            model_path.parent,
            vela_version,
        )
        for model_path, npu_config
        in itertools.product(unoptimized_models, npu_configs)
    ]
