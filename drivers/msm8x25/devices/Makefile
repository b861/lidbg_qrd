
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc


obj-m	+= lidbg_devices.o
obj-m	+= lidbg_lpc.o

lidbg_devices-objs :=devices.o

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)

# include $(DBG_DRV_PATH)/$(DBG_SOC)/drv_$(DBG_SOC).mk
include $(DBG_ROOT_PATH)/build_cfg.mk
KBUILD_EXTRA_SYMBOLS := $(DBG_DRV_PATH)/$(DBG_SOC)/Module.symvers $(DBG_DRV_PATH)/Module.symvers

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=arm  CROSS_COMPILE=$(DBG_CROSS_COMPILE) modules
 
modules_install:
	cp *.ko $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *_log

.PHONY: modules modules_install clean

