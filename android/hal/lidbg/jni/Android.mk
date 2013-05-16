LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(LOCAL_PATH)/../../../out

LOCAL_SRC_FILES:= \
    com_android_server_LidbgService.cpp \
    onload.cpp


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
    libusbhost \



ifeq ($(WITH_MALLOC_LEAK_CHECK),true)
    LOCAL_CFLAGS += -DMALLOC_LEAK_CHECK
endif

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:=liblidbg_servers

include $(BUILD_SHARED_LIBRARY)
