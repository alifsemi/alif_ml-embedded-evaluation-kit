# /* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
#  * Use, distribution and modification of this code is permitted under the
#  * terms stated in the Alif Semiconductor Software License Agreement
#  *
#  * You should have received a copy of the Alif Semiconductor Software
#  * License Agreement with this file. If not, please write to:
#  * contact@alifsemi.com, or visit: https://alifsemi.com/license
#  *
#  */

set(CMSIS_DEVICE_TARGET cmsis_device)
set(CPU_HEADER_TARGET rte_components)

project(${CMSIS_DEVICE_TARGET}
    DESCRIPTION     "Generic CMSIS start up file for Cortex-M targets"
    LANGUAGES       C CXX ASM)

# 1. We should be cross-compiling (non-native target)
if (NOT ${CMAKE_CROSSCOMPILING})
    message(FATAL_ERROR "No ${CMSIS_DEVICE_TARGET} support for this target.")
endif()

# 2. Check if CMSIS sources have been defined
if (NOT DEFINED CMSIS_SRC_PATH)
    message(FATAL_ERROR "CMSIS_SRC_PATH path should be defined for ${CMSIS_DEVICE_TARGET}.")
endif()

if (NOT DEFINED CORTEX_DFP_SRC_PATH)
    message(FATAL_ERROR "CORTEX_DFP_SRC_PATH path should be defined for ${CMSIS_DEVICE_TARGET}.")
endif()


string(TOLOWER "${ALIF_CORE}" ALIF_CORE_STRING)

# 3.1 Create an interface library for CPU header only
add_library(${CPU_HEADER_TARGET} INTERFACE)

## Interface include directories:
target_include_directories(${CPU_HEADER_TARGET}
    INTERFACE
    ${CMSIS_SRC_PATH}/CMSIS/Core/Include
    ${CMSIS_SRC_PATH}/CMSIS/Driver/Include
    ${CORTEX_DFP_SRC_PATH}/Device/${ARM_CPU}/Include
    ${BOARDLIB_PATH}
    )

target_include_directories(${CPU_HEADER_TARGET}
    INTERFACE
    ${ALIF_DEVICE_SKU}
    ${ALIF_DEVICE_SKU}/${ALIF_CORE_STRING}
    ${ALIF_CMSIS_PATH}/drivers/include
    ${ALIF_CMSIS_PATH}/Device/core/common/include
    ${ALIF_CMSIS_PATH}/Device/core/${ALIF_CORE_STRING}/config
    ${ALIF_CMSIS_PATH}/Device/soc/${ALIF_DEVICE_SKU}
    ${ALIF_CMSIS_PATH}/Device/soc/${ALIF_DEVICE_SKU}/include
    ${ALIF_CMSIS_PATH}/Device/soc/${ALIF_DEVICE_SKU}/include/${ALIF_CORE_STRING}
    ${ALIF_CMSIS_PATH}/Device/system/include
    )


# 3.2 Create static library
add_library(${CMSIS_DEVICE_TARGET} STATIC)

## Include directories - public
target_include_directories(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Include
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Include/config
    ${ALIF_CMSIS_PATH}/drivers/include
    ${ALIF_CMSIS_PATH}/components/Include
    ${ALIF_CMSIS_PATH}/Device/core/common/config
    ${BOARDLIB_PATH}
    ${ALIF_CMSIS_PATH}/ospi_xip/source/ospi # hopyfully this wont'd be needed and ospi_private.h will be moved (also change the name)
    )

target_include_directories(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source
    )


## Platform sources
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ALIF_CMSIS_PATH}/Device/core/common/source/cache.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/mpu.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/pm.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/sau_tcm_ns_setup.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/startup.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/system.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/tgu.c
    ${ALIF_CMSIS_PATH}/Device/core/common/source/vectors.c
    ${ALIF_CMSIS_PATH}/Device/system/source/sys_utils.c
    ${ALIF_CMSIS_PATH}/Device/system/source/sys_clocks.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_I2C.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_PDM.c
    ${ALIF_CMSIS_PATH}/drivers/source/cdc.c
    ${ALIF_CMSIS_PATH}/drivers/source/cpi.c
    ${ALIF_CMSIS_PATH}/drivers/source/pdm.c
    ${ALIF_CMSIS_PATH}/drivers/source/dma_ctrl.c
    ${ALIF_CMSIS_PATH}/drivers/source/dma_op.c
    ${ALIF_CMSIS_PATH}/drivers/source/dsi.c
    ${ALIF_CMSIS_PATH}/drivers/source/i2c.c
    ${ALIF_CMSIS_PATH}/drivers/source/i3c.c
    ${ALIF_CMSIS_PATH}/drivers/source/i2s.c
    ${ALIF_CMSIS_PATH}/drivers/source/pinconf.c
    ${ALIF_CMSIS_PATH}/drivers/source/uart.c
    ${ALIF_CMSIS_PATH}/drivers/source/ospi.c
    ${ALIF_CMSIS_PATH}/components/Source/driver_phy.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/DPHY_Common.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_OSPI.c
    ${ALIF_CMSIS_PATH}/components/Source/IS25WX256.c
    )

# Alif DFP RTE_Components does not support yet building the module if it is disabled
if (NOT IS_BALLETTO_DEVICE)
    target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_CPI.c
    ${ALIF_CMSIS_PATH}/drivers/source/csi.c
    $<$<STREQUAL:${ALIF_CAMERA_MODULE},MT9M114>:${ALIF_CMSIS_PATH}/components/Source/MT9M114_Camera_Sensor.c>
    $<$<STREQUAL:${ALIF_CAMERA_MODULE},ARX3A0>:${ALIF_CMSIS_PATH}/components/Source/arx3A0_camera_sensor.c>
    )
endif() # not DevKit_E1C


target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_GPIO.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_IO.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_I2S.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_USART.c
    )

# Alif DFP RTE_Components does not support yet building the module if it is disabled
if (NOT IS_BALLETTO_DEVICE)
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_HWSEM.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_I3C.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_DMA.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_CDC200.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_MIPI_DSI.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Driver_MIPI_CSI2.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/Camera_Sensor_i2c.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/DPHY_DSI.c
    ${ALIF_CMSIS_PATH}/Alif_CMSIS/Source/DPHY_CSI2.c
    ${ALIF_CMSIS_PATH}/components/Source/ILI9806E_LCD_panel.c
)
endif()

## Board sources
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${BOARDLIB_PATH}/appkit_gen2/board_init.c
    ${BOARDLIB_PATH}/devkit_gen2/board_init.c
    ${BOARDLIB_PATH}/devkit_e1c/board_init.c
    ${BOARDLIB_PATH}/devkit_e8/board_init.c
    )


# Device definition needs to be set, is checked in source files to include correct header
target_compile_definitions(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${ARM_CPU}
    ${ALIF_CORE}
    TARGET_BOARD=BOARD_${TARGET_BOARD}
    ALIF_CAMERA_MODULE_${ALIF_CAMERA_MODULE}=1)

# Tell linker that reset interrupt handler is our entry point
target_link_options(
    ${CMSIS_DEVICE_TARGET}
    INTERFACE
    --entry Reset_Handler)

message(STATUS "Alif cmsis-pack: SE_SERVICES_SUPPORT: ${SE_SERVICES_SUPPORT}")

# Link libraries
target_link_libraries(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${CPU_HEADER_TARGET}
    $<$<BOOL:${SE_SERVICES_SUPPORT}>:alif_se_services>
)

# Check if semihosting configuration is available
if (COMMAND configure_semihosting)
    configure_semihosting(${CMSIS_DEVICE_TARGET} ${CONSOLE_SEMIHOSTING})
endif()

# 4 Display status:arm_ethosu_npu_irq_handler
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${CMSIS_DEVICE_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_SYSTEM_ARCH                      : " ${CMAKE_SYSTEM_ARCH})
message(STATUS "*******************************************************")
