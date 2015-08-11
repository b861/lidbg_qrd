#ifndef __FLYPLATFORM_H__
#define __FLYPLATFORM_H__

#include <common.h>
#include <stdarg.h>
#include <bedbug/bedbug.h>
#include <armlinux/config.h>
#include "rkloader.h"	//flash read/write

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

/*lpc i2c config*/
#define LPC_I2C_ADDR 0xa0
#define LPC_SDA_GPIO   2
#define LPC_SCL_GPIO   3

#define FFBM_MODE_BUF_SIZE   8

#define dprintf(level, x...) printf(x)

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

extern struct fbcon_config *config;

extern unsigned page_mask;

void fb_config();
#endif

