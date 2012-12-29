
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc


obj-m	+= lidbg_videoin.o
lidbg_videoin-objs := lidbg_videoin_main.o tw9912.o TC358746XBG.o i2c_io.o video_init_config.o
#lidbg_videoin-objs := hello.o

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)
#include $(DBG_DRV_PATH)/$(DBG_SOC)/drv_$(DBG_SOC).mk

ifdef DBG_DBG
EXTRA_CFLAGS += -DFLY_DEBUG
endif

modules:
	#env
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
 
modules_install:
	ls

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers

.PHONY: modules modules_install clean

