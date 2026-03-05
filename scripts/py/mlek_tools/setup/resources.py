#  SPDX-FileCopyrightText:  Copyright 2021-2026 Arm Limited and/or its
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
"""
Resource download, Vela optimisation, and ExecuTorch setup orchestration.
"""
import concurrent.futures
import dataclasses
import errno
import fnmatch
import itertools
import json
import logging
import os
import re
import sys
import typing
from pathlib import Path

from .npu_config import NpuConfig, NpuConfigs, get_default_npu_config_from_name
from .python_venv import PythonEnv, set_up_python_venv
from .setup_config import SetupConfig, PathsConfig, OptimizationConfig, SetupContext, VelaConfig
from .use_case import ExecutorchResource, UseCase, load_use_case_resources
from .util import download_file, call_command, remove_tree_dir


def get_downloaded_resources_directory(use_case: UseCase, downloads_dir: Path) -> Path:
    """
    Get the directory for a use case's downloaded resources.

    :param use_case:        The use case.
    :param downloads_dir:   The parent directory for all downloaded resources.
    :return:                The directory for the specified use case's resources.
    """
    return downloads_dir / use_case.name


def initialize_use_case_resources_directory(
        use_case: UseCase,
        metadata: typing.Dict,
        download_dir: Path,
        check_clean_folder: bool,
        setup_script_hash_verified: bool,
):
    """
    Initialize the resources_downloaded directory for a use case.

    :param use_case:                    The use case.
    :param metadata:                    The metadata.
    :param download_dir:                The parent directory.
    :param check_clean_folder:          Whether to clean the folder.
    :param setup_script_hash_verified:  Whether the hash of the setup script is verified.
    """
    use_case_resources_dir = get_downloaded_resources_directory(use_case, download_dir)
    try:
        use_case_resources_dir.mkdir()
    except OSError as err:
        if err.errno == errno.EEXIST:
            if check_clean_folder and not setup_script_hash_verified:
                use_case_info = next(
                    (
                        info for info in metadata.get("resources_info", [])
                        if isinstance(info, dict) and info.get("name") == use_case.name
                    ),
                    {},
                )
                url_prefixes = use_case_info.get("url_prefix", [])
                for i, url_prefix in enumerate(url_prefixes):
                    if i >= len(use_case.url_prefix) or url_prefix != use_case.url_prefix[i]:
                        logging.info("Removing %s resources.", use_case.name)
                        remove_tree_dir(use_case_resources_dir)
                        break
        elif err.errno != errno.EEXIST:
            logging.error("Error creating %s directory.", use_case.name)
            raise


def get_resources_to_download(
        use_case: UseCase,
        download_dir: Path
) -> typing.List[typing.Tuple[str, Path]]:
    """
    Build the list of (url, dest) pairs for resources that still need to be downloaded.

    :param use_case:        The use case.
    :param download_dir:    The parent directory.
    :return:                List of (url, destination path) tuples.
    """
    reg_expr_str = r"{url_prefix:(.*\d)}"
    reg_expr_pattern = re.compile(reg_expr_str)
    to_download = []
    for resource in use_case.resources:
        url_prefix_idx = int(reg_expr_pattern.search(resource.url).group(1))
        url = use_case.url_prefix[url_prefix_idx] + re.sub(
            reg_expr_str, "", resource.url
        )

        dest_dir = get_downloaded_resources_directory(use_case, download_dir)
        if resource.sub_folder is not None:
            dest_dir = dest_dir / resource.sub_folder

        os.makedirs(dest_dir, exist_ok=True)
        dest = dest_dir / resource.name

        if dest.is_file():
            logging.info("File %s exists, skipping download.", dest)
        else:
            to_download.append((url, dest))
    return to_download


def run_vela(
        config: NpuConfig,
        python_env: PythonEnv,
        model: Path,
        config_file: Path,
        output_dir: Path
) -> bool:
    """
    Run Vela on the specified model.

    :param config:          The NPU configuration.
    :param python_env:      The Python virtual environment.
    :param model:           The model path.
    :param config_file:     The Vela .ini config file path.
    :param output_dir:      The output directory.
    :return:                True if optimisation was skipped, False otherwise.
    """
    new_suffix = f"_vela_{config.config_id}.tflite"
    new_vela_optimised_model_path = output_dir / (model.stem + new_suffix)

    if new_vela_optimised_model_path.is_file():
        logging.info(
            "File %s exists, skipping optimisation.",
            new_vela_optimised_model_path
        )
        return True

    work_dir = output_dir / config.config_name / model.stem

    vela_command_arena_cache_size = ""
    if config.arena_cache_size:
        vela_command_arena_cache_size = f"--arena-cache-size={config.arena_cache_size}"

    vela_exe = python_env.bin_dir / "vela"
    vela_command = (
            f'"{vela_exe}" {model} '
            + f"--accelerator-config={config.config_name} "
            + "--optimise Performance "
            + f"--config {config_file} "
            + f"--memory-mode={config.memory_mode} "
            + f"--system-config={config.system_config} "
            + f"--output-dir={work_dir} "
            + f"{vela_command_arena_cache_size}"
    )

    call_command(vela_command, buffer_logs=True)

    for vela_output in work_dir.glob("*"):
        new_file_name = f"{vela_output.stem}_{config.config_id}{vela_output.suffix}"
        logging.info("Renaming %s to %s.", vela_output.name, new_file_name)
        vela_output.rename(output_dir / new_file_name)
    work_dir.rmdir()

    return False


def find_unoptimized_tflite_files(download_dir: Path) -> typing.List[Path]:
    """
    Find paths for .tflite files that have not yet been optimised with Vela.

    :param download_dir:    The parent directory in which to search.
    :return:                A list of paths for unoptimized .tflite files.
    """
    return [
        Path(dirpath) / f
        for dirpath, dirnames, files in os.walk(download_dir)
        for f in fnmatch.filter(files, "*.tflite")
        if "vela" not in f
    ]


def initialize_resources_directory(
        download_dir: Path,
        check_clean_folder: bool,
        metadata_file_path: Path,
        setup_script_hash: str,
        vela_version: str,
) -> typing.Tuple[typing.Dict, bool]:
    """
    Set up the resources_downloaded directory and check if the setup script has changed.

    :param download_dir:        Path to the resources_downloaded directory.
    :param check_clean_folder:  Determines whether to clean the downloads directory.
    :param metadata_file_path:  Path to the metadata file.
    :param setup_script_hash:   The MD5 hash of the calling setup script.
    :param vela_version:        The expected Vela version string.
    :return:                    The metadata dict and a bool indicating whether the
                                setup script hash is verified (unchanged since last run).
    """
    metadata_dict = {}
    setup_script_hash_verified = False

    if download_dir.is_dir():
        logging.info("'resources_downloaded' directory exists.")
        if check_clean_folder and metadata_file_path.is_file():
            with open(metadata_file_path, encoding="utf8") as metadata_file:
                metadata_dict = json.load(metadata_file)

            vela_in_metadata = metadata_dict["ethosu_vela_version"]
            if vela_in_metadata != vela_version:
                logging.info(
                    "Vela version in metadata is %s, current %s."
                    " Removing the resources and re-downloading them.",
                    vela_in_metadata,
                    vela_version,
                )
                remove_tree_dir(download_dir)
                metadata_dict = {}
            else:
                setup_script_hash_verified = (
                        metadata_dict.get("set_up_script_md5sum") == setup_script_hash
                )
    else:
        download_dir.mkdir()

    return metadata_dict, setup_script_hash_verified


def install_executorch(executorch_path: Path, python_env: PythonEnv) -> None:
    """
    Install ExecuTorch Python bindings within the Python virtual environment.

    :param executorch_path: Root of the ExecuTorch source tree.
    :param python_env:      The Python virtual environment.
    """
    if not executorch_path.is_dir():
        raise NotADirectoryError(f'Invalid dir {executorch_path}')

    install_script = executorch_path / 'install_executorch.sh'
    env = {**os.environ, 'PATH': f"{python_env.bin_dir}:{os.environ.get('PATH', '')}"}
    call_command(
        command=f'{install_script} --clean && {install_script}',
        env=env,
    )


def optimize_executorch_model(
        model_name: typing.Union[str, Path],
        npu_config: typing.Optional[NpuConfig],
        setup_context: SetupContext,
        output_dir: Path,
        lowering_script: Path = None
) -> bool:
    """
    Generate an optimized .pte file for ExecuTorch.

    :param model_name:      Model to optimize. Can be a model name, path to a .py script,
                            or a .pt/.pt2 checkpoint path.
    :param npu_config:      The NPU config to optimize for. If None, a TOSA PTE file
                            is generated for native host.
    :param setup_context:   The setup context.
    :param output_dir:      The output directory.
    :param lowering_script: Optional script for lowering to a specific NPU backend.
                            Defaults to aot_arm_compiler if not supplied.
    :return:                True if optimization was skipped, False otherwise.
    """
    vela_config_file = setup_context.paths_config.vela_config_file

    model_res = None
    if str(model_name).endswith('.py'):
        model_res = model_name
        model_name = Path(model_res).name.split('.')[0]
    elif str(model_name).endswith('.pt') or str(model_name).endswith('.pt2'):
        model_res = output_dir / model_name
        model_name = Path(model_res).name.split('.')[0]

    if npu_config is not None:
        cfg = (f" --target {npu_config.config_name}"
               f" --system_config {npu_config.system_config}"
               f" --memory_mode {npu_config.memory_mode}"
               f" --config {vela_config_file}"
               " --delegate --quantize")
        optimized_model_name = f"{model_name}_arm_delegate_{npu_config.config_name}.pte"
    else:
        cfg = " --target TOSA-1.0+FP"
        optimized_model_name = output_dir / f"{model_name}_arm_TOSA-1.0+FP.pte"

    optimized_model_path = output_dir / optimized_model_name
    logging.info('Looking for %s', optimized_model_path)
    if optimized_model_path.is_file():
        logging.info(
            "File %s exists, skipping optimisation.",
            optimized_model_path
        )
        return True

    python_env = setup_context.python_env
    python_exe = python_env.python
    optimize_arg = "-m examples.arm.aot_arm_compiler" if lowering_script is None \
        else lowering_script
    # Prepend the venv's bin dir to PATH so that binaries installed into the venv
    # (e.g. flatc, required by the ExecuTorch serializer) are discoverable by
    # subprocesses spawned by aot_arm_compiler.
    venv_env = os.environ.copy()
    venv_env["PATH"] = str(python_env.bin_dir) + os.pathsep + venv_env.get("PATH", "")
    call_command(
        command=(f'"{python_exe}" {optimize_arg}'
                 f" --model_name={model_name if model_res is None else model_res}"
                 f" {cfg}"
                 f" --output {output_dir}"),
        cwd=setup_context.paths_config.executorch_path,
        buffer_logs=False,
        capture_output=False,
        env=venv_env,
    )

    return False


def setup_executorch(
        setup_context: SetupContext,
) -> None:
    """
    Install ExecuTorch in the Python virtual environment.
    Note: ExecuTorch setup is not supported on Microsoft Windows.

    :param setup_context: SetupContext.
    """
    if sys.platform not in ['linux', 'darwin']:
        raise EnvironmentError(f'{sys.platform} does not support ExecuTorch set up.')

    python_env = setup_context.python_env
    executorch_path = setup_context.paths_config.executorch_path

    if not python_env.is_installed("executorch"):
        install_executorch(executorch_path, python_env)


def setup_vela(
        python_env: PythonEnv,
        parallel: int,
        vela_config: VelaConfig,
) -> None:
    """
    Install Vela into the Python virtual environment.

    :param python_env:  The Python virtual environment.
    :param parallel:    Number of threads used to build Vela (only used when
                        vela_config.install_from_source is True).
    :param vela_config: VelaConfig instance with version, url, and install_from_source.
    """
    if vela_config.install_from_source:
        python_env.pip_install_if_needed(
            f"git+{vela_config.url}@{vela_config.version}",
            installed_name="ethos-u-vela",
            environment={"CMAKE_BUILD_PARALLEL_LEVEL": parallel},
        )
    else:
        python_env.pip_install_if_needed(f"ethos-u-vela=={vela_config.version}")


def update_metadata(
        metadata_dict: typing.Dict,
        setup_script_hash: str,
        use_case_resources: typing.List[UseCase],
        metadata_file_path: Path,
        vela_version: str,
) -> None:
    """
    Update the metadata file.

    :param metadata_dict:       The metadata dictionary to update.
    :param setup_script_hash:   The setup script hash.
    :param use_case_resources:  The use case resources metadata.
    :param metadata_file_path:  The metadata file path.
    :param vela_version:        The Vela version string to record.
    """
    metadata_dict["ethosu_vela_version"] = vela_version
    metadata_dict["set_up_script_md5sum"] = setup_script_hash.strip("\n")
    metadata_dict["resources_info"] = [dataclasses.asdict(uc) for uc in use_case_resources]

    with open(metadata_file_path, "w", encoding="utf8") as metadata_file:
        json.dump(metadata_dict, metadata_file, indent=4, default=str)


def install_executorch_project(
        python_env: PythonEnv,
        executorch_resource: ExecutorchResource,
) -> None:
    """
    Install dependencies required for the specified ExecuTorch resource project.

    :param python_env:          The Python virtual environment.
    :param executorch_resource: ExecuTorch resource.
    """
    if executorch_resource.requirements_path:
        if executorch_resource.requirements_path.exists():
            python_env.pip_install_requirements(executorch_resource.requirements_path)
        else:
            raise FileNotFoundError(
                f"Requirements file does not exist: {executorch_resource.requirements_path}"
            )


def optimize_tflite_models_async(
        executor: concurrent.futures.ThreadPoolExecutor,
        unoptimized_models: typing.List[Path],
        npu_configs: typing.List[NpuConfig],
        python_env: PythonEnv,
        vela_config_file: Path,
) -> typing.List[concurrent.futures.Future]:
    """
    Run Vela on a list of models for the given NPU configs using a ThreadPoolExecutor.

    :param executor:            The ThreadPoolExecutor.
    :param unoptimized_models:  The list of tflite models to be optimized.
    :param npu_configs:         The NPU configs for which to optimize the models.
    :param python_env:          The Python virtual environment.
    :param vela_config_file:    Path to the Vela .ini config file.
    :return:                    A list of futures for the optimizations.
    """
    return [
        executor.submit(
            run_vela,
            npu_config,
            python_env,
            model_path,
            vela_config_file,
            model_path.parent
        )
        for model_path, npu_config
        in itertools.product(unoptimized_models, npu_configs)
    ]


def optimize_executorch_models_async(
        executor: concurrent.futures.ThreadPoolExecutor,
        use_cases: typing.List[UseCase],
        npu_configs: typing.List[NpuConfig],
        setup_context: SetupContext,
) -> typing.List[concurrent.futures.Future]:
    """
    Download and optimize ExecuTorch models for the specified use cases and NPU configs
    using a ThreadPoolExecutor.

    :param executor:        The ThreadPoolExecutor.
    :param use_cases:       The use cases for which to optimize ExecuTorch models.
    :param npu_configs:     The NPU configs for which to optimize the models.
    :param setup_context:   The setup context.
    :return:                A list of futures for the optimizations.
    """
    return list(itertools.chain(*(
        [
            executor.submit(
                optimize_executorch_model,
                executorch_model.model_name,
                npu_config,
                setup_context,
                setup_context.paths_config.downloads_dir / use_case.name,
                executorch_model.lowering_script
            )
            for executorch_model in use_case.executorch_resources
            if not (npu_config is not None
                    and npu_config.processor_id in executorch_model.excluded_npu_processor_ids)
        ]
        for use_case, npu_config
        in itertools.product(use_cases, npu_configs)
    )))


def parallel_setup(
        setup_context: SetupContext,
        use_cases: typing.List[UseCase],
        resources_to_download: typing.List[typing.Tuple[str, Path]],
        tflm_npu_configs: typing.List[NpuConfig],
        executorch_npu_configs: typing.List[NpuConfig],
) -> None:
    """
    Download and optimize models using a thread pool.

    :param setup_context:           The setup context.
    :param use_cases:               The list of use cases to generate resources for.
    :param resources_to_download:   The list of resources yet to be downloaded.
    :param tflm_npu_configs:        NPU configs for TensorFlow Lite Micro model optimization.
    :param executorch_npu_configs:  NPU configs for ExecuTorch model optimization.
    """
    optimize_tflite = (setup_context.setup_config.set_up_tensorflow
                       and setup_context.setup_config.run_vela_on_models)
    optimize_executorch = (setup_context.setup_config.set_up_executorch
                           and setup_context.setup_config.run_vela_on_models)
    with concurrent.futures.ThreadPoolExecutor(
            max_workers=setup_context.setup_config.parallel
    ) as executor:
        download_futures = [
            executor.submit(
                download_file, url, dest, setup_context.setup_config.http_headers
            )
            for url, dest in resources_to_download
        ]
        concurrent.futures.wait(download_futures, return_when=concurrent.futures.ALL_COMPLETED)

        model_optimize_futures = []
        if optimize_executorch:
            model_optimize_futures += optimize_executorch_models_async(
                executor, use_cases, executorch_npu_configs, setup_context
            )
        if optimize_tflite:
            model_paths = find_unoptimized_tflite_files(setup_context.paths_config.downloads_dir)
            model_optimize_futures += optimize_tflite_models_async(
                executor,
                model_paths,
                tflm_npu_configs,
                setup_context.python_env,
                setup_context.paths_config.vela_config_file,
            )
        concurrent.futures.wait(
            model_optimize_futures, return_when=concurrent.futures.ALL_COMPLETED
        )

        optimisation_skipped = any(future.result() for future in model_optimize_futures)
        if optimisation_skipped:
            logging.warning("One or more optimisations were skipped.")
            logging.warning(
                "To optimise all the models, please remove the directory %s.",
                setup_context.paths_config.downloads_dir
            )


def serial_setup(
        setup_context: SetupContext,
        use_cases: typing.List[UseCase],
        resources_to_download: typing.List[typing.Tuple[str, Path]],
        tflm_npu_configs: typing.List[NpuConfig],
        executorch_npu_configs: typing.List[NpuConfig],
) -> None:
    """
    Download and optimize models without parallelism.

    :param setup_context:           The setup context.
    :param use_cases:               The list of use cases to generate resources for.
    :param resources_to_download:   The list of resources yet to be downloaded.
    :param tflm_npu_configs:        NPU configs for TensorFlow Lite Micro model optimization.
    :param executorch_npu_configs:  NPU configs for ExecuTorch model optimization.
    """
    for url, dest in resources_to_download:
        download_file(url, dest, setup_context.setup_config.http_headers)

    optimisation_skipped = False

    if (setup_context.setup_config.set_up_tensorflow
            and setup_context.setup_config.run_vela_on_models):
        model_paths = find_unoptimized_tflite_files(setup_context.paths_config.downloads_dir)
        for model_path in model_paths:
            for config in tflm_npu_configs:
                optimisation_skipped = run_vela(
                    config,
                    setup_context.python_env,
                    model_path,
                    setup_context.paths_config.vela_config_file,
                    output_dir=model_path.parent
                ) or optimisation_skipped

    if (setup_context.setup_config.set_up_executorch
            and setup_context.setup_config.run_vela_on_models):
        to_optimize = itertools.product(use_cases, executorch_npu_configs)
        for use_case, npu_config in to_optimize:
            for executorch_resource in use_case.executorch_resources:
                if (
                    npu_config is not None
                    and npu_config.processor_id
                    in executorch_resource.excluded_npu_processor_ids
                ):
                    logging.info(
                        'Skipping %s for %s (excluded NPU processor)',
                        executorch_resource.model_name, npu_config.processor_id
                    )
                    continue
                optimisation_skipped = optimize_executorch_model(
                    model_name=executorch_resource.model_name,
                    npu_config=npu_config,
                    setup_context=setup_context,
                    output_dir=setup_context.paths_config.downloads_dir / use_case.name,
                    lowering_script=executorch_resource.lowering_script
                ) or optimisation_skipped

    if optimisation_skipped:
        logging.warning("One or more optimisations were skipped.")
        logging.warning(
            "To optimise all the models, please remove the directory %s.",
            setup_context.paths_config.downloads_dir
        )


def set_up_resources(
        setup_config: SetupConfig,
        optimization_config: OptimizationConfig,
        paths_config: PathsConfig,
        vela_config: VelaConfig,
        setup_script_hash: str,
        default_npu_configs: NpuConfigs,
        default_downloads_path: typing.Optional[Path] = None,
        min_python_version: typing.Tuple[int, int] = (3, 10),
) -> Path:
    """
    Run the full resource setup: venv, Vela, ExecuTorch, downloads, and model optimisation.

    :param setup_config:           General setup configuration.
    :param optimization_config:    Configuration related to model optimization.
    :param paths_config:           Paths configuration (including vela_config_file).
    :param vela_config:            VelaConfig instance with version, url, and install_from_source.
    :param setup_script_hash:      MD5 hash of the calling setup script, used to detect
                                   whether resources need refreshing.
    :param default_npu_configs:    NpuConfigs instance representing the default set of NPU
                                   configurations to always optimise for.
    :param default_downloads_path: If provided and paths_config.downloads_dir differs from
                                   this, a warning is logged about the non-default path.
    :param min_python_version:     Minimum required Python version as a ``(major, minor)``
                                   tuple. Defaults to ``(3, 10)``.
    :return:                       Path to the root of the virtual environment.
    """
    # pylint: disable=too-many-arguments,too-many-positional-arguments,too-many-locals

    context = SetupContext(setup_config, optimization_config, paths_config)

    if default_downloads_path and paths_config.downloads_dir != default_downloads_path:
        logging.warning(
            "Non-default downloads path '%s'. "
            "Ensure your build configuration points to this directory.",
            paths_config.downloads_dir.absolute()
        )

    metadata_file_path = (
        paths_config.metadata_file
        if paths_config.metadata_file is not None
        else paths_config.downloads_dir / "resources_downloaded_metadata.json"
    )

    if sys.version_info < min_python_version:
        raise RuntimeError(
            f"Python {min_python_version[0]}.{min_python_version[1]}+ is required."
        )
    logging.info("Using Python version: %s", sys.version_info)

    use_case_resources = load_use_case_resources(
        paths_config.use_case_resources_files,
        setup_config.use_case_names
    )

    metadata_dict, setup_script_hash_verified = initialize_resources_directory(
        paths_config.downloads_dir,
        setup_config.check_clean_folder,
        metadata_file_path,
        setup_script_hash,
        vela_config.version,
    )

    venv_dir = (
        paths_config.venv_dir
        if paths_config.venv_dir is not None
        else paths_config.downloads_dir / 'env'
    )
    context.python_env = set_up_python_venv(
        venv_dir,
        paths_config.additional_requirements_file
    )

    setup_vela(
        context.python_env,
        setup_config.parallel,
        vela_config,
    )

    if setup_config.set_up_executorch:
        setup_executorch(context)
        for use_case in use_case_resources:
            for executorch_resource in use_case.executorch_resources:
                if executorch_resource.has_requirements():
                    logging.info("Installing dependencies for %s", executorch_resource.model)
                    install_executorch_project(context.python_env, executorch_resource)

    npu_configs = [
        get_default_npu_config_from_name(npu_config_name, optimization_config.arena_cache_size)
        for npu_config_name in set(
            default_npu_configs.names
            + list(optimization_config.additional_npu_config_names)
        )
    ]

    executorch_npu_configs = [
        config for config in npu_configs
        if config.processor_id not in setup_config.executorch_excluded_npu_processor_ids
    ]
    # Append None to generate TOSA PTE files for native host as well.
    executorch_npu_configs.append(None)

    logging.info("Downloading resources.")
    to_download = []
    for use_case in use_case_resources:
        initialize_use_case_resources_directory(
            use_case,
            metadata_dict,
            paths_config.downloads_dir,
            setup_config.check_clean_folder,
            setup_script_hash_verified
        )
        to_download += get_resources_to_download(use_case, download_dir=paths_config.downloads_dir)

    if setup_config.parallel > 1:
        parallel_setup(context, use_case_resources, to_download, npu_configs,
                       executorch_npu_configs)
    else:
        serial_setup(context, use_case_resources, to_download, npu_configs,
                     executorch_npu_configs)

    logging.info("Collecting and writing metadata.")
    update_metadata(
        metadata_dict,
        setup_script_hash.strip("\n"),
        use_case_resources,
        metadata_file_path,
        vela_config.version,
    )

    return context.python_env.path
