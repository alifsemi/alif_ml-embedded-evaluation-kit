#/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement 
# *
# * You should have received a copy of the Alif Semiconductor Software 
# * License Agreement with this file. If not, please write to: 
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

cmake_minimum_required(VERSION 3.21.0)

project(ensemble_display
    DESCRIPTION     "Ensemble Display"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(ENSEMBLE_LCD_IFACE_TARGET ensemble_lcd_iface)
add_library(${ENSEMBLE_LCD_IFACE_TARGET} INTERFACE)
target_include_directories(${ENSEMBLE_LCD_IFACE_TARGET} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    include)

set(ENSEMBLE_DISPLAY_TARGET ensemble_display)
add_library(${ENSEMBLE_DISPLAY_TARGET} STATIC)

## Include directories - private
target_include_directories(${ENSEMBLE_DISPLAY_TARGET}
    PRIVATE
    source)

## Include directories - public
target_include_directories(${ENSEMBLE_DISPLAY_TARGET}
    PUBLIC
    include)

## Component sources
target_sources(${ENSEMBLE_DISPLAY_TARGET}
    PRIVATE
    src/LCD_Panel.c
    lvgl/Alif240.c
    lvgl/lv_port_disp.c
    lvgl/lv_paint_utils.c
)

# LCD_Panel.c adds scanline IRQ support, and needs to access internal header CDC200_dev.h to do it
target_include_directories(${LCD_ENSEMBLE_COMPONENT_TARGET} PRIVATE
    "$<TARGET_PROPERTY:cmsis_ensemble,SOURCE_DIR>/Alif_CMSIS/Source")


add_compile_options(-Werror-implicit-function-declaration)
# Compile definitions
# target_compile_definitions(${ENSEMBLE_DISPLAY_TARGET}
#     PRIVATE
#     CLCD_CONFIG_BASE=${CLCD_CONFIG_BASE})

## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

## LVGL and Arm-2D libraries
include(${CMAKE_SCRIPTS_DIR}/Arm-2D.cmake)
target_include_directories(${ARM_2D_TARGET} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/lvgl)

include(${CMAKE_SCRIPTS_DIR}/lvgl.cmake)
target_include_directories(${LVGL_TARGET} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/lvgl)
                           
# We configured LVGL to use Arm-2D, and it needs our ticker API
target_link_libraries(${LVGL_TARGET} PRIVATE ${ARM_2D_TARGET} ${LCD_IFACE_TARGET})

## Add dependencies
target_link_libraries(${ENSEMBLE_DISPLAY_TARGET} PUBLIC
    ${LCD_IFACE_TARGET}
    ${ENSEMBLE_LCD_IFACE_TARGET}
    image_iface
    log
    cmsis_ensemble
    rte_components
    ${LVGL_TARGET})

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${ENSEMBLE_DISPLAY_TARGET})
message(STATUS "*******************************************************")
