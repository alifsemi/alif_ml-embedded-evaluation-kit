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
        "resources": [{"name": "wav2letter_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/wav2letter_int8.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_input/input_2_int8/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_output/Identity_int8/0.npy"}]
    },
    {
        "use_case_name": "img_class",
        "resources": [{"name": "mobilenet_v2_1.0_224_quantized_1_default_1.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8/mobilenet_v2_1.0_224_quantized_1_default_1.tflite"},
                      {"name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8/testing_input/input/0.npy"},
                      {"name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8/testing_output/output/0.npy"}]
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
        "use_case_name": "kws_asr",
        "resources": [{"name": "wav2letter_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/wav2letter_int8.tflite"},
                      {"sub_folder": "asr", "name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_input/input_2_int8/0.npy"},
                      {"sub_folder": "asr", "name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/speech_recognition/wav2letter/tflite_int8/testing_output/Identity_int8/0.npy"},
                      {"name": "ds_cnn_clustered_int8.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite"},
                      {"sub_folder": "kws", "name": "ifm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_input/input_2/0.npy"},
                      {"sub_folder": "kws", "name": "ofm0.npy",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/testing_output/Identity/0.npy"}]
    },
    {
        "use_case_name": "inference_runner",
        "resources": [{"name": "dnn_s_quantized.tflite",
                       "url": "https://github.com/ARM-software/ML-zoo/raw/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b/models/keyword_spotting/dnn_small/tflite_int8/dnn_s_quantized.tflite"}
                      ]
    },]


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


def set_up_resources(run_vela_on_models=False):
    """
    Helpers function that retrieve the output from a command.

    Parameters:
    ----------
    run_vela_on_models (bool):    Specifies if run vela on downloaded models.
    """
    current_file_dir = os.path.dirname(os.path.abspath(__file__))
    download_dir = os.path.abspath(os.path.join(current_file_dir, "resources_downloaded"))
    logging.basicConfig(filename='log_build_default.log', level=logging.DEBUG)
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

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
    requirements = ["ethos-u-vela==2.1.1"]
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

        for model in models:
            output_dir = os.path.dirname(model)
            command = (f". {env_activate} && vela {model} " +
                       "--accelerator-config=ethos-u55-128 " +
                       "--block-config-limit=0 " +
                       f"--config {config_file} " +
                       "--memory-mode=Shared_Sram " +
                       "--system-config=Ethos_U55_High_End_Embedded " +
                       f"--output-dir={output_dir}")
            call_command(command)
            # model name after compiling with vela is an initial model name + _vela suffix
            vela_optimised_model_path = str(model).replace(".tflite", "_vela.tflite")
            # we want it to be initial model name + _vela_H128 suffix which indicates selected MAC config.
            new_vela_optimised_model_path = vela_optimised_model_path.replace("_vela.tflite", "_vela_H128.tflite")
            # rename default vela model
            os.rename(vela_optimised_model_path, new_vela_optimised_model_path)
            logging.info(f"Renaming {vela_optimised_model_path} to {new_vela_optimised_model_path}.")


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("--skip-vela",
                        help="Do not run Vela optimizer on downloaded models.",
                        action="store_true")
    args = parser.parse_args()
    set_up_resources(not args.skip_vela)
