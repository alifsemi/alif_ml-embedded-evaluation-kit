#  SPDX-FileCopyrightText:  Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
Utility script to convert a set of pairs of npy files in a given location into
corresponding cpp files and a single hpp file referencing the vectors
from the cpp files.
"""
import math
import typing
from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path

import numpy as np
from jinja2 import Environment, FileSystemLoader

from gen_utils import GenUtils

# pylint: disable=duplicate-code
parser = ArgumentParser()

parser.add_argument(
    "--data_folder_path",
    type=str,
    help="path to ifm-ofm npy folder to convert."
)

parser.add_argument(
    "--source_folder_path",
    type=str,
    help="path to source folder to be generated."
)

parser.add_argument(
    "--header_folder_path",
    type=str,
    help="path to header folder to be generated."
)

parser.add_argument(
    "--usecase",
    type=str,
    default="",
    help="Test data file suffix."
)

parser.add_argument(
    "--namespaces",
    action='append',
    default=[]
)

parser.add_argument(
    "--license_template",
    type=str,
    help="Header template file",
    default="header_template.txt"
)

parser.add_argument(
    "-v",
    "--verbosity",
    action="store_true"
)

parsed_args = parser.parse_args()

env = Environment(loader=FileSystemLoader(Path(__file__).parent / 'templates'),
                  trim_blocks=True,
                  lstrip_blocks=True)


# pylint: enable=duplicate-code
@dataclass
class TestDataParams:
    """
    Template params for TestData.hpp + TestData.ccc
    """
    ifm_count: int
    ofm_count: int
    ifm_var_names: typing.List[str]
    ifm_var_sizes: typing.List[int]
    ofm_var_names: typing.List[str]
    ofm_var_sizes: typing.List[int]
    data_type: str


@dataclass
class IofmParams:
    """
    Template params for iofmdata.cc
    """
    var_name: str
    data_type: str


def write_hpp_file(
        template_params: TestDataParams,
        header_filename: str,
        cc_file_path: str,
        header_template_file: str
):
    """
    Write TestData.hpp and TestData.cc

    @param template_params:         Template parameters
    @param header_filename:         TestData.hpp path
    @param cc_file_path:            TestData.cc path
    @param header_template_file:    Header template file name
    """
    header_file_path = Path(parsed_args.header_folder_path) / header_filename

    print(f"++ Generating {header_file_path}")
    hdr = GenUtils.gen_header(env, header_template_file)
    env \
        .get_template('TestData.hpp.template') \
        .stream(common_template_header=hdr,
                ifm_count=template_params.ifm_count,
                ofm_count=template_params.ofm_count,
                ifm_var_names=template_params.ifm_var_names,
                ifm_var_sizes=template_params.ifm_var_sizes,
                ofm_var_names=template_params.ofm_var_names,
                ofm_var_sizes=template_params.ofm_var_sizes,
                data_type=template_params.data_type,
                namespaces=parsed_args.namespaces) \
        .dump(str(header_file_path))

    env \
        .get_template('TestData.cc.template') \
        .stream(common_template_header=hdr,
                include_h=header_filename,
                ifm_var_names=template_params.ifm_var_names,
                ofm_var_names=template_params.ofm_var_names,
                data_type=template_params.data_type,
                namespaces=parsed_args.namespaces) \
        .dump(str(cc_file_path))


def write_individual_cc_file(
        template_params: IofmParams,
        header_filename: str,
        filename: str,
        cc_filename: Path,
        header_template_file: str
):
    """
    Write iofmdata.cc

    @param template_params:         Template parameters
    @param header_filename:         Header file name
    @param filename:                Input file name
    @param cc_filename:             iofmdata.cc file name
    @param header_template_file:    Header template file name
    """
    print(f"++ Converting {filename} to {cc_filename.name}")
    hdr = GenUtils.gen_header(env, header_template_file, filename)

    # Convert the image and write it to the cc file
    fm_data = (np.load(Path(parsed_args.data_folder_path) / filename)).flatten()
    type(fm_data.dtype)
    hex_line_generator = (', '.join(map(hex, sub_arr))
                          for sub_arr in np.array_split(fm_data, math.ceil(len(fm_data) / 20)))

    env \
        .get_template('iofmdata.cc.template') \
        .stream(common_template_header=hdr,
                include_h=header_filename,
                var_name=template_params.var_name,
                fm_data=hex_line_generator,
                data_type=template_params.data_type,
                namespaces=parsed_args.namespaces) \
        .dump(str(cc_filename))


def get_npy_vec_size(filename: str) -> int:
    """
    Gets the size of the array in the npy file
    Args:
        filename: npy file path.
    Return:
        size in bytes
    """
    data = np.load(Path(parsed_args.data_folder_path) / filename)
    return data.size * data.dtype.itemsize


def write_cc_files(args, count, iofm_data_type, add_usecase_fname, prefix):
    """
    Write all cc files

    @param args:                User-provided args
    @param count:               File count
    @param iofm_data_type:      Data type
    @param add_usecase_fname:   Use case suffix
    @param prefix:              Prefix (ifm/ofm)
    @return:                    Names and sizes of generated C++ arrays
    """
    array_names = []
    sizes = []

    header_filename = get_header_filename(add_usecase_fname)

    # In the data_folder_path there should be pairs of ifm-ofm
    # It's assumed the ifm-ofm naming convention: ifm0.npy-ofm0.npy, ifm1.npy-ofm1.npy
    # count = int(len(list(Path(args.data_folder_path).glob(f'{prefix}*.npy'))))

    for idx in range(count):
        # Save the fm cc file
        base_name = prefix + str(idx)
        filename = base_name + ".npy"
        array_name = base_name + add_usecase_fname
        cc_filename = Path(args.source_folder_path) / (array_name + ".cc")
        array_names.append(array_name)

        template_params = IofmParams(
            var_name=array_name,
            data_type=iofm_data_type,
        )

        write_individual_cc_file(
            template_params, header_filename, filename, cc_filename, args.license_template
        )
        sizes.append(get_npy_vec_size(filename))

    return array_names, sizes


def get_header_filename(use_case_filename):
    """
    Get the header file name from the use case file name

    @param use_case_filename:   The use case file name
    @return:                    The header file name
    """
    return "TestData" + use_case_filename + ".hpp"


def get_cc_filename(use_case_filename):
    """
    Get the cc file name from the use case file name

    @param use_case_filename:   The use case file name
    @return:                    The cc file name
    """
    return "TestData" + use_case_filename + ".cc"


def main(args):
    """
    Generate test data
    @param args:    Parsed args
    """
    add_usecase_fname = ("_" + args.usecase) if (args.usecase != "") else ""
    header_filename = get_header_filename(add_usecase_fname)
    common_cc_filename = get_cc_filename(add_usecase_fname)

    # In the data_folder_path there should be pairs of ifm-ofm
    # It's assumed the ifm-ofm naming convention: ifm0.npy-ofm0.npy, ifm1.npy-ofm1.npy
    ifms_count = int(len(list(Path(args.data_folder_path).glob('ifm*.npy'))))
    ofms_count = int(len(list(Path(args.data_folder_path).glob('ofm*.npy'))))

    iofm_data_type = "int8_t"
    if ifms_count > 0:
        iofm_data_type = "int8_t" \
            if (np.load(str(Path(args.data_folder_path) / "ifm0.npy")).dtype == np.int8) \
            else "uint8_t"

    ifm_array_names, ifm_sizes = write_cc_files(
        args, ifms_count, iofm_data_type, add_usecase_fname, prefix="ifm"
    )

    ofm_array_names, ofm_sizes = write_cc_files(
        args, ofms_count, iofm_data_type, add_usecase_fname, prefix="ofm"
    )

    common_cc_filepath = Path(args.source_folder_path) / common_cc_filename

    template_params = TestDataParams(
        ifm_count=ifms_count,
        ofm_count=ofms_count,
        ifm_var_names=ifm_array_names,
        ifm_var_sizes=ifm_sizes,
        ofm_var_names=ofm_array_names,
        ofm_var_sizes=ofm_sizes,
        data_type=iofm_data_type,
    )

    write_hpp_file(
        template_params, header_filename, common_cc_filepath, args.license_template
    )


if __name__ == '__main__':
    if parsed_args.verbosity:
        print("Running gen_test_data_cpp with args: " + str(parsed_args))
    main(parsed_args)
