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
    include
    lvgl
    lvgl/src)

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
    src/mipi_dsi_host.c
    src/image_buff.c
    lvgl/lv_port_disp.c
    lvgl/src/core/lv_disp.c
    lvgl/src/core/lv_event.c
    lvgl/src/core/lv_group.c
    lvgl/src/core/lv_indev_scroll.c
    lvgl/src/core/lv_indev.c
    lvgl/src/core/lv_obj_class.c
    lvgl/src/core/lv_obj_draw.c
    lvgl/src/core/lv_obj_pos.c
    lvgl/src/core/lv_obj_scroll.c
    lvgl/src/core/lv_obj_style_gen.c
    lvgl/src/core/lv_obj_style.c
    lvgl/src/core/lv_obj_tree.c
    lvgl/src/core/lv_obj.c
    lvgl/src/core/lv_refr.c
    lvgl/src/core/lv_theme.c
    lvgl/src/draw/sw/lv_draw_sw.c
    lvgl/src/draw/sw/lv_draw_sw_arc.c
    lvgl/src/draw/sw/lv_draw_sw_blend.c
    lvgl/src/draw/sw/lv_draw_sw_gradient.c
    lvgl/src/draw/sw/lv_draw_sw_img.c
    lvgl/src/draw/sw/lv_draw_sw_letter.c
    lvgl/src/draw/sw/lv_draw_sw_line.c
    lvgl/src/draw/sw/lv_draw_sw_polygon.c
    lvgl/src/draw/sw/lv_draw_sw_rect.c
    lvgl/src/draw/lv_draw_arc.c
    lvgl/src/draw/lv_draw_img.c
    lvgl/src/draw/lv_draw_label.c
    lvgl/src/draw/lv_draw_line.c
    lvgl/src/draw/lv_draw_mask.c
    lvgl/src/draw/lv_draw_rect.c
    lvgl/src/draw/lv_draw.c
    lvgl/src/draw/lv_img_buf.c
    lvgl/src/draw/lv_img_cache.c
    lvgl/src/draw/lv_img_decoder.c
    lvgl/src/font/lv_font.c
    lvgl/src/font/lv_font_fmt_txt.c
    lvgl/src/font/lv_font_montserrat_14.c
    lvgl/src/font/lv_font_montserrat_28.c
    lvgl/src/hal/lv_hal_disp.c
    lvgl/src/hal/lv_hal_indev.c
    lvgl/src/hal/lv_hal_tick.c
    lvgl/src/misc/lv_anim.c
    lvgl/src/misc/lv_area.c
    lvgl/src/misc/lv_async.c
    lvgl/src/misc/lv_color.c
    lvgl/src/misc/lv_fs.c
    lvgl/src/misc/lv_gc.c
    lvgl/src/misc/lv_ll.c
    lvgl/src/misc/lv_math.c
    lvgl/src/misc/lv_mem.c
    lvgl/src/misc/lv_printf.c
    lvgl/src/misc/lv_style_gen.c
    lvgl/src/misc/lv_style.c
    lvgl/src/misc/lv_timer.c
    lvgl/src/misc/lv_tlsf.c
    lvgl/src/misc/lv_txt.c
    lvgl/src/misc/lv_utils.c
    lvgl/src/widgets/lv_arc.c
    lvgl/src/widgets/lv_bar.c
    lvgl/src/widgets/lv_btn.c
    lvgl/src/widgets/lv_btnmatrix.c
    lvgl/src/widgets/lv_checkbox.c
    lvgl/src/widgets/lv_dropdown.c
    lvgl/src/widgets/lv_img.c
    lvgl/src/widgets/lv_label.c
    lvgl/src/widgets/lv_line.c
    lvgl/src/widgets/lv_roller.c
    lvgl/src/widgets/lv_slider.c
    lvgl/src/widgets/lv_switch.c
    lvgl/src/widgets/lv_table.c
    lvgl/src/widgets/lv_textarea.c
    lvgl/src/extra/lv_extra.c
    lvgl/src/extra/layouts/flex/lv_flex.c
    lvgl/src/extra/layouts/grid/lv_grid.c
    lvgl/src/extra/themes/default/lv_theme_default.c
    lvgl/src/extra/widgets/tabview/lv_tabview.c
    lvgl/src/extra/widgets/tileview/lv_tileview.c
    lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c
    lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
    lvgl/src/extra/widgets/calendar/lv_calendar.c
    lvgl/src/extra/widgets/chart/lv_chart.c
    lvgl/src/extra/widgets/colorwheel/lv_colorwheel.c
    lvgl/src/extra/widgets/keyboard/lv_keyboard.c
    lvgl/src/extra/widgets/led/lv_led.c
    lvgl/src/extra/widgets/list/lv_list.c
    lvgl/src/extra/widgets/menu/lv_menu.c
    lvgl/src/extra/widgets/meter/lv_meter.c
    lvgl/src/extra/widgets/msgbox/lv_msgbox.c
    lvgl/src/extra/widgets/spinbox/lv_spinbox.c
    lvgl/src/extra/widgets/spinner/lv_spinner.c
    lvgl/src/extra/widgets/win/lv_win.c

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

## Add dependencies
target_link_libraries(${LCD_ENSEMBLE_COMPONENT_TARGET} PUBLIC
    ${ENSEMBLE_LCD_IFACE_TARGET}
    log
    cmsis_ensemble
    rte_components)

# Display status
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "*******************************************************")
message(STATUS "Library                                : " ${LCD_ENSEMBLE_COMPONENT_TARGET})
message(STATUS "*******************************************************")
