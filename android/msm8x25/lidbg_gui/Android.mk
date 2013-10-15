LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(LOCAL_PATH)/../out
LOCAL_SRC_FILES := \
	debug_gui.c \
	GUI_StockC.c \
	font5_7.c \
	FONT8_8.c \
	font24_32.c \
	GUI_BASIC.c \
	debug_lcd_io.c \
	lidbg_kmsg.c

LOCAL_MODULE_TAGS:= optional

LOCAL_MODULE := lidbg_kmsg

include $(BUILD_EXECUTABLE)
