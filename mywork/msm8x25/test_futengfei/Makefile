
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc


obj-m	+= test_kernel_creat_file.o

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

