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
"""Opt-in logging formatters and setup helpers for command-line tools."""
import logging
import sys
import threading
import typing
from argparse import ArgumentTypeError
from dataclasses import dataclass
from pathlib import Path

LOG_FORMAT = "%(asctime)s [%(levelname)s] %(message)s"
THREAD_LOG_FORMAT = "%(asctime)s [%(levelname)s] [%(thread_label)s] %(message)s"
DATE_FORMAT = "%Y-%m-%d %H:%M:%S"
LEVEL_WIDTH = 8
THREAD_LABEL_WIDTH = 12
LOG_LEVELS = {
    "DEBUG": logging.DEBUG,
    "INFO": logging.INFO,
    "WARNING": logging.WARNING,
    "ERROR": logging.ERROR,
    "CRITICAL": logging.CRITICAL,
}


def parse_log_level(value: str) -> int:
    """Parse a case-insensitive logging level name.

    :param value:  User-provided logging level name.
    :return:       Logging module level value.
    :raises ArgumentTypeError:  If the value is not a supported log level.
    """
    normalized_value = value.upper()
    log_level = LOG_LEVELS.get(normalized_value)
    if log_level is None:
        valid_values = ", ".join(sorted(LOG_LEVELS))
        raise ArgumentTypeError(
            f"Invalid log level '{value}'. Valid values: {valid_values}"
        )
    return log_level


def _fit_center(value: str, width: int) -> str:
    """Return a string truncated if needed and centre-padded to a fixed width.

    :param value:  String value to fit.
    :param width:  Target display width.
    :return:       Fitted string.
    """
    if len(value) > width:
        value = value[:width]
    return value.center(width)


@dataclass(frozen=True)
class LoggingOptions:
    """Options used by :func:`configure_logging`."""
    console_level: int = logging.INFO
    file_level: int = logging.DEBUG
    file_mode: str = "w"
    use_colour: typing.Optional[bool] = None
    show_thread: bool = False
    colour_threads: typing.Optional[bool] = None


class ThreadColourAssigner:
    """Assign stable ANSI colours to threads for the lifetime of the process."""

    _COLOURS = (
        "\033[31m",
        "\033[32m",
        "\033[33m",
        "\033[34m",
        "\033[35m",
        "\033[36m",
        "\033[91m",
        "\033[92m",
        "\033[93m",
        "\033[94m",
        "\033[95m",
        "\033[96m",
    )

    def __init__(self):
        """Create an assigner for per-thread colours.
        """
        self._lock = threading.Lock()
        self._thread_colours: typing.Dict[int, str] = {}
        self._next_colour_index = 0
        self._colours = list(self._COLOURS)

    def colour_for_thread(self, thread_id: int) -> str:
        """Return the stable colour assigned to a thread.

        :param thread_id:  Thread identifier from a log record.
        :return:           ANSI colour escape sequence.
        """
        with self._lock:
            colour = self._thread_colours.get(thread_id)
            if colour is None:
                colour = self._colours[self._next_colour_index]
                self._thread_colours[thread_id] = colour
                self._next_colour_index = (
                    self._next_colour_index + 1
                ) % len(self._colours)
            return colour

    def reset(self):
        """Clear all thread colour assignments."""
        with self._lock:
            self._thread_colours.clear()
            self._next_colour_index = 0


class ThreadFormatter(logging.Formatter):
    """Formatter that can add fixed-width components to log records."""

    def format(self, record: logging.LogRecord) -> str:
        """Format a log record with fixed-width components.

        :param record:  Log record to format.
        :return:        Formatted log message.
        """
        original_levelname = record.levelname
        has_original_thread_label = hasattr(record, "thread_label")
        original_thread_label = getattr(record, "thread_label", None)
        record.levelname = _fit_center(record.levelname, LEVEL_WIDTH)
        if not has_original_thread_label:
            record.thread_label = _fit_center(record.threadName, THREAD_LABEL_WIDTH)

        try:
            return super().format(record)
        finally:
            record.levelname = original_levelname
            if not has_original_thread_label:
                delattr(record, "thread_label")
            else:
                record.thread_label = original_thread_label


class ColourFormatter(logging.Formatter):
    """Formatter that colourises log level names and optional thread labels."""

    _RESET = "\033[0m"
    _COLOURS = {
        logging.DEBUG: "\033[36m",
        logging.INFO: "\033[32m",
        logging.WARNING: "\033[33m",
        logging.ERROR: "\033[31m",
        logging.CRITICAL: "\033[1;31m",
    }

    def __init__(
            self,
            *args,
            thread_colour_assigner: typing.Optional[ThreadColourAssigner] = None,
            **kwargs
    ):
        """Create a colour formatter.

        :param thread_colour_assigner:  Optional assigner for per-thread colours.
        """
        super().__init__(*args, **kwargs)
        self._thread_colour_assigner = thread_colour_assigner

    def format(self, record: logging.LogRecord) -> str:
        """Format a log record with ANSI-coloured components.

        :param record:  Log record to format.
        :return:        Formatted log message.
        """
        original_levelname = record.levelname
        has_original_thread_label = hasattr(record, "thread_label")
        original_thread_label = getattr(record, "thread_label", None)
        levelname = _fit_center(record.levelname, LEVEL_WIDTH)
        colour = self._COLOURS.get(record.levelno)
        if colour is not None:
            levelname = f"{colour}{levelname}{self._RESET}"
        record.levelname = levelname
        if self._thread_colour_assigner is not None:
            thread_colour = self._thread_colour_assigner.colour_for_thread(record.thread)
            thread_label = _fit_center(record.threadName, THREAD_LABEL_WIDTH)
            record.thread_label = (
                f"{thread_colour}{thread_label}{self._RESET}"
            )

        try:
            return logging.Formatter.format(self, record)
        finally:
            record.levelname = original_levelname
            if not has_original_thread_label:
                if hasattr(record, "thread_label"):
                    delattr(record, "thread_label")
            else:
                record.thread_label = original_thread_label


def configure_logging(
        log_file: Path,
        options: LoggingOptions = LoggingOptions(),
):
    """Configure root logging with file and console handlers.

    This helper is opt-in. Importing :mod:`mlek_tools` does not configure
    logging, so downstream projects remain free to use their own logging policy.

    :param log_file:  Path to the log file.
    :param options:   Logging behaviour options.
    """
    use_colour = options.use_colour
    if use_colour is None:
        use_colour = sys.stdout.isatty()
    colour_threads = options.colour_threads
    if colour_threads is None:
        colour_threads = use_colour

    log_format = THREAD_LOG_FORMAT if options.show_thread else LOG_FORMAT
    thread_colour_assigner = (
        ThreadColourAssigner() if options.show_thread and colour_threads else None
    )

    root_logger = logging.getLogger()
    root_logger.handlers.clear()
    root_logger.setLevel(min(options.console_level, options.file_level))

    file_handler = logging.FileHandler(log_file, mode=options.file_mode, encoding="utf-8")
    file_handler.setLevel(options.file_level)
    file_handler.setFormatter(ThreadFormatter(log_format, datefmt=DATE_FORMAT))
    root_logger.addHandler(file_handler)

    stream_handler = logging.StreamHandler(sys.stdout)
    stream_handler.setLevel(options.console_level)
    if use_colour:
        stream_formatter = ColourFormatter(
            log_format,
            datefmt=DATE_FORMAT,
            thread_colour_assigner=thread_colour_assigner,
        )
    else:
        stream_formatter = ThreadFormatter(log_format, datefmt=DATE_FORMAT)
    stream_handler.setFormatter(stream_formatter)
    root_logger.addHandler(stream_handler)
