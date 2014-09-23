EXTRA_CFLAGS += -DBUILD_SOC
EXTRA_CFLAGS += -DNOT_USE_MEM_LOG

EXTRA_CFLAGS += -I$(DBG_KERNEL_SRC_DIR)/arch/arm/mach-ac83xx/include
EXTRA_CFLAGS += -I$(DBG_KERNEL_SRC_DIR)/arch/arm/mach-ac33xx/include/mach
EXTRA_CFLAGS += -I$(DBG_KERNEL_SRC_DIR)/../autochips/kernel/arch/arm/mach-ac33xx/include/mach
EXTRA_CFLAGS += -I$(DBG_KERNEL_SRC_DIR)/../autochips/platform/kernel/drivers/inc
