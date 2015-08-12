#ifndef __FLYPLATFORM_H__
#define __FLYPLATFORM_H__

#include <common.h>
#include <stdarg.h>
#include <bedbug/bedbug.h>
#include <armlinux/config.h>
#include "rkloader.h"	//flash read/write

#define BOARD_VERSION 0

DECLARE_GLOBAL_DATA_PTR;

/* debug levels */
#define CRITICAL 0
#define ALWAYS 0
#define INFO 1
#define SPEW 2

#define INVALID_PTN -1

#define dprintf(level, x...) printf(x)

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

extern struct fbcon_config *config;
extern unsigned page_mask;
#endif

