LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    com_android_server_LidbgJniService.cpp \
    onload.cpp

LOCAL_C_INCLUDES += \
    $(JNI_H_INCLUDE) \
    frameworks/base/services \
    frameworks/base/core/jni \
    external/skia/include/core
ifeq ($(BOARD_USES_ALSA_AUDIO),true)
    LOCAL_CFLAGS += -DALSA_HEADSET_DETECTION
    LOCAL_CFLAGS += -include $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/linux/input.h
    LOCAL_ADDITIONAL_DEPENDENCIES := $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
endif

LOCAL_SHARED_LIBRARIES := \
    libandroid_runtime \
    libcutils \
    libhardware \
    libhardware_legacy \
    libnativehelper \
    libsystem_server \
    libutils \
    libui \
    libinput \
    libskia \
    libgui \
    libusbhost

ifeq ($(WITH_MALLOC_LEAK_CHECK),true)
    LOCAL_CFLAGS += -DMALLOC_LEAK_CHECK
endif
LOCAL_MODULE_PATH := $(LOCAL_PATH)/../../../../out
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= lib_lidbg_servers

include $(BUILD_SHARED_LIBRARY)
