#  Copyright (c) 2021 Arm Limited. All rights reserved.
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

import os
from argparse import ArgumentParser

"""
This file is used as part of post build steps to generate 'images.txt' file
which can be copied over onto the MPS3 board's SD card. The purpose is to
limit having to manually edit the file based on different load regions that
the build scatter file might dictate.
"""

def is_commented(line):
    if (line.startswith(";")):
        return True
    else:
        return False


def is_load_rom(line):
    load_region_specifiers = ['LOAD_ROM', 'LD_ROM', 'LOAD_REGION']

    for load_specifier in load_region_specifiers:
        if line.startswith(load_specifier):
            return True

    return False


class TargetSubsystem:

    def __init__(self, target_subsystem_name: str):
        """
        Constructor for target class.
        Arguments:
            target_subsystem_name: name of the target subsystem
        """
        # Dict with mem map and binary names we expect
        self.subsystems = {
            "sse-200": {
                "mmap_mcc" : {
                    # FPGA addr |  MCC addr  |
                    "0x00000000": "0x00000000", # ITCM (NS)
                    "0x10000000": "0x01000000", # ITCM (S)
                    "0x20000000": "0x02000000", # DTCM (NS)
                    "0x30000000": "0x03000000", # DTCM (S)
                    "0x60000000": "0x08000000"  # DDR (NS)
                },
                "bin_names": {
                    0: "itcm.bin",
                    1: "dram.bin"
                }
            },
            "sse-300": {
                "mmap_mcc" : {
                    # FPGA addr |  MCC addr  |
                    "0x00000000": "0x00000000", # ITCM (NS)
                    "0x01000000": "0x02000000", # BRAM or FPGA's data SRAM (NS)
                    "0x60000000": "0x08000000", # DDR (NS)
                    "0x70000000": "0x0c000000"  # DDR (S)
                },
                "bin_names": {
                    0: "itcm.bin",
                    1: "dram.bin"
                }
            }
        }

        self.name = target_subsystem_name


    def is_supported(self, target_subsystem: str) -> bool:
        """
        Checks if the target subsystem exists within systems
        supported by this script
        """
        if target_subsystem in self.subsystems.keys():
            return True

        print(f"Platforms supported: {self.subsystems.keys()}")
        return False


    def mps3_mappings(self) -> dict:
        """
        Returns the FPGA <--> MCC address translations
        as a dict
        """
        if self.is_supported(self.name):
            return self.subsystems[self.name]['mmap_mcc']
        return {}


    def mps3_bin_names(self) -> dict:
        """
        Returns expected binary names for the executable built
        for Cortex-M55 or Cortex-M55+Ethos-U55 targets in the
        form of a dict with index and name
        """
        if self.is_supported(self.name):
            return self.subsystems[self.name]['bin_names']

        return {}


def main(args):
    """
    Generates the output txt file with MCC to FPGA address mapping used
    that is used by the MCC on FPGA to load executable regions into
    correct regions in memory.
    """
    # List out arguments used:
    scatter_file_path = args.scatter_file_path
    target_subsystem_name = args.target_subsystem
    output_file_path = args.output_file_path

    target = TargetSubsystem(target_subsystem_name=target_subsystem_name)

    if target.is_supported(target_subsystem_name) != True:
        print(f'Target {target_subsystem_name} not supported.')
        return

    with open(scatter_file_path,'r') as scatter_file:
        lines_read = scatter_file.readlines()
        str_list = []

        bin_names = None
        mem_map = None

        mem_map = target.mps3_mappings()
        bin_names = target.mps3_bin_names()

        str_list.append("TITLE: Arm MPS3 FPGA prototyping board Images Configuration File\n")
        str_list.append("[IMAGES]\n\n")

        cnt = 0
        for line in lines_read:
            if is_commented(line) or is_load_rom(line) != True:
                continue

            addr = line.split()[1]

            if mem_map.get(addr, None) == None:
                raise RuntimeError(
                    'Translation for this address unavailable')
            if cnt > len(bin_names):
                raise RuntimeError(
                    f"bin names len exceeded: {cnt}")

            str_list.append("IMAGE" + str(cnt) + "ADDRESS: " +
                mem_map[addr] + " ; MCC@" + mem_map[addr] +
                " <=> FPGA@"  + addr + "\n")
            str_list.append("IMAGE" + str(cnt) + "UPDATE: AUTO\n")
            str_list.append("IMAGE" + str(cnt) + "FILE: \SOFTWARE\\" +
                bin_names[cnt] + "\n\n")
            cnt += 1

        if cnt > 0 and cnt < 33:
            str_list.insert(2,
                "TOTALIMAGES: {} ;Number of Images (Max: 32)\n\n".format(
                    cnt))
        else:
            raise RuntimeError('Invalid image count')

        if os.path.exists(output_file_path):
            os.remove(output_file_path)
        print(''.join(str_list), file=open(output_file_path, "a"))


if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("--scatter_file_path", type=str, required=True,
                        help="Path to the scatter file")
    parser.add_argument("--target_subsystem", type=str, required=True,
                        help="Target subsystem in use")
    parser.add_argument("--output_file_path", type=str, required=True,
                        help="Output file path")
    args = parser.parse_args()
    main(args)
