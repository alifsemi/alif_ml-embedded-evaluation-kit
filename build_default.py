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

import os
import subprocess
import shutil
import multiprocessing
import logging
import sys
from argparse import ArgumentParser

from set_up_default_resources import set_up_resources, \
                                     get_default_npu_config_from_name, \
                                     valid_npu_config_names, \
                                     default_npu_config_names


def run(toolchain: str,
        download_resources: bool,
        run_vela_on_models: bool,
        npu_config_name: str):
    """
    Run the helpers scripts.

    Parameters:
    ----------
    toolchain (str)          :    Specifies if 'gnu' or 'arm' toolchain needs to be used.
    download_resources (bool):    Specifies if 'Download resources' step is performed.
    run_vela_on_models (bool):    Only if `download_resources` is True, specifies if run vela on downloaded models.
    npu_config_name(str)     :    Ethos-U NPU configuration name. See "valid_npu_config_names"
    """

    current_file_dir = os.path.dirname(os.path.abspath(__file__))

    # 1. Make sure the toolchain is supported, and set the right one here
    supported_toolchain_ids = ["gnu", "arm"]
    assert (toolchain in supported_toolchain_ids,
        f"Toolchain must be from {supported_toolchain_ids}")
    if toolchain == "arm":
        toolchain_file_name = "bare-metal-armclang.cmake"
    elif toolchain == "gnu":
        toolchain_file_name = "bare-metal-gcc.cmake"

    # 2. Download models if specified
    if download_resources is True:
        logging.info("Downloading resources.")
        set_up_resources(run_vela_on_models=run_vela_on_models,
                         additional_npu_config_names=[npu_config_name])

    # 3. Build default configuration
    logging.info("Building default configuration.")
    target_platform = "mps3"
    target_subsystem = "sse-300"
    ethos_u_cfg = get_default_npu_config_from_name(npu_config_name)
    build_dir = os.path.join(current_file_dir,
        f"cmake-build-{target_platform}-{target_subsystem}-{npu_config_name}-{toolchain}")
    try:
        os.mkdir(build_dir)
    except FileExistsError:
        # Directory already exists, clean it
        for filename in os.listdir(build_dir):
            filepath = os.path.join(build_dir, filename)
            try:
                if os.path.isfile(filepath) or os.path.islink(filepath):
                    os.unlink(filepath)
                elif os.path.isdir(filepath):
                    shutil.rmtree(filepath)
            except Exception as e:
                logging.error('Failed to delete %s. Reason: %s' % (filepath, e))

    os.chdir(build_dir)
    cmake_toolchain_file = os.path.join(current_file_dir, "scripts", "cmake",
                                        "toolchains", toolchain_file_name)
    cmake_command = (f"cmake .. -DTARGET_PLATFORM={target_platform}" +
                     f" -DTARGET_SUBSYSTEM={target_subsystem}" +
                     f" -DCMAKE_TOOLCHAIN_FILE={cmake_toolchain_file}" +
                     f" -DETHOS_U_NPU_ID={ethos_u_cfg.ethos_u_npu_id}" +
                     f" -DETHOS_U_NPU_CONFIG_ID={ethos_u_cfg.ethos_u_config_id}")

    logging.info(cmake_command)
    state = subprocess.run(cmake_command, shell=True, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT)
    logging.info(state.stdout.decode('utf-8'))

    make_command = f"make -j{multiprocessing.cpu_count()}"
    logging.info(make_command)
    state = subprocess.run(make_command, shell=True, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT)
    logging.info(state.stdout.decode('utf-8'))


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("--toolchain", default="gnu",
                        help="""
                        Specify the toolchain to use (Arm or GNU).
                        Options are [gnu, arm]; default is gnu.
                        """)
    parser.add_argument("--skip-download",
                        help="Do not download resources: models and test vectors",
                        action="store_true")
    parser.add_argument("--skip-vela",
                        help="Do not run Vela optimizer on downloaded models.",
                        action="store_true")
    parser.add_argument("--npu-config-name",
                        help=f"""Arm Ethos-U configuration to build for. Choose from:
                        {valid_npu_config_names}""",
                        default=default_npu_config_names[0])
    args = parser.parse_args()

    logging.basicConfig(filename='log_build_default.log', level=logging.DEBUG,
                        filemode='w')
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    run(args.toolchain.lower(),
        not args.skip_download,
        not args.skip_vela,
        args.npu_config_name)
