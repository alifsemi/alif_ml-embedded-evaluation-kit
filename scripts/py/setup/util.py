#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2025 Arm Limited and/or its
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
Utility functions for setup
"""
import logging
import os
import shutil
import subprocess
import typing
import urllib
import urllib.request
from pathlib import Path
from urllib.error import URLError


def download_file(url: str, dest: Path) -> Path:
    """
    Download a file

    @param url:     The URL of the file to download
    @param dest:    The destination of downloaded file
    """
    try:
        with urllib.request.urlopen(url) as g:
            with open(dest, "b+w") as f:
                f.write(g.read())
                logging.info("- Downloaded %s to %s.", url, dest)
    except URLError:
        logging.error("URLError while downloading %s.", url)
        raise
    return dest


def call_command(
        command: str,
        verbose: bool = True,
        cwd: typing.Optional[Path] = None
) -> str:
    """
    Helpers function that call subprocess and return the output.

    Parameters:
    ----------
    command (string):   Specifies the command to run.
    verbose (bool):     When True, log the command before running.
    cwd (Path):         Set the working directory in which to run the command.
    """
    if verbose:
        logging.info(command)
    try:
        proc = subprocess.run(
            command,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            shell=True,
            cwd=cwd
        )
        log = proc.stdout.decode("utf-8")
        logging.info(log)
        return log
    except subprocess.CalledProcessError as err:
        log = err.stdout.decode("utf-8")
        logging.error(log)
        raise err


def remove_tree_dir(dir_path: Path):
    """
    Delete and re-create a directory

    Parameters
    ----------
    dir_path    : The directory path
    """
    try:
        # Remove the full directory.
        shutil.rmtree(dir_path)
        # Re-create an empty one.
        os.mkdir(dir_path)
    except OSError:
        logging.error("Failed to delete %s.", dir_path)
