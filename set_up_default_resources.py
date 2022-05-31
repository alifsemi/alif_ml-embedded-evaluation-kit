#!/usr/bin/env python3
#  Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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
import errno
import fnmatch
import json
import logging
import os
import re
import shutil
import subprocess
import sys
import urllib.request
from argparse import ArgumentParser
from argparse import ArgumentTypeError
from collections import namedtuple
from urllib.error import URLError
from pathlib import Path

from scripts.py.check_update_resources_downloaded import get_md5sum_for_file


json_uc_res = [
    {
        "use_case_name": "ad",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/"
        ],
        "resources": [
            {
                "name": "ad_medium_int8.tflite",
                "url": "{url_prefix:0}ad_medium_int8.tflite",
            },
            {"name": "ifm0.npy", "url": "{url_prefix:0}testing_input/input/0.npy"},
            {"name": "ofm0.npy", "url": "{url_prefix:0}testing_output/Identity/0.npy"},
        ],
    },
    {
        "use_case_name": "asr",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/"
        ],
        "resources": [
            {
                "name": "wav2letter_pruned_int8.tflite",
                "url": "{url_prefix:0}wav2letter_pruned_int8.tflite",
            },
            {
                "name": "ifm0.npy",
                "url": "{url_prefix:0}testing_input/input_2_int8/0.npy",
            },
            {
                "name": "ofm0.npy",
                "url": "{url_prefix:0}testing_output/Identity_int8/0.npy",
            },
        ],
    },
    {
        "use_case_name": "img_class",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/"
        ],
        "resources": [
            {
                "name": "mobilenet_v2_1.0_224_INT8.tflite",
                "url": "{url_prefix:0}mobilenet_v2_1.0_224_INT8.tflite",
            },
            {
                "name": "ifm0.npy",
                "url": "{url_prefix:0}testing_input/tfl.quantize/0.npy",
            },
            {
                "name": "ofm0.npy",
                "url": "{url_prefix:0}testing_output/MobilenetV2/Predictions/Reshape_11/0.npy",
            },
        ],
    },
    {
        "use_case_name": "object_detection",
        "url_prefix": [
            "https://github.com/emza-vs/ModelZoo/blob/v1.0/object_detection/"
        ],
        "resources": [
            {
                "name": "yolo-fastest_192_face_v4.tflite",
                "url": "{url_prefix:0}yolo-fastest_192_face_v4.tflite?raw=true",
            }
        ],
    },
    {
        "use_case_name": "kws",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/"
        ],
        "resources": [
            {"name": "ifm0.npy", "url": "{url_prefix:0}testing_input/input/0.npy"},
            {"name": "ofm0.npy", "url": "{url_prefix:0}testing_output/Identity/0.npy"},
            {
                "name": "kws_micronet_m.tflite",
                "url": "{url_prefix:0}kws_micronet_m.tflite",
            },
        ],
    },
    {
        "use_case_name": "vww",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/"
        ],
        "resources": [
            {
                "name": "vww4_128_128_INT8.tflite",
                "url": "{url_prefix:0}vww4_128_128_INT8.tflite",
            },
            {"name": "ifm0.npy", "url": "{url_prefix:0}testing_input/input/0.npy"},
            {"name": "ofm0.npy", "url": "{url_prefix:0}testing_output/Identity/0.npy"},
        ],
    },
    {
        "use_case_name": "kws_asr",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/",
            "https://github.com/ARM-software/ML-zoo/raw/9f506fe52b39df545f0e6c5ff9223f671bc5ae00/models/keyword_spotting/micronet_medium/tflite_int8/",
        ],
        "resources": [
            {
                "name": "wav2letter_pruned_int8.tflite",
                "url": "{url_prefix:0}wav2letter_pruned_int8.tflite",
            },
            {
                "sub_folder": "asr",
                "name": "ifm0.npy",
                "url": "{url_prefix:0}testing_input/input_2_int8/0.npy",
            },
            {
                "sub_folder": "asr",
                "name": "ofm0.npy",
                "url": "{url_prefix:0}testing_output/Identity_int8/0.npy",
            },
            {
                "sub_folder": "kws",
                "name": "ifm0.npy",
                "url": "{url_prefix:1}testing_input/input/0.npy",
            },
            {
                "sub_folder": "kws",
                "name": "ofm0.npy",
                "url": "{url_prefix:1}testing_output/Identity/0.npy",
            },
            {
                "name": "kws_micronet_m.tflite",
                "url": "{url_prefix:1}kws_micronet_m.tflite",
            },
        ],
    },
    {
        "use_case_name": "noise_reduction",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/"
        ],
        "resources": [
            {"name": "rnnoise_INT8.tflite", "url": "{url_prefix:0}rnnoise_INT8.tflite"},
            {
                "name": "ifm0.npy",
                "url": "{url_prefix:0}testing_input/main_input_int8/0.npy",
            },
            {
                "name": "ifm1.npy",
                "url": "{url_prefix:0}testing_input/vad_gru_prev_state_int8/0.npy",
            },
            {
                "name": "ifm2.npy",
                "url": "{url_prefix:0}testing_input/noise_gru_prev_state_int8/0.npy",
            },
            {
                "name": "ifm3.npy",
                "url": "{url_prefix:0}testing_input/denoise_gru_prev_state_int8/0.npy",
            },
            {
                "name": "ofm0.npy",
                "url": "{url_prefix:0}testing_output/Identity_int8/0.npy",
            },
            {
                "name": "ofm1.npy",
                "url": "{url_prefix:0}testing_output/Identity_1_int8/0.npy",
            },
            {
                "name": "ofm2.npy",
                "url": "{url_prefix:0}testing_output/Identity_2_int8/0.npy",
            },
            {
                "name": "ofm3.npy",
                "url": "{url_prefix:0}testing_output/Identity_3_int8/0.npy",
            },
            {
                "name": "ofm4.npy",
                "url": "{url_prefix:0}testing_output/Identity_4_int8/0.npy",
            },
        ],
    },
    {
        "use_case_name": "inference_runner",
        "url_prefix": [
            "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/dnn_small/tflite_int8/"
        ],
        "resources": [
            {
                "name": "dnn_s_quantized.tflite",
                "url": "{url_prefix:0}dnn_s_quantized.tflite",
            }
        ],
    },
]

# Valid NPU configurations:
valid_npu_config_names = [
    "ethos-u55-32",
    "ethos-u55-64",
    "ethos-u55-128",
    "ethos-u55-256",
    "ethos-u65-256",
    "ethos-u65-512",
]

# Default NPU configurations (these are always run when the models are optimised)
default_npu_config_names = [valid_npu_config_names[2], valid_npu_config_names[4]]

# NPU config named tuple
NPUConfig = namedtuple(
    "NPUConfig",
    [
        "config_name",
        "memory_mode",
        "system_config",
        "ethos_u_npu_id",
        "ethos_u_config_id",
        "arena_cache_size",
    ],
)

# The internal SRAM size for Corstone-300 implementation on MPS3 specified by AN552
# The internal SRAM size for Corstone-310 implementation on MPS3 specified by AN555
# is 4MB, but we are content with the 2MB specified below.
mps3_max_sram_sz = 2 * 1024 * 1024  # 2 MiB (2 banks of 1 MiB each)


def call_command(command: str, verbose: bool = True) -> str:
    """
    Helpers function that call subprocess and return the output.

    Parameters:
    ----------
    command (string):  Specifies the command to run.
    """
    if verbose:
        logging.info(command)
    proc = subprocess.run(
        command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True
    )
    log = proc.stdout.decode("utf-8")
    if proc.returncode == 0 and verbose:
        logging.info(log)
    else:
        logging.error(log)
    proc.check_returncode()
    return log


def get_default_npu_config_from_name(
    config_name: str, arena_cache_size: int = 0
) -> NPUConfig:
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
    NPUConfig: An NPU config named tuple populated with defaults for the given
               config name
    """
    if config_name not in valid_npu_config_names:
        raise ValueError(
            f"""
            Invalid Ethos-U NPU configuration.
            Select one from {valid_npu_config_names}.
            """
        )

    strings_ids = ["ethos-u55-", "ethos-u65-"]
    processor_ids = ["U55", "U65"]
    prefix_ids = ["H", "Y"]
    memory_modes = ["Shared_Sram", "Dedicated_Sram"]
    system_configs = ["Ethos_U55_High_End_Embedded", "Ethos_U65_High_End"]
    memory_modes_arena = {
        # For shared SRAM memory mode, we use the MPS3 SRAM size by default.
        "Shared_Sram": mps3_max_sram_sz if arena_cache_size <= 0 else arena_cache_size,
        # For dedicated SRAM memory mode, we do not override the arena size. This is expected to
        # be defined in the Vela configuration file instead.
        "Dedicated_Sram": None if arena_cache_size <= 0 else arena_cache_size,
    }

    for i in range(len(strings_ids)):
        if config_name.startswith(strings_ids[i]):
            npu_config_id = config_name.replace(strings_ids[i], prefix_ids[i])
            return NPUConfig(
                config_name=config_name,
                memory_mode=memory_modes[i],
                system_config=system_configs[i],
                ethos_u_npu_id=processor_ids[i],
                ethos_u_config_id=npu_config_id,
                arena_cache_size=memory_modes_arena[memory_modes[i]],
            )

    return None


def remove_tree_dir(dir_path):
    try:
        # Remove the full directory.
        shutil.rmtree(dir_path)
        # Re-create an empty one.
        os.mkdir(dir_path)
    except Exception as e:
        logging.error(f"Failed to delete {dir_path}.")


def set_up_resources(
    run_vela_on_models: bool = False,
    additional_npu_config_names: tuple = (),
    arena_cache_size: int = 0,
    check_clean_folder: bool = False,
    additional_requirements_file: str = "") -> (Path, Path):
    """
    Helpers function that retrieve the output from a command.

    Parameters:
    ----------
    run_vela_on_models (bool):  Specifies if run vela on downloaded models.
    additional_npu_config_names(list):  list of strings of Ethos-U NPU configs.
    arena_cache_size (int): Specifies arena cache size in bytes. If a value
                            greater than 0 is provided, this will be taken
                            as the cache size. If 0, the default values, as per
                            the NPU config requirements, are used.
    check_clean_folder (bool): Indicates whether the resources folder needs to
                               be checked for updates and cleaned.
    additional_requirements_file (str): Path to a requirements.txt file if
                                        additional packages need to be
                                        installed.

    Returns
    -------

    Tuple of pair of Paths: (download_directory_path,  virtual_env_path)

    download_directory_path: Root of the directory where the resources have been downloaded to.
    virtual_env_path: Path to the root of virtual environment.
    """
    # Paths.
    current_file_dir = Path(__file__).parent.resolve()
    download_dir = current_file_dir / "resources_downloaded"
    metadata_file_path = download_dir / "resources_downloaded_metadata.json"

    metadata_dict = dict()
    vela_version = "3.4.0"
    py3_major_version_minimum = 3  # Python >= 3.7 is required
    py3_minor_version_minimum = 7

    # Is Python minimum requirement matched?
    py3_version = sys.version_info
    if (
        py3_version.major < py3_major_version_minimum
        or py3_version.minor < py3_minor_version_minimum
    ):
        raise Exception(
            "ERROR: Python3.7+ is required, please see the documentation on how to update it."
        )

    setup_script_hash_verified = False
    setup_script_hash = get_md5sum_for_file(Path(__file__).resolve())

    try:
        #   1.1 Does the download dir exist?
        download_dir.mkdir()
    except OSError as e:
        if e.errno == errno.EEXIST:
            logging.info("'resources_downloaded' directory exists.")
            # Check and clean?
            if check_clean_folder and metadata_file_path.is_file():
                with open(metadata_file_path) as metadata_file:
                    metadata_dict = json.load(metadata_file)
                    vela_in_metadata = metadata_dict["ethosu_vela_version"]
                    if vela_in_metadata != vela_version:
                        # Check if all the resources needs to be removed and regenerated.
                        # This can happen when the Vela version has changed.
                        logging.info(
                            f"Vela version in metadata is {vela_in_metadata}, current {vela_version}. Removing the resources and re-download them."
                        )
                        remove_tree_dir(download_dir)
                        metadata_dict = dict()
                    else:
                        # Check if the set_up_default_resorces.py has changed from last setup
                        setup_script_hash_verified = (
                            metadata_dict.get("set_up_script_md5sum")
                            == setup_script_hash
                        )
        else:
            raise

    # 1.2 Does the virtual environment exist?
    env_dirname = "env"
    env_path = download_dir / env_dirname
    env_python = str(env_path / "bin" / "python3")
    env_activate = str(env_path / "bin" / "activate")

    if not env_path.is_dir():
        os.chdir(download_dir)
        # Create the virtual environment.
        command = f"python3 -m venv {env_dirname}"
        call_command(command)
        commands = ["pip install --upgrade pip", "pip install --upgrade setuptools"]
        for c in commands:
            command = f"{env_python} -m {c}"
            call_command(command)
        os.chdir(current_file_dir)

    # 1.3 Make sure to have all the requirements
    requirements = [f"ethos-u-vela=={vela_version}"]
    command = f"{env_python} -m pip freeze"
    packages = call_command(command)
    for req in requirements:
        if req not in packages:
            command = f"{env_python} -m pip install {req}"
            call_command(command)

    # 1.4 Install additional requirements, if a valid file has been provided
    if additional_requirements_file and os.path.isfile(additional_requirements_file):
        command = f"{env_python} -m pip install -r {additional_requirements_file}"
        call_command(command)

    # 2. Download models
    logging.info("Downloading resources.")
    for uc in json_uc_res:
        use_case_name = uc["use_case_name"]
        res_url_prefix = uc["url_prefix"]
        try:
            #  Does the usecase_name download dir exist?
            (download_dir / use_case_name).mkdir()
        except OSError as e:
            if e.errno == errno.EEXIST:
                # The usecase_name download dir exist.
                if check_clean_folder and not setup_script_hash_verified:
                    for idx, metadata_uc_url_prefix in enumerate(
                        [
                            f
                            for f in metadata_dict["resources_info"]
                            if f["use_case_name"] == use_case_name
                        ][0]["url_prefix"]
                    ):
                        if metadata_uc_url_prefix != res_url_prefix[idx]:
                            logging.info(f"Removing {use_case_name} resources.")
                            remove_tree_dir(download_dir / use_case_name)
                            break
            elif e.errno != errno.EEXIST:
                logging.error(f"Error creating {use_case_name} directory.")
                raise

        reg_expr_str = r"{url_prefix:(.*\d)}"
        reg_expr_pattern = re.compile(reg_expr_str)
        for res in uc["resources"]:
            res_name = res["name"]
            url_prefix_idx = int(reg_expr_pattern.search(res["url"]).group(1))
            res_url = res_url_prefix[url_prefix_idx] + re.sub(
                reg_expr_str, "", res["url"]
            )

            sub_folder = ""
            if "sub_folder" in res:
                try:
                    #  Does the usecase_name/sub_folder download dir exist?
                    (download_dir / use_case_name / res["sub_folder"]).mkdir()
                except OSError as e:
                    if e.errno != errno.EEXIST:
                        logging.error(
                            f"Error creating {use_case_name} / {res['sub_folder']} directory."
                        )
                        raise
                sub_folder = res["sub_folder"]

            res_dst = download_dir / use_case_name / sub_folder / res_name

            if res_dst.is_file():
                logging.info(f"File {res_dst} exists, skipping download.")
            else:
                try:
                    g = urllib.request.urlopen(res_url)
                    with open(res_dst, "b+w") as f:
                        f.write(g.read())
                        logging.info(f"- Downloaded {res_url} to {res_dst}.")
                except URLError:
                    logging.error(f"URLError while downloading {res_url}.")
                    raise

    # 3. Run vela on models in resources_downloaded
    # New models will have same name with '_vela' appended.
    # For example:
    # original model:    kws_micronet_m.tflite
    # after vela model:  kws_micronet_m_vela_H128.tflite
    #
    # Note: To avoid to run vela twice on the same model, it's supposed that
    # downloaded model names don't contain the 'vela' word.
    if run_vela_on_models is True:
        config_file = current_file_dir / "scripts" / "vela" / "default_vela.ini"
        models = [
            Path(dirpath) / f
            for dirpath, dirnames, files in os.walk(download_dir)
            for f in fnmatch.filter(files, "*.tflite")
            if "vela" not in f
        ]

        # Consolidate all config names while discarding duplicates:
        config_names = list(set(default_npu_config_names + additional_npu_config_names))

        # Get npu config tuple for each config name in a list:
        npu_configs = [
            get_default_npu_config_from_name(name, arena_cache_size)
            for name in config_names
        ]

        logging.info(f"All models will be optimised for these configs:")
        for config in npu_configs:
            logging.info(config)

        optimisation_skipped = False

        for model in models:
            output_dir = model.parent
            # model name after compiling with vela is an initial model name + _vela suffix
            vela_optimised_model_path = model.parent / (model.stem + "_vela.tflite")

            for config in npu_configs:
                vela_command_arena_cache_size = ""

                if config.arena_cache_size:
                    vela_command_arena_cache_size = (
                        f"--arena-cache-size={config.arena_cache_size}"
                    )

                vela_command = (
                    f". {env_activate} && vela {model} "
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
                new_suffix = "_vela_" + config.ethos_u_config_id + ".tflite"
                new_vela_optimised_model_path = model.parent / (model.stem + new_suffix)

                if new_vela_optimised_model_path.is_file():
                    logging.info(
                        f"File {new_vela_optimised_model_path} exists, skipping optimisation."
                    )
                    optimisation_skipped = True
                    continue

                call_command(vela_command)

                # Rename default vela model.
                vela_optimised_model_path.rename(new_vela_optimised_model_path)
                logging.info(
                    f"Renaming {vela_optimised_model_path} to {new_vela_optimised_model_path}."
                )

        # If any optimisation was skipped, show how to regenerate:
        if optimisation_skipped:
            logging.warning("One or more optimisations were skipped.")
            logging.warning(
                f"To optimise all the models, please remove the directory {download_dir}."
            )

    # 4. Collect and write metadata
    logging.info("Collecting and write metadata.")
    metadata_dict["ethosu_vela_version"] = vela_version
    metadata_dict["set_up_script_md5sum"] = setup_script_hash.strip("\n")
    metadata_dict["resources_info"] = json_uc_res

    with open(metadata_file_path, "w") as metadata_file:
        json.dump(metadata_dict, metadata_file, indent=4)

    return download_dir, env_path


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
                        {valid_npu_config_names}""",
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
        type=str,
        default=Path(__file__).parent.resolve() / 'scripts' / 'py' / 'requirements.txt'
    )

    args = parser.parse_args()

    if args.arena_cache_size < 0:
        raise ArgumentTypeError("Arena cache size cannot not be less than 0")

    if not Path(args.requirements_file).is_file():
        raise ArgumentTypeError(f"Invalid requirements file: {args.requirements_file}")

    logging.basicConfig(filename="log_build_default.log", level=logging.DEBUG)
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    set_up_resources(
        not args.skip_vela,
        args.additional_ethos_u_config_name,
        args.arena_cache_size,
        args.clean,
        args.requirements_file,
    )
