#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2022-2023, 2026 Arm Limited and/or
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
Contains methods to check if the downloaded resources need to be refreshed
"""
import hashlib
import json
import sys
from argparse import ArgumentParser
from pathlib import Path


def get_sha256sum_for_file(filepath: Path) -> str:
    """Compute the SHA-256 hex digest of a file's contents.

    :param filepath:  Path to the file.
    :return:          Hex string of the SHA-256 digest.
    """
    sha256 = hashlib.sha256()
    with open(filepath, mode='rb') as f:
        for chunk in iter(lambda: f.read(8192), b""):
            sha256.update(chunk)
    return sha256.hexdigest()


def check_update_resources_downloaded(
        resource_downloaded_dir: str, set_up_script_path: str
) -> int:
    """Check if the downloaded resources need to be refreshed.

    :param resource_downloaded_dir:  Path to resources_downloaded folder.
    :param set_up_script_path:       Path to set_up_default_resources.py file.
    :return:                         Status code for the CMake caller.
    """

    metadata_file_path = Path(resource_downloaded_dir) / "resources_downloaded_metadata.json"

    if metadata_file_path.is_file():
        with open(metadata_file_path, encoding="utf8") as metadata_json:
            metadata_dict = json.load(metadata_json)

        sha256_key = 'set_up_script_sha256sum'
        set_up_script_sha256sum_metadata = ''

        if sha256_key in metadata_dict:
            set_up_script_sha256sum_metadata = metadata_dict[sha256_key]

        set_up_script_sha256sum_current = get_sha256sum_for_file(Path(set_up_script_path))

        if set_up_script_sha256sum_current == set_up_script_sha256sum_metadata:
            return 0

        # Return code 1 if the resources need to be refreshed.
        print('Error: hash mismatch!')
        print(f'Metadata: {set_up_script_sha256sum_metadata}')
        print(f'Current : {set_up_script_sha256sum_current}')
        return 1

    # Return error code 2 if the file doesn't exist.
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
    STATUS = check_update_resources_downloaded(
        args.resource_downloaded_dir,
        args.setup_script_path
    )
    sys.exit(STATUS)
