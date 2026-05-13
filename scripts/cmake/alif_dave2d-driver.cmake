#  Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
#  Use, distribution and modification of this code is permitted under the
#  terms stated in the Alif Semiconductor Software License Agreement
#
#  You should have received a copy of the Alif Semiconductor Software
#  License Agreement with this file. If not, please write to:
#  contact@alifsemi.com, or visit: https://alifsemi.com/license

set(DAVE2D_DRIVER_DIR ${MLEK_DEPENDENCY_ROOT_DIR}/alif_dave2d-driver)
set(DAVE2D_DRIVER_D0_SRC_DIR ${DAVE2D_DRIVER_DIR}/d0/src)
set(DAVE2D_DRIVER_D1_SRC_DIR ${DAVE2D_DRIVER_DIR}/d1/src)
set(DAVE2D_DRIVER_D2_SRC_DIR ${DAVE2D_DRIVER_DIR}/d2/src)

add_library(alif_dave2d-driver)

target_include_directories(alif_dave2d-driver PUBLIC
    ${DAVE2D_DRIVER_DIR}/d0/inc
    ${DAVE2D_DRIVER_DIR}/d1/inc
    ${DAVE2D_DRIVER_DIR}/d2/inc
    ${DAVE2D_DRIVER_DIR}
)

# d0 sources
target_sources(alif_dave2d-driver PRIVATE
    ${DAVE2D_DRIVER_D0_SRC_DIR}/dave_d0lib.c
    ${DAVE2D_DRIVER_D0_SRC_DIR}/dave_d0_mm_dynamic.c
    ${DAVE2D_DRIVER_D0_SRC_DIR}/dave_d0_mm_fixed_range_fixed_blkcnt.c
    ${DAVE2D_DRIVER_D0_SRC_DIR}/dave_d0_mm_fixed_range.c
)

# d1 sources
target_sources(alif_dave2d-driver PRIVATE
    ${DAVE2D_DRIVER_D1_SRC_DIR}/dave_base.c
    ${DAVE2D_DRIVER_D1_SRC_DIR}/dave_irq.c
    ${DAVE2D_DRIVER_D1_SRC_DIR}/dave_memory.c
)

# d2 sources
target_sources(alif_dave2d-driver PRIVATE
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_64bitoperation.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_blit.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_box.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_circle.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_context.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_curve.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_dlist.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_driver.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_edge.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_errorcodes.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_gradient.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_hardware.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_line.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_math.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_memory.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_pattern.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_perfcount.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_polyline.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_quad.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_rbuffer.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_render.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_texture.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_wedge.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_triangle.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_utility.c
    ${DAVE2D_DRIVER_D2_SRC_DIR}/dave_viewport.c
)

## Add dependencies
target_link_libraries(alif_dave2d-driver PUBLIC
    rte_components
    cmsis_device
)
