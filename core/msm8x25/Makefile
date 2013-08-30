
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc

obj-m	+= lidbg_soc_$(DBG_SOC).o

lidbg_soc_$(DBG_SOC)-objs := m_$(DBG_SOC).o lidbg_io_$(DBG_SOC).o  lidbg_ad_$(DBG_SOC).o lidbg_pwm_$(DBG_SOC).o

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)
include $(DBG_DRV_PATH)/$(DBG_SOC)/soc.mk

include $(DBG_ROOT_PATH)/build_cfg.mk

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=arm  CROSS_COMPILE=$(DBG_CROSS_COMPILE) modules
 
modules_install:
	cp *.ko $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers *_log *.o.d

.PHONY: modules modules_install clean

