include $(DBG_ROOT_PATH)/build/build.mk
include $(DBG_DRIVERS_PATH)/drivers.mk

ifeq ($(DBG_PLATFORM),msm7627a)
obj-$(CONFIG_TOUCHSCREEN)	+= lidbg_ts_probe.o
obj-$(CONFIG_TOUCHSCREEN)	+= gt801.o
obj-$(CONFIG_TOUCHSCREEN)	+= gt80x_update.o
endif
obj-$(CONFIG_TOUCHSCREEN)	+= lidbg_ts_probe_new.o
obj-$(CONFIG_TOUCHSCREEN)	+= lidbg_ts_to_recov.o
obj-$(CONFIG_TOUCHSCREEN)	+= gt811.o
obj-$(CONFIG_TOUCHSCREEN)	+= gt911.o
