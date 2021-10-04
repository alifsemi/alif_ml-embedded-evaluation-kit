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

# CMake configuration file for peripheral memory map for MPS3 as per SSE-300 design
###################################################################################################
#                                         Mem sizes                                               #
###################################################################################################
set(ITCM_SIZE             "0x00080000" CACHE STRING "ITCM size:       512 kiB")
set(DTCM_BLK_SIZE         "0x00020000" CACHE STRING "DTCM size:       128 kiB, 4 banks")
set(BRAM_SIZE             "0x00200000" CACHE STRING "BRAM size:         2 MiB")
set(ISRAM0_SIZE           "0x00200000" CACHE STRING "ISRAM0 size:       2 MiB")
set(ISRAM1_SIZE           "0x00200000" CACHE STRING "ISRAM1 size:       2 MiB")
set(QSPI_SRAM_SIZE        "0x00800000" CACHE STRING "QSPI Flash size:   8 MiB")
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

# This parameter is based on the linker/scatter script for SSE-300. Do not change this parameter
# in isolation.
set(DESIGN_NAME            "Arm Corstone-300 (SSE-300)" CACHE STRING "Design name")

# SRAM size reserved for activation buffers
math(EXPR ACTIVATION_BUF_SRAM_SZ "${ISRAM0_SIZE} + ${ISRAM1_SIZE}" OUTPUT_FORMAT HEXADECIMAL)

###################################################################################################
#           Base addresses for dynamic loads (to be used for FVP form only)                       #
###################################################################################################
# This parameter is also mentioned in the linker/scatter script for SSE-300. Do not change these
# parameters in isolation.
set(DYNAMIC_MODEL_BASE      "${DDR4_BLK1_BASE_S}" CACHE STRING
                            "Region to be used for dynamic load of model into memory")
set(DYNAMIC_MODEL_SIZE      "0x02000000" CACHE STRING "Size of the space reserved for the model")
math(EXPR DYNAMIC_IFM_BASE  "${DYNAMIC_MODEL_BASE} + ${DYNAMIC_MODEL_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_IFM_SIZE        "0x01000000" CACHE STRING "Size of the space reserved for the IFM")
math(EXPR DYNAMIC_OFM_BASE  "${DYNAMIC_IFM_BASE} + ${DYNAMIC_IFM_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
set(DYNAMIC_OFM_SIZE        "0x01000000" CACHE STRING "Size of the space reserved for the OFM")

###################################################################################################
#                     Base addresses for peripherals - non secure                                 #
###################################################################################################
set(CMSDK_GPIO0_BASE      "0x41100000" CACHE STRING "User GPIO 0 Base Address (4KB)")
set(CMSDK_GPIO1_BASE      "0x41101000" CACHE STRING "User GPIO 1 Base Address (4KB)")
set(CMSDK_GPIO2_BASE      "0x41102000" CACHE STRING "User GPIO 2 Base Address (4KB)")
set(CMSDK_GPIO3_BASE      "0x41103000" CACHE STRING "User GPIO 3 Base Address (4KB)")

set(AHB_USER0_BASE        "0x41104000" CACHE STRING "AHB USER 0 Base Address (4KB)")
set(AHB_USER1_BASE        "0x41105000" CACHE STRING "AHB USER 1 Base Address (4KB)")
set(AHB_USER2_BASE        "0x41106000" CACHE STRING "AHB USER 2 Base Address (4KB)")
set(AHB_USER3_BASE        "0x41107000" CACHE STRING "AHB USER 3 Base Address (4KB)")

set(DMA0_BASE             "0x41200000" CACHE STRING "DMA0 (4KB)")
set(DMA1_BASE             "0x41201000" CACHE STRING "DMA1 (4KB)")
set(DMA2_BASE             "0x41202000" CACHE STRING "DMA2 (4KB)")
set(DMA3_BASE             "0x41203000" CACHE STRING "DMA3 (4KB)")

set(SMSC9220_BASE         "0x41400000" CACHE STRING "Ethernet SMSC9220 Base Address (1MB)")
set(USB_BASE              "0x41500000" CACHE STRING "USB Base Address (1MB)")

set(USER_APB0_BASE        "0x41700000" CACHE STRING "User APB0")
set(USER_APB1_BASE        "0x41701000" CACHE STRING "User APB1")
set(USER_APB2_BASE        "0x41702000" CACHE STRING "User APB2")
set(USER_APB3_BASE        "0x41703000" CACHE STRING "User APB3")

set(QSPI_XIP_BASE         "0x41800000" CACHE STRING "QSPI XIP config Base Address ")
set(QSPI_WRITE_BASE       "0x41801000" CACHE STRING "QSPI write config Base Address ")

if (ETHOS_U_NPU_ENABLED)
    set(ETHOS_U_NPU_BASE        "0x48102000" CACHE STRING "Ethos-U NPU base address")
    set(ETHOS_U_NPU_TA0_BASE    "0x48103000" CACHE STRING "Ethos-U NPU's timing adapter 0 base address")
    set(ETHOS_U_NPU_TA1_BASE    "0x48103200" CACHE STRING "Ethos-U NPU's timing adapter 1 base address")
endif (ETHOS_U_NPU_ENABLED)

set(MPS3_I2C0_BASE        "0x49200000" CACHE STRING "Touch Screen I2C Base Address ")
set(MPS3_I2C1_BASE        "0x49201000" CACHE STRING "Audio Interface I2C Base Address ")
set(MPS3_SSP2_BASE        "0x49202000" CACHE STRING "ADC SPI PL022 Base Address")
set(MPS3_SSP3_BASE        "0x49203000" CACHE STRING "Shield 0 SPI PL022 Base Address")
set(MPS3_SSP4_BASE        "0x49204000" CACHE STRING "Shield 1 SPI PL022 Base Address")
set(MPS3_I2C2_BASE        "0x49205000" CACHE STRING "Shield 0 SBCon Base Address ")
set(MPS3_I2C3_BASE        "0x49206000" CACHE STRING "Shield 1 SBCon Base Address ")

set(USER_APB_BASE         "0x49207000" CACHE STRING "User APB")
set(MPS3_I2C5_BASE        "0x49208000" CACHE STRING "DDR EPROM I2C SBCon Base Address ")

set(MPS3_SCC_BASE         "0x49300000" CACHE STRING "SCC Base Address ")
set(MPS3_AAIC_I2S_BASE    "0x49301000" CACHE STRING "Audio Interface I2S Base Address ")
set(MPS3_FPGAIO_BASE      "0x49302000" CACHE STRING "FPGA IO Base Address ")

set(CMSDK_UART0_BASE      "0x49303000" CACHE STRING "UART 0 Base Address ")
set(CMSDK_UART1_BASE      "0x49304000" CACHE STRING "UART 1 Base Address ")
set(CMSDK_UART2_BASE      "0x49305000" CACHE STRING "UART 2 Base Address ")
set(CMSDK_UART3_BASE      "0x49306000" CACHE STRING "UART 3 Base Address Shield 0")
set(CMSDK_UART4_BASE      "0x49307000" CACHE STRING "UART 4 Base Address Shield 1")
set(CMSDK_UART5_BASE      "0x49308000" CACHE STRING "UART 5 Base Address ")

set(CLCD_CONFIG_BASE      "0x4930A000" CACHE STRING "CLCD CONFIG Base Address ")
set(RTC_BASE              "0x4930B000" CACHE STRING "RTC Base address ")

###################################################################################################
#                     Base addresses for peripherals - secure                                     #
###################################################################################################
set(SEC_CMSDK_GPIO0_BASE   "0x51100000" CACHE STRING "User GPIO 0 Base Address (4KB)")
set(SEC_CMSDK_GPIO1_BASE   "0x51101000" CACHE STRING "User GPIO 1 Base Address (4KB)")
set(SEC_CMSDK_GPIO2_BASE   "0x51102000" CACHE STRING "User GPIO 2 Base Address (4KB)")
set(SEC_CMSDK_GPIO3_BASE   "0x51103000" CACHE STRING "User GPIO 3 Base Address (4KB)")

set(SEC_AHB_USER0_BASE     "0x51104000" CACHE STRING "AHB USER 0 Base Address (4KB)")
set(SEC_AHB_USER1_BASE     "0x51105000" CACHE STRING "AHB USER 1 Base Address (4KB)")
set(SEC_AHB_USER2_BASE     "0x51106000" CACHE STRING "AHB USER 2 Base Address (4KB)")
set(SEC_AHB_USER3_BASE     "0x51107000" CACHE STRING "AHB USER 3 Base Address (4KB)")

set(SEC_DMA0_BASE          "0x51200000" CACHE STRING "DMA0 (4KB)")
set(SEC_DMA1_BASE          "0x51201000" CACHE STRING "DMA1 (4KB)")
set(SEC_DMA2_BASE          "0x51202000" CACHE STRING "DMA2 (4KB)")
set(SEC_DMA3_BASE          "0x51203000" CACHE STRING "DMA3 (4KB)")

set(SEC_SMSC9220_BASE      "0x51400000" CACHE STRING "Ethernet SMSC9220 Base Address (1MB)")
set(SEC_USB_BASE           "0x51500000" CACHE STRING "USB Base Address (1MB)")

set(SEC_USER_APB0_BASE     "0x51700000" CACHE STRING "User APB0 Base Address")
set(SEC_USER_APB1_BASE     "0x51701000" CACHE STRING "User APB1 Base Address")
set(SEC_USER_APB2_BASE     "0x51702000" CACHE STRING "User APB2 Base Address")
set(SEC_USER_APB3_BASE     "0x51703000" CACHE STRING "User APB3 Base Address")

set(SEC_QSPI_XIP_BASE      "0x51800000" CACHE STRING "QSPI XIP config Base Address ")
set(SEC_QSPI_WRITE_BASE    "0x51801000" CACHE STRING "QSPI write config Base Address ")

if (ETHOS_U_NPU_ENABLED)
    set(SEC_ETHOS_U_NPU_BASE     "0x58102000" CACHE STRING "Ethos-U NPU base address")
    set(SEC_ETHOS_U_NPU_TA0_BASE "0x58103000" CACHE STRING "Ethos-U NPU's timing adapter 0 base address")
    set(SEC_ETHOS_U_NPU_TA1_BASE "0x58103200" CACHE STRING "Ethos-U NPU's timing adapter 1 base address")
endif (ETHOS_U_NPU_ENABLED)

set(SEC_MPS3_I2C0_BASE     "0x58200000" CACHE STRING "Touch Screen I2C Base Address ")
set(SEC_MPS3_I2C1_BASE     "0x58201000" CACHE STRING "Audio Interface I2C Base Address ")
set(SEC_MPS3_SSP2_BASE     "0x58202000" CACHE STRING "ADC SPI PL022 Base Address")
set(SEC_MPS3_SSP3_BASE     "0x58203000" CACHE STRING "Shield 0 SPI PL022 Base Address")
set(SEC_MPS3_SSP4_BASE     "0x58204000" CACHE STRING "Shield 1 SPI PL022 Base Address")
set(SEC_MPS3_I2C2_BASE     "0x58205000" CACHE STRING "Shield 0 SBCon Base Address ")
set(SEC_MPS3_I2C3_BASE     "0x58206000" CACHE STRING "Shield 1 SBCon Base Address ")

set(SEC_USER_APB_BASE      "0x58207000" CACHE STRING "User APB Base Address")
set(SEC_MPS3_I2C5_BASE     "0x58208000" CACHE STRING "DDR EPROM I2C SBCon Base Address ")

set(SEC_MPS3_SCC_BASE         "0x58300000" CACHE STRING "SCC Base Address ")
set(SEC_MPS3_AAIC_I2S_BASE    "0x58301000" CACHE STRING "Audio Interface I2S Base Address ")
set(SEC_MPS3_FPGAIO_BASE      "0x58302000" CACHE STRING "FPGA IO Base Address ")

set(SEC_CMSDK_UART0_BASE      "0x58303000" CACHE STRING "UART 0 Base Address ")
set(SEC_CMSDK_UART1_BASE      "0x58304000" CACHE STRING "UART 1 Base Address ")
set(SEC_CMSDK_UART2_BASE      "0x58305000" CACHE STRING "UART 2 Base Address ")
set(SEC_CMSDK_UART3_BASE      "0x58306000" CACHE STRING "UART 3 Base Address Shield 0")
set(SEC_CMSDK_UART4_BASE      "0x58307000" CACHE STRING "UART 4 Base Address Shield 1")
set(SEC_CMSDK_UART5_BASE      "0x58308000" CACHE STRING "UART 5 Base Address ")

set(SEC_CLCD_CONFIG_BASE      "0x5830A000" CACHE STRING "CLCD CONFIG Base Address ")
set(SEC_RTC_BASE              "0x5830B000" CACHE STRING "RTC Base address ")


###################################################################################################
#                                           MPCs                                                  #
###################################################################################################
set(MPC_ISRAM0_BASE_S     "0x50083000" CACHE STRING "ISRAM0 Memory Protection Controller Secure base address")
set(MPC_ISRAM1_BASE_S     "0x50084000" CACHE STRING "ISRAM1 Memory Protection Controller Secure base address")
set(MPC_BRAM_BASE_S       "0x57000000" CACHE STRING "SRAM Memory Protection Controller Secure base address")
set(MPC_QSPI_BASE_S       "0x57001000" CACHE STRING "QSPI Memory Protection Controller Secure base address")
set(MPC_DDR4_BASE_S       "0x57002000" CACHE STRING "DDR4 Memory Protection Controller Secure base address")

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
set(MGMT_PPU_IRQn                 "14" CACHE STRING " MGMT_PPU" )
set(SYS_PPU_IRQn                  "15" CACHE STRING " SYS_PPU" )
set(CPU0_PPU_IRQn                 "16" CACHE STRING " CPU0_PPU" )
set(DEBUG_PPU_IRQn                "26" CACHE STRING " DEBUG_PPU" )
set(TIMER3_AON_IRQn               "27" CACHE STRING " TIMER3_AON" )
set(CPU0CTIIQ0_IRQn               "28" CACHE STRING " CPU0CTIIQ0" )
set(CPU0CTIIQ01_IRQn              "29" CACHE STRING " CPU0CTIIQ01" )

set(SYS_TSTAMP_COUNTER_IRQn       "32" CACHE STRING " System timestamp counter interrupt" )
set(UARTRX0_IRQn                  "33" CACHE STRING " UART 0 RX Interrupt                   ")
set(UARTTX0_IRQn                  "34" CACHE STRING " UART 0 TX Interrupt                   ")
set(UARTRX1_IRQn                  "35" CACHE STRING " UART 1 RX Interrupt                   ")
set(UARTTX1_IRQn                  "36" CACHE STRING " UART 1 TX Interrupt                   ")
set(UARTRX2_IRQn                  "37" CACHE STRING " UART 2 RX Interrupt                   ")
set(UARTTX2_IRQn                  "38" CACHE STRING " UART 2 TX Interrupt                   ")
set(UARTRX3_IRQn                  "39" CACHE STRING " UART 3 RX Interrupt                   ")
set(UARTTX3_IRQn                  "40" CACHE STRING " UART 3 TX Interrupt                   ")
set(UARTRX4_IRQn                  "41" CACHE STRING " UART 4 RX Interrupt                   ")
set(UARTTX4_IRQn                  "42" CACHE STRING " UART 4 TX Interrupt                   ")
set(UART0_IRQn                    "43" CACHE STRING " UART 0 combined Interrupt             ")
set(UART1_IRQn                    "44" CACHE STRING " UART 1 combined Interrupt             ")
set(UART2_IRQn                    "45" CACHE STRING " UART 2 combined Interrupt             ")
set(UART3_IRQn                    "46" CACHE STRING " UART 3 combined Interrupt             ")
set(UART4_IRQn                    "47" CACHE STRING " UART 4 combined Interrupt             ")
set(UARTOVF_IRQn                  "48" CACHE STRING " UART 0,1,2,3,4 Overflow Interrupt     ")
set(ETHERNET_IRQn                 "49" CACHE STRING " Ethernet Interrupt                    ")
set(I2S_IRQn                      "50" CACHE STRING " Audio I2S Interrupt                   ")
set(TSC_IRQn                      "51" CACHE STRING " Touch Screen Interrupt                ")
set(USB_IRQn                      "52" CACHE STRING " USB Interrupt                         ")
set(SPI2_IRQn                     "53" CACHE STRING " ADC (SPI) Interrupt                   ")
set(SPI3_IRQn                     "54" CACHE STRING " SPI 3 Interrupt (Shield 0)            ")
set(SPI4_IRQn                     "55" CACHE STRING " SPI 4 Interrupt (Sheild 1)            ")

if (ETHOS_U_NPU_ENABLED)
set(EthosU_IRQn                   "56" CACHE STRING " Ethos-U55 Interrupt                  ")
endif ()

set(GPIO0_IRQn                    "69" CACHE STRING " GPIO 0 Combined Interrupt             ")
set(GPIO1_IRQn                    "70" CACHE STRING " GPIO 1 Combined Interrupt             ")
set(GPIO2_IRQn                    "71" CACHE STRING " GPIO 2 Combined Interrupt             ")
set(GPIO3_IRQn                    "72" CACHE STRING " GPIO 3 Combined Interrupt             ")

set(GPIO0_0_IRQn                  "73" CACHE STRING "")
set(GPIO0_1_IRQn                  "74" CACHE STRING "")
set(GPIO0_2_IRQn                  "75" CACHE STRING "")
set(GPIO0_3_IRQn                  "76" CACHE STRING "")
set(GPIO0_4_IRQn                  "77" CACHE STRING "")
set(GPIO0_5_IRQn                  "78" CACHE STRING "")
set(GPIO0_6_IRQn                  "79" CACHE STRING "")
set(GPIO0_7_IRQn                  "80" CACHE STRING "")
set(GPIO0_8_IRQn                  "81" CACHE STRING "")
set(GPIO0_9_IRQn                  "82" CACHE STRING "")
set(GPIO0_10_IRQn                 "83" CACHE STRING "")
set(GPIO0_11_IRQn                 "84" CACHE STRING "")
set(GPIO0_12_IRQn                 "85" CACHE STRING "")
set(GPIO0_13_IRQn                 "86" CACHE STRING "")
set(GPIO0_14_IRQn                 "87" CACHE STRING "")
set(GPIO0_15_IRQn                 "88" CACHE STRING "")
set(GPIO1_0_IRQn                  "89" CACHE STRING "")
set(GPIO1_1_IRQn                  "90" CACHE STRING "")
set(GPIO1_2_IRQn                  "91" CACHE STRING "")
set(GPIO1_3_IRQn                  "92" CACHE STRING "")
set(GPIO1_4_IRQn                  "93" CACHE STRING "")
set(GPIO1_5_IRQn                  "94" CACHE STRING "")
set(GPIO1_6_IRQn                  "95" CACHE STRING "")
set(GPIO1_7_IRQn                  "96" CACHE STRING "")
set(GPIO1_8_IRQn                  "97" CACHE STRING "")
set(GPIO1_9_IRQn                  "98" CACHE STRING "")
set(GPIO1_10_IRQn                 "99" CACHE STRING "")
set(GPIO1_11_IRQn                 "100" CACHE STRING "")
set(GPIO1_12_IRQn                 "101" CACHE STRING "")
set(GPIO1_13_IRQn                 "102" CACHE STRING "")
set(GPIO1_14_IRQn                 "103" CACHE STRING "")
set(GPIO1_15_IRQn                 "104" CACHE STRING "")
set(GPIO2_0_IRQn                  "105" CACHE STRING "")
set(GPIO2_1_IRQn                  "106" CACHE STRING "")
set(GPIO2_2_IRQn                  "107" CACHE STRING "")
set(GPIO2_3_IRQn                  "108" CACHE STRING "")
set(GPIO2_4_IRQn                  "109" CACHE STRING "")
set(GPIO2_5_IRQn                  "110" CACHE STRING "")
set(GPIO2_6_IRQn                  "111" CACHE STRING "")
set(GPIO2_7_IRQn                  "112" CACHE STRING "")
set(GPIO2_8_IRQn                  "113" CACHE STRING "")
set(GPIO2_9_IRQn                  "114" CACHE STRING "")
set(GPIO2_10_IRQn                 "115" CACHE STRING "")
set(GPIO2_11_IRQn                 "116" CACHE STRING "")
set(GPIO2_12_IRQn                 "117" CACHE STRING "")
set(GPIO2_13_IRQn                 "118" CACHE STRING "")
set(GPIO2_14_IRQn                 "119" CACHE STRING "")
set(GPIO2_15_IRQn                 "120" CACHE STRING "")
set(GPIO3_0_IRQn                  "121" CACHE STRING "")
set(GPIO3_1_IRQn                  "122" CACHE STRING "")
set(GPIO3_2_IRQn                  "123" CACHE STRING "")
set(GPIO3_3_IRQn                  "124" CACHE STRING "")
set(UARTRX5_IRQn                  "125" CACHE STRING "UART 5 RX Interrupt")
set(UARTTX5_IRQn                  "126" CACHE STRING "UART 5 TX Interrupt")
set(UART5_IRQn                    "127" CACHE STRING "UART 5 combined Interrupt")
