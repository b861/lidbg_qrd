LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(DBG_BUILD_PATH)/build_cfg.mk

LOCAL_SRC_FILES := LidbgCameraMain.cpp LidbgCameraUsb.cpp

LOCAL_C_INCLUDES := \
        frameworks/native/include/media/openmax \
        hardware/qcom/display/libgralloc \
        hardware/qcom/media/libstagefrighthw \
        
LOCAL_C_INCLUDES += system/media/camera/include
LOCAL_C_INCLUDES += hardware/qcom/display/libgralloc

ifeq ($(PLATFORM_VERSION),5.0.2) 
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/media
endif

LOCAL_SHARED_LIBRARIES :=  libutils liblog libcutils  libcamera_client 

LOCAL_MODULE := camera.$(DBG_PLATFORM)
#LOCAL_MODULE := camera.default
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
