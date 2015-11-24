
#SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,FastPowerOn)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,LidbgPmService)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,FlyBootService)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,SleepTest)
include $(SUBDIR_MAKEFILES)
