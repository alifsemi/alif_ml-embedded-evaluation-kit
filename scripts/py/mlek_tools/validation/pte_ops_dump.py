#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2026 Arm Limited and/or
#  its affiliates <open-source-office@arm.com>
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
Dump kernel ops and delegate calls from an ExecuTorch PTE file.

This script is useful for checking which operators are present and
whether the Arm Ethos-U NPU delegate path is being used.
"""

import argparse
import io
import os
import re
import sys
import warnings
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from typing import Optional, Tuple

from mlek_tools.validation.npu_validation import parse_accelerator_config

def _ensure_flatc_on_path() -> None:
    """
    Ensure the active Python environment's bin dir is on PATH for flatc.
    """
    venv_bin = Path(sys.executable).resolve().parent
    if str(venv_bin) not in os.environ.get("PATH", ""):
        os.environ["PATH"] = f"{venv_bin}:{os.environ.get('PATH', '')}"


def _load_pte_program(buf: bytes):
    """
    Deserialize a PTE buffer while silencing import-time warnings/noise.
    """
    _ensure_flatc_on_path()
    with (
        warnings.catch_warnings(),
        redirect_stdout(io.StringIO()),
        redirect_stderr(io.StringIO()),
    ):
        warnings.simplefilter("ignore", UserWarning)
        warnings.simplefilter("ignore", SyntaxWarning)
        try:
            from executorch.exir._serialize import _deserialize_pte_binary  # type: ignore  # pylint: disable=import-error,import-outside-toplevel
        except Exception as exc:  # pylint: disable=broad-except
            print(f"warning: failed to import executorch: {exc}")
            return None
    try:
        program = _deserialize_pte_binary(buf)
        if hasattr(program, "program"):
            return program.program
        return program
    except Exception as exc:  # pylint: disable=broad-except
        print(f"warning: failed to deserialize pte: {exc}")
        return None


def _load_delegate_call_class():
    """
    Load the ExecuTorch DelegateCall class for isinstance checks.
    """
    _ensure_flatc_on_path()
    with (
        warnings.catch_warnings(),
        redirect_stdout(io.StringIO()),
        redirect_stderr(io.StringIO()),
    ):
        warnings.simplefilter("ignore", UserWarning)
        warnings.simplefilter("ignore", SyntaxWarning)
        try:
            from executorch.exir.schema import DelegateCall  # type: ignore  # pylint: disable=import-error,import-outside-toplevel
            return DelegateCall
        except Exception as exc:  # pylint: disable=broad-except
            print(f"warning: failed to import executorch schema: {exc}")
            return None


def _print_ops(plan) -> None:
    """
    Print kernel operators in the execution plan.
    """
    print("++ == Kernel ops ==")
    for i, op in enumerate(plan.operators):
        name = f"{op.name}.{op.overload}" if op.overload else op.name
        print(f"++ {i:3d}: {name}")


def _print_delegate_calls(plan, delegate_call_cls) -> None:
    """
    Print delegate calls in the execution plan.
    """
    print("++ == Delegate calls ==")
    delegate_calls = 0
    for i, instr in enumerate(plan.chains[0].instructions):
        args = instr.instr_args
        if delegate_call_cls is not None and isinstance(args, delegate_call_cls):
            backend = plan.delegates[args.delegate_index]
            print(f"++ {i:3d}: delegate backend id = {backend.id}")
            delegate_calls += 1
    if delegate_calls == 0:
        print("++ (none)")


def _print_delegate_payloads(plan) -> None:
    """
    Print delegate payload references in the execution plan.
    """
    print("++ == Delegate payload refs ==")
    if not plan.delegates:
        print("++ (none)")
        return
    for i, d in enumerate(plan.delegates):
        print(
            "++ "
            f"{i:3d}: id={d.id} processed={{location={d.processed.location}, "
            f"index={d.processed.index}}}"
        )


def _print_non_const_buffers(plan) -> None:
    """
    Print non-constant buffer sizes (memory planner output) if present.
    """
    print("++ == Non-const buffer sizes ==")
    sizes_attr = getattr(plan, "non_const_buffer_sizes", None)
    sizes = sizes_attr() if callable(sizes_attr) else sizes_attr
    if sizes is None:
        print("++ (none)")
        return

    total = 0
    if hasattr(sizes, "size") and hasattr(sizes, "Get"):
        for i in range(sizes.size()):
            size = int(sizes.Get(i))
            total += size
            print(f"++   {i}: {size} bytes")
    else:
        for i, size in enumerate(sizes):
            size = int(size)
            total += size
            print(f"++   {i}: {size} bytes")
    print(f"++   total: {total} bytes")


def print_vela_flags(mem_mode: Optional[str], config: Optional[str]) -> None:
    """
    Print Vela flags if available.
    """
    if mem_mode or config:
        npu_id, macs = parse_accelerator_config(config)
        print(
            "++ PTE Vela flags: "
            f"memory_mode={mem_mode or 'unknown'}, "
            f"accelerator_config={config or 'unknown'}"
        )
        if npu_id or macs:
            print(
                "++ PTE accelerator details: "
                f"npu={npu_id or 'unknown'}, macs={macs or 'unknown'}"
            )


def dump_pte_ops(pte_path: Path) -> int:
    """
    Dump kernel ops and delegate calls for a PTE file.

    Returns:
        0 on success, non-zero on errors.
    """
    if not pte_path.is_file():
        print(f"error: file not found: {pte_path}")
        return 2

    buf = pte_path.read_bytes()
    program = _load_pte_program(buf)
    if program is None:
        return 4
    plan = program.execution_plan[0]

    vela_mem_mode, vela_config = extract_vela_args(buf)
    print_vela_flags(vela_mem_mode, vela_config)

    delegate_call_cls = _load_delegate_call_class()
    _print_ops(plan)
    _print_delegate_calls(plan, delegate_call_cls)
    _print_delegate_payloads(plan)
    _print_non_const_buffers(plan)

    return 0


def extract_vela_args(buf: bytes) -> Tuple[Optional[str], Optional[str]]:
    """
    Extract Vela command line flags embedded in the PTE file.

    Returns:
        (memory_mode, accelerator_config)
    """
    # PTEs embed ASCII flags; latin1 decode preserves byte values safely.
    text = buf.decode("latin1", errors="ignore")
    mem_mode = None
    config = None

    mem_match = re.search(r"--memory-mode=([A-Za-z_]+)", text)
    if mem_match:
        mem_mode = mem_match.group(1)

    config_match = re.search(r"--accelerator-config=([A-Za-z0-9_-]+)", text)
    if config_match:
        config = config_match.group(1)

    return mem_mode, config


def has_ethos_u_delegate(buf: bytes) -> bool:
    """
    Detect Arm Ethos-U NPU usage in a PTE file.

    Prefer ExecuTorch delegate metadata; fall back to string markers.
    """
    program = _load_pte_program(buf)
    if program is None:
        return False
    try:
        plan = program.execution_plan[0]
        for delegate in plan.delegates:
            if "ethos" in delegate.id.lower():
                return True
    except Exception:  # pylint: disable=broad-except
        pass
    for marker in (b"EthosUBackend", b"ethos-u"):
        if marker in buf:
            return True
    return False


def main() -> int:
    """
    CLI entrypoint.

    Returns:
        0 on success, non-zero on errors.
    """
    parser = argparse.ArgumentParser(
        description="List kernel ops and delegate calls in an ExecuTorch PTE file."
    )
    parser.add_argument(
        "pte_path",
        type=Path,
        help="Path to the .pte model file",
    )
    args = parser.parse_args()

    return dump_pte_ops(args.pte_path)


if __name__ == "__main__":
    raise SystemExit(main())
