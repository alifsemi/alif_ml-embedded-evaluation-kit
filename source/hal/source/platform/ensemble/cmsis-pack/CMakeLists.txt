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
    config/${ENSEMBLE_CORE}
    Device/${ENSEMBLE_CORE}/Include
    Device/${ENSEMBLE_CORE}/Config
    ${CMSIS_SRC_PATH}/CMSIS/Core/Include
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Include
    )

# 3.2 Create static library
add_library(${CMSIS_DEVICE_TARGET} STATIC)

## Include directories - public
target_include_directories(${CMSIS_DEVICE_TARGET}
    PUBLIC
    Alif_CMSIS/Include
    components/Include
    )

target_include_directories(${CMSIS_DEVICE_TARGET}
    PRIVATE
    Alif_CMSIS/Source
    )

## Platform sources
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    Device/${ENSEMBLE_CORE}/Source/mpu_${ENSEMBLE_CORE}.c
    Device/${ENSEMBLE_CORE}/Source/startup_${ENSEMBLE_CORE}.c
    Device/${ENSEMBLE_CORE}/Source/system_${ENSEMBLE_CORE}.c
    Device/${ENSEMBLE_CORE}/Source/system_utils.c
    Device/${ENSEMBLE_CORE}/Source/tgu_${ENSEMBLE_CORE}.c
    Alif_CMSIS/Source/Driver_GPIO.c
    Alif_CMSIS/Source/Driver_I2S.c
    Alif_CMSIS/Source/Driver_I3C.c
    Alif_CMSIS/Source/Driver_PINMUX_AND_PINPAD.c
    Alif_CMSIS/Source/Driver_USART.c
    Alif_CMSIS/Source/GPIO_ll_drv.c
    Alif_CMSIS/Source/LPTIMER_ll_drv.c
    components/Source/PHY_Generic.c
    Alif_CMSIS/Source/Driver_DMA.c
    Alif_CMSIS/Source/DMA_Opcode.c
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:Alif_CMSIS/Source/Driver_CDC200.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:Alif_CMSIS/Source/Driver_MIPI_DSI.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:Alif_CMSIS/Source/DPHY_init.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:components/Source/ILI9806E_LCD_panel.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:Alif_CMSIS/Source/Driver_Camera_Controller.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:components/Source/arx3A0_camera_sensor.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:Alif_CMSIS/Source/Driver_MIPI_CSI2.c>
    $<$<STREQUAL:${ENSEMBLE_CORE},M55_HP>:Alif_CMSIS/Source/Camera_Sensor_i2c.c>
    )

# Device definition needs to be set, is checked in source files to include correct header
target_compile_definitions(${CMSIS_DEVICE_TARGET} PUBLIC ${ARM_CPU})

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
    configure_semihosting(${CMSIS_DEVICE_TARGET} OFF)
endif()

# 4 Display status:arm_ethosu_npu_irq_handler
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${CMSIS_DEVICE_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_SYSTEM_ARCH                      : " ${CMAKE_SYSTEM_ARCH})
message(STATUS "*******************************************************")
