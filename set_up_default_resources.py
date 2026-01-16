#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2021-2026 Arm Limited
#  and/or its affiliates <open-source-office@arm.com>
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
Script to set up default resources for ML Embedded Evaluation Kit
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
import textwrap
import typing
from argparse import ArgumentParser, ArgumentTypeError, Action
from enum import Enum
from pathlib import Path

from scripts.py.check_update_resources_downloaded import get_md5sum_for_file
from scripts.py.setup.npu_config import NpuConfigs, NpuConfig
from scripts.py.setup.python_venv import install_pip_package_if_needed, set_up_python_venv
from scripts.py.setup.python_venv import is_pip_package_installed, install_requirements
from scripts.py.setup.setup_config import SetupConfig, PathsConfig, OptimizationConfig, SetupContext
from scripts.py.setup.use_case import ExecutorchResourceType, ExecutorchResource
from scripts.py.setup.use_case import UseCase, load_use_case_resources
from scripts.py.setup.util import download_file, call_command, remove_tree_dir

# Supported version of Python and Vela
VELA_VERSION = "4.4.1"
py3_version_minimum = (3, 10)

# If true, install Vela from source using VELA_VERSION as a git branch/tag name
# If false, install Vela package from PyPi using VELA_VERSION as the version
INSTALL_VELA_FROM_SOURCE = False

u85_macs_to_system_configs = {
    128: "Ethos_U85_SYS_DRAM_Low",
    256: "Ethos_U85_SYS_DRAM_Low",
    512: "Ethos_U85_SYS_DRAM_Mid_512",
    1024: "Ethos_U85_SYS_DRAM_Mid_1024",
    2048: "Ethos_U85_SYS_DRAM_High_2048",
}

# Valid NPU configurations:
valid_npu_configs = NpuConfigs.create(
    *(
        NpuConfig(
            name_prefix="ethos-u55",
            macs=macs,
            processor_id="U55",
            prefix_id="H",
            memory_mode="Shared_Sram",
            system_config="Ethos_U55_High_End_Embedded",
        ) for macs in (32, 64, 128, 256)
    ),
    *(
        NpuConfig(
            name_prefix="ethos-u65",
            macs=macs,
            processor_id="U65",
            prefix_id="Y",
            memory_mode="Dedicated_Sram",
            system_config="Ethos_U65_High_End"
        ) for macs in (256, 512)
    ),
    *(
        NpuConfig(
            name_prefix="ethos-u85",
            macs=macs,
            processor_id="U85",
            prefix_id="Z",
            memory_mode="Dedicated_Sram",
            system_config=u85_macs_to_system_configs[macs]
        ) for macs in (128, 256, 512, 1024, 2048)
    )
)

# Default NPU configurations (these are always run when the models are optimised)
default_npu_configs = NpuConfigs.create(
    valid_npu_configs.get("ethos-u55", 128),
    valid_npu_configs.get("ethos-u65", 256),
    valid_npu_configs.get("ethos-u85", 256),
)


class MLFramework(Enum):
    """
    Enum to pick ML framework to use for build.
    """
    TENSORFLOW_LITE_MICRO = "tflm"
    EXECUTORCH = "executorch"


valid_ml_frameworks: typing.Set[str] = {f.value for f in MLFramework}

current_file_dir = Path(__file__).parent.resolve()
default_use_case_resources_path = current_file_dir / 'resources' / 'use_case_resources.json'
default_requirements_path = current_file_dir / 'scripts' / 'py' / 'requirements.txt'
default_downloads_path = current_file_dir / 'resources_downloaded'
default_executorch_path = current_file_dir / 'dependencies' / 'executorch'
vela_config_file = current_file_dir / "scripts" / "vela" / "default_vela.ini"

VELA_URL = "https://git.gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-vela.git"


def get_default_npu_config_from_name(
        config_name: str, arena_cache_size: int = 0
) -> typing.Optional[NpuConfig]:
    """
    Gets the file suffix for the TFLite file from the `accelerator_config` string.
    :param config_name:         Ethos-U NPU configuration from valid_npu_config_names
    :param arena_cache_size:    Specifies arena cache size in bytes. If a value
                                greater than 0 is provided, this will be taken
                                as the cache size. If 0, the default values, as per
                                the NPU config requirements, are used.
    :return                     An NpuConfig populated with defaults for the given config name
    """
    npu_config = valid_npu_configs.get_by_name(config_name)
    if not npu_config:
        raise ValueError(
            f"""
            Invalid Ethos-U NPU configuration.
            Select one from {valid_npu_configs.names}.
            """
        )
    return npu_config.overwrite_arena_cache_size(arena_cache_size)


def initialize_use_case_resources_directory(
        use_case: UseCase,
        metadata: typing.Dict,
        download_dir: Path,
        check_clean_folder: bool,
        setup_script_hash_verified: bool,
):
    """
    Initialize the resources_downloaded directory for a use case
    :param use_case:                    The use case
    :param metadata:                    The metadata
    :param download_dir:                The parent directory
    :param check_clean_folder:          Whether to clean the folder
    :param setup_script_hash_verified:  Whether the hash of this script is verified
    :return                             The path to this use case's downloaded resources
    """
    use_case_resources_dir = get_downloaded_resources_directory(use_case, download_dir)
    try:
        #  Does the usecase_name download dir exist?
        use_case_resources_dir.mkdir()
    except OSError as err:
        if err.errno == errno.EEXIST:
            # The usecase_name download dir exist.
            if check_clean_folder and not setup_script_hash_verified:
                for resources_info in metadata.get("resources_info", []):
                    use_case_info = info[0] if len(info := [
                        f for f in resources_info if f["name"] == use_case.name
                    ]) > 0 else {}
                    for i, url_prefix in enumerate(use_case_info.get("url_prefix", [])):
                        if url_prefix != use_case.url_prefix[i]:
                            logging.info("Removing %s resources.", use_case.name)
                            remove_tree_dir(use_case_resources_dir)
                            break
        elif err.errno != errno.EEXIST:
            logging.error("Error creating %s directory.", use_case.name)
            raise


def get_downloaded_resources_directory(use_case: UseCase, downloads_dir: Path) -> Path:
    """
    Get the directory for a use case's downloaded resources
    :param use_case:        The use case
    :param downloads_dir:   The parent directory for all downloaded resources
    :return:                The directory for the specified use case's resources
    """
    return downloads_dir / use_case.name


def get_resources_to_download(
        use_case: UseCase,
        download_dir: Path
) -> typing.List[typing.Tuple[str, Path]]:
    """
    Download the resources associated with a use case
    :param use_case:                    The use case
    :param download_dir:                The parent directory
    :param parallel:                    Number of download threads to use
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
        env_activate_cmd: str,
        model: Path,
        config_file: Path,
        output_dir: Path
) -> bool:
    """
    Run vela on the specified model
    :param config:              The NPU configuration
    :param env_activate_cmd:    The Python venv activation command
    :param model:               The model
    :param config_file:         The vela config file
    :param output_dir:          The output directory
    :return:                    True if the optimisation was skipped, false otherwise
    """
    # We want the name to include the configuration suffix. For example: vela_H128,
    # vela_Y512 etc.
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
        vela_command_arena_cache_size = (
            f"--arena-cache-size={config.arena_cache_size}"
        )

    vela_command = (
            f"{env_activate_cmd} && vela {model} "
            + f"--accelerator-config={config.config_name} "
            + "--optimise Performance "
            + f"--config {config_file} "
            + f"--memory-mode={config.memory_mode} "
            + f"--system-config={config.system_config} "
            + f"--output-dir={work_dir} "
            + f"{vela_command_arena_cache_size}"
    )

    call_command(vela_command, buffer_logs=True)

    # Relocate any other files output by Vela, e.g. csv output data
    for vela_output in work_dir.glob("*"):
        new_file_name = f"{vela_output.stem}_{config.config_id}{vela_output.suffix}"
        logging.info("Renaming %s to %s.", vela_output.name, new_file_name)
        vela_output.rename(output_dir / new_file_name)
    work_dir.rmdir()

    return False


def find_unoptimized_tflite_files(download_dir: Path) -> typing.List[Path]:
    """
    Find paths for .tflite files that have not yet been optimised with Vela
    :param download_dir:    The parent directory in which to search for .tflite files
    :return:                A list of paths for unoptimized .tflite files
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
        setup_script_hash: str
) -> typing.Tuple[typing.Dict, bool]:
    """
    Sets up the resources_downloaded directory and checks to see if this script
    has been modified since the last time resources were downloaded
    :param download_dir:        Path to the resources_downloaded directory
    :param check_clean_folder:  Determines whether to clean the downloads directory
    :param metadata_file_path:  Path to the metadata file
    :param setup_script_hash:   The md5 hash of this script
    :return:                    The metadata and a boolean to indicate whether this
                                script has changed since it was last run
    """
    metadata_dict = {}
    setup_script_hash_verified = False

    if download_dir.is_dir():
        logging.info("'resources_downloaded' directory exists.")
        # Check and clean?
        if check_clean_folder and metadata_file_path.is_file():
            with open(metadata_file_path, encoding="utf8") as metadata_file:
                metadata_dict = json.load(metadata_file)

            vela_in_metadata = metadata_dict["ethosu_vela_version"]
            if vela_in_metadata != VELA_VERSION:
                # Check if all the resources needs to be removed and regenerated.
                # This can happen when the Vela version has changed.
                logging.info(
                    ("Vela version in metadata is %s, current %s."
                     " Removing the resources and re-download them.",
                     vela_in_metadata,
                     VELA_VERSION
                     )
                )
                remove_tree_dir(download_dir)
                metadata_dict = {}
            else:
                # Check if the set_up_default_resorces.py has changed from last setup
                setup_script_hash_verified = (
                        metadata_dict.get("set_up_script_md5sum")
                        == setup_script_hash
                )
    else:
        download_dir.mkdir()

    return metadata_dict, setup_script_hash_verified


def install_executorch(executorch_path: Path, env_activate_cmd: str) -> None:
    """
    Installs ExecuTorch Python bindings within Python virtual environment.

    :param executorch_path:  Root of Executorch source tree.
    :param env_activate_cmd: Command to activate the Python virtual
                             environment where we need to install.
    """
    if not executorch_path.is_dir():
        raise NotADirectoryError(f'Invalid dir {executorch_path}')

    if len(env_activate_cmd.strip()) == 0:
        raise ValueError('venv activation command cannot be empty.')

    install_script = executorch_path / 'install_executorch.sh'

    call_command(
        command=f'{env_activate_cmd} && {install_script} --clean && {install_script}',
        verbose=True
    )


def optimize_executorch_model(
        model_name: str | Path,
        npu_config: NpuConfig,
        setup_context: SetupContext,
        output_dir: Path,
        lowering_script: Path = None
):
    """
    Generate an optimized .pte file for ExecuTorch
    :param model_name:      Model to optimize. This can be a name of an ExecuTorch
                            model or path to a Python script returning a tuple of
                            `torch.nn.Module` and representative input tensor.
    :param npu_config:      The NPU config for which to optimize. If this is
                            None, a TOSA PTE file is generated for native host.
    :param setup_context:   The setup context
    :param output_dir:      The output directory
    :param lowering_script  Optional script for lowering to specific NPU config backend.
                            Default aot_arm_compiler will be used if this is not supplied.
    :return:                True if optimization was skipped, False otherwise
    """
    model_res = None

    # This section sets up the script/checkpoint path to be passed to the
    # aot_arm_compiler (or lowering script) if needed. This helps with skipping
    # PTE file generation if the model already exists by resetting the
    # `model_name`.
    if str(model_name).endswith('.py'):
        model_res = model_name
        model_name = Path(model_res).name.split('.')[0]
    elif str(model_name).endswith('.pt2'):
        model_res = output_dir / model_name
        model_name = Path(model_res).name.split('.')[0]

    if npu_config is not None:
        # pylint: disable=fixme
        # TODO: Remove this once Arm Ethos-U55 NPU is supported.
        if (str(model_name).find('conformer') >= 0 and npu_config.processor_id == "U55"):
            logging.info('Conformer model is currently unsupported for %s', npu_config)
            return False

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

    optimize_arg = "-m examples.arm.aot_arm_compiler" if lowering_script is None\
                      else lowering_script
    call_command(
        command=(f"{setup_context.env_activate_cmd} && python3 {optimize_arg}"
                 f" --model_name={model_name if model_res is None else model_res}"
                 f" {cfg}"
                 f" --output {output_dir}"),
        cwd=setup_context.paths_config.executorch_path,
        verbose=True,
        buffer_logs=False,
        capture_output=False
    )

    return False


def setup_executorch(setup_context: SetupContext):
    """
    Installs TOSA tools and ExecuTorch in the Python virtual environment.
    Note: ExecuTorch setup currently not supported on Microsoft Windows based systems.
    :param setup_context:       SetupContext
    """
    if sys.platform not in ['linux', 'darwin']:
        raise EnvironmentError(f'{sys.platform} does not support ExecuTorch set up.')

    # Install TOSA tools:
    executorch_path = setup_context.paths_config.executorch_path
    if not is_pip_package_installed('tosa-tools', setup_context.env_activate_cmd):
        tosa_req_file = executorch_path / 'backends' / 'arm' / 'requirements-arm-tosa.txt'
        logging.info('Installing TOSA tools using version specified in %s',
                     tosa_req_file)
        call_command(('CMAKE_POLICY_VERSION_MINIMUM=3.5 BUILD_PYBIND=1 '
                      f'{setup_context.env_activate_cmd} && '
                      f'pip install --no-dependencies -r{tosa_req_file}'),
                     cwd=executorch_path)
    else:
        logging.info('tosa-tools package is already installed.')

    # Install ExecuTorch package
    if not is_pip_package_installed("executorch", setup_context.env_activate_cmd):
        install_executorch(executorch_path, setup_context.env_activate_cmd)


def setup_vela(
        env_activate_cmd: str,
        parallel: int
):
    """
    Install Vela into the Python virtual environment
    :param env_activate_cmd:    The command for activating the Python virtual environment
    :param parallel:            Number of threads used to build Vela (only used when
                                `INSTALL_VELA_FROM_SOURCE` is set to `True`)
    """
    if INSTALL_VELA_FROM_SOURCE:
        install_pip_package_if_needed(
            f"git+{VELA_URL}@{VELA_VERSION}",
            env_activate_cmd,
            installed_package_name="ethos-u-vela",
            environment={
                "CMAKE_BUILD_PARALLEL_LEVEL": parallel,
            },
        )
    else:
        install_pip_package_if_needed(
            f"ethos-u-vela=={VELA_VERSION}",
            env_activate_cmd
        )


def update_metadata(
        metadata_dict: typing.Dict,
        setup_script_hash: str,
        use_case_resources: typing.List[UseCase],
        metadata_file_path: Path
):
    """
    Update the metadata file
    :param metadata_dict        :   The metadata dictionary to update
    :param setup_script_hash    :   The setup script hash
    :param use_case_resources   :   The use case resources metadata
    :param metadata_file_path   :   The metadata file path
    """
    metadata_dict["ethosu_vela_version"] = VELA_VERSION
    metadata_dict["set_up_script_md5sum"] = setup_script_hash.strip("\n")
    metadata_dict["resources_info"] = [dataclasses.asdict(uc) for uc in use_case_resources]

    with open(metadata_file_path, "w", encoding="utf8") as metadata_file:
        json.dump(metadata_dict, metadata_file, indent=4, default=str)


def get_default_use_cases_names() -> typing.List[str]:
    """
    Get the names of the default use cases
    :return :   List of use case names as strings
    """
    use_case_resources = load_use_case_resources([default_use_case_resources_path])
    return [uc.name for uc in use_case_resources]


def check_paths_config(paths_config: PathsConfig):
    """
    Runs pre-setup checks on the paths config
    :param paths_config:    PathsConfig used for setup
    """
    if paths_config.downloads_dir != default_downloads_path:
        message = f"""
        You have specified a non-default path for downloading use case resources.
        To configure the CMake project, you will need to supply the following argument:

            cmake \\
                -DRESOURCES_PATH={paths_config.downloads_dir.absolute()} \\
                ...
        """.strip("\n")
        logging.warning(textwrap.dedent(message))


def install_executorch_project(
        env_activate_cmd: str,
        executorch_resource: ExecutorchResource
):
    """
    Install dependencies required for specified ExecuTorch resource project
    :param env_activate_cmd:    Python env activation command
    :param executorch_resource: ExecuTorch resource
    """
    if executorch_resource.requirements_path:
        if executorch_resource.requirements_path.exists():
            install_requirements(env_activate_cmd, executorch_resource.requirements_path)
        else:
            raise FileNotFoundError(
                f"Requirements file does not exist: {executorch_resource.requirements_path}"
            )


def optimize_tflite_models_async(
        executor: concurrent.futures.ThreadPoolExecutor,
        unoptimized_models: typing.List[Path],
        npu_configs: typing.List[NpuConfig],
        env_activate_cmd: str
) -> typing.List[concurrent.futures.Future[bool]]:
    """
    Run Vela on a list of models for the given NPU configs using a ThreadPoolExecutor
    :param executor:            The ThreadPoolExecutor
    :param unoptimized_models:  The list of tflite models to be optimized
    :param npu_configs:         The NPU configs for which to optimize the models
    :param env_activate_cmd:    The Python environment activation command
    :return:                    A list of futures for the optimizations
    """
    return [
        executor.submit(
            run_vela,
            npu_config,
            env_activate_cmd,
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
        setup_context: SetupContext
) -> typing.List[concurrent.futures.Future[bool]]:
    """
    Download and optimize ExecuTorch models for the specified use cases and NPU Configs
    using a ThreadPoolExecutor
    :param executor:        The ThreadPoolExecutor
    :param use_cases:       The use cases for which to download and optimize ExecuTorch models
    :param npu_configs:     The NPU configs for which to optimize the models
    :param setup_context:   The setup context
    :return:                A list of futures for the optimizations
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
):
    """
    Download and optimize models using a thread pool
    :param setup_context:           The setup context
    :param use_cases:               The list of use cases to generate resources for
    :param resources_to_download:   The list of resources yet to be downloaded
    :param tflm_npu_configs:        The list of NPU configs for which to generate
                                    optimized TensorFlow Lite Micro models
    :param executorch_npu_configs:  The list of NPU configs for which to generate
                                    optimized ExecuTorch models
    """
    optimize_tflite = (setup_context.setup_config.set_up_tensorflow
                       and setup_context.setup_config.run_vela_on_models)
    optimize_executorch = (setup_context.setup_config.set_up_executorch
                           and setup_context.setup_config.run_vela_on_models)
    with concurrent.futures.ThreadPoolExecutor(
            max_workers=setup_context.setup_config.parallel
    ) as executor:
        # Start parallel downloads
        download_futures = [
            executor.submit(
                download_file, url, dest, setup_context.setup_config.http_headers
            )
            for url, dest in resources_to_download
        ]
        # Wait for all models to finish downloading
        concurrent.futures.wait(
            download_futures,
            return_when=concurrent.futures.ALL_COMPLETED
        )
        # Start optimizing previously-downloaded tflite models and ExecuTorch models
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
                setup_context.env_activate_cmd
            )
        # Wait for all models to finish optimizing
        concurrent.futures.wait(
            model_optimize_futures,
            return_when=concurrent.futures.ALL_COMPLETED
        )

        optimisation_skipped = any(future.result() for future in model_optimize_futures)

        # If any optimisation was skipped, show how to regenerate:
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
        executorch_npu_configs: typing.List[NpuConfig]
):
    """
    Download and optimize models without parallelism
    :param setup_context:           The setup context
    :param use_cases:               The list of use cases to generate resources for
    :param resources_to_download:   The list of resources yet to be downloaded
    :param tflm_npu_configs:        The list of NPU configs for which to generate
                                    optimized TensorFlow Lite Micro models
    :param executorch_npu_configs:  The list of NPU configs for which to generate
                                    optimized ExecuTorch models
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
                    setup_context.env_activate_cmd,
                    model_path,
                    vela_config_file,
                    output_dir=model_path.parent
                ) or optimisation_skipped

    if (setup_context.setup_config.set_up_executorch
            and setup_context.setup_config.run_vela_on_models):
        to_optimize = itertools.product(use_cases, executorch_npu_configs)
        for use_case, npu_config in to_optimize:
            for executorch_resource in use_case.executorch_resources:
                optimisation_skipped = optimize_executorch_model(
                    model_name=executorch_resource.model_name,
                    npu_config=npu_config,
                    setup_context=setup_context,
                    output_dir=setup_context.paths_config.downloads_dir / use_case.name,
                    lowering_script=executorch_resource.lowering_script
                ) or optimisation_skipped

    # If any optimisation was skipped, show how to regenerate:
    if optimisation_skipped:
        logging.warning("One or more optimisations were skipped.")
        logging.warning(
            "To optimise all the models, please remove the directory %s.",
            setup_context.paths_config.downloads_dir
        )


def set_up_resources(
        setup_config: SetupConfig,
        optimization_config: OptimizationConfig,
        paths_config: PathsConfig
) -> Path:
    """
    Run the setup.
    :param setup_config         :   General setup configuration
    :param optimization_config  :   Configuration related to model optimization
    :param paths_config         :   Paths configuration
    :return                     :   Path to the root of virtual environment.
    """
    context = SetupContext(setup_config, optimization_config, paths_config)

    # Paths.
    check_paths_config(paths_config)
    metadata_file_path = paths_config.downloads_dir / "resources_downloaded_metadata.json"

    # Is Python minimum requirement matched?
    if sys.version_info < py3_version_minimum:
        raise RuntimeError(
            f"ERROR: Python{'.'.join(str(i) for i in py3_version_minimum)}+ is required,"
            f" please see the documentation on how to update it."
        )
    logging.info("Using Python version: %s", sys.version_info)

    use_case_resources = load_use_case_resources(
        paths_config.use_case_resources_files,
        setup_config.use_case_names
    )
    setup_script_hash = get_md5sum_for_file(Path(__file__).resolve())

    metadata_dict, setup_script_hash_verified = initialize_resources_directory(
        paths_config.downloads_dir,
        setup_config.check_clean_folder,
        metadata_file_path,
        setup_script_hash
    )

    context.env_path, context.env_activate_cmd = set_up_python_venv(
        paths_config.downloads_dir,
        paths_config.additional_requirements_file
    )

    setup_vela(context.env_activate_cmd, setup_config.parallel)

    if setup_config.set_up_executorch:
        setup_executorch(context)
        for use_case in use_case_resources:
            for executorch_resource in use_case.executorch_resources:
                if executorch_resource.type == ExecutorchResourceType.LOCAL_PROJECT:
                    logging.info("Installing dependencies for %s", executorch_resource.model)
                    install_executorch_project(context.env_activate_cmd, executorch_resource)

    # Download models
    npu_configs = [
        get_default_npu_config_from_name(npu_config_name, optimization_config.arena_cache_size)
        for npu_config_name in set(
            default_npu_configs.names
            + list(optimization_config.additional_npu_config_names)
        )
    ]

    # Arm AOT compiler (helper script within ExecuTorch) does not list support
    # for Arm Ethos-U65 yet.
    executorch_npu_configs = [config for config in npu_configs if config.processor_id != "U65"]

    # For ExecuTorch, we need to generate TOSA PTE files for native host as well.
    # Appending None here to the list as a temporary workaround.
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
        to_download += get_resources_to_download(
            use_case,
            download_dir=paths_config.downloads_dir,
        )

    if setup_config.parallel > 1:
        parallel_setup(
            context,
            use_case_resources,
            to_download,
            npu_configs,
            executorch_npu_configs
        )
    else:
        serial_setup(
            context,
            use_case_resources,
            to_download,
            npu_configs,
            executorch_npu_configs
        )

    # Collect and write metadata
    logging.info("Collecting and write metadata.")
    update_metadata(
        metadata_dict,
        setup_script_hash.strip("\n"),
        use_case_resources,
        metadata_file_path
    )

    return context.env_path


class HttpHeadersAction(Action):
    """
    Action for collecting HTTP headers into a [domain] -> [[header1], [header2]...] dict mapping
    """

    def __call__(self, _, namespace, values, option_string=None):
        domain, header = values
        all_current_values = getattr(namespace, self.dest, None) or {}
        headers_for_domain = all_current_values.get(domain, [])
        headers_for_domain.append(tuple(v.strip() for v in header.split(":")))
        all_current_values[domain] = headers_for_domain
        setattr(namespace, self.dest, all_current_values)


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument(
        "--skip-vela",
        help="Do not run Vela optimizer on downloaded models.",
        action="store_true",
    )
    parser.add_argument(
        "--ml-frameworks",
        help=f"""Specify the ML frameworks for which to set up resources.
        Valid values are: {valid_ml_frameworks}
        """,
        nargs="+",
        default=[MLFramework.TENSORFLOW_LITE_MICRO.value],
        action="store",
    )
    parser.add_argument(
        "--additional-ethos-u-config-name",
        help=f"""Additional (non-default) configurations for Vela:
                        {valid_npu_configs.names}""",
        default=[],
        action="append",
    )
    parser.add_argument(
        "--use-case",
        help=f"""Only set up resources for the specified use case (can specify multiple times).
        Valid values are: {get_default_use_cases_names()}
        """,
        default=[],
        action="append",
    )
    parser.add_argument(
        "--arena-cache-size",
        help="Arena cache size in bytes (if overriding the defaults)",
        type=int,
        default=0,
    )
    parser.add_argument(
        "--clean",
        help="Clean the directory and optimize the downloaded resources",
        action="store_true",
    )
    parser.add_argument(
        "--parallel",
        help="Number of threads to use for downloads and model optimisation",
        type=int,
        default=1
    )
    parser.add_argument(
        "--requirements-file",
        help="Path to requirements.txt file to install additional packages",
        type=Path,
        default=default_requirements_path
    )
    parser.add_argument(
        "--use-case-resources-file",
        help="Path to a use case resources file",
        type=Path,
        nargs="+",
        default=[],
        action="append",
    )
    parser.add_argument(
        "--downloads-dir",
        help="Path to downloaded model resources",
        type=Path,
        default=default_downloads_path
    )
    parser.add_argument(
        "--http-header",
        help="""Specify HTTP Headers to set when downloading from a domain
            Example:
                --http-header my-internal-website.com 'Authorization: Bearer $TOKEN'"""
        ,
        type=str,
        metavar=("DOMAIN", "HEADER"),
        nargs=2,
        default={},
        action=HttpHeadersAction,
    )

    parsed_args = parser.parse_args()

    if parsed_args.arena_cache_size < 0:
        raise ArgumentTypeError("Arena cache size cannot not be less than 0")

    if not Path(parsed_args.requirements_file).is_file():
        raise ArgumentTypeError(f"Invalid requirements file: {parsed_args.requirements_file}")

    logging.basicConfig(filename="log_build_default.log", level=logging.DEBUG)
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    ml_frameworks = valid_ml_frameworks \
        if len(parsed_args.ml_frameworks) == 0 \
        else valid_ml_frameworks.intersection(parsed_args.ml_frameworks)

    setup = SetupConfig(
        run_vela_on_models=not parsed_args.skip_vela,
        use_case_names=parsed_args.use_case,
        check_clean_folder=parsed_args.clean,
        set_up_executorch=MLFramework.EXECUTORCH.value in ml_frameworks,
        set_up_tensorflow=MLFramework.TENSORFLOW_LITE_MICRO.value in ml_frameworks,
        parallel=parsed_args.parallel,
        http_headers=parsed_args.http_header
    )

    optimization = OptimizationConfig(
        additional_npu_config_names=parsed_args.additional_ethos_u_config_name,
        arena_cache_size=parsed_args.arena_cache_size,
    )

    use_case_resources_files = [default_use_case_resources_path] \
        if len(parsed_args.use_case_resources_file) == 0 \
        else list(itertools.chain(*parsed_args.use_case_resources_file))

    paths = PathsConfig(
        use_case_resources_files=use_case_resources_files,
        downloads_dir=parsed_args.downloads_dir,
        additional_requirements_file=parsed_args.requirements_file,
        executorch_path=default_executorch_path
    )

    set_up_resources(setup, optimization, paths)
