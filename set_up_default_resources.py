#!/usr/bin/env python3

#  Copyright (c) 2021 Arm Limited. All rights reserved.
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

import os, errno
import urllib.request
import subprocess
import fnmatch
import logging
import sys

from argparse import ArgumentParser
from urllib.error import URLError
from collections import namedtuple


json_uc_res = [{
    "use_case_name": "ad",
    "resources": [{"name": "ad_medium_int8.tflite",
                   "url": "https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/ad_medium_int8.tflite"},
                  {"name": "ifm0.npy",
                   "url": "https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/testing_input/input/0.npy"},
                  {"name": "ofm0.npy",
                   "url": "https://github.com/ARM-software/ML-zoo/raw/7c32b097f7d94aae2cd0b98a8ed5a3ba81e66b18/models/anomaly_detection/micronet_medium/tflite_int8/testing_output/Identity/0.npy"}]
},
    {
        "use_case_name": "asr",
        "resources": [{"name": "wav2letter_pruned_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/wav2letter_pruned_int8.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_input/input_2_int8/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_output/Identity_int8/0.npy"}]
    },
    {
        "use_case_name": "img_class",
        "resources": [{"name": "mobilenet_v2_1.0_224_INT8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/mobilenet_v2_1.0_224_INT8.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/testing_input/tfl.quantize/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_int8/testing_output/MobilenetV2/Predictions/Reshape_11/0.npy"}]
    },
    {
        "use_case_name": "kws",
        "resources": [{"name": "ds_cnn_clustered_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_input/input_2/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_output/Identity/0.npy"}]
    },
     {
        "use_case_name": "vww",
        "resources": [{"name": "vww4_128_128_INT8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/vww4_128_128_INT8.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/testing_input/input/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/7dd3b16bb84007daf88be8648983c07f3eb21140/models/visual_wake_words/micronet_vww4/tflite_int8/testing_output/Identity/0.npy"}]
    },
    {
        "use_case_name": "kws_asr",
        "resources": [{"name": "wav2letter_pruned_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/wav2letter_pruned_int8.tflite"},
                      {"sub_folder": "asr", "name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_input/input_2_int8/0.npy"},
                      {"sub_folder": "asr", "name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8/testing_output/Identity_int8/0.npy"},
                      {"name": "ds_cnn_clustered_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite"},
                      {"sub_folder": "kws", "name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_input/input_2/0.npy"},
                      {"sub_folder": "kws", "name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_output/Identity/0.npy"}]
    },
    {
        "use_case_name": "noise_reduction",
        "resources": [{"name": "rnnoise_INT8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/rnnoise_INT8.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/main_input_int8/0.npy"},
                      {"name": "ifm1.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/vad_gru_prev_state_int8/0.npy"},
                      {"name": "ifm2.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/noise_gru_prev_state_int8/0.npy"},
                      {"name": "ifm3.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_input/denoise_gru_prev_state_int8/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_int8/0.npy"},
                      {"name": "ofm1.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_1_int8/0.npy"},
                      {"name": "ofm2.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_2_int8/0.npy"},
                      {"name": "ofm3.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_3_int8/0.npy"},
                      {"name": "ofm4.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/a061600058097a2785d6f1f7785e5a2d2a142955/models/noise_suppression/RNNoise/tflite_int8/testing_output/Identity_4_int8/0.npy"},
                     ]
    },
    {
        "use_case_name": "inference_runner",
        "resources": [{"name": "dnn_s_quantized.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/dnn_small/tflite_int8/dnn_s_quantized.tflite"}
                      ]
    },]

# Valid NPU configurations:
valid_npu_config_names = [
        'ethos-u55-32', 'ethos-u55-64',
        'ethos-u55-128', 'ethos-u55-256',
        'ethos-u65-256','ethos-u65-512']

# Default NPU configurations (these are always run when the models are optimised)
default_npu_config_names = [valid_npu_config_names[2], valid_npu_config_names[4]]

# NPU config named tuple
NPUConfig = namedtuple('NPUConfig',['config_name',
                                    'memory_mode',
                                    'system_config',
                                    'ethos_u_npu_id',
                                    'ethos_u_config_id'])

# The internal SRAM size for Corstone-300 implementation on MPS3 specified by AN552
mps3_max_sram_sz = 2 * 1024 * 1024 # 2 MiB (2 banks of 1 MiB each)


def call_command(command: str) -> str:
    """
    Helpers function that call subprocess and return the output.

    Parameters:
    ----------
    command (string):  Specifies the command to run.
    """
    logging.info(command)
    proc = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    log = proc.stdout.decode("utf-8")
    if proc.returncode == 0:
        logging.info(log)
    else:
        logging.error(log)
    proc.check_returncode()
    return log


def get_default_npu_config_from_name(config_name: str) -> NPUConfig:
    """
    Gets the file suffix for the tflite file from the
    `accelerator_config` string.

    Parameters:
    ----------
    config_name (str):   Ethos-U NPU configuration from valid_npu_config_names

    Returns:
    -------
    NPUConfig: An NPU config named tuple populated with defaults for the given
               config name
    """
    if config_name not in valid_npu_config_names:
        raise ValueError(f"""
            Invalid Ethos-U NPU configuration.
            Select one from {valid_npu_config_names}.
            """)

    strings_ids = ["ethos-u55-", "ethos-u65-"]
    processor_ids = ["U55", "U65"]
    prefix_ids = ["H", "Y"]
    memory_modes = ["Shared_Sram", "Dedicated_Sram"]
    system_configs = ["Ethos_U55_High_End_Embedded", "Ethos_U65_High_End"]

    for i in range(len(strings_ids)):
        if config_name.startswith(strings_ids[i]):
            npu_config_id = config_name.replace(strings_ids[i], prefix_ids[i])
            return  NPUConfig(config_name=config_name,
                              memory_mode=memory_modes[i],
                              system_config=system_configs[i],
                              ethos_u_npu_id=processor_ids[i],
                              ethos_u_config_id=npu_config_id)

    return None


def set_up_resources(run_vela_on_models=False,
                     additional_npu_config_names=[],
                     arena_cache_size=mps3_max_sram_sz):
    """
    Helpers function that retrieve the output from a command.

    Parameters:
    ----------
    run_vela_on_models (bool):    Specifies if run vela on downloaded models.
    additional_npu_config_names(list): list of strings of Ethos-U NPU configs.
    arena_cache_size(int):    Specifies arena cache size in bytes.
    """
    current_file_dir = os.path.dirname(os.path.abspath(__file__))
    download_dir = os.path.abspath(os.path.join(current_file_dir, "resources_downloaded"))

    try:
        #   1.1 Does the download dir exist?
        os.mkdir(download_dir)
    except OSError as e:
        if e.errno == errno.EEXIST:
            logging.info("'resources_downloaded' directory exists.")
        else:
            raise

    # 1.2 Does the virtual environment exist?
    env_python = str(os.path.abspath(os.path.join(download_dir, "env", "bin", "python3")))
    env_activate = str(os.path.abspath(os.path.join(download_dir, "env", "bin", "activate")))
    if not os.path.isdir(os.path.join(download_dir, "env")):
        os.chdir(download_dir)
        # Create the virtual environment
        command = "python3 -m venv env"
        call_command(command)
        commands = ["pip install --upgrade pip", "pip install --upgrade setuptools"]
        for c in commands:
            command = f"{env_python} -m {c}"
            call_command(command)
        os.chdir(current_file_dir)
    # 1.3 Make sure to have all the requirement
    requirements = ["ethos-u-vela==3.1.0"]
    command = f"{env_python} -m pip freeze"
    packages = call_command(command)
    for req in requirements:
        if req not in packages:
            command = f"{env_python} -m pip install {req}"
            call_command(command)

    # 2. Download models
    for uc in json_uc_res:
        try:
            #  Does the usecase_name download dir exist?
            os.mkdir(os.path.join(download_dir, uc["use_case_name"]))
        except OSError as e:
            if e.errno != errno.EEXIST:
                logging.error(f"Error creating {uc['use_case_name']} directory.")
                raise

        for res in uc["resources"]:
            res_name = res["name"]
            res_url = res["url"]
            if "sub_folder" in res:
                try:
                    #  Does the usecase_name/sub_folder download dir exist?
                    os.mkdir(os.path.join(download_dir, uc["use_case_name"], res["sub_folder"]))
                except OSError as e:
                    if e.errno != errno.EEXIST:
                        logging.error(f"Error creating {uc['use_case_name']} / {res['sub_folder']} directory.")
                        raise
                res_dst = os.path.join(download_dir,
                                       uc["use_case_name"],
                                       res["sub_folder"],
                                       res_name)
            else:
                res_dst = os.path.join(download_dir,
                                       uc["use_case_name"],
                                       res_name)

            if os.path.isfile(res_dst):
                logging.info(f"File {res_dst} exists, skipping download.")
            else:
                try:
                    g = urllib.request.urlopen(res_url)
                    with open(res_dst, 'b+w') as f:
                        f.write(g.read())
                        logging.info(f"- Downloaded {res_url} to {res_dst}.")
                except URLError:
                    logging.error(f"URLError while downloading {res_url}.")
                    raise

    # 3. Run vela on models in resources_downloaded
    # New models will have same name with '_vela' appended.
    # For example:
    #   original model:  ds_cnn_clustered_int8.tflite
    # after vela model:  ds_cnn_clustered_int8_vela_H128.tflite
    #
    # Note: To avoid to run vela twice on the same model, it's supposed that
    # downloaded model names don't contain the 'vela' word.
    if run_vela_on_models is True:
        config_file = os.path.join(current_file_dir, "scripts", "vela", "default_vela.ini")
        models = [os.path.join(dirpath, f)
                  for dirpath, dirnames, files in os.walk(download_dir)
                  for f in fnmatch.filter(files, '*.tflite') if "vela" not in f]

        # Consolidate all config names while discarding duplicates:
        config_names = list(set(default_npu_config_names + additional_npu_config_names))

        # Get npu config tuple for each config name in a list:
        npu_configs = [get_default_npu_config_from_name(name) for name in config_names]

        logging.info(f'All models will be optimised for these configs:')
        for config in npu_configs:
            logging.info(config)

        for model in models:
            output_dir = os.path.dirname(model)
            # model name after compiling with vela is an initial model name + _vela suffix
            vela_optimised_model_path = str(model).replace(".tflite", "_vela.tflite")

            for config in npu_configs:
                vela_command = (f". {env_activate} && vela {model} " +
                       f"--accelerator-config={config.config_name} " +
                       "--optimise Performance " +
                       f"--config {config_file} " +
                       f"--memory-mode={config.memory_mode} " +
                       f"--system-config={config.system_config} " +
                       f"--output-dir={output_dir} " +
                       f"--arena-cache-size={arena_cache_size} ")

                # we want the name to include the configuration suffix. For example: vela_H128,
                # vela_Y512 etc.
                new_suffix = "_vela_" + config.ethos_u_config_id + '.tflite'
                new_vela_optimised_model_path = (
                    vela_optimised_model_path.replace("_vela.tflite", new_suffix))

                if os.path.isfile(new_vela_optimised_model_path):
                    logging.info(f"File {new_vela_optimised_model_path} exists, skipping optimisation.")
                    continue

                call_command(vela_command)

                # rename default vela model
                os.rename(vela_optimised_model_path, new_vela_optimised_model_path)
                logging.info(f"Renaming {vela_optimised_model_path} to {new_vela_optimised_model_path}.")


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("--skip-vela",
                        help="Do not run Vela optimizer on downloaded models.",
                        action="store_true")
    parser.add_argument("--additional-ethos-u-config-name",
                        help=f"""Additional (non-default) configurations for Vela:
                        {valid_npu_config_names}""",
                        default=[], action="append")
    parser.add_argument("--arena-cache-size",
                        help="Arena cache size in bytes",
                        type=int,
                        default=mps3_max_sram_sz)
    args = parser.parse_args()

    logging.basicConfig(filename='log_build_default.log', level=logging.DEBUG)
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    set_up_resources(not args.skip_vela,
                     args.additional_ethos_u_config_name,
                     args.arena_cache_size)
