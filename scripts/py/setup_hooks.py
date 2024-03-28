#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2022-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
Adds the git hooks script into the appropriate location
"""
import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

HOOKS_SCRIPT = "git_pre_push_hooks.sh"


def set_hooks_dir(hooks_dir: str):
    """
    Set the hooks path in the git configuration
    @param hooks_dir:   The hooks directory
    """
    command = f'git config core.hooksPath {hooks_dir}'
    with subprocess.Popen(command.split(), stdout=subprocess.PIPE) as process:
        process.communicate()
        return_code = process.returncode

    if return_code != 0:
        raise RuntimeError(f"Could not configure git hooks path, exited with code {return_code}")


def add_pre_push_hooks(hooks_dir: str):
    """
    Copies the git hooks scripts into the specified location
    @param hooks_dir:   The specified git hooks directory
    """
    pre_push = "pre-push"
    file_path = os.path.join(hooks_dir, pre_push)
    file_exists = os.path.exists(file_path)
    if file_exists:
        os.remove(file_path)

    script_path = Path(__file__).resolve().parent / HOOKS_SCRIPT
    shutil.copy(script_path, hooks_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("git_hooks_path")
    args = parser.parse_args()

    if not os.path.exists(args.git_hooks_path):
        print('Error! The Git hooks directory you supplied does not exist.')
        sys.exit()

    add_pre_push_hooks(args.git_hooks_path)
    set_hooks_dir(args.git_hooks_path)
