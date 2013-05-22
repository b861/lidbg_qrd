
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc

obj-m	+= lidbg_share.o
obj-m	+= lidbg_io.o
obj-m	+= lidbg_ad.o
obj-m	+= lidbg_i2c.o
obj-m	+= lidbg_spi.o
# obj-m	+= lidbg_serial.o
obj-m	+= lidbg_key.o
obj-m	+= lidbg_touch.o
obj-m	+= lidbg_common.o
obj-m	+= lidbg_servicer.o
obj-m	+= lidbg_msg.o
obj-m	+= lidbg_main.o

lidbg_common-objs :=  lidbg_cmn.o  lidbg_mem.o lidbg_display.o
lidbg_main-objs := lidbg.o 

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)
include $(DBG_DRV_PATH)/$(DBG_SOC)/drv_$(DBG_SOC).mk

include $(DBG_ROOT_PATH)/build_cfg.mk

modules:
	#env
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
 
modules_install:
	cp lidbg_main.ko $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *_log

.PHONY: modules modules_install clean

