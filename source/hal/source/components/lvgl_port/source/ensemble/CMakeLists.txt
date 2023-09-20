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

project(lvgl_port
    DESCRIPTION     "Ensemble LVGL port"
    LANGUAGES       C CXX ASM)

set(ROTATE_DISPLAY "0" CACHE STRING "Rotate display by 0, 90, 180 or 270 degrees")
set_property(CACHE ROTATE_DISPLAY PROPERTY STRINGS "0" "90" "180" "270")

set(LV_THEME_DEFAULT_DARK "1" CACHE STRING "0: Light mode; 1: Dark mode")
set_property(CACHE LV_THEME_DEFAULT_DARK PROPERTY STRINGS "0" "1")

## LVGL and Arm-2D libraries
include(${CMAKE_SCRIPTS_DIR}/Arm-2D.cmake)
target_include_directories(${ARM_2D_TARGET} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(${CMAKE_SCRIPTS_DIR}/lvgl.cmake)
target_include_directories(${LVGL_TARGET} PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)

# We configured LVGL to use Arm-2D and it needs to reach lv_port.h
target_link_libraries(${LVGL_TARGET} PRIVATE ${ARM_2D_TARGET})
target_link_libraries(${LVGL_TARGET} PUBLIC lvgl_port_iface)

# We bind the port code into LVGL itself
target_sources(${LVGL_TARGET}
    PRIVATE
    source/lv_port.c
    source/LCD_Panel.c
)

# LCD_Panel.c adds scanline IRQ support, and needs to access internal header CDC200_dev.h to do it
target_include_directories(${LVGL_TARGET} PRIVATE
    ${ENSEMBLE_CMSIS_PATH}/Alif_CMSIS/Source
)

# Compile definitions
target_compile_definitions(${LVGL_TARGET}
    PRIVATE
    ROTATE_DISPLAY=${ROTATE_DISPLAY}
    PUBLIC
    LV_THEME_DEFAULT_DARK=${LV_THEME_DEFAULT_DARK})

target_link_libraries(${LVGL_TARGET} PRIVATE
    cmsis_ensemble
    platform_drivers_core)
