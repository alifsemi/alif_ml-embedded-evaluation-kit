#!/usr/bin/env python3
#  Copyright (c) 2022 Arm Limited. All rights reserved.
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
import json
import os
import subprocess
import sys
from argparse import ArgumentParser


def check_update_resources_downloaded(
    resource_downloaded_dir: str, set_up_script_path: str
):
    """
    Function that check if the resources downloaded need to be refreshed.

    Parameters:
    ----------
    resource_downloaded_dir (string):  Specifies the path to resources_downloaded folder.
    set_up_script_path (string):       Specifies the path to set_up_default_resources.py file.
    """

    metadata_file_path = os.path.join(
        resource_downloaded_dir, "resources_downloaded_metadata.json"
    )

    if os.path.isfile(metadata_file_path):
        with open(metadata_file_path) as metadata_json:

            metadata_dict = json.load(metadata_json)
            set_up_script_hash = metadata_dict["set_up_script_hash"]
            command = f"git log -1 --pretty=tformat:%H  {set_up_script_path}"

            proc = subprocess.run(
                command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True
            )
            git_commit_hash = proc.stdout.decode("utf-8").strip("\n")
            proc.check_returncode()

            if set_up_script_hash == git_commit_hash:
                return 0
            # Return code 1 if the resources need to be refreshed.
            return 1
    # Return error code 2 if the file doesn't exists.
    return 2


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument(
        "--resource_downloaded_dir",
        help="Resources downloaded directory.",
        type=str,
        required=True)
    parser.add_argument(
        "--setup_script_path",
        help="Path to set_up_default_resources.py.",
        type=str,
        required=True)
    args = parser.parse_args()

    # Check validity of script path
    if not os.path.isfile(args.setup_script_path):
        raise ValueError(f'Invalid script path: {args.setup_script_path}')

    # Check if the repo root directory is a git repository
    root_file_dir = os.path.dirname(os.path.abspath(args.setup_script_path))
    is_git_repo = os.path.exists(os.path.join(root_file_dir, ".git"))

    # if we have a git repo then check the resources are downloaded,
    # otherwise it's considered a prerequisite to have run
    # the set_up_default_resources.py
    status = (
        check_update_resources_downloaded(
            args.resource_downloaded_dir, args.setup_script_path
        )
        if is_git_repo
        else 0
    )
    sys.exit(status)
