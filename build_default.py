#!/usr/bin/env python3
#  SPDX-FileCopyrightText: Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
import logging
import multiprocessing
import os
import shutil
import subprocess
import sys
import threading
from argparse import ArgumentDefaultsHelpFormatter
from argparse import ArgumentParser
from pathlib import Path

from set_up_default_resources import default_npu_config_names
from set_up_default_resources import get_default_npu_config_from_name
from set_up_default_resources import set_up_resources
from set_up_default_resources import valid_npu_config_names


class PipeLogging(threading.Thread):
    def __init__(self, log_level):
        threading.Thread.__init__(self)
        self.logLevel = log_level
        self.fileRead, self.fileWrite = os.pipe()
        self.pipeIn = os.fdopen(self.fileRead)
        self.daemon = False
        self.start()

    def fileno(self):
        return self.fileWrite

    def run(self):
        for line in iter(self.pipeIn.readline, ""):
            logging.log(self.logLevel, line.strip("\n"))

        self.pipeIn.close()

    def close(self):
        os.close(self.fileWrite)


def run(
    toolchain: str,
    download_resources: bool,
    run_vela_on_models: bool,
    npu_config_name: str,
    make_jobs: int,
    make_verbose: bool,
):
    """
    Run the helpers scripts.

    Parameters:
    ----------
    toolchain (str)          :    Specifies if 'gnu' or 'arm' toolchain needs to be used.
    download_resources (bool):    Specifies if 'Download resources' step is performed.
    run_vela_on_models (bool):    Only if `download_resources` is True, specifies if run vela on downloaded models.
    npu_config_name(str)     :    Ethos-U NPU configuration name. See "valid_npu_config_names"
    """

    current_file_dir = Path(__file__).parent.resolve()

    # 1. Make sure the toolchain is supported, and set the right one here
    supported_toolchain_ids = ["gnu", "arm"]
    assert (
        toolchain in supported_toolchain_ids
    ), f"Toolchain must be from {supported_toolchain_ids}"
    if toolchain == "arm":
        toolchain_file_name = "bare-metal-armclang.cmake"
    elif toolchain == "gnu":
        toolchain_file_name = "bare-metal-gcc.cmake"

    # 2. Download models if specified
    if download_resources is True:
        logging.info("Downloading resources.")
        (download_dir, env_path) = set_up_resources(
            run_vela_on_models=run_vela_on_models,
            additional_npu_config_names=[npu_config_name],
            additional_requirements_file=current_file_dir / "scripts" / "py" / "requirements.txt"
        )

    # 3. Build default configuration
    logging.info("Building default configuration.")
    target_platform = "mps3"
    target_subsystem = "sse-300"
    ethos_u_cfg = get_default_npu_config_from_name(npu_config_name)
    build_dir = current_file_dir / f"cmake-build-{target_platform}-{target_subsystem}-{npu_config_name}-{toolchain}"

    try:
        build_dir.mkdir()
    except FileExistsError:
        # Directory already exists, clean it.
        for filepath in build_dir.iterdir():
            try:
                if filepath.is_file() or filepath.is_symlink():
                    filepath.unlink()
                elif filepath.is_dir():
                    shutil.rmtree(filepath)
            except Exception as e:
                logging.error(f"Failed to delete {filepath}. Reason: {e}")

    logpipe = PipeLogging(logging.INFO)

    cmake_toolchain_file = current_file_dir / "scripts" / "cmake" / "toolchains" / toolchain_file_name
    cmake_path = env_path / "bin" / "cmake"
    cmake_command = (
        f"{cmake_path} -B {build_dir} -DTARGET_PLATFORM={target_platform}"
        + f" -DTARGET_SUBSYSTEM={target_subsystem}"
        + f" -DCMAKE_TOOLCHAIN_FILE={cmake_toolchain_file}"
        + f" -DETHOS_U_NPU_ID={ethos_u_cfg.ethos_u_npu_id}"
        + f" -DETHOS_U_NPU_CONFIG_ID={ethos_u_cfg.ethos_u_config_id}"
        + f" -DTENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS=ON"
    )

    logging.info(f"\n\n\n{cmake_command}\n\n\n")
    state = subprocess.run(
        cmake_command, shell=True, stdout=logpipe, stderr=subprocess.STDOUT
    )

    if state.returncode != 0:
        logging.error("Failed to configure the project.")
        logpipe.close()
        sys.exit(state.returncode)

    make_command = f"{cmake_path} --build {build_dir} -j{make_jobs}"
    if make_verbose:
        make_command += " --verbose"
    logging.info(f"\n\n\n{make_command}\n\n\n")
    state = subprocess.run(
        make_command, shell=True, stdout=logpipe, stderr=subprocess.STDOUT
    )

    if state.returncode != 0:
        logging.error("Failed to build project.")
        logpipe.close()
        sys.exit(state.returncode)

    logpipe.close()


if __name__ == "__main__":
    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        "--toolchain",
        default="gnu",
        help="""
            Specify the toolchain to use (Arm or GNU).
            Options are [gnu, arm]; default is gnu.
            """,
    )
    parser.add_argument(
        "--skip-download",
        help="Do not download resources: models and test vectors",
        action="store_true",
    )
    parser.add_argument(
        "--skip-vela",
        help="Do not run Vela optimizer on downloaded models.",
        action="store_true",
    )
    parser.add_argument(
        "--npu-config-name",
        help=f"""Arm Ethos-U configuration to build for. Choose from:
            {valid_npu_config_names}""",
        default=default_npu_config_names[0],
    )
    parser.add_argument(
        "--make-jobs",
        help="Number of jobs to run with make",
        default=multiprocessing.cpu_count(),
    )
    parser.add_argument(
        "--make-verbose", help="Make runs with VERBOSE=1", action="store_true"
    )
    args = parser.parse_args()

    logging.basicConfig(
        filename="log_build_default.log", level=logging.DEBUG, filemode="w"
    )
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    run(
        args.toolchain.lower(),
        not args.skip_download,
        not args.skip_vela,
        args.npu_config_name,
        args.make_jobs,
        args.make_verbose,
    )
