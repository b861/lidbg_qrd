
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,FastPowerOn)
SUBDIR_MAKEFILES += $(call all-named-subdir-makefiles,FlyBootService)

include $(SUBDIR_MAKEFILES)
