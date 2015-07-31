LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/flyaudio/soc/$(DBG_SOC)

include  $(LOCAL_DIR)/$(DBG_SOC)/rules.mk

