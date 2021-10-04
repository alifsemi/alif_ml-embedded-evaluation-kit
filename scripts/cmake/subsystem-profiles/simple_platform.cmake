#----------------------------------------------------------------------------
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
#----------------------------------------------------------------------------

# CMake configuration file for peripheral memory map for simple platform. This is a stripped down
# version of Arm Corstone-300 platform with minimal peripherals to be able to use Ethos-U55. However,
# for ease of integration with Arm FastModel Tools, it uses PL011 as the UART component instead of
# the CMSDK UART block used by the MPS3 FPGA and FVP implementations.
###################################################################################################
#                                         Mem sizes                                               #
###################################################################################################
set(ITCM_SIZE             "0x00080000" CACHE STRING "ITCM size:       512 kiB")
set(DTCM_BLK_SIZE         "0x00020000" CACHE STRING "DTCM size:       128 kiB, 4 banks")
set(BRAM_SIZE             "0x00200000" CACHE STRING "BRAM size:         2 MiB")
set(ISRAM0_SIZE           "0x00200000" CACHE STRING "ISRAM0 size:       2 MiB")
set(ISRAM1_SIZE           "0x00200000" CACHE STRING "ISRAM1 size:       2 MiB")
set(DDR4_BLK_SIZE         "0x10000000" CACHE STRING "DDR4 block size: 256 MiB")

###################################################################################################
#                                Base addresses for memory regions                                #
###################################################################################################
set(ITCM_BASE_NS          "0x00000000" CACHE STRING "Instruction TCM Non-Secure base address")
set(BRAM_BASE_NS          "0x01000000" CACHE STRING "CODE SRAM Non-Secure base address")
set(DTCM0_BASE_NS         "0x20000000" CACHE STRING "Data TCM block 0 Non-Secure base address")
set(DTCM1_BASE_NS         "0x20020000" CACHE STRING "Data TCM block 1 Non-Secure base address")
set(DTCM2_BASE_NS         "0x20040000" CACHE STRING "Data TCM block 2 Non-Secure base address")
set(DTCM3_BASE_NS         "0x20060000" CACHE STRING "Data TCM block 3 Non-Secure base address")
set(ISRAM0_BASE_NS        "0x21000000" CACHE STRING "Internal SRAM Area Non-Secure base address")
set(ISRAM1_BASE_NS        "0x21200000" CACHE STRING "Internal SRAM Area Non-Secure base address")
set(QSPI_SRAM_BASE_NS     "0x28000000" CACHE STRING "QSPI SRAM Non-Secure base address")
set(DDR4_BLK0_BASE_NS     "0x60000000" CACHE STRING "DDR4 block 0 Non-Secure base address")
set(DDR4_BLK1_BASE_NS     "0x80000000" CACHE STRING "DDR4 block 1 Non-Secure base address")
set(DDR4_BLK2_BASE_NS     "0xA0000000" CACHE STRING "DDR4 block 2 Non-Secure base address")
set(DDR4_BLK3_BASE_NS     "0xC0000000" CACHE STRING "DDR4 block 3 Non-Secure base address")

set(ITCM_BASE_S           "0x10000000" CACHE STRING "Instruction TCM Secure base address")
set(BRAM_BASE_S           "0x11000000" CACHE STRING "CODE SRAM Secure base address")
set(DTCM0_BASE_S          "0x30000000" CACHE STRING "Data TCM block 0 Secure base address")
set(DTCM1_BASE_S          "0x30020000" CACHE STRING "Data TCM block 1 Secure base address")
set(DTCM2_BASE_S          "0x30040000" CACHE STRING "Data TCM block 2 Secure base address")
set(DTCM3_BASE_S          "0x30060000" CACHE STRING "Data TCM block 3 Secure base address")
set(ISRAM0_BASE_S         "0x31000000" CACHE STRING "Internal SRAM Area Secure base address")
set(ISRAM1_BASE_S         "0x31200000" CACHE STRING "Internal SRAM Area Secure base address")
set(DDR4_BLK0_BASE_S      "0x70000000" CACHE STRING "DDR4 block 0 Secure base address")
set(DDR4_BLK1_BASE_S      "0x90000000" CACHE STRING "DDR4 block 1 Secure base address")
set(DDR4_BLK2_BASE_S      "0xB0000000" CACHE STRING "DDR4 block 2 Secure base address")
set(DDR4_BLK3_BASE_S      "0xD0000000" CACHE STRING "DDR4 block 3 Secure base address")

###################################################################################################
#                              Application specific config                                        #
###################################################################################################

# This parameter is based on the linker/scatter script for simple platform. Do not change this
# parameter in isolation.
set(DESIGN_NAME            "Simple platform" CACHE STRING "Design name")

# SRAM size reserved for activation buffers
math(EXPR ACTIVATION_BUF_SRAM_SZ "${ISRAM0_SIZE} + ${ISRAM1_SIZE}" OUTPUT_FORMAT HEXADECIMAL)


###################################################################################################
#                                         Base addresses                                          #
###################################################################################################
set(PL011_UART0_BASE            "0x49303000" CACHE STRING "PL011 UART 0 Base Address")

if (ETHOS_U_NPU_ENABLED)
    set(ETHOS_U_NPU_BASE          "0x48102000" CACHE STRING "Ethos-U NPU base address")
    set(ETHOS_U_NPU_TA0_BASE      "0x48103000" CACHE STRING "Ethos-U NPU's timing adapter 0 base address")
    set(ETHOS_U_NPU_TA1_BASE      "0x48103200" CACHE STRING "Ethos-U NPU's timing adapter 1 base address")
    set(SEC_ETHOS_U_NPU_BASE      "0x58102000" CACHE STRING "Ethos-U NPU base address")
    set(SEC_ETHOS_U_NPU_TA0_BASE  "0x58103000" CACHE STRING "Ethos-U NPU's timing adapter 0 base address")
    set(SEC_ETHOS_U_NPU_TA1_BASE  "0x58103200" CACHE STRING "Ethos-U NPU's timing adapter 1 base address")
endif ()

###################################################################################################
#                                           IRQ numbers                                           #
###################################################################################################
if (ETHOS_U_NPU_ENABLED)
    set(EthosU_IRQn             "56"         CACHE STRING "Ethos-U NPU Interrupt")
endif ()
