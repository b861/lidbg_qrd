LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../inc
LOCAL_C_INCLUDES += $(DBG_DRIVERS_PATH)/inc
LOCAL_SRC_FILES := lidbg_hal.c
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
LOCAL_SHARED_LIBRARIES := liblog
include $(DBG_BUILD_PATH)/build_cfg.mk
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE := lidbg_hal.$(TARGET_BOARD_PLATFORM)#default
include $(BUILD_SHARED_LIBRARY)
