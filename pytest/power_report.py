#/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

"""Collects the results of the power tests and writes them to a markdown table.

Every power test records its :class:`jscope_helper.RoundResult` with :func:`record`. When the
pytest session ends, :func:`write_report` writes one table with a row pair (idle + measured) per
use case. See ``power_example.md`` for the layout.

The delta columns are calculated against the base (idle) phase of the same round::

    dmA = mA - idle mA,    dmW = mW - idle mW,    dmJ = dmW * ms / 1000
"""

import dataclasses
import re
from typing import Dict, List, Sequence

from jscope_helper import PhaseStats, RoundResult

MEASURED_PHASES = ('preprocessing', 'inference', 'postprocessing')
PHASE_TITLES = {'preprocessing': 'Preprocess', 'inference': 'Inference',
                'postprocessing': 'Postprocess'}
TOTAL_COLUMNS = ('ms', 'mA', 'mW', 'mJ', 'ΔmA', 'ΔmW', 'ΔmJ')
PHASE_COLUMNS = ('ms', 'mA', 'mW', 'V', 'mJ', 'ΔmA', 'ΔmW', 'ΔmJ')


@dataclasses.dataclass(frozen=True)
class PowerEntry:
    """Result of one power test."""
    name: str
    result: RoundResult


_entries: List[PowerEntry] = []


def record(name: str, result: RoundResult):
    """Record the result of one power test, a later result with the same name replaces it.

    :param name:    Row label, for example ``alif_kws (HE_U55)``.
    :param result:  Measurement result of one round.
    """
    _entries[:] = [e for e in _entries if e.name != name] + [PowerEntry(name, result)]


def entries() -> List[PowerEntry]:
    """Get the recorded results.

    :returns:   Recorded results in the order they were recorded.
    """
    return list(_entries)


def clear():
    """Forget all recorded results."""
    _entries.clear()


def use_case_name(test_name: str) -> str:
    """Derive the use case name from a power test name.

    :param test_name:   Test function name, for example ``test_power_kws[param]``.
    :returns:           Use case name, for example ``alif_kws``.
    """
    base = re.sub(r'\[.*\]$', '', test_name)
    base = re.sub(r'^test_', '', base)
    base = re.sub(r'^power_', '', base)
    base = re.sub(r'_power$', '', base)
    return f'alif_{base}'


def _fmt(value: float, decimals: int) -> str:
    return f'{value:.{decimals}f}'


def _total_values(result: RoundResult) -> Dict[str, float]:
    """Total (preprocessing + inference + postprocessing) values of a round, base excluded."""
    phases = [result.phases[name] for name in MEASURED_PHASES]
    time_ms = sum(p.time_ms for p in phases)
    energy_mj = sum(p.energy_mj for p in phases)
    current_ma = sum(p.current_ma.mean * p.time_ms for p in phases) / time_ms
    power_mw = energy_mj / time_ms * 1000.0
    return {'ms': time_ms, 'mA': current_ma, 'mW': power_mw, 'mJ': energy_mj}


def _deltas(current_ma: float, power_mw: float, time_ms: float,
            base: PhaseStats) -> List[str]:
    """Delta columns against the base level, the energy delta is for the duration time_ms."""
    delta_mw = power_mw - base.power_mw.mean
    return [_fmt(current_ma - base.current_ma.mean, 2), _fmt(delta_mw, 2),
            _fmt(delta_mw * time_ms / 1000.0, 4)]


def _total_cells(result: RoundResult) -> List[str]:
    total = _total_values(result)
    return [_fmt(total['ms'], 3), _fmt(total['mA'], 2), _fmt(total['mW'], 2),
            _fmt(total['mJ'], 4)] + _deltas(total['mA'], total['mW'], total['ms'],
                                           result.phases['base'])


def _phase_cells(phase: PhaseStats, base: PhaseStats) -> List[str]:
    return [_fmt(phase.time_ms, 3), _fmt(phase.current_ma.mean, 2),
            _fmt(phase.power_mw.mean, 2), _fmt(phase.voltage_v.mean, 2),
            _fmt(phase.energy_mj, 4)] + _deltas(phase.current_ma.mean, phase.power_mw.mean,
                                                phase.time_ms, base)


def _row(cells: Sequence[str]) -> str:
    return '| ' + ' | '.join(cells) + ' |'


def format_table(board: str, items: Sequence[PowerEntry]) -> str:
    """Format the results as a markdown table.

    :param board:   Board / SE version description for the group heading row.
    :param items:   Results to include, one row pair per item.
    :returns:       The table as markdown text.
    """
    columns = (len(TOTAL_COLUMNS) + len(PHASE_COLUMNS) * len(MEASURED_PHASES))
    group_row = ['Board / SE Version / Use Case', 'Total'] + [''] * (len(TOTAL_COLUMNS) - 1)
    for name in MEASURED_PHASES:
        group_row += [PHASE_TITLES[name]] + [''] * (len(PHASE_COLUMNS) - 1)
    unit_row = [''] + list(TOTAL_COLUMNS) + list(PHASE_COLUMNS) * len(MEASURED_PHASES)
    align_row = ['-' * 32] + ['-------:'] * columns

    lines = [_row(group_row), _row(align_row), _row(unit_row),
             _row([f'**{board}**'] + [''] * columns)]
    for item in items:
        base = item.result.phases['base']
        idle = [f'{item.name} idle', '', _fmt(base.current_ma.mean, 2),
                _fmt(base.power_mw.mean, 2)] + [''] * (columns - 3)
        measured = [item.name] + _total_cells(item.result)
        for name in MEASURED_PHASES:
            measured += _phase_cells(item.result.phases[name], base)
        lines += [_row(idle), _row(measured)]
    return '\n'.join(lines) + '\n'


def write_report(path: str, board: str, core: str = '') -> bool:
    """Write the recorded results to a markdown file.

    :param path:    Output file.
    :param board:   Board / SE version description for the group heading row.
    :param core:    Core / NPU description appended to every row label, for example ``HE_U55``.
    :returns:       False if there were no results and nothing was written.
    """
    if not _entries:
        return False
    suffix = f' ({core})' if core else ''
    items = [PowerEntry(e.name + suffix, e.result) for e in _entries]
    with open(path, 'w', encoding='utf-8') as report:
        report.write(format_table(board, items))
    return True
