
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES += libcutils libutils 
LOCAL_LDLIBS += -llog
LOCAL_SRC_FILES:= \/par    usb_sd_mount_test.c
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE := usb_sd_mount_test
include $(BUILD_EXECUTABLE)
