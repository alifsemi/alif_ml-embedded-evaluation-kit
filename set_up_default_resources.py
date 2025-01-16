#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2021-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
import dataclasses
import errno
import fnmatch
import json
import logging
import os
import re
import shutil
import subprocess
import sys
import textwrap
import typing
import urllib.request
import venv
from argparse import ArgumentParser
from argparse import ArgumentTypeError
from dataclasses import dataclass
from pathlib import Path
from urllib.error import URLError

from scripts.py.check_update_resources_downloaded import get_md5sum_for_file
from scripts.py.vela_configs import NpuConfigs, NpuConfig

# Supported version of Python and Vela
VELA_VERSION = "4.0.1"
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

current_file_dir = Path(__file__).parent.resolve()
default_use_case_resources_path = current_file_dir / 'scripts' / 'py' / 'use_case_resources.json'
default_requirements_path = current_file_dir / 'scripts' / 'py' / 'requirements.txt'
default_downloads_path = current_file_dir / 'resources_downloaded'


@dataclass(frozen=True)
class UseCaseResource:
    """
    Represent a use case's resource
    """
    name: str
    url: str
    sub_folder: typing.Optional[str] = None


@dataclass(frozen=True)
class UseCase:
    """
    Represent a use case
    """
    name: str
    url_prefix: str
    resources: typing.List[UseCaseResource]


@dataclass(frozen=True)
class SetupConfig:
    """
    Configuration for setup behaviour.

    Attributes:
        run_vela_on_models (bool)           :   Whether to run Vela on the downloaded models
        additional_npu_config_names (list)  :   List of strings of Ethos-U NPU configs.
        use_case_names (list)               :   List of names of use cases to set up resources for
                                                (default is all).
        arena_cache_size (int)              :   Specifies arena cache size in bytes. If a value
                                                greater than 0 is provided, this will be taken
                                                as the cache size. If 0, the default values, as per
                                                the NPU config requirements, are used.
        check_clean_folder (bool)           :   Indicates whether the resources folder needs to
                                                be checked for updates and cleaned.
    """
    run_vela_on_models: bool = False
    additional_npu_config_names: typing.List[str] = ()
    use_case_names: typing.List[str] = ()
    arena_cache_size: int = 0
    check_clean_folder: bool = False


@dataclass(frozen=True)
class PathsConfig:
    """
    Configuration of paths to resources used by the setup process.

    Attributes:
        additional_requirements_file (str)  :   Path to a requirements.txt file if
                                                additional packages need to be
                                                installed.
        use_case_resources_file (Path)      :   Path to a JSON file containing the use case
                                                metadata resources.

        downloads_dir (Path)                :  Path to store model resources files.
    """
    additional_requirements_file: Path = ""
    use_case_resources_file: Path = ""
    downloads_dir: Path = ""


def load_use_case_resources(
        use_case_resources_file: Path,
        use_case_names: typing.List[str] = ()
) -> typing.List[UseCase]:
    """
    Load use case metadata resources

    Parameters
    ----------
    use_case_resources_file :   Path to a JSON file containing the use case
                                metadata resources.
    use_case_names          :   List of named use cases to restrict
                                resource loading to.
    Returns
    -------
    The use cases resources object parsed to a dict
    """

    with open(use_case_resources_file, encoding="utf8") as f:
        parsed_use_cases = json.load(f)
        use_cases = (
            UseCase(
                name=u["name"],
                url_prefix=u["url_prefix"],
                resources=[UseCaseResource(**r) for r in u["resources"]],
            )
            for u in parsed_use_cases
        )

        if len(use_case_names) == 0:
            return list(use_cases)

        return [uc for uc in use_cases if uc.name in use_case_names]


def call_command(command: str, verbose: bool = True) -> str:
    """
    Helpers function that call subprocess and return the output.

    Parameters:
    ----------
    command (string):  Specifies the command to run.
    """
    if verbose:
        logging.info(command)
    try:
        proc = subprocess.run(
            command, check=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True
        )
        log = proc.stdout.decode("utf-8")
        logging.info(log)
        return log
    except subprocess.CalledProcessError as err:
        log = err.stdout.decode("utf-8")
        logging.error(log)
        raise err


def get_default_npu_config_from_name(
        config_name: str, arena_cache_size: int = 0
) -> typing.Optional[NpuConfig]:
    """
    Gets the file suffix for the TFLite file from the
    `accelerator_config` string.

    Parameters:
    ----------
    config_name (str):      Ethos-U NPU configuration from valid_npu_config_names

    arena_cache_size (int): Specifies arena cache size in bytes. If a value
                            greater than 0 is provided, this will be taken
                            as the cache size. If 0, the default values, as per
                            the NPU config requirements, are used.

    Returns:
    -------
    NpuConfig: An NpuConfig populated with defaults for the given config name
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


def remove_tree_dir(dir_path: Path):
    """
    Delete and re-create a directory

    Parameters
    ----------
    dir_path    : The directory path
    """
    try:
        # Remove the full directory.
        shutil.rmtree(dir_path)
        # Re-create an empty one.
        os.mkdir(dir_path)
    except OSError:
        logging.error("Failed to delete %s.", dir_path)


def initialize_use_case_resources_directory(
        use_case: UseCase,
        metadata: typing.Dict,
        download_dir: Path,
        check_clean_folder: bool,
        setup_script_hash_verified: bool,
):
    """
    Initialize the resources_downloaded directory for a use case

    @param use_case:                    The use case
    @param metadata:                    The metadata
    @param download_dir:                The parent directory
    @param check_clean_folder:          Whether to clean the folder
    @param setup_script_hash_verified:  Whether the hash of this script is verified
    """
    try:
        #  Does the usecase_name download dir exist?
        (download_dir / use_case.name).mkdir()
    except OSError as err:
        if err.errno == errno.EEXIST:
            # The usecase_name download dir exist.
            if check_clean_folder and not setup_script_hash_verified:
                for idx, metadata_uc_url_prefix in enumerate(
                        [
                            f
                            for f in metadata["resources_info"]
                            if f["name"] == use_case.name
                        ][0]["url_prefix"]
                ):
                    if metadata_uc_url_prefix != use_case.url_prefix[idx]:
                        logging.info("Removing %s resources.", use_case.name)
                        remove_tree_dir(download_dir / use_case.name)
                        break
        elif err.errno != errno.EEXIST:
            logging.error("Error creating %s directory.", use_case.name)
            raise


def download_file(url: str, dest: Path):
    """
    Download a file

    @param url:     The URL of the file to download
    @param dest:    The destination of downloaded file
    """
    try:
        with urllib.request.urlopen(url) as g:
            with open(dest, "b+w") as f:
                f.write(g.read())
                logging.info("- Downloaded %s to %s.", url, dest)
    except URLError:
        logging.error("URLError while downloading %s.", url)
        raise


def download_resources(
        use_case: UseCase,
        metadata: typing.Dict,
        download_dir: Path,
        check_clean_folder: bool,
        setup_script_hash_verified: bool,
):
    """
    Download the resources associated with a use case

    @param use_case:                    The use case
    @param metadata:                    The metadata
    @param download_dir:                The parent directory
    @param check_clean_folder:          Whether to clean the folder
    @param setup_script_hash_verified:  Whether the hash is already verified
    """
    initialize_use_case_resources_directory(
        use_case,
        metadata,
        download_dir,
        check_clean_folder,
        setup_script_hash_verified
    )

    reg_expr_str = r"{url_prefix:(.*\d)}"
    reg_expr_pattern = re.compile(reg_expr_str)
    for res in use_case.resources:
        res_name = res.name
        url_prefix_idx = int(reg_expr_pattern.search(res.url).group(1))
        res_url = use_case.url_prefix[url_prefix_idx] + re.sub(
            reg_expr_str, "", res.url
        )

        sub_folder = ""
        if res.sub_folder is not None:
            try:
                #  Does the usecase_name/sub_folder download dir exist?
                (download_dir / use_case.name / res.sub_folder).mkdir()
            except OSError as err:
                if err.errno != errno.EEXIST:
                    logging.error(
                        "Error creating %s/%s directory.",
                        use_case.name,
                        res.sub_folder
                    )
                    raise
            sub_folder = res.sub_folder

        res_dst = download_dir / use_case.name / sub_folder / res_name

        if res_dst.is_file():
            logging.info("File %s exists, skipping download.", res_dst)
        else:
            download_file(res_url, res_dst)


def run_vela(
        config: NpuConfig,
        env_activate_cmd: str,
        model: Path,
        config_file: Path,
        output_dir: Path
) -> bool:
    """
    Run vela on the specified model
    @param config:              The NPU configuration
    @param env_activate_cmd:    The Python venv activation command
    @param model:               The model
    @param config_file:         The vela config file
    @param output_dir:          The output directory
    @return:                    True if the optimisation was skipped, false otherwise
    """
    # model name after compiling with vela is an initial model name + _vela suffix
    vela_optimised_model_path = model.parent / (model.stem + "_vela.tflite")

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
            + f"--output-dir={output_dir} "
            + f"{vela_command_arena_cache_size}"
    )

    # We want the name to include the configuration suffix. For example: vela_H128,
    # vela_Y512 etc.
    new_suffix = "_vela_" + config.config_id + ".tflite"
    new_vela_optimised_model_path = model.parent / (model.stem + new_suffix)

    skip_optimisation = new_vela_optimised_model_path.is_file()

    if skip_optimisation:
        logging.info(
            "File %s exists, skipping optimisation.",
            new_vela_optimised_model_path
        )
    else:
        call_command(vela_command)

        # Rename default vela model.
        vela_optimised_model_path.rename(new_vela_optimised_model_path)
        logging.info(
            "Renaming %s to %s.",
            vela_optimised_model_path,
            new_vela_optimised_model_path
        )

    return skip_optimisation


def run_vela_on_all_models(
        download_dir: Path,
        env_activate_cmd: str,
        arena_cache_size: int,
        npu_config_names: typing.List[str]
):
    """
    Run vela on downloaded models for the specified NPU configurations

    @param download_dir:        Path to the downloaded resources directory
    @param env_activate_cmd:    Command used to activate Python venv
    @param npu_config_names:    Names of NPU configurations for which to run Vela
    @param arena_cache_size:    The arena cache size
    """
    config_file = current_file_dir / "scripts" / "vela" / "default_vela.ini"
    models = [
        Path(dirpath) / f
        for dirpath, dirnames, files in os.walk(download_dir)
        for f in fnmatch.filter(files, "*.tflite")
        if "vela" not in f
    ]

    # Get npu config tuple for each config name in a list:
    npu_configs = [
        get_default_npu_config_from_name(name, arena_cache_size)
        for name in npu_config_names
    ]

    logging.info("All models will be optimised for these configs:")
    for config in npu_configs:
        logging.info(config)

    optimisation_skipped = False

    for model in models:
        for config in npu_configs:
            optimisation_skipped = run_vela(
                config,
                env_activate_cmd,
                model,
                config_file,
                output_dir=model.parent
            ) or optimisation_skipped

    # If any optimisation was skipped, show how to regenerate:
    if optimisation_skipped:
        logging.warning("One or more optimisations were skipped.")
        logging.warning(
            "To optimise all the models, please remove the directory %s.",
            download_dir
        )


def initialize_resources_directory(
        download_dir: Path,
        check_clean_folder: bool,
        metadata_file_path: Path,
        setup_script_hash: str
) -> typing.Tuple[typing.Dict, bool]:
    """
    Sets up the resources_downloaded directory and checks to see if this script
    has been modified since the last time resources were downloaded

    @param download_dir:        Path to the resources_downloaded directory
    @param check_clean_folder:  Determines whether to clean the downloads directory
    @param metadata_file_path:  Path to the metadata file
    @param setup_script_hash:   The md5 hash of this script
    @return:                    The metadata and a boolean to indicate whether this
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


def set_up_python_venv(
        download_dir: Path,
        additional_requirements_file: Path = ""
):
    """
    Set up the Python environment with which to set up the resources

    @param download_dir:                    Path to the resources_downloaded directory
    @param additional_requirements_file:    Optional additional requirements file
    @return:                                Path to the venv Python binary + activate command
    """
    env_dirname = "env"
    env_path = download_dir / env_dirname

    venv_builder = venv.EnvBuilder(with_pip=True, upgrade_deps=True)
    venv_context = venv_builder.ensure_directories(env_dir=env_path)

    env_python = Path(venv_context.env_exe)

    if not env_python.is_file():
        # Create the virtual environment using current interpreter's venv
        # (not necessarily the system's Python3)
        venv_builder.create(env_dir=env_path)

    if sys.platform == "win32":
        env_activate = Path(f"{venv_context.bin_path}/activate.bat")
        env_activate_cmd = str(env_activate)
    else:
        env_activate = Path(f"{venv_context.bin_path}/activate")
        env_activate_cmd = f". {env_activate}"

    if not env_activate.is_file():
        venv_builder.install_scripts(venv_context, venv_context.bin_path)

    # Install additional requirements first, if a valid file has been provided
    if additional_requirements_file and os.path.isfile(additional_requirements_file):
        command = f"{env_python} -m pip install -r {additional_requirements_file}"
        call_command(command)

    # Make sure to have all the main requirements
    if INSTALL_VELA_FROM_SOURCE:
        requirements = [
            f"git+https://review.mlplatform.org/ml/ethos-u/ethos-u-vela.git@{VELA_VERSION}"
        ]
    else:
        requirements = [f"ethos-u-vela=={VELA_VERSION}"]

    command = f"{env_python} -m pip freeze"
    packages = call_command(command)
    for req in requirements:
        if req not in packages:
            command = f"{env_python} -m pip install {req}"
            call_command(command)

    return env_path, env_activate_cmd


def update_metadata(
        metadata_dict: typing.Dict,
        setup_script_hash: str,
        use_case_resources: typing.List[UseCase],
        metadata_file_path: Path
):
    """
    Update the metadata file

    @param metadata_dict        :   The metadata dictionary to update
    @param setup_script_hash    :   The setup script hash
    @param use_case_resources   :   The use case resources metadata
    @param metadata_file_path   :   The metadata file path
    """
    metadata_dict["ethosu_vela_version"] = VELA_VERSION
    metadata_dict["set_up_script_md5sum"] = setup_script_hash.strip("\n")
    metadata_dict["resources_info"] = [dataclasses.asdict(uc) for uc in use_case_resources]

    with open(metadata_file_path, "w", encoding="utf8") as metadata_file:
        json.dump(metadata_dict, metadata_file, indent=4)


def get_default_use_cases_names() -> typing.List[str]:
    """
    Get the names of the default use cases

    :return :   List of use case names as strings
    """
    use_case_resources = load_use_case_resources(default_use_case_resources_path)
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


def set_up_resources(setup_config: SetupConfig, paths_config: PathsConfig) -> Path:
    """
    Helpers function that retrieve the output from a command.

    Parameters:
    ----------
    args (SetupArgs)        :   Arguments used to set up the project.

    Returns
    -------

    Tuple of pairs of Paths: (download_directory_path,  virtual_env_path)

    download_directory_path :   Root of the directory where the resources have been downloaded to.
    virtual_env_path        :   Path to the root of virtual environment.
    """
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
        paths_config.use_case_resources_file,
        setup_config.use_case_names
    )
    setup_script_hash = get_md5sum_for_file(Path(__file__).resolve())

    metadata_dict, setup_script_hash_verified = initialize_resources_directory(
        paths_config.downloads_dir,
        setup_config.check_clean_folder,
        metadata_file_path,
        setup_script_hash
    )

    env_path, env_activate = set_up_python_venv(
        paths_config.downloads_dir,
        paths_config.additional_requirements_file
    )

    # Download models
    logging.info("Downloading resources.")
    for use_case in use_case_resources:
        download_resources(
            use_case,
            metadata_dict,
            paths_config.downloads_dir,
            setup_config.check_clean_folder,
            setup_script_hash_verified
        )

    # Run vela on models in resources_downloaded
    # New models will have same name with '_vela' appended.
    # For example:
    # original model:    kws_micronet_m.tflite
    # after vela model:  kws_micronet_m_vela_H128.tflite
    #
    # Note: To avoid to run vela twice on the same model, it's supposed that
    # downloaded model names don't contain the 'vela' word.
    if setup_config.run_vela_on_models is True:
        # Consolidate all config names while discarding duplicates:
        run_vela_on_all_models(
            paths_config.downloads_dir,
            env_activate,
            setup_config.arena_cache_size,
            npu_config_names=list(
                set(default_npu_configs.names + list(setup_config.additional_npu_config_names))
            )
        )

    # Collect and write metadata
    logging.info("Collecting and write metadata.")
    update_metadata(
        metadata_dict,
        setup_script_hash.strip("\n"),
        use_case_resources,
        metadata_file_path
    )

    return env_path


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument(
        "--skip-vela",
        help="Do not run Vela optimizer on downloaded models.",
        action="store_true",
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
        "--requirements-file",
        help="Path to requirements.txt file to install additional packages",
        type=Path,
        default=default_requirements_path
    )
    parser.add_argument(
        "--use-case-resources-file",
        help="Path to the use case resources file",
        type=Path,
        default=default_use_case_resources_path
    )
    parser.add_argument(
        "--downloads-dir",
        help="Path to downloaded model resources",
        type=Path,
        default=default_downloads_path
    )

    parsed_args = parser.parse_args()

    if parsed_args.arena_cache_size < 0:
        raise ArgumentTypeError("Arena cache size cannot not be less than 0")

    if not Path(parsed_args.requirements_file).is_file():
        raise ArgumentTypeError(f"Invalid requirements file: {parsed_args.requirements_file}")

    logging.basicConfig(filename="log_build_default.log", level=logging.DEBUG)
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    setup = SetupConfig(
        run_vela_on_models=not parsed_args.skip_vela,
        additional_npu_config_names=parsed_args.additional_ethos_u_config_name,
        use_case_names=parsed_args.use_case,
        arena_cache_size=parsed_args.arena_cache_size,
        check_clean_folder=parsed_args.clean,
    )

    paths = PathsConfig(
        use_case_resources_file=parsed_args.use_case_resources_file,
        downloads_dir=parsed_args.downloads_dir,
        additional_requirements_file=parsed_args.requirements_file,
    )

    set_up_resources(setup, paths)
