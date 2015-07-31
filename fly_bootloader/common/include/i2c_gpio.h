#ifndef __I2C_GPIO_H__
#define __I2C_GPIO_H__

#include <i2c_qup.h>

#define u8 unsigned char
#define  	u16		unsigned short

struct i2c_gpio_dev {
	char *name;
	int sda_pin;
	int scl_pin;
	int retries;
	unsigned int udelay;
};

int bit_xfer(struct i2c_gpio_dev *dev, struct i2c_msg msgs[], int num);

#endif
