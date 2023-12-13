set(CMSIS_DEVICE_TARGET cmsis_ensemble)
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

# 3.1 Create an interface library for CPU header only
add_library(${CPU_HEADER_TARGET} INTERFACE)

## Interface include directories:
target_include_directories(${CPU_HEADER_TARGET}
    INTERFACE
    ${CMSIS_SRC_PATH}/CMSIS/Core/Include
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Include
    ${BOARDLIB_PATH}
    )

if (TARGET_REVISION STREQUAL "A")
target_include_directories(${CPU_HEADER_TARGET}
    INTERFACE
    config
    config/${ENSEMBLE_CORE}
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Include
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Config
    )
else()
target_include_directories(${CPU_HEADER_TARGET}
    INTERFACE
    config-b
    config-b/${ENSEMBLE_CORE}
    ${ENSEMBLE_CMSIS_PATH}/drivers/include
    ${ENSEMBLE_CMSIS_PATH}/Device/common/include
    ${ENSEMBLE_CMSIS_PATH}/Device/core/${ENSEMBLE_CORE}/include
    ${ENSEMBLE_CMSIS_PATH}/Device/core/${ENSEMBLE_CORE}/config
    )
endif()

# 3.2 Create static library
add_library(${CMSIS_DEVICE_TARGET} STATIC)

## Include directories - public
target_include_directories(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Include
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Include/config
    ${ENSEMBLE_CMSIS_PATH}/drivers/include
    ${ENSEMBLE_CMSIS_PATH}/components/Include
    ${ENSEMBLE_CMSIS_PATH}/Device/common/config
    ${BOARDLIB_PATH}
    )

target_include_directories(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source
    )

## Platform sources
if (TARGET_REVISION STREQUAL "A")
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Source/mpu_${ENSEMBLE_CORE}.c
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Source/startup_${ENSEMBLE_CORE}.c
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Source/system_${ENSEMBLE_CORE}.c
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Source/system_utils.c
    ${ENSEMBLE_CMSIS_PATH}/Device/${ENSEMBLE_CORE}/Source/tgu_${ENSEMBLE_CORE}.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/DMA_Opcode.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_Camera_Controller.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_PINMUX_AND_PINPAD.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/GPIO_ll_drv.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/drv_i2c_bitbang.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/LPTIMER_ll_drv.c
    ${ENSEMBLE_CMSIS_PATH}/components/Source/PHY_Generic.c
    )
else()
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ENSEMBLE_CMSIS_PATH}/Device/core/${ENSEMBLE_CORE}/source/startup_${ENSEMBLE_CORE}.c
    ${ENSEMBLE_CMSIS_PATH}/Device/common/source/mpu_M55.c
    ${ENSEMBLE_CMSIS_PATH}/Device/common/source/system_M55.c
    ${ENSEMBLE_CMSIS_PATH}/Device/common/source/system_utils.c
    ${ENSEMBLE_CMSIS_PATH}/Device/common/source/tgu_M55.c
    ${ENSEMBLE_CMSIS_PATH}/Device/common/source/pm.c
    ${ENSEMBLE_CMSIS_PATH}/Device/common/source/clk.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_CPI.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_I2C.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_PDM.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_OSPI.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/cdc.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/cpi.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/csi.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/pdm.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/dma_ctrl.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/dma_op.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/dsi.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/i2c.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/i3c.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/i2s.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/pinconf.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/uart.c
    ${ENSEMBLE_CMSIS_PATH}/drivers/source/ospi.c
    ${ENSEMBLE_CMSIS_PATH}/components/Source/driver_phy.c
    ${ENSEMBLE_CMSIS_PATH}/components/Source/IS25WX256.c
    )
endif()


target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_GPIO.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_I2S.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_I3C.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_HWSEM.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_USART.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_DMA.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_CDC200.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_MIPI_DSI.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/DPHY_init.c
    ${ENSEMBLE_CMSIS_PATH}/components/Source/ILI9806E_LCD_panel.c
    ${ENSEMBLE_CMSIS_PATH}/components/Source/arx3A0_camera_sensor.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Driver_MIPI_CSI2.c
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source/Camera_Sensor_i2c.c
    )

## Board sources
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    ${BOARDLIB_PATH}/appkit_alpha1/board_init.c
    ${BOARDLIB_PATH}/appkit_alpha2/board_init.c
    ${BOARDLIB_PATH}/appkit_gen2/board_init.c
    ${BOARDLIB_PATH}/devkit_gen1/board_init.c
    ${BOARDLIB_PATH}/devkit_gen2/board_init.c
    ${BOARDLIB_PATH}/devkit_b0_cob/board_init.c
    )


# Device definition needs to be set, is checked in source files to include correct header
target_compile_definitions(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${ARM_CPU}
    ${ENSEMBLE_CORE}
    TARGET_BOARD=BOARD_${TARGET_BOARD})

# Tell linker that reset interrupt handler is our entry point
target_link_options(
    ${CMSIS_DEVICE_TARGET}
    INTERFACE
    --entry Reset_Handler)

# Link libraries
target_link_libraries(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${CPU_HEADER_TARGET}
    ensemble_services)

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
