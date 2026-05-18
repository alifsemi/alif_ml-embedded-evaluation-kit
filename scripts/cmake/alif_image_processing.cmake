#  Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

set(AIPL_DIR ${MLEK_DEPENDENCY_ROOT_DIR}/aipl)
set(AIPL_SRC_DIR ${AIPL_DIR}/source)

add_library(aipl STATIC)

target_include_directories(aipl PUBLIC
    ${AIPL_DIR}/include
    ${AIPL_DIR}/include/default
    ${AIPL_DIR}/include/dave2d
    ${AIPL_DIR}/include/helium
)

target_include_directories(aipl PRIVATE
    ${AIPL_DIR}/external/include
    ${AIPL_DIR}/config
)

# Directly use config template if no config was provided
if (NOT EXISTS ${AIPL_DIR}/config/aipl_config.h)
    file(COPY ${AIPL_DIR}/aipl_config_template.h DESTINATION ${AIPL_DIR}/config)
    file(RENAME ${AIPL_DIR}/config/aipl_config_template.h ${AIPL_DIR}/config/aipl_config.h)
endif()

target_sources(aipl PRIVATE
    ${AIPL_SRC_DIR}/aipl_color_formats.c
    ${AIPL_SRC_DIR}/aipl_image.c
    ${AIPL_SRC_DIR}/aipl_error.c
    ${AIPL_SRC_DIR}/aipl_dave2d.c
    ${AIPL_SRC_DIR}/aipl_crop.c
    ${AIPL_SRC_DIR}/dave2d/aipl_crop_dave2d.c
    ${AIPL_SRC_DIR}/default/aipl_crop_default.c
    ${AIPL_SRC_DIR}/aipl_flip.c
    ${AIPL_SRC_DIR}/dave2d/aipl_flip_dave2d.c
    ${AIPL_SRC_DIR}/default/aipl_flip_default.c
    ${AIPL_SRC_DIR}/helium/aipl_flip_helium.c
    ${AIPL_SRC_DIR}/aipl_resize.c
    ${AIPL_SRC_DIR}/dave2d/aipl_resize_dave2d.c
    ${AIPL_SRC_DIR}/default/aipl_resize_default.c
    ${AIPL_SRC_DIR}/helium/aipl_resize_helium.c
    ${AIPL_SRC_DIR}/aipl_rotate.c
    ${AIPL_SRC_DIR}/dave2d/aipl_rotate_dave2d.c
    ${AIPL_SRC_DIR}/default/aipl_rotate_default.c
    ${AIPL_SRC_DIR}/helium/aipl_rotate_helium.c
    ${AIPL_SRC_DIR}/aipl_color_correction.c
    ${AIPL_SRC_DIR}/default/aipl_color_correction_default.c
    ${AIPL_SRC_DIR}/helium/aipl_color_correction_helium.c
    ${AIPL_SRC_DIR}/aipl_white_balance.c
    ${AIPL_SRC_DIR}/default/aipl_white_balance_default.c
    ${AIPL_SRC_DIR}/helium/aipl_white_balance_helium.c
    ${AIPL_SRC_DIR}/aipl_lut_transform.c
    ${AIPL_SRC_DIR}/default/aipl_lut_transform_default.c
    ${AIPL_SRC_DIR}/helium/aipl_lut_transform_helium.c
    ${AIPL_SRC_DIR}/aipl_color_conversion.c
    ${AIPL_SRC_DIR}/dave2d/aipl_color_conversion_dave2d.c
    ${AIPL_SRC_DIR}/helium/aipl_color_conversion_helium.c
    ${AIPL_SRC_DIR}/default/aipl_color_conversion_default.c
    ${AIPL_SRC_DIR}/aipl_demosaic.c
    ${AIPL_SRC_DIR}/default/aipl_demosaic_default.c
    ${AIPL_SRC_DIR}/helium/aipl_demosaic_helium.c
)

## Add dependencies
target_link_libraries(aipl PUBLIC
    rte_components
    cmsis_device
)
