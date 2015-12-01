LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := MobileRateFlow.apk
LOCAL_SRC_FILES := MobileRateFlow.apk
LOCAL_MODULE_CLASS := bin
LOCAL_MODULE_TAGS := optional debug eng tests samples
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ES.ko
LOCAL_SRC_FILES := ESFileExplorer.apk
LOCAL_MODULE_CLASS := bin
LOCAL_MODULE_TAGS := optional debug eng tests samples
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := ST.ko
LOCAL_SRC_FILES := StabilityTest.apk
LOCAL_MODULE_CLASS := bin
LOCAL_MODULE_TAGS := optional debug eng tests samples
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := GPS.ko
LOCAL_SRC_FILES := gps_test.apk
LOCAL_MODULE_CLASS := bin
LOCAL_MODULE_TAGS := optional debug eng tests samples
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := ftpd
LOCAL_SRC_FILES := ftpd
LOCAL_MODULE_CLASS := bin
LOCAL_MODULE_TAGS := optional debug eng tests samples
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
include $(BUILD_PREBUILT)


#include $(CLEAR_VARS)
#LOCAL_MODULE := FastBoot.apk
#LOCAL_SRC_FILES := FastBoot.apk
#LOCAL_MODULE_CLASS := bin
#LOCAL_MODULE_TAGS := optional debug eng tests samples
#LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE := FlyBootService.apk
#LOCAL_SRC_FILES := FlyBootService.apk
#LOCAL_MODULE_CLASS := bin
#LOCAL_MODULE_TAGS := optional debug eng tests samples
#LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
#include $(BUILD_PREBUILT)
