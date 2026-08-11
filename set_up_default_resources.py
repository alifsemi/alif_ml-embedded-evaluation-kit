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

This script is a thin CLI wrapper around mlek_tools.setup.resources.
Project-specific defaults (versions, paths) are defined here; all orchestration
logic lives in the mlek_tools package.
"""
import itertools
import logging
import sys
import typing
import dataclasses
from argparse import ArgumentParser, ArgumentTypeError, Action
from enum import Enum
from pathlib import Path

from scripts.py.mlek_tools.setup.npu_config import NpuConfigs, valid_npu_configs
from scripts.py.mlek_tools.setup.python_venv import PythonEnv
from scripts.py.mlek_tools.setup.resources import set_up_resources
from scripts.py.mlek_tools.setup.setup_config import (
    OptimizationConfig,
    PathsConfig,
    SetupConfig,
    VelaConfig,
)
from scripts.py.mlek_tools.setup.use_case import load_use_case_resources
from scripts.py.mlek_tools.setup.util import get_md5sum_for_file


class MLFramework(Enum):
    """Supported ML frameworks for resource setup."""
    TENSORFLOW_LITE_MICRO = "tflm"
    EXECUTORCH = "executorch"


valid_ml_frameworks: typing.Set[str] = {f.value for f in MLFramework}

VELA_VERSION = "5.0.0"
INSTALL_VELA_FROM_SOURCE = False
VELA_URL = "https://git.gitlab.arm.com/artificial-intelligence/ethos-u/ethos-u-vela.git"
MIN_PYTHON_VERSION = (3, 10)
# NPU processor IDs excluded from ExecuTorch optimisation due to toolchain limitations.
EXECUTORCH_EXCLUDED_NPU_PROCESSOR_IDS = ("U65",)

# Default NPU configurations (always optimised when models are downloaded)
default_npu_configs = NpuConfigs.create(
    valid_npu_configs.get("ethos-u55", 128),
    valid_npu_configs.get("ethos-u55", 256),
    valid_npu_configs.get("ethos-u85", 256),
)

_current_file_dir = Path(__file__).parent.resolve()
default_use_case_resources_path = _current_file_dir / "resources" / "use_case_resources.json"
default_requirements_path = _current_file_dir / "scripts" / "py" / "requirements.txt"
default_downloads_path = _current_file_dir / "resources_downloaded"
default_executorch_path = _current_file_dir / "dependencies" / "executorch"
_default_vela_config_file = _current_file_dir / "scripts" / "vela" / "ensemble_vela.ini"

_vela_config = VelaConfig(
    version=VELA_VERSION,
    url=VELA_URL,
    install_from_source=INSTALL_VELA_FROM_SOURCE,
)


# ---------------------------------------------------------------------------
# Helpers that depend on project-specific defaults
# ---------------------------------------------------------------------------

def get_default_use_cases_names() -> typing.List[str]:
    """
    Get the names of all default use cases.

    :return:    List of use case names as strings.
    """
    use_case_resources = load_use_case_resources([default_use_case_resources_path])
    return [uc.name for uc in use_case_resources]


# ---------------------------------------------------------------------------
# Public entry point (re-exported for build_default.py compatibility)
# ---------------------------------------------------------------------------

def set_up_resources_with_defaults(
        setup_config: SetupConfig,
        optimization_config: OptimizationConfig,
        paths_config: PathsConfig,
) -> Path:
    """
    Convenience wrapper that fills in project-specific defaults before calling
    the library-level set_up_resources().

    :param setup_config:        General setup configuration.
    :param optimization_config: Configuration related to model optimization.
    :param paths_config:        Paths configuration. If vela_config_file is not set,
                                the project default is used.
    :return:                    Path to the root of the virtual environment.
    """
    # Fill in default vela_config_file if the caller left it unset.
    if paths_config.vela_config_file is None:
        paths_config = dataclasses.replace(
            paths_config, vela_config_file=_default_vela_config_file
        )

    setup_script_hash = get_md5sum_for_file(Path(__file__).resolve())

    venv_path = set_up_resources(
        setup_config=setup_config,
        optimization_config=optimization_config,
        paths_config=paths_config,
        vela_config=_vela_config,
        setup_script_hash=setup_script_hash,
        default_npu_configs=default_npu_configs,
        default_downloads_path=default_downloads_path,
        min_python_version=MIN_PYTHON_VERSION,
    )

    python_env = PythonEnv(venv_path)

    # Install the mlek_tools package itself as an editable install so that
    # the entry-point scripts (gen-audio, gen-model-cpp, pte-ops-dump, …)
    # are available inside the venv without needing sys.path manipulation.
    python_env.pip_install_if_needed(
        f"-e {_current_file_dir / 'scripts' / 'py'}",
        installed_name="mlek-tools",
    )

    # Install CI test-suite dependencies (pytest, pytest-xdist) so that
    # the tests/ci/ suite can be run directly from the activated venv.
    _test_requirements = _current_file_dir / "tests" / "ci" / "requirements.txt"
    if _test_requirements.is_file():
        python_env.pip_install_requirements(_test_requirements)

    return venv_path


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

class HttpHeadersAction(Action):
    """
    Argparse action that collects HTTP headers into a
    {domain: [(key, value), ...]} mapping.
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
        "--ml-frameworks", "--ml-framework",
        help=f"Specify the ML frameworks for which to set up resources. "
             f"Valid values are: {valid_ml_frameworks}",
        nargs="+",
        default=[MLFramework.TENSORFLOW_LITE_MICRO.value],
        action="store",
    )
    parser.add_argument(
        "--additional-ethos-u-config-name",
        help=f"Additional (non-default) configurations for Vela: {valid_npu_configs.names}",
        default=[],
        action="append",
    )
    parser.add_argument(
        "--use-case",
        help=f"Only set up resources for the specified use case (can specify multiple times). "
             f"Valid values are: {get_default_use_cases_names()}",
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
        default=1,
    )
    parser.add_argument(
        "--requirements-file",
        help="Path to requirements.txt file to install additional packages",
        type=Path,
        default=default_requirements_path,
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
        default=default_downloads_path,
    )
    parser.add_argument(
        "--executorch-path",
        help="Path to the root of the ExecuTorch source tree",
        type=Path,
        default=default_executorch_path,
    )
    parser.add_argument(
        "--http-header",
        help="Specify HTTP Headers to set when downloading from a domain. "
             "Example: --http-header my-internal-website.com 'Authorization: Bearer $TOKEN'",
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
        http_headers=parsed_args.http_header,
        executorch_excluded_npu_processor_ids=EXECUTORCH_EXCLUDED_NPU_PROCESSOR_IDS,
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
        executorch_path=parsed_args.executorch_path,
        vela_config_file=_default_vela_config_file,
    )

    set_up_resources_with_defaults(setup, optimization, paths)
