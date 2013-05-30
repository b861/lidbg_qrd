
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_PATH := $(LOCAL_PATH)/../out
LOCAL_SHARED_LIBRARIES += libcutils libutils
LOCAL_STATIC_LIBRARIES += libminui libpixelflinger_static libpng libz
LOCAL_LDLIBS += -llog
LOCAL_SRC_FILES:= \/par    lidbg_servicer.c ui.c default_recovery_ui.c
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE := lidbg_servicer
include $(BUILD_EXECUTABLE)
