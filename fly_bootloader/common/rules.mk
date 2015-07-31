LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)/include -I$(LK_TOP_DIR)/app/aboot -I$(LK_TOP_DIR)/dev/ssbi/include
INCLUDES += -I$(LK_TOP_DIR)/flyaudio/soc

OBJS +=\
	$(LOCAL_DIR)/flyaboot.o \
	$(LOCAL_DIR)/flykey.o	\
	$(LOCAL_DIR)/flymeg.o	\
	$(LOCAL_DIR)/showlogo.o \
	$(LOCAL_DIR)/i2c_screen.o \
	$(LOCAL_DIR)/flyfbcon.o\
	$(LOCAL_DIR)/i2c_gpio.o

