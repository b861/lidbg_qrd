
ifeq ($(CONFIG_HAL_GPSLIB),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,gpslib)
endif

ifeq ($(CONFIG_HAL_PX3_GPS),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,px3_gps)
endif

ifeq ($(CONFIG_HAL_SERVER),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_servicer)
endif

ifeq ($(CONFIG_HAL_VOLD_8x25Q),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,8x25q_vold)
endif

ifeq ($(CONFIG_HAL_CAMERA_8x25Q),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,8x25q_camera)
endif

ifeq ($(CONFIG_HAL_USERVER),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_userver)
endif

ifeq ($(CONFIG_HAL_VOLD_8x26),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,8x26_vold)
endif

ifeq ($(CONFIG_HAL_VOLD_8x26_5_0),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,8x26_vold_5.0)
endif

ifeq ($(CONFIG_HAL_VOLD_mt3360),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,mt3360_vold)
endif

ifeq ($(CONFIG_HAL_LOAD),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_load)
endif

ifeq ($(CONFIG_HAL_UART_SEND_DATA),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_uart_send_data)
endif

ifeq ($(CONFIG_HAL_ANDROID_SERVER),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_android_server)
endif

ifeq ($(CONFIG_HAL_USB_CAMERA),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,hal_camera_usb)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,hal_camera_study_v4l2)
endif

ifeq ($(CONFIG_HAL_PARTED),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_parted)
endif

ifeq ($(CONFIG_HAL_VOLD_PX3),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,px3_vold)
endif

#SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,uvccam_test)

SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,lidbg_umount)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,apk)

SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,apps)

include $(SUBDIR_MAKEFILES)
