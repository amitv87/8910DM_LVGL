###################################################
# Sources
###################################################


IC_LV_WIDGETS_SRC = middleware/incar/lv_widgets

C_FILES += $(IC_LV_WIDGETS_SRC)/animation.c
C_FILES += $(IC_LV_WIDGETS_SRC)/control_center.c
C_FILES += $(IC_LV_WIDGETS_SRC)/infocard_center.c
C_FILES += $(IC_LV_WIDGETS_SRC)/message_center.c
C_FILES += $(IC_LV_WIDGETS_SRC)/mainmenu.c
C_FILES += $(IC_LV_WIDGETS_SRC)/title_bar.c
C_FILES += $(IC_LV_WIDGETS_SRC)/main_screen.c
C_FILES += $(IC_LV_WIDGETS_SRC)/clockface/clockface.c

#i18n
C_FILES += $(IC_LV_WIDGETS_SRC)/i18n/lv_i18n.c

#fonts
C_FILES += $(IC_LV_WIDGETS_SRC)/fonts/iclv_font.c
C_FILES += $(IC_LV_WIDGETS_SRC)/fonts/opposans_14.c

###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/incar/lv_widgets
CFLAGS  += -I$(SOURCE_DIR)/middleware/incar/lv_widgets/config
CFLAGS  += -I$(SOURCE_DIR)/middleware/incar/lv_widgets/i18n
CFLAGS  += -I$(SOURCE_DIR)/middleware/incar/lv_widgets/fonts
