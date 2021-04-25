###################################################
# Sources
###################################################


IC_HAL_SRC = middleware/incar/hal/src

C_FILES += $(IC_HAL_SRC)/ic_hal_rtc.c
###################################################
# include path
###################################################
CFLAGS  += -I$(SOURCE_DIR)/middleware/incar/hal/inc
