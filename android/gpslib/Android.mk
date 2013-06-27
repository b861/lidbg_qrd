LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := gps.msm7627a

LOCAL_MODULE_PATH := $(LOCAL_PATH)/../out

LOCAL_SRC_FILES := gps_ublox.c

LOCAL_SHARED_LIBRARIES := liblog libcutils

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
