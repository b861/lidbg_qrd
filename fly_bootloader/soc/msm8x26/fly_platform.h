#ifndef __FLYPLATFORM_H__
#define __FLYPLATFORM_H__

#include <app.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <arch/arm.h>
#include <arch/ops.h>
#include <arch/arm/mmu.h>
#include <bits.h>
#include <baseband.h>
#include <boot_stats.h>
#include <blsp_qup.h>
#include <crypto_hash.h>
#include <debug.h>
#include <dev/keys.h>
#include <dev/udc.h>
#include <dev/lcdc.h>
#include <dev/i2c.h>
#include <dev/gpio.h>
#include <dev/ssbi.h>
#include <dev/fbcon.h>
#include <dev/flash.h>
#include <dev/gpio_keypad.h>
#include <gsbi.h>
#include <lib/console.h>
#include <lib/fs.h>
#include <lib/ptable.h>
#include <i2c_qup.h>
#include <kernel/thread.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#include <mmc.h>
#include <mddi.h>
#include <mmu.h>
#include <platform.h>
#include <pm8x41_adc.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <platform/debug.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/timer.h>
#include <partition_parser.h>
#include <qgic.h>
#include <reg.h>
#include <smem.h>
#include <target.h>
#include <uart_dm.h>

#include "bootimg.h"
#include "devinfo.h"
#include "image_verify.h"
#include "recovery.h"
#include "scm.h"
#include "sparse_format.h"

#define FFBM_MODE_BUF_SIZE   8

/* ctp def */
#define CTP_I2C_ADDR 0x5d

#define CTP_SDA_GPIO 6
#define CTP_SCL_GPIO 7
#define CTP_RST_GPIO 12
#define CTP_INT_GPIO 14

//#define CTP_DBG
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MAX_LENGTH 240
#define GTP_REG_CONFIG_DATA   0x8047

#endif

