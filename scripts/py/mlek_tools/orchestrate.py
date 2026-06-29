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
"""Top-level orchestrator: ties together download, install, and optimization phases."""
import concurrent.futures
import dataclasses
import itertools
import json
import logging
import sys
import typing
from pathlib import Path

from mlek_tools.config.download import DownloadConfig
from mlek_tools.config.executorch import ExecuTorchConfig
from mlek_tools.config.npu import NpuConfig
from mlek_tools.config.optimizer import OptimizerConfig
from mlek_tools.config.paths import PathsConfig
from mlek_tools.config.tflite import TfliteConfig
from mlek_tools.download.resources import (
    ResourceDownload,
    download_resources,
    get_resources_to_download,
    initialize_resources_directory,
    initialize_use_case_resources_directory,
)
from mlek_tools.optimize.executorch import submit_executorch_optimizations
from mlek_tools.optimize.executorch import ExecuTorchOptimizationContext, optimize_executorch_model
from mlek_tools.optimize.tflite import find_unoptimized_tflite_files, submit_tflite_optimizations
from mlek_tools.optimize.tflite import run_vela
from mlek_tools.setup.install import install_executorch_project, setup_executorch, setup_vela
from mlek_tools.setup.python_venv import set_up_python_venv
from mlek_tools.use_case.model import UseCase, load_use_case_resources


def _merge_npu_configs(
        default_npu_configs: typing.Sequence[NpuConfig],
        additional_npu_config_names: typing.Sequence[str],
) -> typing.List[NpuConfig]:
    """
    Merge default and additional NPU configurations without losing memory modes.

    :param default_npu_configs:             Configs supplied by the caller.
    :param additional_npu_config_names:     Extra config names requested by users.
    :return:                                Ordered unique NPU configs.
    """
    merged_configs: typing.List[NpuConfig] = []
    seen_configs: typing.Set[NpuConfig] = set()

    for config in itertools.chain(
            default_npu_configs,
            (
                NpuConfig.from_name(config_name)
                for config_name in additional_npu_config_names
            ),
    ):
        if config not in seen_configs:
            seen_configs.add(config)
            merged_configs.append(config)

    return merged_configs


def _update_metadata(
        metadata_dict: typing.Dict,
        setup_script_hash: str,
        use_case_resources: typing.List[UseCase],
        metadata_file_path: Path,
        vela_version: str,
) -> None:
    metadata_dict["ethosu_vela_version"] = vela_version
    metadata_dict["set_up_script_sha256sum"] = setup_script_hash.strip("\n")
    metadata_dict["resources_info"] = [dataclasses.asdict(uc) for uc in use_case_resources]
    with open(metadata_file_path, "w", encoding="utf8") as metadata_file:
        json.dump(metadata_dict, metadata_file, indent=4, default=str)


def _run_optimize_phase(
        use_cases: typing.List[UseCase],
        tflm_npu_configs: typing.List[NpuConfig],
        executorch_npu_configs: typing.List[typing.Optional[NpuConfig]],
        python_env,
        tflite_config: TfliteConfig,
        executorch_config: ExecuTorchConfig,
        optimizer_config: OptimizerConfig,
        downloads_dir: Path,
        parallel: int,
) -> None:
    """Run TFLM and ExecuTorch optimization; warn if any outputs were skipped."""
    # pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
    # pylint: disable=too-many-branches
    optimize_tflite = tflite_config.enabled and tflite_config.run_vela
    optimize_executorch = executorch_config.enabled and executorch_config.run_lowering
    executorch_optimization_context = ExecuTorchOptimizationContext(
        python_env=python_env,
        executorch_config=executorch_config,
        optimizer_config=optimizer_config,
        output_dir=downloads_dir,
        vela_version=tflite_config.vela_version,
    )

    if parallel > 1:
        with concurrent.futures.ThreadPoolExecutor(
                max_workers=parallel,
                thread_name_prefix="optimise",
        ) as executor:
            futures: typing.List[concurrent.futures.Future] = []

            if optimize_tflite:
                unoptimized = find_unoptimized_tflite_files(downloads_dir)
                futures += submit_tflite_optimizations(
                    executor,
                    unoptimized,
                    tflm_npu_configs,
                    python_env,
                    optimizer_config,
                    tflite_config.vela_version,
                )
            if optimize_executorch:
                futures += submit_executorch_optimizations(
                    executor, use_cases, executorch_npu_configs,
                    executorch_optimization_context
                )

            concurrent.futures.wait(futures, return_when=concurrent.futures.ALL_COMPLETED)
            skipped = False
            for future in futures:
                skipped = future.result() or skipped
    else:
        skipped = False

        if optimize_tflite:
            for model_path in find_unoptimized_tflite_files(downloads_dir):
                for config in tflm_npu_configs:
                    skipped = run_vela(
                        config,
                        python_env,
                        model_path,
                        optimizer_config,
                        model_path.parent,
                        tflite_config.vela_version,
                    ) or skipped

        if optimize_executorch:
            for use_case, npu_config in itertools.product(use_cases, executorch_npu_configs):
                for resource in use_case.executorch_resources:
                    if (
                        npu_config is not None
                        and npu_config.processor_id
                        in resource.excluded_npu_processor_ids
                    ):
                        logging.info(
                            "Skipping %s for %s (excluded NPU processor)",
                            resource.model_name, npu_config.processor_id
                        )
                        continue
                    skipped = optimize_executorch_model(
                        model_name=resource.model_name,
                        npu_config=npu_config,
                        context=dataclasses.replace(
                            executorch_optimization_context,
                            output_dir=downloads_dir / use_case.name,
                        ),
                        lowering_script=resource.lowering_script,
                    ) or skipped

    if skipped:
        logging.warning("One or more optimisations were skipped.")
        logging.warning(
            "Optimized model outputs with matching metadata are reused automatically; "
            "use --clean only when downloaded resources need a metadata refresh."
        )


def set_up_resources(
        download_config: DownloadConfig,
        optimizer_config: OptimizerConfig,
        tflite_config: TfliteConfig,
        executorch_config: ExecuTorchConfig,
        paths_config: PathsConfig,
        setup_script_hash: str,
        default_npu_configs: typing.Sequence[NpuConfig],
        default_downloads_path: typing.Optional[Path] = None,
        min_python_version: typing.Tuple[int, int] = (3, 10),
) -> Path:
    """Run the full resource setup pipeline.

    Execution order:
    1. Validate Python version.
    2. Create/reuse virtual environment and install requirements.
    3. Install Vela (if TFLM enabled).
    4. Install ExecuTorch (if ExecuTorch enabled).
    5. **Download phase** — all use case resources.
    6. **Optimize phase** — TFLM Vela and/or ExecuTorch lowering.
    7. Write metadata.

    :param download_config:         Download and operational settings.
    :param optimizer_config:        NPU optimization settings (single source of truth).
    :param tflite_config:           TFLite Micro / Vela framework settings.
    :param executorch_config:       ExecuTorch framework settings.
    :param paths_config:            Project filesystem paths.
    :param setup_script_hash:       SHA-256 of the calling setup script for cache validation.
    :param default_npu_configs:     NPU configs always optimized for.
    :param default_downloads_path:  Logged as a warning if ``paths_config.downloads_dir`` differs.
    :param min_python_version:      Minimum Python version tuple.
    :return:                        Path to the root of the virtual environment.
    """
    # pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals
    if sys.version_info < min_python_version:
        raise RuntimeError(
            f"Python {min_python_version[0]}.{min_python_version[1]}+ is required."
        )
    logging.info("Using Python version: %s", sys.version_info)

    if default_downloads_path and paths_config.downloads_dir != default_downloads_path:
        logging.warning(
            "Non-default downloads path '%s'. "
            "Ensure your build configuration points to this directory.",
            paths_config.downloads_dir.absolute(),
        )

    metadata_file_path = (
        paths_config.metadata_file
        if paths_config.metadata_file is not None
        else paths_config.downloads_dir / "resources_downloaded_metadata.json"
    )

    use_case_resources = load_use_case_resources(
        paths_config.use_case_resources_files,
        download_config.use_case_names,
    )

    metadata_dict, setup_script_hash_verified = initialize_resources_directory(
        paths_config.downloads_dir,
        download_config.check_clean_folder,
        metadata_file_path,
        setup_script_hash,
        tflite_config.vela_version,
    )

    venv_dir = (
        paths_config.venv_dir
        if paths_config.venv_dir is not None
        else paths_config.downloads_dir / "env"
    )
    python_env = set_up_python_venv(venv_dir, paths_config.requirements_files)

    if tflite_config.enabled or executorch_config.run_lowering:
        setup_vela(python_env, tflite_config, parallel=download_config.parallel)

    if executorch_config.enabled:
        setup_executorch(python_env, executorch_config)
        for use_case in use_case_resources:
            for resource in use_case.executorch_resources:
                if resource.has_requirements():
                    logging.info("Installing dependencies for %s", resource.model)
                    install_executorch_project(python_env, resource)

    # Resolve which NPU configs to compile for, preserving memory-mode variants.
    tflm_npu_configs = _merge_npu_configs(
        default_npu_configs,
        optimizer_config.additional_npu_config_names,
    )

    executorch_npu_configs: typing.List[typing.Optional[NpuConfig]] = [
        config for config in tflm_npu_configs
        if config.processor_id not in executorch_config.excluded_npu_processor_ids
    ]
    executorch_npu_configs.append(None)  # None → TOSA/native host build

    # ── Download phase ───────────────────────────────────────────────────────
    logging.info("Downloading resources.")
    to_download: typing.List[ResourceDownload] = []
    for use_case in use_case_resources:
        initialize_use_case_resources_directory(
            use_case,
            metadata_dict,
            paths_config.downloads_dir,
            download_config.check_clean_folder,
            setup_script_hash_verified,
        )
        to_download += get_resources_to_download(use_case, paths_config.downloads_dir)

    download_resources(to_download, download_config.http_headers, download_config.parallel)

    # ── Optimize phase ───────────────────────────────────────────────────────
    _run_optimize_phase(
        use_cases=use_case_resources,
        tflm_npu_configs=tflm_npu_configs,
        executorch_npu_configs=executorch_npu_configs,
        python_env=python_env,
        tflite_config=tflite_config,
        executorch_config=executorch_config,
        optimizer_config=optimizer_config,
        downloads_dir=paths_config.downloads_dir,
        parallel=download_config.parallel,
    )

    logging.info("Collecting and writing metadata.")
    _update_metadata(
        metadata_dict,
        setup_script_hash,
        use_case_resources,
        metadata_file_path,
        tflite_config.vela_version,
    )

    return python_env.path
