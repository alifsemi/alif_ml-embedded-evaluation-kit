#  Copyright (c) 2021 - 2022 Arm Limited. All rights reserved.
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
import datetime
import math
import os
import numpy as np
from pathlib import Path

from argparse import ArgumentParser
from jinja2 import Environment, FileSystemLoader

parser = ArgumentParser()
parser.add_argument("--data_folder_path", type=str, help="path to ifm-ofm npy folder to convert.")
parser.add_argument("--source_folder_path", type=str, help="path to source folder to be generated.")
parser.add_argument("--header_folder_path", type=str, help="path to header folder to be generated.")
parser.add_argument("--usecase", type=str, default="", help="Test data file suffix.")
parser.add_argument("--namespaces", action='append', default=[])
parser.add_argument("--license_template", type=str, help="Header template file",
                    default="header_template.txt")
parser.add_argument("-v", "--verbosity", action="store_true")

args = parser.parse_args()

env = Environment(loader=FileSystemLoader(Path(__file__).parent / 'templates'),
                  trim_blocks=True,
                  lstrip_blocks=True)


def write_hpp_file(header_filename, cc_file_path, header_template_file, num_ifms, num_ofms,
                   ifm_array_names, ifm_sizes, ofm_array_names, ofm_sizes, iofm_data_type):
    header_file_path = Path(args.header_folder_path) / header_filename

    print(f"++ Generating {header_file_path}")
    header_template = env.get_template(header_template_file)
    hdr = header_template.render(script_name=Path(__file__).name,
                                 gen_time=datetime.datetime.now(),
                                 year=datetime.datetime.now().year)
    env.get_template('TestData.hpp.template').stream(common_template_header=hdr,
                                                   ifm_count=num_ifms,
                                                   ofm_count=num_ofms,
                                                   ifm_var_names=ifm_array_names,
                                                   ifm_var_sizes=ifm_sizes,
                                                   ofm_var_names=ofm_array_names,
                                                   ofm_var_sizes=ofm_sizes,
                                                   data_type=iofm_data_type,
                                                   namespaces=args.namespaces) \
        .dump(str(header_file_path))

    env.get_template('TestData.cc.template').stream(common_template_header=hdr,
                                                  include_h=header_filename,
                                                  ifm_var_names=ifm_array_names,
                                                  ofm_var_names=ofm_array_names,
                                                  data_type=iofm_data_type,
                                                  namespaces=args.namespaces) \
        .dump(str(cc_file_path))


def write_individual_cc_file(filename, cc_filename, header_filename, header_template_file, array_name, iofm_data_type):
    print(f"++ Converting {filename} to {cc_filename.name}")
    header_template = env.get_template(header_template_file)
    hdr = header_template.render(script_name=Path(__file__).name,
                                 gen_time=datetime.datetime.now(),
                                 file_name=filename,
                                 year=datetime.datetime.now().year)

    # Convert the image and write it to the cc file
    fm_data = (np.load(Path(args.data_folder_path) / filename)).flatten()
    type(fm_data.dtype)
    hex_line_generator = (', '.join(map(hex, sub_arr))
                          for sub_arr in np.array_split(fm_data, math.ceil(len(fm_data) / 20)))

    env.get_template('iofmdata.cc.template').stream(common_template_header=hdr,
                                                 include_h=header_filename,
                                                 var_name=array_name,
                                                 fm_data=hex_line_generator,
                                                 data_type=iofm_data_type,
                                                 namespaces=args.namespaces) \
        .dump(str(cc_filename))


def get_npy_vec_size(filename: str) -> int:
    """
    Gets the size of the array in the npy file
    Args:
        filename: npy file path.
    Return:
        size in bytes
    """
    data = np.load(Path(args.data_folder_path) / filename)
    return data.size * data.dtype.itemsize


def main(args):
    # Keep the count of the images converted
    ifm_array_names = []
    ofm_array_names = []

    add_usecase_fname = ("_" + args.usecase) if (args.usecase != "") else ""
    header_filename = "TestData" + add_usecase_fname + ".hpp"
    common_cc_filename = "TestData" + add_usecase_fname + ".cc"

    # In the data_folder_path there should be pairs of ifm-ofm
    # It's assumed the ifm-ofm naming convention: ifm0.npy-ofm0.npy, ifm1.npy-ofm1.npy
    ifms_count = int(len(list(Path(args.data_folder_path).glob('ifm*.npy'))))
    ofms_count = int(len(list(Path(args.data_folder_path).glob('ofm*.npy'))))

    #i_ofms_count = int(len([name for name in os.listdir(os.path.join(args.data_folder_path)) if name.lower().endswith('.npy')]) / 2)

    iofm_data_type = "int8_t"
    if ifms_count > 0:
        iofm_data_type = "int8_t" if (np.load(Path(args.data_folder_path) / "ifm0.npy").dtype == np.int8) else "uint8_t"

    ifm_sizes = []
    ofm_sizes = []

    for idx in range(ifms_count):
        # Save the fm cc file
        base_name = "ifm" + str(idx)
        filename = base_name+".npy"
        array_name = base_name + add_usecase_fname
        cc_filename = Path(args.source_folder_path) / (array_name + ".cc")
        ifm_array_names.append(array_name)
        write_individual_cc_file(filename, cc_filename, header_filename, args.license_template, array_name, iofm_data_type)
        ifm_sizes.append(get_npy_vec_size(filename))

    for idx in range(ofms_count):
        # Save the fm cc file
        base_name = "ofm" + str(idx)
        filename = base_name+".npy"
        array_name = base_name + add_usecase_fname
        cc_filename = Path(args.source_folder_path) / (array_name + ".cc")
        ofm_array_names.append(array_name)
        write_individual_cc_file(filename, cc_filename, header_filename, args.license_template, array_name, iofm_data_type)
        ofm_sizes.append(get_npy_vec_size(filename))

    common_cc_filepath = Path(args.source_folder_path) / common_cc_filename
    write_hpp_file(header_filename, common_cc_filepath, args.license_template,
                   ifms_count, ofms_count, ifm_array_names, ifm_sizes, ofm_array_names, ofm_sizes, iofm_data_type)


if __name__ == '__main__':
    if args.verbosity:
        print("Running gen_test_data_cpp with args: "+str(args))
    main(args)
