# Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
# All rights reserved.
#
# This software is supplied "AS IS" without any warranties.
# RDA assumes no responsibility or liability for the use of the software,
# conveys no license or title under any patent, copyright, or mask work
# right to the product. RDA reserves the right to make changes in the
# software without notification.  RDA also make no representation or
# warranty that such application will be suitable for the specified use
# without further testing or modification.

#configure_file(include/lv_gui_config.h.in ${out_inc_dir}/lv_gui_config.h)

set(target lvgl)
#add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_LVGL)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SRC_DIR} lvgl include lv_lib_png)
#target_link_libraries(${target} PRIVATE kernel driver hal ql_api_common)
target_sources(${target} PRIVATE
    lvgl/src/lv_core/lv_group.c
    lvgl/src/lv_core/lv_indev.c
    lvgl/src/lv_core/lv_disp.c
    lvgl/src/lv_core/lv_obj.c
    lvgl/src/lv_core/lv_refr.c
    lvgl/src/lv_core/lv_style.c

    lvgl/src/lv_draw/lv_draw_mask.c
    lvgl/src/lv_draw/lv_draw_blend.c
    lvgl/src/lv_draw/lv_draw_rect.c
    lvgl/src/lv_draw/lv_draw_label.c
    lvgl/src/lv_draw/lv_draw_line.c
    lvgl/src/lv_draw/lv_draw_img.c
    lvgl/src/lv_draw/lv_draw_arc.c
    lvgl/src/lv_draw/lv_draw_triangle.c
    lvgl/src/lv_draw/lv_img_decoder.c
    lvgl/src/lv_draw/lv_img_cache.c
    lvgl/src/lv_draw/lv_img_buf.c

    lvgl/src/lv_font/lv_font.c
    lvgl/src/lv_font/lv_font_fmt_txt.c
    lvgl/src/lv_font/lv_font_loader.c
    lvgl/src/lv_font/lv_font_montserrat_12.c
    lvgl/src/lv_font/lv_font_montserrat_14.c
    lvgl/src/lv_font/lv_font_montserrat_16.c
    lvgl/src/lv_font/lv_font_montserrat_18.c
    lvgl/src/lv_font/lv_font_montserrat_20.c
    lvgl/src/lv_font/lv_font_montserrat_22.c
    lvgl/src/lv_font/lv_font_montserrat_24.c
    lvgl/src/lv_font/lv_font_montserrat_26.c
    lvgl/src/lv_font/lv_font_montserrat_28.c
    lvgl/src/lv_font/lv_font_montserrat_30.c
    lvgl/src/lv_font/lv_font_montserrat_32.c
    lvgl/src/lv_font/lv_font_montserrat_34.c
    lvgl/src/lv_font/lv_font_montserrat_36.c
    lvgl/src/lv_font/lv_font_montserrat_38.c
    lvgl/src/lv_font/lv_font_montserrat_40.c
    lvgl/src/lv_font/lv_font_montserrat_42.c
    lvgl/src/lv_font/lv_font_montserrat_44.c
    lvgl/src/lv_font/lv_font_montserrat_46.c
    lvgl/src/lv_font/lv_font_montserrat_48.c
    lvgl/src/lv_font/lv_font_montserrat_12_subpx.c
    lvgl/src/lv_font/lv_font_montserrat_28_compressed.c
    lvgl/src/lv_font/lv_font_unscii_8.c
    lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.c

    lvgl/src/lv_font/lv_font_roboto_12.c
    lvgl/src/lv_font/lv_font_roboto_16.c
    lvgl/src/lv_font/lv_font_roboto_22.c
    lvgl/src/lv_font/lv_font_roboto_28.c

    lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.c

    lvgl/src/lv_hal/lv_hal_disp.c
    lvgl/src/lv_hal/lv_hal_indev.c
    lvgl/src/lv_hal/lv_hal_tick.c

    lvgl/src/lv_misc/lv_area.c
    lvgl/src/lv_misc/lv_task.c
    lvgl/src/lv_misc/lv_fs.c
    lvgl/src/lv_misc/lv_anim.c
    lvgl/src/lv_misc/lv_mem.c
    lvgl/src/lv_misc/lv_ll.c
    lvgl/src/lv_misc/lv_color.c
    lvgl/src/lv_misc/lv_txt.c
    lvgl/src/lv_misc/lv_txt_ap.c
    lvgl/src/lv_misc/lv_math.c
    lvgl/src/lv_misc/lv_log.c
    lvgl/src/lv_misc/lv_gc.c
    lvgl/src/lv_misc/lv_utils.c
    lvgl/src/lv_misc/lv_async.c
    lvgl/src/lv_misc/lv_printf.c
    lvgl/src/lv_misc/lv_bidi.c
    lvgl/src/lv_misc/lv_debug.c

    lvgl/src/lv_themes/lv_theme.c
    lvgl/src/lv_themes/lv_theme_material.c
    lvgl/src/lv_themes/lv_theme_mono.c
    lvgl/src/lv_themes/lv_theme_empty.c
    lvgl/src/lv_themes/lv_theme_template.c

    lvgl/src/lv_widgets/lv_arc.c
    lvgl/src/lv_widgets/lv_bar.c
    lvgl/src/lv_widgets/lv_checkbox.c
    lvgl/src/lv_widgets/lv_cpicker.c
    lvgl/src/lv_widgets/lv_dropdown.c
    lvgl/src/lv_widgets/lv_keyboard.c
    lvgl/src/lv_widgets/lv_line.c
    lvgl/src/lv_widgets/lv_msgbox.c
    lvgl/src/lv_widgets/lv_spinner.c
    lvgl/src/lv_widgets/lv_roller.c
    lvgl/src/lv_widgets/lv_table.c
    lvgl/src/lv_widgets/lv_tabview.c
    lvgl/src/lv_widgets/lv_tileview.c
    lvgl/src/lv_widgets/lv_btn.c
    lvgl/src/lv_widgets/lv_calendar.c
    lvgl/src/lv_widgets/lv_chart.c
    lvgl/src/lv_widgets/lv_canvas.c
    lvgl/src/lv_widgets/lv_gauge.c
    lvgl/src/lv_widgets/lv_label.c
    lvgl/src/lv_widgets/lv_list.c
    lvgl/src/lv_widgets/lv_slider.c
    lvgl/src/lv_widgets/lv_textarea.c
    lvgl/src/lv_widgets/lv_spinbox.c
    lvgl/src/lv_widgets/lv_btnmatrix.c
    lvgl/src/lv_widgets/lv_cont.c
    lvgl/src/lv_widgets/lv_img.c
    lvgl/src/lv_widgets/lv_imgbtn.c
    lvgl/src/lv_widgets/lv_led.c
    lvgl/src/lv_widgets/lv_linemeter.c
    lvgl/src/lv_widgets/lv_page.c
    lvgl/src/lv_widgets/lv_switch.c
    lvgl/src/lv_widgets/lv_win.c
    lvgl/src/lv_widgets/lv_objmask.c

    lv_lib_png/lv_png.c 
    lv_lib_png/lodepng.c
)
relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})
