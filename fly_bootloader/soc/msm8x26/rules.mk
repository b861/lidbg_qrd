INCLUDES += -I$(DBG_BOOTLOADER_DIR)/platform/msm_shared/include -I$(DBG_BOOTLOADER_DIR)/include/dev
INCLUDES += -I$(DBG_BOOTLOADER_DIR)/include -I$(DBG_BOOTLOADER_DIR)/dev/pmic/pm8x41/include
INCLUDES += -I$(DBG_BOOTLOADER_DIR)/flyaudio/common/include

OBJS +=\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/fly_interface.o	\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/adc_func.o	\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/gpio_func.o	\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/dsi83.o	\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/fb_func.o	\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/ptn_func.o	\
	$(DBG_BOOTLOADER_DIR)/flyaudio/soc/$(DBG_SOC)/display_func.o
