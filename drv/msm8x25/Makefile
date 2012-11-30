
KERNELDIR = $(DBG_KERNEL_DIR)
INSTALLDIR = $(DBG_COPY_DIR)

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

CROSS_COMPILE = $(DBG_CROSS_COMPILE)
CC = $(CROSS_COMPILE)gcc

obj-m	+= lidbg_soc_$(DBG_SOC).o
obj-m	+= lidbg_fly_soc.o
obj-m	+= lidbg_rpc.o

lidbg_soc_$(DBG_SOC)-objs := m_$(DBG_SOC).o lidbg_io_$(DBG_SOC).o  lidbg_ad_$(DBG_SOC).o lidbg_pwm_$(DBG_SOC).o lidbg_init_$(DBG_SOC).o lidbg_serial_$(DBG_SOC).o 

lidbg_fly_soc-objs :=fly_soc.o

EXTRA_CFLAGS := -I$(DBG_DRV_PATH)/inc  -I$(DBG_DRV_PATH)/$(DBG_SOC)/inc  -I$(DBG_DEV_PATH)/$(DBG_SOC)
include $(DBG_DRV_PATH)/$(DBG_SOC)/drv_$(DBG_SOC).mk

modules:
	#env
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
 
modules_install:
	cp soc_$(DBG_SOC).ko $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers

.PHONY: modules modules_install clean

