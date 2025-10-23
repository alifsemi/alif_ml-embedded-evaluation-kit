# ----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
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
# ----------------------------------------------------------------------------
"""
Utility script to optimize Conformer model for ExecuTorch.
"""

import argparse
from pathlib import Path
from dataclasses import dataclass
import logging
import torch

from executorch.backends.arm.ethosu import EthosUPartitioner, EthosUCompileSpec

# pylint: disable=W0611
from executorch.backends.arm.quantizer import (
    EthosUQuantizer,
    get_symmetric_quantization_config,
)

from executorch.exir import (
    EdgeCompileConfig,
    ExecutorchBackendConfig,
    to_edge_transform_and_lower,
)
from executorch.extension.export_util.utils import save_pte_program

logger = logging.getLogger(__name__)
if not logger.handlers:
    logging.basicConfig(level=logging.DEBUG)

@dataclass(frozen=True)
class TargetConfig:
    """
    Utility class to store target configuration.
    """
    target_name: str
    system_config: str
    memory_mode: str

def generate_pte(exported_program_path: Path|str,
                 cfg: TargetConfig,
                 config_ini: Path|str,
                 output: Path|str):
    """
    Generate a PTE file given the target config.
    :param exported_program_path:   Path to the exported program to be loaded.
    :param cfg:                     Target configuration object.
    :param config_ini:              Vela configuration file path.
    :param output:                  Output directory to save the pte file.
    """
    quant_exported_program = torch.export.load(exported_program_path)

    target = cfg.target_name
    system_config = cfg.system_config
    memory_mode = cfg.memory_mode

    model_name = Path(exported_program_path).name.split('.')[0]

    if target.startswith("TOSA"):
        logger.info("TOSA target spec without delegation")
        partitioner = None
        output_model_name = f"{model_name}_arm_{target}.pte"
    elif "ethos-u" in target:
        logger.info("%s target spec", target)
        compile_spec = EthosUCompileSpec(
                target=target,
                system_config=system_config,
                memory_mode=memory_mode,
                extra_flags=["--output-format=raw", "--debug-force-regor"],
                config_ini=config_ini)
        partitioner = EthosUPartitioner(compile_spec)
        output_model_name = f"{model_name}_arm_delegate_{target}.pte"

    else:
        raise ValueError(f"Invalid target {target}")

    # Lower the exported program to the Ethos-U backend
    logger.info("Calling to_edge_transform_and_lower")
    edge_program_manager = to_edge_transform_and_lower(
        quant_exported_program,
        partitioner=[partitioner] if partitioner else None,
        compile_config=EdgeCompileConfig(
            _check_ir_validity=False,
        ),
    )
    executorch_program_manager = edge_program_manager.to_executorch(
        config=ExecutorchBackendConfig(extract_delegate_segments=False)
    )
    if Path(output).is_dir():
        output = Path(output) / output_model_name

    save_pte_program(executorch_program_manager, str(output))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-m",
        "--model_name",
        required=True,
        help="Path to the model as an exported program."
    )
    parser.add_argument(
        "-t",
        "--target",
        action="store",
        required=False,
        default="ethos-u85-256",
        help="For ArmBackend delegated models."
    )
    parser.add_argument(
        "-o",
        "--output",
        action="store",
        required=False,
        help="Output directory to save the model in."
    )
    parser.add_argument(
        "--system_config",
        required=False,
        default=None,
        help="System configuration to select from the Vela configuration file (see vela.ini)."
    )
    parser.add_argument(
        "--memory_mode",
        required=False,
        default=None,
        help="Memory mode to select from the Vela configuration file (see vela.ini)."
    )
    parser.add_argument(
        "--config",
        required=False,
        default="Arm/vela.ini",
        help="Specify custom vela configuration file (vela.ini)",
    )
    args, _ = parser.parse_known_args()

    if not str(args.model_name).endswith(".pt2"):
        raise ValueError(f"Model name {args.model_name} should be a pt2 file")

    generate_pte(exported_program_path=args.model_name,
                 cfg=TargetConfig(target_name=args.target,
                                  system_config=args.system_config,
                                  memory_mode=args.memory_mode),
                 config_ini=args.config,
                 output=args.output)
