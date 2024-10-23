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
Script to build the ML Embedded Evaluation kit using default configuration
"""
import logging
import multiprocessing
import os
import shutil
import subprocess
import sys
import threading
from argparse import ArgumentDefaultsHelpFormatter
from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path

from set_up_default_resources import PathsConfig
from set_up_default_resources import SetupConfig
from set_up_default_resources import default_downloads_path
from set_up_default_resources import default_npu_configs
from set_up_default_resources import default_requirements_path
from set_up_default_resources import default_use_case_resources_path
from set_up_default_resources import get_default_npu_config_from_name
from set_up_default_resources import set_up_resources
from set_up_default_resources import valid_npu_configs


@dataclass(frozen=True)
class BuildConfig:
    """
    Args used to build the project.

    Attributes:
        toolchain (str)            : Specifies if 'gnu' or 'arm' toolchain needs to be used.
        download_resources (bool)  : Specifies if 'Download resources' step is performed.
        run_vela_on_models (bool)  : Only if `download_resources` is True, specifies whether to
                                     run Vela on downloaded models.
        npu_config_name (str)      : Ethos-U NPU configuration name. See "valid_npu_config_names"
        make_jobs (int)            : The number of make jobs to use (`-j` flag).
        make_verbose (bool)        : Runs make with VERBOSE=1.
    """
    toolchain: str
    download_resources: bool
    run_vela_on_models: bool
    npu_config_name: str
    make_jobs: int
    make_verbose: bool


class PipeLogging(threading.Thread):
    """
    Class used to log stdout from subprocesses
    """

    def __init__(self, log_level):
        threading.Thread.__init__(self)
        self.log_level = log_level
        self.file_read, self.file_write = os.pipe()
        self.pipe_in = os.fdopen(self.file_read)
        self.daemon = False
        self.start()

    def fileno(self):
        """
        Get self.file_write

        Returns
        -------
        self.file_write
        """
        return self.file_write

    def run(self):
        """
        Log the contents of self.pipe_in
        """
        for line in iter(self.pipe_in.readline, ""):
            logging.log(self.log_level, line.strip("\n"))

        self.pipe_in.close()

    def close(self):
        """
        Close the pipe
        """
        os.close(self.file_write)


def get_toolchain_file_name(toolchain: str) -> str:
    """
    Get the name of the toolchain file for the toolchain.

    Parameters
    ----------
    toolchain   : name of the specified toolchain

    Returns
    -------
    name of the toolchain file corresponding to the specified
    toolchain
    """
    if toolchain == "arm":
        return "bare-metal-armclang.cmake"

    if toolchain == "gnu":
        return "bare-metal-gcc.cmake"

    raise ValueError("Toolchain must be one of: gnu, arm")


def prep_build_dir(
        current_file_dir: Path,
        target_platform: str,
        target_subsystem: str,
        npu_config_name: str,
        toolchain: str
) -> Path:
    """
    Create or clean the build directory for this project.

    Parameters
    ----------
    current_file_dir    : The current directory of the running script
    target_platform     : The name of the target platform, e.g. "mps3"
    target_subsystem:   : The name of the target subsystem, e.g. "sse-300"
    npu_config_name     : The NPU config name, e.g. "ethos-u55-32"
    toolchain           : The name of the specified toolchain, e.g."arm"

    Returns
    -------
    The path to the build directory
    """
    build_dir = (
            current_file_dir /
            f"cmake-build-{target_platform}-{target_subsystem}-{npu_config_name}-{toolchain}"
    )

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
            except OSError as err:
                logging.error("Failed to delete %s. Reason: %s", filepath, err)

    return build_dir


def run_command(
        command: str,
        logpipe: PipeLogging,
        fail_message: str
):
    """
    Run a command and exit upon failure.

    Parameters
    ----------
    command         : The command to run
    logpipe         : The PipeLogging object to capture stdout
    fail_message    : The message to log upon a non-zero exit code
    """
    logging.info("\n\n\n%s\n\n\n", command)

    try:
        subprocess.run(
            command, check=True, shell=True, stdout=logpipe, stderr=subprocess.STDOUT
        )
    except subprocess.CalledProcessError as err:
        logging.error(fail_message)
        logpipe.close()
        sys.exit(err.returncode)


def run(build_config: BuildConfig):
    """
    Run the helpers scripts.

    Parameters:
    ----------
    args (BuildArgs)    : Arguments used to build the project
    """

    current_file_dir = Path(__file__).parent.resolve()

    # 1. Make sure the toolchain is supported, and set the right one here
    toolchain_file_name = get_toolchain_file_name(build_config.toolchain)

    # 2. Download models if specified
    if build_config.download_resources is True:
        logging.info("Downloading resources.")
        setup_config = SetupConfig(
            run_vela_on_models=build_config.run_vela_on_models,
            additional_npu_config_names=[build_config.npu_config_name],
        )
        paths_config = PathsConfig(
            additional_requirements_file=default_requirements_path,
            use_case_resources_file=default_use_case_resources_path,
            downloads_dir=default_downloads_path
        )
        env_path = set_up_resources(setup_config, paths_config)
    else:
        env_path = default_downloads_path / "env"

    # 3. Build default configuration
    logging.info("Building default configuration.")
    target_platform = "mps3"
    target_subsystem = "sse-300"

    build_dir = prep_build_dir(
        current_file_dir,
        target_platform,
        target_subsystem,
        build_config.npu_config_name,
        build_config.toolchain
    )

    logpipe = PipeLogging(logging.INFO)

    cmake_toolchain_file = (
            current_file_dir /
            "scripts" /
            "cmake" /
            "toolchains" /
            toolchain_file_name
    )
    ethos_u_cfg = get_default_npu_config_from_name(build_config.npu_config_name)
    cmake_path = env_path / "bin" / "cmake"
    cmake_command = (
        f"{cmake_path} -B {build_dir} -DTARGET_PLATFORM={target_platform}"
        f" -DTARGET_SUBSYSTEM={target_subsystem}"
        f" -DCMAKE_TOOLCHAIN_FILE={cmake_toolchain_file}"
        f" -DETHOS_U_NPU_ID={ethos_u_cfg.processor_id}"
        f" -DETHOS_U_NPU_CONFIG_ID={ethos_u_cfg.config_id}"
        " -DTENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS=ON"
    )

    run_command(cmake_command, logpipe, fail_message="Failed to configure the project.")

    make_command = f"{cmake_path} --build {build_dir} -j{build_config.make_jobs}"
    if build_config.make_verbose:
        make_command += " --verbose"

    run_command(make_command, logpipe, fail_message="Failed to build project.")

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
            {valid_npu_configs.names}""",
        default=default_npu_configs.names[0],
    )
    parser.add_argument(
        "--make-jobs",
        help="Number of jobs to run with make",
        default=multiprocessing.cpu_count(),
    )
    parser.add_argument(
        "--make-verbose", help="Make runs with VERBOSE=1", action="store_true"
    )
    parsed_args = parser.parse_args()

    logging.basicConfig(
        filename="log_build_default.log", level=logging.DEBUG, filemode="w"
    )
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    build = BuildConfig(
        toolchain=parsed_args.toolchain.lower(),
        download_resources=not parsed_args.skip_download,
        run_vela_on_models=not parsed_args.skip_vela,
        npu_config_name=parsed_args.npu_config_name,
        make_jobs=parsed_args.make_jobs,
        make_verbose=parsed_args.make_verbose
    )

    run(build)
