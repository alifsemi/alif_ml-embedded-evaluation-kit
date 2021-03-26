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

# CMake configuration file for peripheral memory map for MPS3 as per SSE-200 design
###################################################################################################
#                              Application specific config                                        #
###################################################################################################

# This parameter is based on the linker/scatter script for SSE-200. Do not change this parameter
# in isolation.
set(ACTIVATION_BUF_SRAM_SZ "0x00200000" CACHE STRING "Maximum SRAM size for activation buffers")
set(DESIGN_NAME            "SSE-200"    CACHE STRING "Design name")
###################################################################################################
#                                         Mem sizes                                               #
###################################################################################################
set(ITCM_SIZE             "0x00100000" CACHE STRING "ITCM size:         1 MiB")
set(DTCM_BLK_SIZE         "0x00100000" CACHE STRING "DTCM size:         1 MiB, 4 banks")
set(BRAM_SIZE             "0x00200000" CACHE STRING "BRAM size:         2 MiB")
set(QSPI_SRAM_SIZE        "0x00800000" CACHE STRING "QSPI Flash size:   8 MiB")
set(DDR4_BLK_SIZE         "0x10000000" CACHE STRING "DDR4 block size: 256 MiB")

###################################################################################################
#                                         Base addresses                                          #
###################################################################################################
set(ITCM_BASE_NS          "0x00000000" CACHE STRING "Instruction TCM Non-Secure base address")
set(BRAM_BASE_NS          "0x01000000" CACHE STRING "CODE SRAM Non-Secure base address")
set(DTCM0_BASE_NS         "0x20000000" CACHE STRING "Data TCM block 0 Non-Secure base address")
set(DTCM1_BASE_NS         "0x20100000" CACHE STRING "Data TCM block 1 Non-Secure base address")
set(DTCM2_BASE_NS         "0x20200000" CACHE STRING "Data TCM block 2 Non-Secure base address")
set(DTCM3_BASE_NS         "0x20300000" CACHE STRING "Data TCM block 3 Non-Secure base address")
set(QSPI_SRAM_BASE_NS     "0x28000000" CACHE STRING "QSPI SRAM Non-Secure base address")
set(DDR4_BLK0_BASE_NS     "0x60000000" CACHE STRING "DDR4 block 0 Non-Secure base address")
set(DDR4_BLK1_BASE_NS     "0x80000000" CACHE STRING "DDR4 block 1 Non-Secure base address")
set(DDR4_BLK2_BASE_NS     "0xA0000000" CACHE STRING "DDR4 block 2 Non-Secure base address")
set(DDR4_BLK3_BASE_NS     "0xC0000000" CACHE STRING "DDR4 block 3 Non-Secure base address")

set(ITCM_BASE_S           "0x10000000" CACHE STRING "Instruction TCM Secure base address")
set(BRAM_BASE_S           "0x11000000" CACHE STRING "CODE SRAM Secure base address")
set(DTCM0_BASE_S          "0x30000000" CACHE STRING "Data TCM block 0 Secure base address")
set(DTCM1_BASE_S          "0x30100000" CACHE STRING "Data TCM block 1 Secure base address")
set(DTCM2_BASE_S          "0x30200000" CACHE STRING "Data TCM block 2 Secure base address")
set(DTCM3_BASE_S          "0x30300000" CACHE STRING "Data TCM block 3 Secure base address")
set(DDR4_BLK0_BASE_S      "0x70000000" CACHE STRING "DDR4 block 0 Secure base address")
set(DDR4_BLK1_BASE_S      "0x90000000" CACHE STRING "DDR4 block 1 Secure base address")
set(DDR4_BLK2_BASE_S      "0xB0000000" CACHE STRING "DDR4 block 2 Secure base address")
set(DDR4_BLK3_BASE_S      "0xD0000000" CACHE STRING "DDR4 block 3 Secure base address")

set(CMSDK_GPIO0_BASE      "0x41100000" CACHE STRING "User GPIO 0 Base Address")
set(CMSDK_GPIO1_BASE      "0x41101000" CACHE STRING "User GPIO 1 Base Address")
set(CMSDK_GPIO2_BASE      "0x41102000" CACHE STRING "User GPIO 2 Base Address")
set(CMSDK_GPIO3_BASE      "0x41103000" CACHE STRING "User GPIO 3 Base Address")

if (ETHOS_U55_ENABLED)
    set(ETHOS_U55_BASE       "0x41700000" CACHE STRING "Ethos-U55 base address")
    set(ETHOS_U55_TA0_BASE   "0x41701000" CACHE STRING "Ethos-U55's timing adapter 0 base address")
    set(ETHOS_U55_TA1_BASE   "0x41701200" CACHE STRING "Ethos-U55's timing adapter 1 base address")
endif ()

set(MPS3_I2C0_BASE        "0x41200000" CACHE STRING "Touch Screen I2C Base Address ")
set(MPS3_I2C1_BASE        "0x41201000" CACHE STRING "Audio Interface I2C Base Address ")
set(MPS3_SSP2_BASE        "0x41202000" CACHE STRING "ADC SPI PL022 Base Address")
set(MPS3_SSP3_BASE        "0x41203000" CACHE STRING "Shield 0 SPI PL022 Base Address")

set(MPS3_SSP4_BASE        "0x41204000" CACHE STRING "Shield 1 SPI PL022 Base Address")
set(MPS3_I2C2_BASE        "0x41205000" CACHE STRING "Shield 0 SBCon Base Address ")
set(MPS3_I2C3_BASE        "0x41206000" CACHE STRING "Shield 1 SBCon Base Address ")

set(MPS3_I2C4_BASE        "0x41207000" CACHE STRING "HDMI I2C SBCon Base Address ")
set(MPS3_I2C5_BASE        "0x41208000" CACHE STRING "DDR EPROM I2C SBCon Base Address ")
set(MPS3_SCC_BASE         "0x41300000" CACHE STRING "SCC Base Address ")
set(MPS3_AAIC_I2S_BASE    "0x41301000" CACHE STRING "Audio Interface I2S Base Address ")
set(MPS3_FPGAIO_BASE      "0x41302000" CACHE STRING "FPGA IO Base Address ")
set(CMSDK_UART0_BASE      "0x41303000" CACHE STRING "UART 0 Base Address ")
set(CMSDK_UART1_BASE      "0x41304000" CACHE STRING "UART 1 Base Address ")
set(CMSDK_UART2_BASE      "0x41305000" CACHE STRING "UART 2 Base Address ")
set(CMSDK_UART3_BASE      "0x41306000" CACHE STRING "UART 3 Base Address Shield 0")

set(CMSDK_UART4_BASE      "0x41307000" CACHE STRING "UART 4 Base Address Shield 1")
set(CMSDK_UART5_BASE      "0x41308000" CACHE STRING "UART 5 Base Address ")
set(HDMI_AUDIO_BASE       "0x41309000" CACHE STRING "HDMI AUDIO Base Address ")
set(CLCD_CONFIG_BASE      "0x4130A000" CACHE STRING "CLCD CONFIG Base Address ")
set(RTC_BASE              "0x4130B000" CACHE STRING "RTC Base address ")
set(SMSC9220_BASE         "0x41400000" CACHE STRING "Ethernet SMSC9220 Base Address ")
set(USB_BASE              "0x41500000" CACHE STRING "USB Base Address ")

set(MPS3_eMMC_BASE        "0x41702000" CACHE STRING "User eMMC Base Address")
set(USER_BASE             "0x41703000" CACHE STRING "User ? Base Address ")

set(QSPI_XIP_BASE         "0x41800000" CACHE STRING "QSPI XIP config Base Address ")
set(QSPI_WRITE_BASE       "0x41801000" CACHE STRING "QSPI write config Base Address ")

set(SEC_CMSDK_GPIO0_BASE  "0x51100000" CACHE STRING "User GPIO 0 Base Address")
set(SEC_CMSDK_GPIO1_BASE  "0x51101000" CACHE STRING "User GPIO 0 Base Address")
set(SEC_CMSDK_GPIO2_BASE  "0x51102000" CACHE STRING "User GPIO 0 Base Address")
set(SEC_CMSDK_GPIO3_BASE  "0x51103000" CACHE STRING "User GPIO 0 Base Address")

set(SEC_MPS3_I2C0_BASE    "0x51200000" CACHE STRING "Touch Screen I2C Base Address ")
set(SEC_MPS3_I2C1_BASE    "0x51201000" CACHE STRING "Audio Interface I2C Base Address ")
set(SEC_MPS3_SSP2_BASE    "0x51202000" CACHE STRING "ADC SPI PL022 Base Address")
set(SEC_MPS3_SSP3_BASE    "0x51203000" CACHE STRING "Shield 0 SPI PL022 Base Address")

set(SEC_MPS3_SSP4_BASE    "0x51204000" CACHE STRING "Shield 1 SPI PL022 Base Address")
set(SEC_MPS3_I2C2_BASE    "0x51205000" CACHE STRING "Shield 0 SBCon Base Address ")
set(SEC_MPS3_I2C3_BASE    "0x51206000" CACHE STRING "Shield 1 SBCon Base Address ")

set(SEC_MPS3_I2C4_BASE    "0x51207000" CACHE STRING "HDMI I2C SBCon Base Address ")
set(SEC_MPS3_I2C5_BASE    "0x51208000" CACHE STRING "DDR EPROM I2C SBCon Base Address ")
set(SEC_MPS3_SCC_BASE     "0x51300000" CACHE STRING "SCC Base Address ")
set(SEC_MPS3_AAIC_I2S_BASE     "0x51301000" CACHE STRING "Audio Interface I2S Base Address ")
set(SEC_MPS3_FPGAIO_BASE   "0x51302000" CACHE STRING "FPGA IO Base Address ")
set(SEC_CMSDK_UART0_BASE   "0x51303000" CACHE STRING "UART 0 Base Address ")
set(SEC_CMSDK_UART1_BASE   "0x51304000" CACHE STRING "UART 1 Base Address ")
set(SEC_CMSDK_UART2_BASE   "0x51305000" CACHE STRING "UART 2 Base Address ")
set(SEC_CMSDK_UART3_BASE   "0x51306000" CACHE STRING "UART 3 Base Address Shield 0")

set(SEC_CMSDK_UART4_BASE   "0x51307000" CACHE STRING "UART 4 Base Address Shield 1")
set(SEC_CMSDK_UART5_BASE   "0x51308000" CACHE STRING "UART 5 Base Address ")
set(SEC_HDMI_AUDIO_BASE    "0x51309000" CACHE STRING "HDMI AUDIO Base Address ")
set(SEC_CLCD_CONFIG_BASE   "0x5130A000" CACHE STRING "CLCD CONFIG Base Address ")
set(SEC_RTC_BASE           "0x5130B000" CACHE STRING "RTC Base address ")
set(SEC_SMSC9220_BASE      "0x51400000" CACHE STRING "Ethernet SMSC9220 Base Address ")
set(SEC_USB_BASE           "0x51500000" CACHE STRING "USB Base Address ")

if (ETHOS_U55_ENABLED)
    set(SEC_ETHOS_U55_BASE        "0x51700000" CACHE STRING "Ethos-U55 base address")
    set(SEC_ETHOS_U55_TA0_BASE    "0x51701000" CACHE STRING "Ethos-U55's timing adapter 0 base address")
    set(SEC_ETHOS_U55_TA1_BASE    "0x51701200" CACHE STRING "Ethos-U55's timing adapter 1 base address")
endif ()

set(SEC_MMC_BASE          "0x51702000" CACHE STRING "User eMMC Base Address")
set(SEC_USER_BASE         "0x51703000" CACHE STRING "User ? Base Address ")

set(SEC_QSPI_XIP_BASE     "0x51800000" CACHE STRING "QSPI XIP config Base Address ")
set(SEC_QSPI_WRITE_BASE   "0x51801000" CACHE STRING "QSPI write config Base Address ")

###################################################################################################
#                                           IRQ numbers                                           #
###################################################################################################
set(NONSEC_WATCHDOG_RESET_IRQn    " 0" CACHE STRING " Non-Secure Watchdog Reset Interrupt")
set(NONSEC_WATCHDOG_IRQn          " 1" CACHE STRING " Non-Secure Watchdog Interrupt         ")
set(S32K_TIMER_IRQn               " 2" CACHE STRING " S32K Timer Interrupt                  ")
set(TIMER0_IRQn                   " 3" CACHE STRING " TIMER 0 Interrupt                     ")
set(TIMER1_IRQn                   " 4" CACHE STRING " TIMER 1 Interrupt                     ")
set(DUALTIMER_IRQn                " 5" CACHE STRING " Dual Timer Interrupt                  ")
set(MPC_IRQn                      " 9" CACHE STRING " MPC Combined (Secure) Interrupt       ")
set(PPC_IRQn                      "10" CACHE STRING " PPC Combined (Secure) Interrupt       ")
set(MSC_IRQn                      "11" CACHE STRING " MSC Combined (Secure) Interrput       ")
set(BRIDGE_ERROR_IRQn             "12" CACHE STRING " Bridge Error Combined (Secure) Interrupt ")

set(UARTRX0_IRQn                  "32" CACHE STRING " UART 0 RX Interrupt                   ")
set(UARTTX0_IRQn                  "33" CACHE STRING " UART 0 TX Interrupt                   ")
set(UARTRX1_IRQn                  "34" CACHE STRING " UART 1 RX Interrupt                   ")
set(UARTTX1_IRQn                  "35" CACHE STRING " UART 1 TX Interrupt                   ")
set(UARTRX2_IRQn                  "36" CACHE STRING " UART 2 RX Interrupt                   ")
set(UARTTX2_IRQn                  "37" CACHE STRING " UART 2 TX Interrupt                   ")
set(UARTRX3_IRQn                  "38" CACHE STRING " UART 3 RX Interrupt                   ")
set(UARTTX3_IRQn                  "39" CACHE STRING " UART 3 TX Interrupt                   ")
set(UARTRX4_IRQn                  "40" CACHE STRING " UART 4 RX Interrupt                   ")
set(UARTTX4_IRQn                  "41" CACHE STRING " UART 4 TX Interrupt                   ")
set(UART0_IRQn                    "42" CACHE STRING " UART 0 combined Interrupt             ")
set(UART1_IRQn                    "43" CACHE STRING " UART 1 combined Interrupt             ")
set(UART2_IRQn                    "44" CACHE STRING " UART 2 combined Interrupt             ")
set(UART3_IRQn                    "45" CACHE STRING " UART 3 combined Interrupt             ")
set(UART4_IRQn                    "46" CACHE STRING " UART 4 combined Interrupt             ")
set(UARTOVF_IRQn                  "47" CACHE STRING " UART 0,1,2,3,4 Overflow Interrupt     ")
set(ETHERNET_IRQn                 "48" CACHE STRING " Ethernet Interrupt                    ")
set(I2S_IRQn                      "49" CACHE STRING " I2S Interrupt                         ")
set(TSC_IRQn                      "50" CACHE STRING " Touch Screen Interrupt                ")
set(SPI2_IRQn                     "52" CACHE STRING " SPI 2 Interrupt                       ")
set(SPI3_IRQn                     "53" CACHE STRING " SPI 3 Interrupt                       ")
set(SPI4_IRQn                     "54" CACHE STRING " SPI 4 Interrupt                       ")

if (ETHOS_U55_ENABLED)
    if (CPU_CORTEX_M55 EQUAL 1)
        set(EthosU_IRQn           "55" CACHE STRING " Ethos-U55 Interrupt                   ")
    elseif (CPU_CORTEX_M33 EQUAL 1)
        set(EthosU_IRQn           "67" CACHE STRING " Ethos-U55 Interrupt                   ")
    endif()
endif ()

set(GPIO0_IRQn                    "68" CACHE STRING " GPIO 0 Combined Interrupt             ")
set(GPIO1_IRQn                    "69" CACHE STRING " GPIO 1 Combined Interrupt             ")
set(GPIO2_IRQn                    "70" CACHE STRING " GPIO 2 Combined Interrupt             ")
set(GPIO3_IRQn                    "71" CACHE STRING " GPIO 3 Combined Interrupt             ")

set(GPIO0_0_IRQn                  "72" CACHE STRING "")
set(GPIO0_1_IRQn                  "73" CACHE STRING "")
set(GPIO0_2_IRQn                  "74" CACHE STRING "")
set(GPIO0_3_IRQn                  "75" CACHE STRING "")
set(GPIO0_4_IRQn                  "76" CACHE STRING "")
set(GPIO0_5_IRQn                  "77" CACHE STRING "")
set(GPIO0_6_IRQn                  "78" CACHE STRING "")
set(GPIO0_7_IRQn                  "79" CACHE STRING "")
set(GPIO0_8_IRQn                  "80" CACHE STRING "")
set(GPIO0_9_IRQn                  "81" CACHE STRING "")
set(GPIO0_10_IRQn                 "82" CACHE STRING "")
set(GPIO0_11_IRQn                 "83" CACHE STRING "")
set(GPIO0_12_IRQn                 "84" CACHE STRING "")
set(GPIO0_13_IRQn                 "85" CACHE STRING "")
set(GPIO0_14_IRQn                 "86" CACHE STRING "")
set(GPIO0_15_IRQn                 "87" CACHE STRING "")
set(GPIO1_0_IRQn                  "88" CACHE STRING "")
set(GPIO1_1_IRQn                  "89" CACHE STRING "")
set(GPIO1_2_IRQn                  "90" CACHE STRING "")
set(GPIO1_3_IRQn                  "91" CACHE STRING "")
set(GPIO1_4_IRQn                  "92" CACHE STRING "")
set(GPIO1_5_IRQn                  "93" CACHE STRING "")
set(GPIO1_6_IRQn                  "94" CACHE STRING "")
set(GPIO1_7_IRQn                  "95" CACHE STRING "")
set(GPIO1_8_IRQn                  "96" CACHE STRING "")
set(GPIO1_9_IRQn                  "97" CACHE STRING "")
set(GPIO1_10_IRQn                 "98" CACHE STRING "")
set(GPIO1_11_IRQn                 "99" CACHE STRING "")
set(GPIO1_12_IRQn                 "100" CACHE STRING "")
set(GPIO1_13_IRQn                 "101" CACHE STRING "")
set(GPIO1_14_IRQn                 "102" CACHE STRING "")
set(GPIO1_15_IRQn                 "103" CACHE STRING "")
set(GPIO2_0_IRQn                  "104" CACHE STRING "")
set(GPIO2_1_IRQn                  "105" CACHE STRING "")
set(GPIO2_2_IRQn                  "106" CACHE STRING "")
set(GPIO2_3_IRQn                  "107" CACHE STRING "")
set(GPIO2_4_IRQn                  "108" CACHE STRING "")
set(GPIO2_5_IRQn                  "109" CACHE STRING "")
set(GPIO2_6_IRQn                  "110" CACHE STRING "")
set(GPIO2_7_IRQn                  "111" CACHE STRING "")
set(GPIO2_8_IRQn                  "112" CACHE STRING "")
set(GPIO2_9_IRQn                  "113" CACHE STRING "")
set(GPIO2_10_IRQn                 "114" CACHE STRING "")
set(GPIO2_11_IRQn                 "115" CACHE STRING "")
set(GPIO2_12_IRQn                 "116" CACHE STRING "")
set(GPIO2_13_IRQn                 "117" CACHE STRING "")
set(GPIO2_14_IRQn                 "118" CACHE STRING "")
set(GPIO2_15_IRQn                 "119" CACHE STRING "")
set(GPIO3_0_IRQn                  "120" CACHE STRING "")
set(GPIO3_1_IRQn                  "121" CACHE STRING "")
set(GPIO3_2_IRQn                  "122" CACHE STRING "")
set(GPIO3_3_IRQn                  "123" CACHE STRING "")
set(UARTRX5_IRQn                  "124" CACHE STRING "UART 5 RX Interrupt")
set(UARTTX5_IRQn                  "125" CACHE STRING "UART 5 TX Interrupt")
set(UART5_IRQn                    "126" CACHE STRING "UART 5 combined Interrupt")
set(HDCLCD_IRQn                   "127" CACHE STRING "HDCLCD Interrupt")
