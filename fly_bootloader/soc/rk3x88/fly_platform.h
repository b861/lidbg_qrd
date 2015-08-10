#ifndef __FLYPLATFORM_H__
#define __FLYPLATFORM_H__

#include <common.h>
#include <stdarg.h>
#include <bedbug/bedbug.h>
#include <armlinux/config.h>
#include "rkloader.h"	//flash read/write
#include "fbcon.h"

DECLARE_GLOBAL_DATA_PTR;

/* debug levels */
#define CRITICAL 0
#define ALWAYS 0
#define INFO 1
#define SPEW 2

#define INVALID_PTN -1

/* GPIO TLMM: Direction */
#ifndef GPIO_INPUT
#define GPIO_INPUT      0
#endif
#ifndef GPIO_OUTPUT
#define GPIO_OUTPUT     1
#endif


/* ctp def */
#define CTP_I2C_ADDR 0x5d

#define CTP_SDA_GPIO 28
#define CTP_SCL_GPIO 29
#define CTP_RST_GPIO 6
#define CTP_INT_GPIO 7

//#define CTP_DBG
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MAX_LENGTH 240
#define GTP_REG_CONFIG_DATA   0x8047

/* gpio_i2c flags */
#define EIO         5
#define ENOMEM      12
#define EBUSY       16
#define ENODEV      19
#define ENOSYS      38
#define EPROTONOSUPPORT 93
#define ETIMEDOUT   110
#define I2C_SMBUS_BLOCK_MAX 32

/*lpc i2c config*/
#define LPC_I2C_ADDR 0xa0
#define LPC_SDA_GPIO   2
#define LPC_SCL_GPIO   3

/* img flags*/
#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512

#define FFBM_MODE_BUF_SIZE   8

#define DEVICE_MAGIC "ANDROID-BOOT!"
#define DEVICE_MAGIC_SIZE 13

#define dprintf(level, x...) printf(x)

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

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

struct device_info
{
	unsigned char magic[DEVICE_MAGIC_SIZE];
	int is_unlocked;
	int is_tampered;
	int charger_screen_enabled;
};
typedef struct device_info device_info;

extern struct fbcon_config *config;

extern unsigned page_mask;

void fb_config();
#endif

