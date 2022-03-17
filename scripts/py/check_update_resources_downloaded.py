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
import sys
import hashlib
from argparse import ArgumentParser
from pathlib import Path


def get_md5sum_for_file(filepath: str) -> str:
    """
    Function to calculate md5sum for contents of a given file.

    Parameters:
    ----------
    filepath (string):  Path to the required file.

    Returns:
    -------
    Hex digest represented as string.
    """
    md5_sum = hashlib.md5()
    with open(filepath, mode='rb') as f:
        buf = f.read()
        md5_sum.update(buf)
    return md5_sum.hexdigest()


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

    metadata_file_path = Path(resource_downloaded_dir) / "resources_downloaded_metadata.json"

    if metadata_file_path.is_file():
        with open(metadata_file_path) as metadata_json:

            metadata_dict = json.load(metadata_json)
            md5_key = 'set_up_script_md5sum'
            set_up_script_md5sum_metadata = ''

            if md5_key in metadata_dict.keys():
                set_up_script_md5sum_metadata = metadata_dict["set_up_script_md5sum"]

            set_up_script_md5sum_current = get_md5sum_for_file(set_up_script_path)

            if set_up_script_md5sum_current == set_up_script_md5sum_metadata:
                return 0

            # Return code 1 if the resources need to be refreshed.
            print('Error: hash mismatch!')
            print(f'Metadata: {set_up_script_md5sum_metadata}')
            print(f'Current : {set_up_script_md5sum_current}')
            return 1

    # Return error code 2 if the file doesn't exists.
    print(f'Error: could not find {metadata_file_path}')
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

    # Check validity of script path.
    if not Path(args.setup_script_path).is_file():
        raise ValueError(f'Invalid script path: {args.setup_script_path}')

    # Check the resources are downloaded as expected
    status = check_update_resources_downloaded(
                args.resource_downloaded_dir,
                args.setup_script_path)
    sys.exit(status)
