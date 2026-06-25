#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2026 Arm Limited and/or its
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
"""MLEK-owned Arm backend lowering script for ExecuTorch.

Run this script with the ExecuTorch root as the working directory so that the
``examples`` package is importable.  It replaces direct use of
``backends.arm.scripts.aot_arm_compiler`` for Arm backend targets, giving MLEK full
control over the ``EthosUCompileSpec`` construction — in particular the
``extra_flags`` list (``--cop-format``, ``--arena-cache-size``) and
``config_ini``.

Usage::

    python /path/to/executorch_arm_backend_lowering.py \\
        --model_name mv2 \\
        --target ethos-u55-128 \\
        --system_config Ethos_U55_High_End_Embedded \\
        --memory_mode Shared_Sram \\
        --config /path/to/default_vela.ini \\
        --extra-vela-flag="--cop-format=COP1" \\
        --extra-vela-flag="--arena-cache-size=2097152" \\
        --delegate --quantize \\
        --output /path/to/output/dir
"""
import argparse
import logging
import os
import sys
import typing

FORMAT = "[%(levelname)s %(asctime)s %(filename)s:%(lineno)s] %(message)s"
logging.basicConfig(level=logging.WARNING, format=FORMAT)


def _get_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="MLEK ExecuTorch Arm backend lowering script."
    )
    parser.add_argument(
        "-m", "--model_name",
        required=True,
        help="Model name, path to a .py script, or a .pt/.pt2 checkpoint.",
    )
    parser.add_argument(
        "--model_input",
        required=False,
        default=None,
        help="Path to a .pt file containing example inputs.",
    )
    parser.add_argument(
        "-t", "--target",
        required=True,
        help="Ethos-U accelerator config, e.g. 'ethos-u55-128'.",
    )
    parser.add_argument(
        "--system_config",
        required=False,
        default=None,
        help="System configuration name from the Vela config file.",
    )
    parser.add_argument(
        "--memory_mode",
        required=False,
        default=None,
        help="Memory mode name from the Vela config file.",
    )
    parser.add_argument(
        "--config",
        required=False,
        default=None,
        dest="config_ini",
        help="Path to the Vela .ini configuration file.",
    )
    parser.add_argument(
        "--extra-vela-flag",
        dest="extra_vela_flags",
        action="append",
        default=[],
        metavar="FLAG",
        help="Extra Vela flag forwarded to EthosUCompileSpec.extra_flags "
             "(may be specified multiple times).",
    )
    parser.add_argument(
        "-d", "--delegate",
        action="store_true",
        default=False,
        help="Produce an ArmBackend-delegated model.",
    )
    parser.add_argument(
        "-q", "--quantize",
        action="store_true",
        default=False,
        help="Quantize the model before lowering.",
    )
    parser.add_argument(
        "-o", "--output",
        required=False,
        default=None,
        help="Output folder (or .pte filename) for the compiled model.",
    )
    parser.add_argument(
        "--non_strict_export",
        dest="strict_export",
        action="store_false",
        help="Disable strict checking during torch.export.",
    )
    parser.add_argument(
        "--debug",
        action="store_true",
        help="Set logging level to DEBUG.",
    )
    parser.add_argument(
        "-e",
        "--evaluate",
        nargs="?",
        const="generic",
        choices=["generic", "mv2", "deit_tiny", "resnet18"],
        help="Run model evaluation.",
    )
    parser.add_argument(
        "-c",
        "--evaluate_config",
        default=None,
        help="Path to evaluator configuration.",
    )
    parser.add_argument(
        "-i",
        "--intermediates",
        default=None,
        help="Directory for intermediate output.",
    )
    parser.add_argument(
        "-s",
        "--so_library",
        default=None,
        help="Path to a custom operators shared library.",
    )
    parser.add_argument(
        "--bundleio",
        action="store_true",
        default=False,
        help="Produce a BundleIO bpte file.",
    )
    parser.add_argument(
        "--etrecord",
        action="store_true",
        default=False,
        help="Produce an etrecord file.",
    )
    parser.add_argument(
        "--enable_qdq_fusion_pass",
        action="store_true",
        default=False,
        help="Enable quantized qdq fusion passes.",
    )
    parser.add_argument(
        "--enable_debug_mode",
        choices=["json", "tosa"],
        default=None,
        help="Enable ATen-to-TOSA debug mode.",
    )
    parser.add_argument(
        "--direct-drive", "--direct_drive",
        dest="direct_drive",
        action="store_true",
        default=False,
        help="Enable direct-drive lowering.",
    )
    args = parser.parse_args()

    if args.debug:
        logging.basicConfig(level=logging.DEBUG, format=FORMAT, force=True)

    if args.evaluate and (
            args.quantize is None or args.intermediates is None or not args.delegate
    ):
        raise RuntimeError(
            "--evaluate requires --quantize, --intermediates and --delegate to be enabled."
        )

    return args


def _build_compile_spec(args: argparse.Namespace) -> typing.Any:
    """Construct an ``EthosUCompileSpec`` using MLEK's extra flags."""
    # ExecuTorch is an optional mlek_tools dependency and is only required
    # when this script is executed inside the ExecuTorch setup environment.
    from executorch.backends.arm.ethosu import EthosUCompileSpec  # pylint: disable=import-error,import-outside-toplevel

    return EthosUCompileSpec(
        args.target,
        system_config=args.system_config,
        memory_mode=args.memory_mode,
        extra_flags=args.extra_vela_flags,
        config_ini=args.config_ini,
    )


def _apply_replace_quant_nodes(
        edge: typing.Any,
        direct_drive: bool,
        replace_quant_nodes_pass: typing.Callable[[], typing.Any],
) -> typing.Any:
    """
    Apply the Cortex-M quant-node replacement pass when it is required.

    :param edge:                     Edge program to update.
    :param direct_drive:             Whether direct-drive lowering is enabled.
    :param replace_quant_nodes_pass: Replacement pass constructor.
    :return:                         Updated edge program.
    """
    if not direct_drive:
        edge = edge.transform([replace_quant_nodes_pass()])
    return edge


def main():
    """
    Lower an ExecuTorch model for an Ethos-U target.
    """
    # pylint: disable=too-many-locals
    args = _get_args()
    if os.getcwd() not in sys.path:
        sys.path.insert(0, os.getcwd())

    # ExecuTorch helper imports — available once the ExecuTorch venv is active
    # and this script is run with cwd=<executorch_root>.
    from backends.arm.scripts.aot_arm_compiler import (  # pylint: disable=import-error,import-outside-toplevel
        dump_delegation_info,
        get_model_and_inputs_from_name,
        quantize_model,
        QuantMode
    )
    from executorch.exir import (  # pylint: disable=import-error,import-outside-toplevel
        EdgeCompileConfig,
        ExecutorchBackendConfig,
        to_edge_transform_and_lower,
    )
    from executorch.backends.arm.util._factory import create_partitioner  # pylint: disable=import-error,import-outside-toplevel
    from executorch.extension.export_util.utils import save_pte_program  # pylint: disable=import-error,import-outside-toplevel
    from executorch.backends.cortex_m.passes.replace_quant_nodes_pass import ReplaceQuantNodesPass # pylint: disable=import-error,import-outside-toplevel
    import torch  # pylint: disable=import-error,import-outside-toplevel

    original_model, example_inputs = get_model_and_inputs_from_name(
        args.model_name, args.model_input
    )
    model = original_model.eval()

    exported_program = torch.export.export(
        model, example_inputs, strict=args.strict_export
    )
    model = exported_program.module(check_guards=False)

    compile_spec = _build_compile_spec(args)

    if args.delegate:
        if args.quantize:
            # pylint: disable=fixme
            # TODO: Supply calibration_samples argument
            _model_quant, exported_program = quantize_model(
                model,
                example_inputs,
                compile_spec,
                args.model_name,
                args.strict_export,
                QuantMode.INT8,
                calibration_samples=None
            )
        partitioner = create_partitioner(compile_spec)
        edge = to_edge_transform_and_lower(
            exported_program,
            partitioner=[partitioner],
            compile_config=EdgeCompileConfig(_check_ir_validity=False),
        )
    else:
        edge = to_edge_transform_and_lower(
            exported_program,
            compile_config=EdgeCompileConfig(_check_ir_validity=False),
        )

    edge = _apply_replace_quant_nodes(
        edge,
        args.direct_drive,
        ReplaceQuantNodesPass,
    )
    dump_delegation_info(edge, args.intermediates)

    exec_prog = edge.to_executorch(
        config=ExecutorchBackendConfig(extract_delegate_segments=False)
    )

    model_stem = os.path.basename(os.path.splitext(args.model_name)[0])
    output_name = (
        f"{model_stem}_arm_delegate_{args.target}"
        if args.delegate
        else f"{model_stem}_arm_{args.target}"
    )
    output_file_name = f"{output_name}.pte"

    if args.output is not None:
        if args.output.endswith(".pte"):
            output_file_name = args.output
        else:
            output_file_name = os.path.join(args.output, output_file_name)

    save_pte_program(exec_prog, output_file_name)
    print(f"PTE file saved as {output_file_name}")


if __name__ == "__main__":
    main()
