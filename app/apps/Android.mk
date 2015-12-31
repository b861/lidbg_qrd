
#SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,FastPowerOn)
ifeq ($(CONFIG_APP_LIDBGPMSERVICE),y)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,LidbgPmService)
else
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,FlyBootService)
endif
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,SleepTest)
include $(SUBDIR_MAKEFILES)
