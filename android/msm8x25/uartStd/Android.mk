
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_PATH := $(LOCAL_PATH)/../out
LOCAL_SRC_FILES:= \/par    uartTest.c
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE := uart_test
include $(BUILD_EXECUTABLE)
