#----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2024 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
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

#----------------------------------------------------------------------------
# CMake description file for the Arm Ethos-U85 Timing Adapter settings (single
# NPU core with three, four or six AXIs).
#----------------------------------------------------------------------------

# The platform CMake infra should set the base register values for
# TA component to work. For Ethos-U85, we need at least three base addresses.
if (NOT DEFINED TA_SRAM_BASE OR NOT DEFINED TA_EXT_BASE)
    message(FATAL_ERROR "TA_SRAM_BASE and TA_EXT_BASE need to be defined.")
endif ()

message(STATUS "using TA_SRAM_BASE @ ${TA_SRAM_BASE}; TA_EXT_BASE @ ${TA_EXT_BASE}.")

# Timing adapter settings for SRAM
set(SRAM_MAXR       "8"       CACHE STRING "6-bit field. Max no. of pending reads. 0=infinite")
set(SRAM_MAXW       "8"       CACHE STRING "6-bit field. Max no. of pending writes. 0=infinite")
set(SRAM_MAXRW      "0"        CACHE STRING "6-bit field. Max no. of pending reads+writes. 0=infinite")
set(SRAM_RLATENCY   "32"       CACHE STRING "12-bit field. Minimum latency (clock cycles) from AVALID to RVALID.")
set(SRAM_WLATENCY   "32"       CACHE STRING "12-bit field. Minimum latency (clock cycles) from WVALID&WLAST to BVALID.")
set(SRAM_PULSE_ON   "3999"     CACHE STRING "No. of cycles addresses let through (0-65535).")
set(SRAM_PULSE_OFF  "1"        CACHE STRING "No. of cycles addresses blocked (0-65535).")
set(SRAM_BWCAP      "4000"     CACHE STRING "16-bit field. Max no. of 64-bit words transfered per pulse cycle 0=infinite")
set(SRAM_PERFCTRL   "0"        CACHE STRING "6-bit field selecting an event for event counter 0=default")
set(SRAM_PERFCNT    "0"        CACHE STRING "32-bit event counter")
set(SRAM_MODE       "1"        CACHE STRING "Bit 0: 1=enable dynamic clocking to avoid underrun;
                                             Bit 1: 1=enable random AR reordering (0=default);
                                             Bit 2: 1=enable random R reordering (0=default);
                                             Bit 3: 1=enable random B reordering (0=default);
                                             Bit 11-4: Frequency scale 0=full speed, 255=(1/256) speed")
set(SRAM_HISTBIN    "0"        CACHE STRING "Controls which histogram bin (0-15) that should be accessed by HISTCNT.")
set(SRAM_HISTCNT    "0"        CACHE STRING "32-bit field. Read/write the selected histogram bin.")

# Timing adapter settings for EXT
set(EXT_MAXR        "64"       CACHE STRING "6-bit field. Max no. of pending reads. 0=infinite")
set(EXT_MAXW        "32"       CACHE STRING "6-bit field. Max no. of pending writes. 0=infinite")
set(EXT_MAXRW       "0"        CACHE STRING "6-bit field. Max no. of pending reads+writes. 0=infinite")
set(EXT_RLATENCY    "500"      CACHE STRING "12-bit field. Minimum latency (clock cycles) from AVALID to RVALID.")
set(EXT_WLATENCY    "250"      CACHE STRING "12-bit field. Minimum latency (clock cycles) from WVALID&WLAST to BVALID.")
set(EXT_PULSE_ON    "4000"     CACHE STRING "No. of cycles addresses let through (0-65535).")
set(EXT_PULSE_OFF   "1000"     CACHE STRING "No. of cycles addresses blocked (0-65535).")
set(EXT_BWCAP       "3750"     CACHE STRING "16-bit field. Max no. of 64-bit words transfered per pulse cycle 0=infinite")
set(EXT_PERFCTRL    "0"        CACHE STRING "6-bit field selecting an event for event counter 0=default")
set(EXT_PERFCNT     "0"        CACHE STRING "32-bit event counter")
set(EXT_MODE        "1"        CACHE STRING "Bit 0: 1=enable dynamic clocking to avoid underrun;
                                            Bit 1: 1=enable random AR reordering (0=default);
                                            Bit 2: 1=enable random R reordering (0=default);
                                            Bit 3: 1=enable random B reordering (0=default);
                                            Bit 11-4: Frequency scale 0=full speed, 255=(1/256) speed")
set(EXT_HISTBIN     "0"        CACHE STRING "Controls which histogram bin (0-15) that should be accessed by HISTCNT.")
set(EXT_HISTCNT     "0"        CACHE STRING "32-bit field. Read/write the selected histogram bin.")
