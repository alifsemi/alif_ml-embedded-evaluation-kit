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

#----------------------------------------------------------------------------
# CMake description file for the Ethos-U55 Timing Adapter settings (single
# NPU core with two AXIs).
#----------------------------------------------------------------------------

set(TA0_BASE "${SEC_ETHOS_U_NPU_TA0_BASE}"   CACHE STRING "Timing adapter 0: base-address")
set(TA1_BASE "${SEC_ETHOS_U_NPU_TA1_BASE}"   CACHE STRING "Timing adapter 1: base-address")

message(STATUS "using TA0_BASE @ ${TA0_BASE}; TA1_BASE @ ${TA1_BASE}.")

# Timing adapter settings for AXI0
set(TA0_MAXR        "16"       CACHE STRING "6-bit field. Max no. of pending reads. 0=infinite")
set(TA0_MAXW        "16"       CACHE STRING "6-bit field. Max no. of pending writes. 0=infinite")
set(TA0_MAXRW       "0"        CACHE STRING "6-bit field. Max no. of pending reads+writes. 0=infinite")
set(TA0_RLATENCY    "32"       CACHE STRING "12-bit field. Minimum latency (clock cycles) from AVALID to RVALID.")
set(TA0_WLATENCY    "32"       CACHE STRING "12-bit field. Minimum latency (clock cycles) from WVALID&WLAST to BVALID.")
set(TA0_PULSE_ON    "3999"     CACHE STRING "No. of cycles addresses let through (0-65535).")
set(TA0_PULSE_OFF   "1"        CACHE STRING "No. of cycles addresses blocked (0-65535).")
set(TA0_BWCAP       "4000"     CACHE STRING "16-bit field. Max no. of 64-bit words transfered per pulse cycle 0=infinite")
set(TA0_PERFCTRL    "0"        CACHE STRING "6-bit field selecting an event for event counter 0=default")
set(TA0_PERFCNT     "0"        CACHE STRING "32-bit event counter")
set(TA0_MODE        "1"        CACHE STRING "Bit 0: 1=enable dynamic clocking to avoid underrun;
                                             Bit 1: 1=enable random AR reordering (0=default);
                                             Bit 2: 1=enable random R reordering (0=default);
                                             Bit 3: 1=enable random B reordering (0=default);
                                             Bit 11-4: Frequency scale 0=full speed, 255=(1/256) speed")
set(TA0_HISTBIN     "0"        CACHE STRING "Controls which histogram bin (0-15) that should be accessed by HISTCNT.")
set(TA0_HISTCNT     "0"        CACHE STRING "32-bit field. Read/write the selected histogram bin.")

# Timing adapter settings for AXI1
set(TA1_MAXR        "24"       CACHE STRING "6-bit field. Max no. of pending reads. 0=infinite")
set(TA1_MAXW        "12"       CACHE STRING "6-bit field. Max no. of pending writes. 0=infinite")
set(TA1_MAXRW       "0"        CACHE STRING "6-bit field. Max no. of pending reads+writes. 0=infinite")
set(TA1_RLATENCY    "500"      CACHE STRING "12-bit field. Minimum latency (clock cycles) from AVALID to RVALID.")
set(TA1_WLATENCY    "250"      CACHE STRING "12-bit field. Minimum latency (clock cycles) from WVALID&WLAST to BVALID.")
set(TA1_PULSE_ON    "4000"     CACHE STRING "No. of cycles addresses let through (0-65535).")
set(TA1_PULSE_OFF   "1000"     CACHE STRING "No. of cycles addresses blocked (0-65535).")
set(TA1_BWCAP       "1172"     CACHE STRING "16-bit field. Max no. of 64-bit words transfered per pulse cycle 0=infinite")
set(TA1_PERFCTRL    "0"        CACHE STRING "6-bit field selecting an event for event counter 0=default")
set(TA1_PERFCNT     "0"        CACHE STRING "32-bit event counter")
set(TA1_MODE        "1"        CACHE STRING "Bit 0: 1=enable dynamic clocking to avoid underrun;
                                            Bit 1: 1=enable random AR reordering (0=default);
                                            Bit 2: 1=enable random R reordering (0=default);
                                            Bit 3: 1=enable random B reordering (0=default);
                                            Bit 11-4: Frequency scale 0=full speed, 255=(1/256) speed")
set(TA1_HISTBIN     "0"        CACHE STRING "Controls which histogram bin (0-15) that should be accessed by HISTCNT.")
set(TA1_HISTCNT     "0"        CACHE STRING "32-bit field. Read/write the selected histogram bin.")
