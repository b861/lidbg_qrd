
ifeq ($(CONFIG_HAL_GPSLIB),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,gpslib)
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

SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,apk)

include $(SUBDIR_MAKEFILES)
