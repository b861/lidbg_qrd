
ifeq (CONFIG_HAL_GPSLIB,y)
include $(call all-named-subdir-makefiles,gpslib)
endif

#SUBDIR_MAKEFILES := $(call all-named-subdir-makefiles,modules tests)
