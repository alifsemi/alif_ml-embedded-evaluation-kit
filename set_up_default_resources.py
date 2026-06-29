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
Script to set up default resources for ML Embedded Evaluation Kit.

This script is a thin CLI wrapper around mlek_tools.orchestrate.set_up_resources.
Project-specific defaults (versions, paths, NPU selections) are defined here;
all orchestration logic lives in the mlek_tools package.
"""
import itertools
import typing
from argparse import Action, ArgumentParser, ArgumentTypeError
from enum import Enum
from pathlib import Path

from scripts.py.mlek_tools.config.download import DownloadConfig
from scripts.py.mlek_tools.config.executorch import ExecuTorchConfig
from scripts.py.mlek_tools.config.npu import (
    Npu,
    NpuConfig,
    supported_npu_config_names,
)
from scripts.py.mlek_tools.config.optimizer import OptimizerConfig
from scripts.py.mlek_tools.config.paths import PathsConfig
from scripts.py.mlek_tools.config.tflite import TfliteConfig
from scripts.py.mlek_tools.orchestrate import set_up_resources
from scripts.py.mlek_tools.setup.logging_config import (
    LOG_LEVELS,
    LoggingOptions,
    configure_logging,
    parse_log_level,
)
from scripts.py.mlek_tools.setup.python_venv import PythonEnv
from scripts.py.mlek_tools.setup.util import get_sha256sum_for_file
from scripts.py.mlek_tools.use_case.model import load_use_case_resources


class MLFramework(Enum):
    """Supported ML frameworks for resource setup."""
    TENSORFLOW_LITE_MICRO = "tflm"
    EXECUTORCH = "executorch"


valid_ml_frameworks: typing.Set[str] = {f.value for f in MLFramework}

# ── Project-specific constants ────────────────────────────────────────────────

VELA_VERSION = "5.1.0"
INSTALL_VELA_FROM_SOURCE = False
VELA_URL = "https://git.gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-vela.git"
MIN_PYTHON_VERSION = (3, 10)

# NPU processor IDs excluded from ExecuTorch optimisation due to toolchain limitations.
EXECUTORCH_EXCLUDED_NPU_PROCESSOR_IDS = ("U65",)

# Default NPU configurations always optimised when models are downloaded.
default_npu_configs = (
    NpuConfig(Npu.ETHOS_U55, 128, Npu.ETHOS_U55.default_memory_mode),
    NpuConfig(Npu.ETHOS_U65, 256, Npu.ETHOS_U65.default_memory_mode),
    NpuConfig(Npu.ETHOS_U85, 256, Npu.ETHOS_U85.default_memory_mode),
)

_current_file_dir = Path(__file__).parent.resolve()
default_use_case_resources_path = _current_file_dir / "resources" / "use_case_resources.json"
default_requirements_path = _current_file_dir / "scripts" / "py" / "requirements.txt"
default_executorch_requirements_path = (
    _current_file_dir / "scripts" / "py" / "requirements-executorch.txt"
)
default_downloads_path = _current_file_dir / "resources_downloaded"
default_executorch_path = _current_file_dir / "dependencies" / "executorch"
default_cmsis_nn_path = _current_file_dir / "dependencies" / "cmsis-nn"
default_vela_config_file = _current_file_dir / "scripts" / "vela" / "default_vela.ini"

# ── Helpers that depend on project-specific defaults ──────────────────────────

def get_default_use_cases_names() -> typing.List[str]:
    """Return the names of all use cases defined in the default resources file."""
    use_case_resources = load_use_case_resources([default_use_case_resources_path])
    return [uc.name for uc in use_case_resources]


# ── Public entry point (re-exported for build_default.py compatibility) ───────

def set_up_resources_with_defaults(
        download_config: DownloadConfig,
        optimizer_config: OptimizerConfig,
        tflite_config: TfliteConfig,
        executorch_config: ExecuTorchConfig,
        paths_config: PathsConfig,
) -> Path:
    """Convenience wrapper that fills in the setup-script hash and project defaults.

    :return: Path to the root of the virtual environment.
    """
    setup_script_hash = get_sha256sum_for_file(Path(__file__).resolve())

    venv_path = set_up_resources(
        download_config=download_config,
        optimizer_config=optimizer_config,
        tflite_config=tflite_config,
        executorch_config=executorch_config,
        paths_config=paths_config,
        setup_script_hash=setup_script_hash,
        default_npu_configs=default_npu_configs,
        default_downloads_path=default_downloads_path,
        min_python_version=MIN_PYTHON_VERSION,
    )

    python_env = PythonEnv(venv_path)

    # Install mlek_tools as an editable package so entry-point scripts
    # (gen-audio, gen-model-cpp, pte-ops-dump, …) are available in the venv.
    python_env.pip_install_if_needed(
        f"-e {_current_file_dir / 'scripts' / 'py'}",
        installed_name="mlek-tools",
    )

    # Install CI test-suite dependencies so tests/ci/ can run from the venv.
    _test_requirements = _current_file_dir / "tests" / "ci" / "requirements.txt"
    if _test_requirements.is_file():
        python_env.pip_install_requirements(_test_requirements)

    return venv_path


# ── CLI ───────────────────────────────────────────────────────────────────────

class HttpHeadersAction(Action):
    """Argparse action that collects HTTP headers into a {domain: [(key, value), …]} mapping."""

    def __call__(self, _, namespace, values, option_string=None):
        domain, header = values
        all_current_values = getattr(namespace, self.dest, None) or {}
        headers_for_domain = all_current_values.get(domain, [])
        headers_for_domain.append(tuple(v.strip() for v in header.split(":")))
        all_current_values[domain] = headers_for_domain
        setattr(namespace, self.dest, all_current_values)


if __name__ == "__main__":
    parser = ArgumentParser()
    log_level_group = parser.add_mutually_exclusive_group()
    log_level_group.add_argument(
        "--log-level",
        help=f"Minimum log level emitted to the console. Valid values: {sorted(LOG_LEVELS)}",
        type=parse_log_level,
        default=LOG_LEVELS["INFO"],
        metavar="LEVEL",
    )
    log_level_group.add_argument(
        "--verbose",
        help="Enable DEBUG logging on the console.",
        action="store_const",
        const=LOG_LEVELS["DEBUG"],
        dest="log_level",
    )
    parser.add_argument(
        "--skip-vela",
        help="Do not run Vela optimizer on downloaded models.",
        action="store_true",
    )
    parser.add_argument(
        "--ml-frameworks", "--ml-framework",
        help=f"Specify the ML frameworks to set up resources for. "
             f"Valid values: {valid_ml_frameworks}",
        nargs="+",
        choices=sorted(valid_ml_frameworks),
        default=[MLFramework.TENSORFLOW_LITE_MICRO.value],
        action="store",
    )
    parser.add_argument(
        "--additional-npu-config-name",
        "--additional-ethos-u-config-name",
        help=f"Additional NPU configurations for Vela. "
             f"Unqualified names select default memory modes: "
             f"{supported_npu_config_names()}. "
             f"Memory mode may also be specified: "
             f"{supported_npu_config_names(memory_mode=True)}",
        default=[],
        action="append",
        dest="additional_ethos_u_config_name",
    )
    parser.add_argument(
        "--use-case",
        help=f"Only set up resources for the specified use case (repeatable). "
             f"Valid values: {get_default_use_cases_names()}",
        default=[],
        action="append",
    )
    parser.add_argument(
        "--arena-cache-size",
        help="Arena cache size in bytes (overrides the default).",
        type=int,
        default=0,
    )
    parser.add_argument(
        "--clean",
        help="Check existing resource metadata and remove stale resource directories before setup.",
        action="store_true",
    )
    parser.add_argument(
        "--parallel",
        help="Number of threads for downloads and model optimisation.",
        type=int,
        default=1,
    )
    parser.add_argument(
        "--requirements-file",
        help="Path to a requirements.txt file with additional packages.",
        type=Path,
        default=default_requirements_path,
    )
    parser.add_argument(
        "--use-case-resources-file",
        help="Path to a use case resources JSON file.",
        type=Path,
        nargs="+",
        default=[],
        action="append",
    )
    parser.add_argument(
        "--downloads-dir",
        help="Directory for downloaded model resources.",
        type=Path,
        default=default_downloads_path,
    )
    parser.add_argument(
        "--http-header",
        help="HTTP header for a domain. "
             "Example: --http-header my-site.com 'Authorization: Bearer $TOKEN'",
        type=str,
        metavar=("DOMAIN", "HEADER"),
        nargs=2,
        default={},
        action=HttpHeadersAction,
    )

    parsed_args = parser.parse_args()

    if parsed_args.arena_cache_size < 0:
        raise ArgumentTypeError("Arena cache size cannot be less than 0")

    if not Path(parsed_args.requirements_file).is_file():
        raise ArgumentTypeError(f"Invalid requirements file: {parsed_args.requirements_file}")

    configure_logging(
        Path("log_build_default.log"),
        LoggingOptions(
            console_level=parsed_args.log_level,
            file_mode="a",
            show_thread=parsed_args.parallel > 1,
        ),
    )

    ml_frameworks = (
        valid_ml_frameworks
        if len(parsed_args.ml_frameworks) == 0
        else valid_ml_frameworks.intersection(parsed_args.ml_frameworks)
    )

    use_tflm = MLFramework.TENSORFLOW_LITE_MICRO.value in ml_frameworks
    use_et = MLFramework.EXECUTORCH.value in ml_frameworks

    use_case_resources_files = (
        [default_use_case_resources_path]
        if len(parsed_args.use_case_resources_file) == 0
        else list(itertools.chain(*parsed_args.use_case_resources_file))
    )

    requirements_files = [parsed_args.requirements_file]
    if use_et:
        requirements_files.append(default_executorch_requirements_path)

    download = DownloadConfig(
        use_case_names=parsed_args.use_case,
        check_clean_folder=parsed_args.clean,
        parallel=parsed_args.parallel,
        http_headers=parsed_args.http_header,
    )

    optimizer = OptimizerConfig(
        vela_config_file=default_vela_config_file,
        arena_cache_size=parsed_args.arena_cache_size,
        additional_npu_config_names=parsed_args.additional_ethos_u_config_name,
    )

    tflite = TfliteConfig(
        enabled=use_tflm,
        run_vela=not parsed_args.skip_vela,
        vela_version=VELA_VERSION,
        vela_url=VELA_URL,
        vela_install_from_source=INSTALL_VELA_FROM_SOURCE,
    )

    executorch = ExecuTorchConfig(
        enabled=use_et,
        run_lowering=not parsed_args.skip_vela,
        executorch_path=default_executorch_path,
        cmsis_nn_path=default_cmsis_nn_path,
        excluded_npu_processor_ids=EXECUTORCH_EXCLUDED_NPU_PROCESSOR_IDS,
    )

    paths = PathsConfig(
        use_case_resources_files=use_case_resources_files,
        downloads_dir=parsed_args.downloads_dir,
        requirements_files=requirements_files,
    )

    set_up_resources_with_defaults(download, optimizer, tflite, executorch, paths)
