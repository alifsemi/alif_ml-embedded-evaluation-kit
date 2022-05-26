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
    include
    ${CMSIS_SRC_PATH}/CMSIS/Driver/Include
    Device/${ENSEMBLE_CORE}
    Device/${ENSEMBLE_CORE}/Include
    Device/${ENSEMBLE_CORE}/Config
    CMSIS
    CMSIS/Include
    CMSIS/Driver/Inc
    ${CMSIS_SRC_PATH}/CMSIS/Core/Include
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Include
    ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Include/Template)

# 3.2 Create static library
add_library(${CMSIS_DEVICE_TARGET} STATIC)

## Include directories - public
target_include_directories(${CMSIS_DEVICE_TARGET}
    PUBLIC
    ${CMSIS_SRC_PATH}/CMSIS/Driver/Include
    Device/${ENSEMBLE_CORE}
    Device/${ENSEMBLE_CORE}/Include
    Device/${ENSEMBLE_CORE}/Config
    CMSIS
    CMSIS/Include
    CMSIS/Driver/Inc
    )


## Platform sources
target_sources(${CMSIS_DEVICE_TARGET}
    PRIVATE
    Device/${ENSEMBLE_CORE}/Source/mpu_M55_HP.c
    Device/${ENSEMBLE_CORE}/Source/startup_M55_HP.c
    Device/${ENSEMBLE_CORE}/Source/system_M55_HP.c
    Device/${ENSEMBLE_CORE}/Source/system_utils.c
    Device/${ENSEMBLE_CORE}/Source/tgu_M55_HP.c
    CMSIS/Driver/Src/Driver_ADC.c
    CMSIS/Driver/Src/Driver_GPIO.c
    CMSIS/Driver/Src/Driver_I2S.c
    CMSIS/Driver/Src/Driver_I3C.c
    CMSIS/Driver/Src/Driver_LPTIMER.c
    CMSIS/Driver/Src/Driver_PINMUX_AND_PINPAD.c
    CMSIS/Driver/Src/Driver_USART.c
    CMSIS/Driver/Src/Driver_UTIMER.c
    CMSIS/Driver/Src/GPIO_ll_drv.c
    CMSIS/Driver/Src/LPTIMER_ll_drv.c
    CMSIS/Driver/Src/PHY_Generic.c
    CMSIS/Driver/Src/UTIMER_ll_drv.c
    )

## Sources - private
# target_sources(${CMSIS_DEVICE_TARGET}
#     PRIVATE
#     ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Source/system_${ARM_CPU}.c
#     ${CMSIS_SRC_PATH}/Device/ARM/${ARM_CPU}/Source/startup_${ARM_CPU}.c)

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
    ${CPU_HEADER_TARGET})

#configure_semihosting(${CMSIS_DEVICE_TARGET} OFF)
# Check if semihosting configuration is available
# if (COMMAND configure_semihosting)
#     configure_semihosting(${CMSIS_DEVICE_TARGET} OFF)
# endif()

# 4 Display status:arm_ethosu_npu_irq_handler
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${CMSIS_DEVICE_TARGET})
message(STATUS "CMAKE_SYSTEM_PROCESSOR                 : " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_SYSTEM_ARCH                      : " ${CMAKE_SYSTEM_ARCH})
message(STATUS "*******************************************************")
