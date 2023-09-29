#!env/bin/python3

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
Utility script to convert a given text file with labels (annotations for an
NN model output vector) into a vector list initialiser. The intention is for
this script to be called as part of the build framework to auto-generate the
cpp file with labels that can be used in the application without modification.
"""
from argparse import ArgumentParser
from pathlib import Path

from jinja2 import Environment, FileSystemLoader

from gen_utils import GenUtils

# pylint: disable=duplicate-code
parser = ArgumentParser()

# Label file path
parser.add_argument(
    "--labels_file",
    type=str,
    help="Path to the label text file",
    required=True
)

# Output file to be generated
parser.add_argument(
    "--source_folder_path",
    type=str,
    help="path to source folder to be generated.",
    required=True
)

parser.add_argument(
    "--header_folder_path",
    type=str,
    help="path to header folder to be generated.",
    required=True
)

parser.add_argument(
    "--output_file_name",
    type=str,
    help="Required output file name",
    required=True
)

# Namespaces
parser.add_argument(
    "--namespaces",
    action='append',
    default=[]
)

# License template
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
def main(args):
    """
    Generate labels .cpp
    @param args:    Parsed args
    """
    # Get the labels from text file
    with open(args.labels_file, "r", encoding="utf8") as f:
        labels = f.read().splitlines()

    # No labels?
    if len(labels) == 0:
        raise ValueError(f"no labels found in {args.label_file}")

    hdr = GenUtils.gen_header(env, args.license_template, Path(args.labels_file).name)

    hpp_filename = Path(args.header_folder_path) / (args.output_file_name + ".hpp")
    env.get_template('Labels.hpp.template').stream(common_template_header=hdr,
                                                   filename=args.output_file_name.upper(),
                                                   namespaces=args.namespaces) \
        .dump(str(hpp_filename))

    cc_filename = Path(args.source_folder_path) / (args.output_file_name + ".cc")
    env.get_template('Labels.cc.template').stream(common_template_header=hdr,
                                                  labels=labels,
                                                  labelsSize=len(labels),
                                                  namespaces=args.namespaces) \
        .dump(str(cc_filename))


if __name__ == '__main__':
    main(parsed_args)
