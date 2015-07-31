LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/include/dev
INCLUDES += -I$(LK_TOP_DIR)/include -I$(LK_TOP_DIR)/dev/pmic/pm8x41/include
INCLUDES += -I$(LK_TOP_DIR)/flyaudio/common/include

OBJS +=\
	$(LOCAL_DIR)/adc_func.o	\
	$(LOCAL_DIR)/ctp_func.o \
	$(LOCAL_DIR)/gpio_func.o	\
	$(LOCAL_DIR)/dsi83.o	\
	$(LOCAL_DIR)/fb_func.o	\
	$(LOCAL_DIR)/ptn_func.o	\
	$(LOCAL_DIR)/display_func.o
