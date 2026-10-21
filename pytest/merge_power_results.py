#/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

"""Merges the markdown files written by the power tests into one table.

Every power test run writes its own file (see ``--power_results`` in ``conftest.py``) using the
layout of :func:`power_report.format_table`: three header rows, a board row (``**<--power_board>**``)
and a row pair per use case. The merged file has the header rows and the board row only once, at
the top, followed by the use case rows of all the input files in the order of the input files.

A board row that differs from the previous one (the files are from different boards) is kept, so
that every row still belongs to the right board.

Example::

    python merge_power_results.py -o power_results_all.md power_results_*.md
"""

import argparse
import glob
import os
import sys
from typing import List, Optional, Sequence

HEADER_ROWS = 3


def _cells(line: str) -> List[str]:
    """Split a markdown table row to its cells.

    :param line:    One row of a markdown table.
    :returns:       Stripped cell texts without the outer pipes.
    """
    return [cell.strip() for cell in line.strip().strip('|').split('|')]


def is_board_row(line: str) -> bool:
    """Tell if a table row is the board row, the one given with ``--power_board``.

    :param line:    One row of a markdown table.
    :returns:       True if the first cell is bold and all the other cells are empty.
    """
    cells = _cells(line)
    first = cells[0]
    return (len(first) > 4 and first.startswith('**') and first.endswith('**')
            and not any(cells[1:]))


def merge_tables(texts: Sequence[str]) -> str:
    """Merge the power result tables.

    :param texts:       Contents of the result files.
    :returns:           The merged table as markdown text.
    :raises ValueError: If there are no tables to merge.
    """
    header: List[str] = []
    body: List[str] = []
    last_board = ''
    for text in texts:
        lines = [line.rstrip() for line in text.splitlines() if line.strip()]
        if len(lines) <= HEADER_ROWS:
            continue
        if not header:
            header = lines[:HEADER_ROWS]
        elif lines[:HEADER_ROWS] != header:
            print('Warning: table header differs from the first file, using the first one',
                  file=sys.stderr)
        for line in lines[HEADER_ROWS:]:
            if is_board_row(line):
                if line == last_board:
                    continue
                last_board = line
            body.append(line)
    if not header:
        raise ValueError('No power result tables to merge')
    return '\n'.join(header + body) + '\n'


def expand_inputs(patterns: Sequence[str], output: str) -> List[str]:
    """Expand the wildcards of the input arguments.

    :param patterns:    File names or wildcard patterns, a pattern is expanded in sorted order.
    :param output:      Output file, left out if a pattern matches it.
    :returns:           The input files in the order they are merged.
    """
    skip = os.path.abspath(output)
    paths: List[str] = []
    for pattern in patterns:
        matches = sorted(glob.glob(pattern)) or [pattern]
        for path in matches:
            if os.path.abspath(path) != skip and path not in paths:
                paths.append(path)
    return paths


def merge_files(inputs: Sequence[str], output: str):
    """Merge the power result files to one file.

    :param inputs:  Files to merge.
    :param output:  Output file.
    """
    texts = []
    for path in inputs:
        with open(path, encoding='utf-8') as result_file:
            texts.append(result_file.read())
    merged = merge_tables(texts)
    with open(output, 'w', encoding='utf-8') as merged_file:
        merged_file.write(merged)


def main(argv: Optional[Sequence[str]] = None) -> int:
    """Run the merge from the command line.

    :param argv:    Command line arguments, ``sys.argv`` by default.
    :returns:       Process exit code.
    """
    parser = argparse.ArgumentParser(description='Merge the markdown files of the power tests.')
    parser.add_argument('inputs', nargs='+', help='Power result files or wildcard patterns.')
    parser.add_argument('-o', '--output', default='power_results_merged.md',
                        help='Merged file. Default is power_results_merged.md.')
    args = parser.parse_args(argv)

    inputs = expand_inputs(args.inputs, args.output)
    missing = [path for path in inputs if not os.path.isfile(path)]
    if missing:
        print(f'Input file not found: {", ".join(missing)}', file=sys.stderr)
        return 1
    try:
        merge_files(inputs, args.output)
    except ValueError as error:
        print(error, file=sys.stderr)
        return 1
    print(f'Merged {len(inputs)} files to {args.output}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
