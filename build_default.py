#!/usr/bin/env python3
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

from scripts.py.mlek_tools.config.download import DownloadConfig
from scripts.py.mlek_tools.config.executorch import ExecuTorchConfig
from scripts.py.mlek_tools.config.npu import (
    NpuConfig,
    supported_npu_config_names,
)
from scripts.py.mlek_tools.config.optimizer import OptimizerConfig
from scripts.py.mlek_tools.config.paths import PathsConfig
from scripts.py.mlek_tools.config.tflite import TfliteConfig
from scripts.py.mlek_tools.setup.logging_config import LoggingOptions, configure_logging
from set_up_default_resources import EXECUTORCH_EXCLUDED_NPU_PROCESSOR_IDS
from set_up_default_resources import INSTALL_VELA_FROM_SOURCE
from set_up_default_resources import MLFramework, valid_ml_frameworks
from set_up_default_resources import VELA_URL, VELA_VERSION
from set_up_default_resources import default_downloads_path
from set_up_default_resources import default_executorch_path
from set_up_default_resources import default_executorch_requirements_path
from set_up_default_resources import default_npu_configs
from set_up_default_resources import default_requirements_path
from set_up_default_resources import default_use_case_resources_path
from set_up_default_resources import default_vela_config_file
from set_up_default_resources import set_up_resources_with_defaults as set_up_resources

current_file_dir = Path(__file__).parent.resolve()


class PipeLogging(threading.Thread):
    """
    Thread that forwards lines from a pipe to the Python logging system.
    """

    def __init__(self, log_level):
        threading.Thread.__init__(self)
        self.log_level = log_level
        self.file_read, self.file_write = os.pipe()
        self.pipe_in = os.fdopen(self.file_read)
        self.daemon = False
        self.start()

    def fileno(self):
        """Return the writable file descriptor used by subprocess stdout."""
        return self.file_write

    def run(self):
        """Forward each line from the pipe into the Python logger."""
        for line in iter(self.pipe_in.readline, ""):
            logging.log(self.log_level, line.strip("\n"))
        self.pipe_in.close()

    def close(self):
        """Close the write end of the pipe used by the subprocess."""
        os.close(self.file_write)


def run_command(command: str, logpipe: PipeLogging, fail_message: str) -> None:
    """
    Run a shell command, routing stdout through a PipeLogging thread, and exit on failure.

    :param command:         The command to run.
    :param logpipe:         The PipeLogging object to capture stdout.
    :param fail_message:    The message to log upon a non-zero exit code.
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


@dataclass(frozen=True)
class BuildConfig:
    """
    Args used to build the project.

    Attributes:
        toolchain (str)             : Specifies if 'gnu' or 'arm' toolchain needs to be used.
        download_resources (bool)   : Specifies if 'Download resources' step is performed.
        npu_config_name (str)       : Ethos-U NPU configuration name.
        fpga (bool)                 : Build for FPGA instead of FVP
                                    (only when these platforms differ)
        make_jobs (int)             : The number of make jobs to use (`-j` flag).
        make_verbose (bool)         : Runs make with VERBOSE=1.
        ml_framework                : Specifies ML framework to use for the build.
    """
    toolchain: str
    download_resources: bool
    npu_config_name: str
    fpga: bool
    make_jobs: int
    make_verbose: bool
    ml_framework: MLFramework


@dataclass(frozen=True)
class ResourceConfigs:
    """
    Resource setup configuration groups.
    """
    download: DownloadConfig
    optimizer: OptimizerConfig
    tflite: TfliteConfig
    executorch: ExecuTorchConfig
    paths: PathsConfig


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

    if toolchain == "llvm":
        return "bare-metal-llvm.cmake"

    raise ValueError("Toolchain must be one of: 'gnu', 'arm' or 'llvm'")


def prep_build_dir(
        target_platform: str,
        target_subsystem: str,
        build_config: BuildConfig
) -> Path:
    """
    Create or clean the build directory for this project.

    Parameters
    ----------
    target_platform     : The name of the target platform, e.g. "mps3"
    target_subsystem    : The name of the target subsystem, e.g. "sse-300"
    build_config        : Build config object

    Returns
    -------
    The path to the build directory
    """

    build_dir = (
            current_file_dir /
            (f"cmake-build-{target_platform}-{target_subsystem}" +
             f"-{build_config.npu_config_name}-{build_config.toolchain}" +
             f"-{build_config.ml_framework.value}")
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


def run(
        build_config: BuildConfig,
        resource_configs: ResourceConfigs,
):
    """
    Run the helpers scripts.

    Parameters:
    ----------
    build_config (BuildArgs)        : Config for the build
    resource_configs (ResourceConfigs): Resource setup config groups
    """

    # 1. Make sure the toolchain is supported, and set the right one here
    toolchain_file_name = get_toolchain_file_name(build_config.toolchain)

    # 2. Download models if specified
    if build_config.download_resources:
        env_path = set_up_resources(
            resource_configs.download,
            resource_configs.optimizer,
            resource_configs.tflite,
            resource_configs.executorch,
            resource_configs.paths,
        )
    else:
        env_path = resource_configs.paths.downloads_dir / "env"

    # 3. Build default configuration
    logging.info("Building default configuration.")

    # Default platform for Arm Ethos-U85 is Arm Corstone-320
    # using Arm MPS4 based FVP implementation. For other NPUs,
    # we use Arm MPS3 FVP based on Arm Corstone-300 reference.
    if build_config.npu_config_name.startswith('ethos-u85'):
        target_platform = "mps4"
        target_subsystem = "sse-320"
    else:
        target_platform = "mps3"
        target_subsystem = "sse-300"

    build_dir = prep_build_dir(
        target_platform,
        target_subsystem,
        build_config
    )

    logpipe = PipeLogging(logging.INFO)

    cmake_toolchain_file = (
            current_file_dir /
            "scripts" /
            "cmake" /
            "toolchains" /
            toolchain_file_name
    )
    ethos_u_cfg = NpuConfig.from_name(build_config.npu_config_name)
    cmake_path = env_path / "bin" / "cmake"
    if build_config.ml_framework == MLFramework.TENSORFLOW_LITE_MICRO:
        # TensorFlow Lite Micro is already the default option. Just ensure
        # a clean build for the framework.
        framework_arg = '-DTENSORFLOW_LITE_MICRO_CLEAN_DOWNLOADS=ON'
    elif build_config.ml_framework == MLFramework.EXECUTORCH:
        # Set framework to ExecuTorch.
        framework_arg = '-DML_FRAMEWORK=ExecuTorch'
    else:
        raise NotImplementedError(f'Unsupported ML Framework {build_config.ml_framework}')

    fpga_arg = ""
    if build_config.fpga:
        if target_subsystem == "sse-320":
            fpga_arg = "-DFPGA_PLATFORM_SSE_320=ON"
        else:
            logging.warning(
                "--fpga argument passed but we are building for %s;"
                " the binaries will work on both FVP and FPGA", target_subsystem
            )

    # CMake configure
    run_command(
        command=(
            f"{cmake_path} -B {build_dir} -DTARGET_PLATFORM={target_platform}"
            f" -DTARGET_SUBSYSTEM={target_subsystem}"
            f" -DCMAKE_TOOLCHAIN_FILE={cmake_toolchain_file}"
            f" -DETHOS_U_NPU_ID={ethos_u_cfg.processor_id}"
            f" -DETHOS_U_NPU_CONFIG_ID={ethos_u_cfg.config_id}"
            f" -DETHOS_U_NPU_MEMORY_MODE={ethos_u_cfg.memory_mode}"
            f" {framework_arg}"
            f" {fpga_arg}"
        ),
        logpipe=logpipe,
        fail_message="Failed to configure the project."
    )

    # CMake build
    build_command = f"{cmake_path} --build {build_dir} -j{build_config.make_jobs}"
    if build_config.make_verbose:
        build_command += " --verbose"

    run_command(
        command=build_command,
        logpipe=logpipe,
        fail_message="Failed to build project."
    )

    logpipe.close()


if __name__ == "__main__":
    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        "--toolchain",
        default="gnu",
        help="""
            Specify the toolchain to use (Arm, GNU or LLVM).
            Options are [gnu, arm, llvm]; default is gnu.
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
        "--ml-framework",
        help=f"""Specify the ML framework to use for building the examples.
        Valid values are: {valid_ml_frameworks}
        """,
        default=f'{MLFramework.TENSORFLOW_LITE_MICRO.value}'
    )
    parser.add_argument(
        "--npu-config-name",
        help=f"""Arm Ethos-U configuration to build for. Choose from:
            {supported_npu_config_names()}
            Memory mode may also be specified:
            {supported_npu_config_names(memory_mode=True)}""",
        default=default_npu_configs[0].config_name,
    )
    parser.add_argument(
        "--fpga",
        help=(
            "Build for FPGA platform. "
            "This is only used for Arm Corstone-320 "
            "where the FPGA and FVP require different binaries. "
            "For all other platforms the same binary will work on FPGA and FVP."
        ),
        action="store_true",
    )
    parser.add_argument(
        "--make-jobs",
        help="Number of jobs to run with make",
        type=int,
        default=multiprocessing.cpu_count(),
    )
    parser.add_argument(
        "--make-verbose", help="Make runs with VERBOSE=1", action="store_true"
    )
    parsed_args = parser.parse_args()

    configure_logging(
        Path("log_build_default.log"),
        LoggingOptions(
            file_mode="w",
            show_thread=parsed_args.make_jobs > 1,
        ),
    )

    build = BuildConfig(
        toolchain=parsed_args.toolchain.lower(),
        download_resources=not parsed_args.skip_download,
        npu_config_name=parsed_args.npu_config_name,
        fpga=parsed_args.fpga,
        make_jobs=parsed_args.make_jobs,
        make_verbose=parsed_args.make_verbose,
        ml_framework=MLFramework(parsed_args.ml_framework)
    )

    use_tflm = build.ml_framework == MLFramework.TENSORFLOW_LITE_MICRO
    use_executorch = build.ml_framework == MLFramework.EXECUTORCH

    download = DownloadConfig(
        parallel=parsed_args.make_jobs,
    )

    optimizer = OptimizerConfig(
        vela_config_file=default_vela_config_file,
        additional_npu_config_names=[build.npu_config_name],
    )

    tflite = TfliteConfig(
        enabled=use_tflm,
        run_vela=not parsed_args.skip_vela,
        vela_version=VELA_VERSION,
        vela_url=VELA_URL,
        vela_install_from_source=INSTALL_VELA_FROM_SOURCE,
    )

    executorch = ExecuTorchConfig(
        enabled=use_executorch,
        run_lowering=not parsed_args.skip_vela,
        executorch_path=default_executorch_path,
        excluded_npu_processor_ids=EXECUTORCH_EXCLUDED_NPU_PROCESSOR_IDS,
    )

    requirements_files = [default_requirements_path]
    if use_executorch:
        requirements_files.append(default_executorch_requirements_path)

    paths = PathsConfig(
        use_case_resources_files=[default_use_case_resources_path],
        downloads_dir=default_downloads_path,
        requirements_files=requirements_files,
    )

    resources = ResourceConfigs(
        download=download,
        optimizer=optimizer,
        tflite=tflite,
        executorch=executorch,
        paths=paths,
    )

    run(build, resources)
