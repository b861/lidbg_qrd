LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(DBG_BUILD_PATH)/build_cfg.mk
LOCAL_MODULE := camera.$(DBG_PLATFORM)
#LOCAL_MODULE := camera.default
LOCAL_MODULE_PATH := $(DBG_OUT_PATH)
LOCAL_C_INCLUDES += hardware/qcom/display/libgralloc \
        hardware/qcom/media/libstagefrighthw
LOCAL_SRC_FILES := LidbgCameraMain.cpp LidbgCameraUsb.cpp
LOCAL_SHARED_LIBRARIES :=  libutils liblog libcutils libjnigraphics libui libcamera_client libmmcamera_interface libbinder libmmjpeg_interface libhardware
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
