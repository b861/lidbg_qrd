
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc

obj-m	+= lidbg_share.o
obj-m	+= lidbg_io.o
obj-m	+= lidbg_ad.o
obj-m	+= lidbg_i2c_gpio.o
obj-m	+= lidbg_i2c.o
obj-m	+= lidbg_spi.o
obj-m	+= lidbg_key.o
obj-m	+= lidbg_touch.o
obj-m	+= lidbg_common.o
obj-m	+= lidbg_servicer.o
obj-m	+= lidbg_msg.o
obj-m	+= lidbg_main.o
obj-m	+= lidbg_loader.o

lidbg_common-objs :=  lidbg_cmn.o  lidbg_mem.o lidbg_display.o
lidbg_main-objs := lidbg.o 
lidbg_i2c_gpio-objs := i2c-algo-bit.o i2c-gpio.o

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)
include $(DBG_DRV_PATH)/$(DBG_SOC)/drv_$(DBG_SOC).mk

include $(DBG_ROOT_PATH)/build_cfg.mk

modules:
	cp  $(DBG_SYSTEM_DIR)/kernel/drivers/i2c/busses/i2c-gpio.c ./i2c-gpio.c
	cp  $(DBG_SYSTEM_DIR)/kernel/drivers/i2c/algos/i2c-algo-bit.c ./i2c-algo-bit.c
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=arm  CROSS_COMPILE=$(DBG_CROSS_COMPILE) modules
 
modules_install:
	cp *.ko $(INSTALLDIR)
 
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *_log
	rm i2c-gpio.c i2c-algo-bit.c

.PHONY: modules modules_install clean

