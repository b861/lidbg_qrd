
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES += libcutils libutils 
LOCAL_LDLIBS += -llog
LOCAL_SRC_FILES:= \/par    dev_node_test.c
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE := dev_node_test
include $(BUILD_EXECUTABLE)
