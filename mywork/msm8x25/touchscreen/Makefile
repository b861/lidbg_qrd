
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc


obj-m	+= rmi_touch.o
rmi_touch-objs := rmi_bus.o rmi_sensor.o rmi_function.o rmi_f01.o rmi_f05.o rmi_f11.o rmi_f19.o rmi_f34.o rmi_i2c.o

obj-m	+= lidbg_ts_probe.o
obj-m	+= ft5x06_ts.o
obj-m	+= ft5x06_ts_sku7.o
#obj-m	+= tcc_ts_focaltech.o
obj-m	+= gt811.o
obj-m	+= lidbg_ts_to_recov.o
obj-m	+= gt801.o

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)
#include $(DBG_DRV_PATH)/$(DBG_SOC)/drv_$(DBG_SOC).mk

include $(DBG_ROOT_PATH)/build_cfg.mk

modules:
	#env
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
 
modules_install:
	ls

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers

.PHONY: modules modules_install clean

