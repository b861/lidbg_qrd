#ifndef __FLYPLATFORM_H__
#define __FLYPLATFORM_H__

#include <common.h>
#include <stdarg.h>
#include <bedbug/bedbug.h>
#include <armlinux/config.h>
#include "rkloader.h"	//flash read/write

#define bool int
#define FALSE 0
#define TRUE 1

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

/* gpio_i2c flags */
#define EIO         5
#define ENOMEM      12
#define EBUSY       16
#define ENODEV      19
#define ENOSYS      38
#define EPROTONOSUPPORT 93
#define ETIMEDOUT   110
#define I2C_SMBUS_BLOCK_MAX 32

/* img flags*/
#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512

#define FFBM_MODE_BUF_SIZE   8

#define DEVICE_MAGIC "ANDROID-BOOT!"
#define DEVICE_MAGIC_SIZE 13

struct device_info
{
	unsigned char magic[DEVICE_MAGIC_SIZE];
	int is_unlocked;
	int is_tampered;
	int charger_screen_enabled;
};
typedef struct device_info device_info;

/* fbcon */
struct fbcon_config {
	void		*base;
	unsigned	width;
	unsigned	height;
	unsigned	stride;
	unsigned	bpp;
	unsigned	format;

	void		(*update_start)(void);
	int		(*update_done)(void);
};

extern struct fbcon_config *config;
#endif

