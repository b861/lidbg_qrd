

include $(CLEAR_VARS)
LOCAL_MODULE := LidbgJniNative.java
LOCAL_SRC_FILES := ../inc/LidbgJniNative.java
LOCAL_MODULE_CLASS := bin
LOCAL_MODULE_TAGS := optional debug eng tests samples
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
include $(BUILD_PREBUILT)

