#ifndef __I2C_GPIO_H__
#define __I2C_GPIO_H__

#include "soc.h"

struct i2c_gpio_dev {
	char *name;
	int sda_pin;
	int scl_pin;
	int retries;
	unsigned int udelay;
};

int bit_xfer(struct i2c_gpio_dev *dev, struct i2c_msg msgs[], int num);

#endif
