#ifndef __I2C_GPIO_H__
#define __I2C_GPIO_H__

//#define u8 unsigned char
//#define  	u16		unsigned short

struct i2c_gpio_dev {
	char *name;
	int sda_pin;
	int scl_pin;
	int retries;
	unsigned int udelay;
};

struct i2c_msg {
	unsigned short addr;	/* slave address */
	unsigned short flags;
#define I2C_M_TEN           0x0010	/* this is a ten bit chip address */
#define I2C_M_WR            0x0000	/* write data, from master to slave */
#define I2C_M_RD            0x0001	/* read data, from slave to master */
#define I2C_M_NOSTART       0x4000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR  0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK    0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK     0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN      0x0400	/* length will be first received byte */
	unsigned short len;	/* msg length */
	unsigned char *buf;	/* pointer to msg data */
};

int bit_xfer(struct i2c_gpio_dev *dev, struct i2c_msg msgs[], int num);

#endif
