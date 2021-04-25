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

set(target lv_widgets)
#add_app_libraries($<TARGET_FILE:${target}>)

add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${out_lib_dir})
target_compile_definitions(${target} PRIVATE OSI_LOG_TAG=LOG_TAG_LVGL)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_SRC_DIR} ./ clockface config fonts i18n assets/output hal/inc image_resource mainmenu/inc alarm_clock/inc)
target_link_libraries(${target} PRIVATE lvgl function)
#target_link_libraries(${target} PRIVATE kernel driver hal ql_api_common)
target_sources(${target} PRIVATE
    animation.c
    control_center.c
    infocard_center.c
    main_screen.c
    mainmenu.c
    message_center.c
    title_bar.c
    fonts/iclv_font.c
    fonts/opposans_14.c
    clockface/clockface.c
    assets/output/image_id_list.c
    assets/output/IMG_CLOCKFACE_ANALOG1_BG.c
    assets/output/IMG_CLOCKFACE_ANALOG1_MINUTE.c
    assets/output/IMG_CLOCKFACE_ANALOG1_HOUR.c
    assets/output/IMG_CLOCKFACE_ANALOG1_PREVIEW.c
    assets/output/IMG_CLOCKFACE_ANALOG1_SECOND.c
    hal/src/ic_hal_rtc.c
    i18n/lv_i18n.c
    screen_manager.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_BG.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR0.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR1.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR2.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR3.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR4.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR5.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR6.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR7.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR8.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_HOUR9.c
    image_resource/image_resource.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE0.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE1.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE2.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE3.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE4.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE5.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE6.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE7.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE8.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_DATE9.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_SLASH.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_MON.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_TUE.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_WED.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_THU.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_FRI.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_SAT.c
    assets/output/IMG_CLOCKFACE_DIGITAL1_SUN.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_BG.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR0.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR1.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR2.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR3.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR4.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR5.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR6.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR7.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR8.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_HOUR9.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN0.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN1.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN2.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN3.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN4.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN5.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN6.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN7.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN8.c
    assets/output/IMG_CLOCKFACE_DIGITAL2_MIN9.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_BG.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_COLON.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR0.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR1.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR2.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR3.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR4.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR5.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR6.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR7.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR8.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_HOUR9.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_MON.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_TUE.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_WED.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_THU.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_FRI.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_SAT.c
    assets/output/IMG_CLOCKFACE_DIGITAL3_SUN.c
    assets/output/IMG_MAINMENU_PHONEBOOK.c
    assets/output/IMG_MAINMENU_SETTING.c
    assets/output/IMG_MAINMENU_STUDENTCAR.c
    assets/output/IMG_MAINMENU_TEMPERATURE.c
    assets/output/IMG_MAINMENU_TOOLS.c
    assets/output/IMG_MAINMENU_WEATHER.c
    clockface/clockface_table.c
    mainmenu/src/ic_mainmenu_calllog.c
    alarm_clock/src/ic_alarm_clock.c
    assets/output/IMG_IDLE_BATTERY0.c
    assets/output/IMG_IDLE_BATTERY1.c
    assets/output/IMG_IDLE_BATTERY2.c
    assets/output/IMG_IDLE_BATTERY3.c
    assets/output/IMG_IDLE_BATTERY4.c
    assets/output/IMG_IDLE_BATTERY4.c
    assets/output/IMG_IDLE_BATTERY4.c
    assets/output/IMG_PHONEBOOK_DIAL.c
    assets/output/IMG_PHONEBOOK_PB.c
    assets/output/IMG_PHONEBOOK_SELECT1.c
    assets/output/IMG_PHONEBOOK_SELECT2.c
    assets/output/IMG_DIAL_DELETE.c
    assets/output/IMG_DIAL_DIAL.c
    assets/output/IMG_DIAL_H_DELETE.c
    assets/output/IMG_DIAL_H_DIAL.c
    assets/output/IMG_DIAL_H_NUM0.c
    assets/output/IMG_DIAL_H_NUM1.c
    assets/output/IMG_DIAL_H_NUM2.c
    assets/output/IMG_DIAL_H_NUM3.c
    assets/output/IMG_DIAL_H_NUM4.c
    assets/output/IMG_DIAL_H_NUM5.c
    assets/output/IMG_DIAL_H_NUM6.c
    assets/output/IMG_DIAL_H_NUM7.c
    assets/output/IMG_DIAL_H_NUM8.c
    assets/output/IMG_DIAL_H_NUM9.c
    assets/output/IMG_DIAL_H_PLUS.c
    assets/output/IMG_DIAL_H_STAR.c
    assets/output/IMG_DIAL_H_WELL.c
    assets/output/IMG_DIAL_NUM0.c
    assets/output/IMG_DIAL_NUM1.c
    assets/output/IMG_DIAL_NUM2.c
    assets/output/IMG_DIAL_NUM3.c
    assets/output/IMG_DIAL_NUM4.c
    assets/output/IMG_DIAL_NUM5.c
    assets/output/IMG_DIAL_NUM6.c
    assets/output/IMG_DIAL_NUM7.c
    assets/output/IMG_DIAL_NUM8.c
    assets/output/IMG_DIAL_NUM9.c
    assets/output/IMG_DIAL_PLUS.c
    assets/output/IMG_DIAL_STAR.c
    assets/output/IMG_DIAL_WELL.c
    assets/output/IMG_CALL_FATHER.c
    assets/output/IMG_RECORD_FATHER.c
    assets/output/IMG_CALL_GRANDFATHER.c
    assets/output/IMG_RECORD_GRANDFATHER.c
    assets/output/IMG_CALL_MOTHER.c
    assets/output/IMG_RECORD_MOTHER.c
    assets/output/IMG_CALL_GRANDMOTHER.c
    assets/output/IMG_RECORD_GRANDMOTHER.c
    assets/output/IMG_CALL_TEACHER.c
    assets/output/IMG_RECORD_TEACHER.c
    assets/output/IMG_CALL_END.c
    assets/output/IMG_CALL_START.c
    assets/output/IMG_CALL_ERROR.c
    assets/output/IMG_CALL_OK.c
    assets/output/IMG_CALL_IN.c
    assets/output/IMG_CALL_OUT.c
    assets/output/IMG_CALL_MISS.c
    assets/output/IMG_CALL_SELECT.c
    mainmenu/src/ic_mainmenu_phonebook.c
)
relative_glob(srcs include/*.h src/*.c inc/*.h)
beautify_c_code(${target} ${srcs})
