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
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build_baremetal)
set(PLAT_HAL ${CMAKE_CURRENT_SOURCE_DIR}/source/application/hal/platforms/bare-metal)

# If target platform not defined raise an error
# TARGET_PLATFORM either should have been defined by the user or set to default value mps3
if (NOT DEFINED TARGET_PLATFORM)
    message(FATAL_ERROR "Invalid target platform, specify TARGET_PLATFORM=mps3")
endif ()
message(STATUS "target platform ${TARGET_PLATFORM}")

set(SOURCE_GEN_DIR          ${CMAKE_BINARY_DIR}/generated/bsp)
if (NOT DEFINED MEM_PROFILES_SRC_DIR)
    set(MEM_PROFILES_SRC_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/subsystem-profiles)
endif()

set(MEM_PROFILE_TEMPLATE    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/templates/peripheral_memmap.h.template)
set(IRQ_PROFILE_TEMPLATE    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/templates/peripheral_irqs.h.template)
set(MEM_REGIONS_TEMPLATE    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/templates/mem_regions.h.template)
set(TA_SETTINGS_TEMPLATE    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake/templates/timing_adapter_settings.template)
set(LINKER_SCRIPT_DIR       "${PLAT_HAL}/bsp/mem_layout")
set(TENSORFLOW_LITE_MICRO_PLATFORM_LIB_NAME  "libtensorflow-microlite.a")
set(TENSORFLOW_LITE_MICRO_FLAG               "-DTF_LITE_STATIC_MEMORY")
set(ETHOS_U55_FLAG                           "-DARM_NPU=1")

if (ETHOS_U55_ENABLED)
    set(OPTIONAL_FLAGS      "${OPTIONAL_FLAGS} ${ETHOS_U55_FLAG}")
endif ()

# Set specific flags depending on target platform and subsystem
if (TARGET_PLATFORM STREQUAL mps3)
    set(MPS3_PLATFORM_FLAG          "-DMPS3_PLATFORM=1")

    # If target platform is mps3 and subsystem not defined raise an error,
    # TARGET_SUBSYSTEM either should have been defined by the user or set to a default value
    if (NOT DEFINED TARGET_SUBSYSTEM)
        message(FATAL_ERROR "Target subsystem for mps3 undefined, "
                            "specify -DTARGET_SUBSYSTEM=<sse-300>")
    endif ()

    if (TARGET_SUBSYSTEM STREQUAL sse-300)
        message(STATUS          "target subsystem is ${TARGET_SUBSYSTEM}")
        set(BSP_PACKAGE_DIR     "${PLAT_HAL}/bsp/bsp-packs/mps3")
        set(LINKER_SCRIPT_NAME  "${TARGET_PLATFORM}-${TARGET_SUBSYSTEM}")

        # Include the mem profile definitions specific to our target subsystem
        include(${MEM_PROFILES_SRC_DIR}/corstone-${TARGET_SUBSYSTEM}.cmake)
        set(OPTIONAL_FLAGS      "${OPTIONAL_FLAGS} ${MPS3_PLATFORM_FLAG}")

        # For deployment on the MPS3 FPGA platform, we need to produce
        # two bin files - one that is loaded into the ITCM, and another
        # that is loaded into the DDR region.
        set(LINKER_SECTION_TAGS     "*.at_itcm" "*.at_ddr")
        set(LINKER_OUTPUT_BIN_TAGS  "itcm.bin"  "ddr.bin")
        set(MPS3_FPGA_CONFIG        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/${TARGET_PLATFORM}/${TARGET_SUBSYSTEM}/images.txt")
    else ()
        message(FATAL_ERROR "Non compatible target subsystem: ${TARGET_SUBSYSTEM}")
    endif ()
elseif (TARGET_PLATFORM STREQUAL simple_platform)
    set(BSP_PACKAGE_DIR         "${PLAT_HAL}/bsp/bsp-packs/${TARGET_PLATFORM}")
    set(LINKER_SCRIPT_NAME      "${TARGET_PLATFORM}")
    include(${MEM_PROFILES_SRC_DIR}/${TARGET_PLATFORM}.cmake)
    set(OPTIONAL_FLAGS          "${OPTIONAL_FLAGS}")
    set(LINKER_SECTION_TAGS     "*.at_itcm" "*.at_ddr")
    set(LINKER_OUTPUT_BIN_TAGS  "itcm.bin"  "ddr.bin")
else ()
    message(FATAL_ERROR "Non compatible target platform ${TARGET_PLATFORM}")
endif ()

# Add link options for the linker script to be used:
add_linker_script(${LINKER_SCRIPT_DIR} ${LINKER_SCRIPT_NAME})

if (ETHOS_U55_ENABLED)
    USER_OPTION(TA_CONFIG_FILE "Path to the timing adapter configuration file"
            "${CMAKE_SCRIPTS_DIR}/ta_config.cmake"
            FILEPATH)

    # must be included after target subsystem CMake file
    include(${TA_CONFIG_FILE})
endif()

# Generate the memory map header file from the mem profile cmake included in one of
# the previous sections
message(STATUS "Configuring file from ${MEM_PROFILE_TEMPLATE}"
                                   ", ${IRQ_PROFILE_TEMPLATE}"
                                " and ${MEM_REGIONS_TEMPLATE}")

configure_file("${MEM_PROFILE_TEMPLATE}" "${SOURCE_GEN_DIR}/peripheral_memmap.h")
configure_file("${IRQ_PROFILE_TEMPLATE}" "${SOURCE_GEN_DIR}/peripheral_irqs.h")
configure_file("${MEM_REGIONS_TEMPLATE}" "${SOURCE_GEN_DIR}/mem_regions.h")
configure_file("${TA_SETTINGS_TEMPLATE}" "${SOURCE_GEN_DIR}/timing_adapter_settings.h")

message(STATUS "Using BSP package from: ${BSP_PACKAGE_DIR}")

if (DEFINED VERIFY_TEST_OUTPUT)
    message(STATUS "Test output verification flag is: ${VERIFY_TEST_OUTPUT}")
    set(OPTIONAL_FLAGS "${OPTIONAL_FLAGS} -DVERIFY_TEST_OUTPUT=${VERIFY_TEST_OUTPUT}")
endif ()

if (DEFINED LOG_LEVEL)
    message(STATUS "Setting log level to ${LOG_LEVEL}")
    set(OPTIONAL_FLAGS "${OPTIONAL_FLAGS} -DLOG_LEVEL=${LOG_LEVEL}")
endif()

if (DEFINED ACTIVATION_BUF_SRAM_SZ)
    message(STATUS "Maximum SRAM space for activations buffers for this system: ${ACTIVATION_BUF_SRAM_SZ}")
    set(OPTIONAL_FLAGS "${OPTIONAL_FLAGS} -DACTIVATION_BUF_SRAM_SZ=${ACTIVATION_BUF_SRAM_SZ}")
endif()

if (DEFINED ARMCLANG_DEBUG_DWARF_LEVEL)
    message(STATUS "setting dwarf conformance level to gdwarf-${ARMCLANG_DEBUG_DWARF_LEVEL}")
    set(OPTIONAL_FLAGS "${OPTIONAL_FLAGS} -gdwarf-${ARMCLANG_DEBUG_DWARF_LEVEL}")
endif()

set(COMPILER_FLAGS              "${TENSORFLOW_LITE_MICRO_FLAG} ${PROFILING_OPT} ${OPTIONAL_FLAGS}")
# For some reason, cmake doesn't pass the c++ standard flag, adding it manually
set(CMAKE_CXX_FLAGS             "${COMPILER_FLAGS} -std=c++11" CACHE INTERNAL "")
set(CMAKE_C_FLAGS               "${COMPILER_FLAGS}" CACHE INTERNAL "")
set(CMAKE_ASM_COMPILE_OBJECT    ${CMAKE_CXX_FLAGS})

# Tell linker that reset interrupt handler is our entry point
add_link_options(--entry Reset_Handler)

set(PLAT_BSP_INCLUDES
    ${PLAT_HAL}/bsp/cmsis-device/include
    ${PLAT_HAL}/bsp/include/
    ${PLAT_HAL}/bsp/bsp-core/include
    ${BSP_PACKAGE_DIR}/include
)

# Include directories:
set(PLAT_INCLUDE_DIRS
    ${PLAT_BSP_INCLUDES}
    ${PLAT_HAL}/utils/include
    ${PLAT_HAL}/images/include
    ${PLAT_HAL}/data_presentation/lcd/include
    ${PLAT_HAL}/timer/include
    ${SOURCE_GEN_DIR}
    )

# Source files
file(GLOB_RECURSE SRC_PLAT_HAL

    # Higher level HAL sources - software logic implementations
    "${PLAT_HAL}/data_*/*.c"
    "${PLAT_HAL}/images/*.c"
    "${PLAT_HAL}/timer/*.c"
    "${PLAT_HAL}/utils/*.c"

    # Low level HAL sources - these enable interaction with
    # the actual hardware
    "${PLAT_HAL}/bsp/cmsis-device/*.c"
    "${PLAT_HAL}/bsp/bsp-core/*.c"
    "${BSP_PACKAGE_DIR}/*.c"
    )

# Special retarget source to direct stdin, stdout and stderr streams to the
# UART block.
set(PLAT_RETARGET_SOURCE "${PLAT_HAL}/bsp/bsp-core/retarget.c")
