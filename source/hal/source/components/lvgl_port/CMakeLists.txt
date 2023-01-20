#  Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

#########################################################
# LVGL port library                                     #
#########################################################

cmake_minimum_required(VERSION 3.21.0)

project(lvgl_port_component
    DESCRIPTION     "LVGL port library"
    LANGUAGES       C CXX ASM)

# Add top level interface library
set(LVGL_PORT_IFACE_TARGET lvgl_port_iface)
add_library(${LVGL_PORT_IFACE_TARGET} INTERFACE)
target_include_directories(${LVGL_PORT_IFACE_TARGET} INTERFACE include)

if (${TARGET_PLATFORM} STREQUAL ensemble)
    add_subdirectory(source/ensemble)
endif()
