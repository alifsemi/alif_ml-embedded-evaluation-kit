#  SPDX-FileCopyrightText:  Copyright 2025-2026 Arm Limited and/or its
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
"""ExecuTorch model optimization via the MLEK lowering script."""
import concurrent.futures
import itertools
import logging
import os
import subprocess
import threading
import typing
from dataclasses import dataclass, replace
from pathlib import Path

from mlek_tools.config.executorch import ExecuTorchConfig
from mlek_tools.config.npu import NpuConfig
from mlek_tools.config.optimizer import OptimizerConfig
from mlek_tools.optimize.metadata import (
    add_lowering_options_metadata,
    build_optimization_metadata,
    output_matches_optimization_metadata,
    write_optimization_metadata,
)
from mlek_tools.setup.python_venv import PythonEnv
from mlek_tools.setup.util import call_command
from mlek_tools.use_case.model import UseCase

# Path to the MLEK-owned ExecuTorch Arm backend lowering script.
# Used when no custom lowering_script is set on an ExecuTorchResource.
_MLEK_LOWERING_SCRIPT = (
    Path(__file__).parent.parent
    / "lowering"
    / "executorch_arm_backend_lowering.py"
)
_TOSA_LOWERING_MODULE = "-m backends.arm.scripts.aot_arm_compiler"

# Locks of the ExecuTorch models, see _get_executorch_model_lock.
_EXECUTORCH_MODEL_LOCKS: typing.Dict[str, threading.Lock] = {}
_EXECUTORCH_MODEL_LOCKS_GUARD = threading.Lock()


def _get_executorch_model_lock(model_name: str) -> threading.Lock:
    """Get the lock of an ExecuTorch model, used to run only one optimisation of a model at a time.

    The optimisations of the same model for different NPU configs must not run at the same time:
    the example models of ExecuTorch download their sample inputs (e.g. dog.jpg of mv2) to the
    current directory, and processes writing the same file at the same time leave a truncated
    file for the others.

    :param model_name:  Name of the model.
    :return:            The lock of the model.
    """
    with _EXECUTORCH_MODEL_LOCKS_GUARD:
        return _EXECUTORCH_MODEL_LOCKS.setdefault(model_name, threading.Lock())


@dataclass(frozen=True)
class ExecuTorchOptimizationContext:
    """
    Shared state for ExecuTorch optimization.
    """
    python_env: PythonEnv
    executorch_config: ExecuTorchConfig
    optimizer_config: OptimizerConfig
    output_dir: Path
    vela_version: str


@dataclass(frozen=True)
class EthosULoweringCommand:
    """
    Parameters used to invoke MLEK's Ethos-U lowering script.
    """
    python_exe: Path
    lowering_entrypoint: typing.Union[str, Path]
    input_model: typing.Union[str, Path]
    npu_config: NpuConfig
    optimizer_config: OptimizerConfig
    output_dir: Path
    vela_flags: typing.List[str]

    @property
    def command(self) -> str:
        """
        Build the command line for MLEK's Ethos-U lowering script.

        :return: Shell command string.
        """
        extra_flags_args = " ".join(
            f'--extra-vela-flag="{flag}"'
            for flag in self.vela_flags
        )
        config_arg = (
            f'--config="{self.optimizer_config.vela_config_file}"'
            if self.optimizer_config.vela_config_file else ""
        )
        direct_drive_arg = (
            " --direct-drive"
            if self.optimizer_config.direct_drive else ""
        )
        return (
            f'"{self.python_exe}" "{self.lowering_entrypoint}"'
            f' --model_name="{self.input_model}"'
            f" --target={self.npu_config.config_name}"
            f" --system_config={self.npu_config.system_config}"
            f" --memory_mode={self.npu_config.memory_mode}"
            f" {config_arg}"
            f" {extra_flags_args}"
            f" --delegate --quantize"
            f"{direct_drive_arg}"
            f' --output "{self.output_dir}"'
        )


def optimize_executorch_model(
        model_name: typing.Union[str, Path],
        npu_config: typing.Optional[NpuConfig],
        context: ExecuTorchOptimizationContext,
        lowering_script: typing.Optional[Path] = None,
) -> bool:
    """Generate an optimized ``.pte`` file for ExecuTorch.

    For Ethos-U targets the MLEK lowering script is used by default so that
    :class:`~mlek_tools.config.optimizer.OptimizerConfig` settings (Vela config
    file, arena cache size, COP format) are respected.  For TOSA (``npu_config``
    is ``None``) ``aot_arm_compiler`` is called directly.

    :param model_name:          Model name, path to a ``.py`` script, or a
                                ``.pt``/``.pt2`` checkpoint path.
    :param npu_config:          NPU hardware descriptor; ``None`` produces a TOSA
                                ``.pte`` for native host.
    :param context:             Shared ExecuTorch optimization context.
    :param lowering_script:     Optional custom lowering script override.
    :return:                    ``True`` if optimization was skipped.
    """
    model_res = None
    if str(model_name).endswith('.py'):
        model_res = model_name
        model_name = Path(model_res).name.split('.')[0]
    elif str(model_name).endswith('.pt') or str(model_name).endswith('.pt2'):
        model_res = context.output_dir / model_name
        model_name = Path(model_res).name.split('.')[0]

    if npu_config is not None:
        optimized_model_name = (
            f"{model_name}_{npu_config.delegate_target_suffix}.pte"
        )
        vela_flags = context.optimizer_config.to_vela_extra_flags(npu_config.memory_mode)
        lowering_entrypoint: typing.Union[str, Path] = (
            lowering_script if lowering_script is not None else _MLEK_LOWERING_SCRIPT
        )
    else:
        optimized_model_name = f"{model_name}_arm_TOSA-1.0+FP.pte"
        vela_flags = []
        lowering_entrypoint = (
            lowering_script if lowering_script is not None else _TOSA_LOWERING_MODULE
        )

    input_model = model_res if model_res is not None else model_name
    optimized_model_path = context.output_dir / optimized_model_name
    lowering_options = (
        {"direct_drive": context.optimizer_config.direct_drive}
        if npu_config is not None else None
    )
    optimization_metadata = build_optimization_metadata(
        input_model,
        npu_config,
        context.vela_version,
        vela_flags,
        lowering_entrypoint,
    )
    add_lowering_options_metadata(optimization_metadata, lowering_options)
    logging.info("Looking for %s", optimized_model_path)
    if output_matches_optimization_metadata(optimized_model_path, optimization_metadata):
        logging.info("File %s exists, skipping optimisation.", optimized_model_path)
        return True

    if optimized_model_path.is_file():
        optimized_model_path.unlink()

    python_exe = context.python_env.python
    venv_env = os.environ.copy()
    venv_env["PATH"] = str(context.python_env.bin_dir) + os.pathsep + venv_env.get("PATH", "")

    if npu_config is not None:
        # Ethos-U path: use MLEK lowering script for full OptimizerConfig control.
        command = EthosULoweringCommand(
            python_exe=python_exe,
            lowering_entrypoint=lowering_entrypoint,
            input_model=input_model,
            npu_config=npu_config,
            optimizer_config=context.optimizer_config,
            output_dir=context.output_dir,
            vela_flags=vela_flags,
        )
        target_name = npu_config.config_name
        with _get_executorch_model_lock(str(model_name)):
            logging.info("Optimising ExecuTorch model %s (%s)", model_name, target_name)
            try:
                # The output is buffered and logged when the command has finished so that the
                # output of the models optimised in parallel does not get mixed and the error
                # of a failed one is found next to the failure.
                call_command(
                    command=command.command,
                    cwd=context.executorch_config.executorch_path,
                    buffer_logs=True,
                    capture_output=True,
                    env=venv_env,
                )
            except subprocess.CalledProcessError:
                logging.error(
                    "Optimising ExecuTorch model %s (%s) failed, see the output above.",
                    model_name, target_name,
                )
                raise
    elif lowering_script is not None:
        call_command(
            command=(
                f'"{python_exe}" "{lowering_script}"'
                f' --model_name="{input_model}"'
                " --target=TOSA-1.0+FP"
                f' --output "{context.output_dir}"'
            ),
            cwd=context.executorch_config.executorch_path,
            buffer_logs=False,
            capture_output=False,
            env=venv_env,
        )
    else:
        # TOSA path: aot_arm_compiler handles this natively.
        call_command(
            command=(
                f'"{python_exe}" {_TOSA_LOWERING_MODULE}'
                f' --model_name="{input_model}"'
                f" --target=TOSA-1.0+FP"
                f' --output "{context.output_dir}"'
            ),
            cwd=context.executorch_config.executorch_path,
            buffer_logs=False,
            capture_output=False,
            env=venv_env,
        )

    write_optimization_metadata(optimized_model_path, optimization_metadata)

    return False


def submit_executorch_optimizations(
        executor: concurrent.futures.ThreadPoolExecutor,
        use_cases: typing.List[UseCase],
        npu_configs: typing.List[typing.Optional[NpuConfig]],
        context: ExecuTorchOptimizationContext,
) -> typing.List[concurrent.futures.Future]:
    """Submit ExecuTorch lowering tasks to *executor* for all resource × NPU-config pairs.

    :return: List of futures; each resolves to ``True`` if the run was skipped.
    """
    return list(itertools.chain(*(
        [
            executor.submit(
                optimize_executorch_model,
                executorch_resource.model_name,
                npu_config,
                replace(context, output_dir=context.output_dir / use_case.name),
                executorch_resource.lowering_script,
            )
            for executorch_resource in use_case.executorch_resources
            if not (
                npu_config is not None
                and npu_config.processor_id
                in executorch_resource.excluded_npu_processor_ids
            )
        ]
        for use_case, npu_config
        in itertools.product(use_cases, npu_configs)
    )))
