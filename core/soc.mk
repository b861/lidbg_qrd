KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc


EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_SOC_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)

include $(DBG_ROOT_PATH)/build_cfg.mk

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=arm  CROSS_COMPILE=$(DBG_CROSS_COMPILE) modules
 
modules_install:
	cp *.ko $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *_log *.o.d

.PHONY: modules modules_install clean


