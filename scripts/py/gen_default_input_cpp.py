#  SPDX-FileCopyrightText:  Copyright 2021, 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
Utility script to generate the minimum InputFiles.hpp and cpp files required by an application.
"""
from argparse import ArgumentParser
from pathlib import Path

from jinja2 import Environment, FileSystemLoader

from gen_utils import GenUtils

parser = ArgumentParser()

# pylint: disable=duplicate-code
parser.add_argument(
    "--header_folder_path",
    type=str,
    help="path to header folder to be generated."
)

parser.add_argument(
    "--license_template",
    type=str,
    help="Header template file",
    default="header_template.txt"

)

parsed_args = parser.parse_args()

env = Environment(loader=FileSystemLoader(Path(__file__).parent / 'templates'),
                  trim_blocks=True,
                  lstrip_blocks=True)


# pylint: enable=duplicate-code
def write_hpp_file(header_file_path, header_template_file):
    """
    Write .hpp file
    @param header_file_path:        Header file path
    @param header_template_file:    Header template file
    """
    print(f"++ Generating {header_file_path}")
    hdr = GenUtils.gen_header(env, header_template_file)
    env \
        .get_template('default.hpp.template') \
        .stream(common_template_header=hdr) \
        .dump(str(header_file_path))


def main(args):
    """
    Generate InputFiles.hpp + .cpp
    @param args:    Parsed args
    """
    header_filename = "InputFiles.hpp"
    header_filepath = Path(args.header_folder_path) / header_filename
    write_hpp_file(header_filepath, args.license_template)


if __name__ == '__main__':
    main(parsed_args)
