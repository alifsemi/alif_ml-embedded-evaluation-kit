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
    include)

## Include directories - private
target_include_directories(${LCD_ENSEMBLE_COMPONENT_TARGET}
    PRIVATE
    source)

## Component sources
target_sources(${LCD_ENSEMBLE_COMPONENT_TARGET}
    PRIVATE
    glcd_ensemble.c
    Alif240.c
    src/LCD_Panel.c
    src/image_buff.c
    lvgl/lv_port_disp.c
)


add_compile_options(-Werror-implicit-function-declaration)
# Compile definitions
# target_compile_definitions(${LCD_ENSEMBLE_COMPONENT_TARGET}
#     PRIVATE
#     CLCD_CONFIG_BASE=${CLCD_CONFIG_BASE})

## Logging utilities:
if (NOT TARGET log)
    if (NOT DEFINED LOG_PROJECT_DIR)
        message(FATAL_ERROR "LOG_PROJECT_DIR needs to be defined.")
    endif()
    add_subdirectory(${LOG_PROJECT_DIR} ${CMAKE_BINARY_DIR}/log)
endif()

set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/lv_conf.h CACHE STRING "" FORCE)
add_subdirectory(${DEPENDENCY_ROOT_DIR}/lvgl ${CMAKE_BINARY_DIR}/lvgl)

## Add dependencies
target_link_libraries(${LCD_ENSEMBLE_COMPONENT_TARGET} PUBLIC
    ${ENSEMBLE_LCD_IFACE_TARGET}
    log
    cmsis_ensemble
    rte_components
    lvgl)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${LCD_ENSEMBLE_COMPONENT_TARGET})
message(STATUS "*******************************************************")
