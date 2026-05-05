#  Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

set(LVGL_PORT_DIR ${MLEK_DEPENDENCY_ROOT_DIR}/alif_lvgl-dave2d/lv_dave2d)
set(LVGL_PORT_SRC_DIR ${LVGL_PORT_DIR}/src)

add_library(alif_lvgl-dave2d)

target_include_directories(alif_lvgl-dave2d PUBLIC
    ${LVGL_PORT_DIR}/inc
)

target_include_directories(alif_lvgl-dave2d PRIVATE
    ${LVGL_PORT_DIR}
    ${LVGL_PORT_SRC_DIR}
)

# alif_lvgl-dave2d sources are missing some includes
# this is a workaround
# string(TOLOWER "${ALIF_CORE}" ALIF_CORE_STRING)

# target_precompile_headers(alif_lvgl-dave2d
#   PRIVATE
#   ${ENSEMBLE_CMSIS_PATH}/Device/core/${ENSEMBLE_CORE}/include/${ENSEMBLE_CORE}.h
# )
# ${ALIF_CMSIS_PATH}/Device/soc/${ALIF_DEVICE_SKU}/include/${ALIF_CORE_STRING}

target_sources(alif_lvgl-dave2d PRIVATE
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_arc.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_border.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_fill.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_image.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_label.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_line.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_mask_rectangle.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_triangle.c
    ${LVGL_PORT_SRC_DIR}/lv_draw_dave2d_utils.c
)

## Add dependencies
target_link_libraries(alif_lvgl-dave2d PUBLIC
    cmsis_device
    lvgl_config_interface
    alif_dave2d-driver
)
