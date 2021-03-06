/****************************************************************************
 *
 *		Copyright(c) 2013-2014 Yamaha Corporation. All rights reserved.
 *
 *		Module		: yad1driver_d.h
 *
 *		Description	: Yamaha Audio engine Device version 1
 *						device driver local definition.
 *
 *		Version		: 1.1.0		2014.07.25
 *
 ****************************************************************************/

#ifndef YAD1DD_D_H
#define YAD1DD_D_H


#include "yad1machdep.h"

#if		(YAD1_DEVICE_IF	== YAD1_DEVICE_IF_SPI )
	#define	YAD1DD_SPI_COM_WRITE		(0x00)
	#define	YAD1DD_SPI_COM_READ			(0x80)
	#define	YAD1DD_SPI_COM_ADR_MASK		(0x3F)
	#define	YAD1DD_SPI_COM_BURST		(0x01)
#elif	(YAD1_DEVICE_IF	== YAD1_DEVICE_IF_I2C )
	#define	YAD1DD_I2C_COM_WRITE		(0x00)
	#define	YAD1DD_I2C_COM_READ			(0x01)
	#define	YAD1DD_I2C_COM_ADR_MASK		(0x3F)
	#define	YAD1DD_I2C_COM_BURST		(0x01)
	#define	YAD1DD_DIGITALBLOCK_SA		(0x11)
	#define	YAD1DD_ANALOGBLOCK_SA		(0x3A)
#endif

#define	YAD1DD_REGADR_A_REG_A		(0x00)
#define	YAD1DD_REGADR_A_REG_D		(0x01)
#define	YAD1DD_REGADR_MA_REG_A		(0x0C)
#define	YAD1DD_REGADR_MA_REG_D		(0x0D)
#define	YAD1DD_REGADR_MB_REG_A		(0x0E)
#define	YAD1DD_REGADR_MB_REG_D		(0x0F)
#define	YAD1DD_REGADR_B_REG_A		(0x10)
#define	YAD1DD_REGADR_B_REG_D		(0x11)
#define	YAD1DD_REGADR_C_REG_A		(0x28)
#define	YAD1DD_REGADR_C_REG_D		(0x29)
#define	YAD1DD_REGADR_E_REG_A		(0x20)
#define	YAD1DD_REGADR_E_REG_D		(0x21)
#define	YAD1DD_REGADR_F_REG_A		(0x30)
#define	YAD1DD_REGADR_F_REG_D		(0x31)
#define	YAD1DD_REGADR_ANA_REG_A		(0x06)
#define	YAD1DD_REGADR_ANA_REG_D		(0x07)
#define	YAD1DD_REGADR_CD_REG_A		(0x08)
#define	YAD1DD_REGADR_CD_REG_D		(0x09)

#define	YAD1DD_REGFLAG_AINC			(0x80)

#define	YAD1DD_COMTYPE_IF_REG_W		(0x01)
#define	YAD1DD_COMTYPE_A_REG_W		(0x02)
#define	YAD1DD_COMTYPE_MA_REG_W		(0x03)
#define	YAD1DD_COMTYPE_MB_REG_W		(0x04)
#define	YAD1DD_COMTYPE_B_REG_W		(0x05)
#define	YAD1DD_COMTYPE_C_REG_W		(0x06)
#define	YAD1DD_COMTYPE_E_REG_W		(0x07)
#define	YAD1DD_COMTYPE_F_REG_W		(0x08)
#define	YAD1DD_COMTYPE_ANA_REG_W	(0x09)
#define	YAD1DD_COMTYPE_CD_REG_W		(0x0A)

#define	YAD1DD_COMTYPE_DIRECT_W		(0x0F)

#define	YAD1DD_COMTYPE_IF_REG_W_V	(0x81)
#define	YAD1DD_COMTYPE_A_REG_W_V	(0x82)
#define	YAD1DD_COMTYPE_MA_REG_W_V	(0x83)
#define	YAD1DD_COMTYPE_MB_REG_W_V	(0x84)
#define	YAD1DD_COMTYPE_B_REG_W_V	(0x85)
#define	YAD1DD_COMTYPE_C_REG_W_V	(0x86)
#define	YAD1DD_COMTYPE_E_REG_W_V	(0x87)
#define	YAD1DD_COMTYPE_F_REG_W_V	(0x88)
#define	YAD1DD_COMTYPE_ANA_REG_W_V	(0x89)
#define	YAD1DD_COMTYPE_CD_REG_W_V	(0x8A)

#define	YAD1DD_COMTYPE_DIRECT_W_V	(0x8F)

#define	YAD1DD_COMTYPE_IF_REG_F		(0x11)
#define	YAD1DD_COMTYPE_A_REG_F		(0x12)
#define	YAD1DD_COMTYPE_MA_REG_F		(0x13)
#define	YAD1DD_COMTYPE_MB_REG_F		(0x14)
#define	YAD1DD_COMTYPE_B_REG_F		(0x15)
#define	YAD1DD_COMTYPE_C_REG_F		(0x16)
#define	YAD1DD_COMTYPE_E_REG_F		(0x17)
#define	YAD1DD_COMTYPE_F_REG_F		(0x18)
#define	YAD1DD_COMTYPE_ANA_REG_F	(0x19)
#define	YAD1DD_COMTYPE_CD_REG_F		(0x1A)

#define	YAD1DD_COMTYPE_WAIT			(0x1F)

#define	YAD1DD_COMTYPE_MEM_W_IF		(0x61)
#define	YAD1DD_COMTYPE_MEM_W_A		(0x62)
#define	YAD1DD_COMTYPE_MEM_W_MA		(0x63)
#define	YAD1DD_COMTYPE_MEM_W_MB		(0x64)
#define	YAD1DD_COMTYPE_MEM_W_B		(0x65)
#define	YAD1DD_COMTYPE_MEM_W_C		(0x66)
#define	YAD1DD_COMTYPE_MEM_W_E		(0x67)
#define	YAD1DD_COMTYPE_MEM_W_F		(0x68)
#define	YAD1DD_COMTYPE_MEM_W_ANA	(0x69)
#define	YAD1DD_COMTYPE_MEM_W_CD		(0x6A)

#define	YAD1DD_COMTYPE_MEM_CTRL		(0x6F)

#define	YAD1DD_COMTYPE_MEM_CP_IF	(0x71)
#define	YAD1DD_COMTYPE_MEM_CP_A		(0x72)
#define	YAD1DD_COMTYPE_MEM_CP_MA	(0x73)
#define	YAD1DD_COMTYPE_MEM_CP_MB	(0x74)
#define	YAD1DD_COMTYPE_MEM_CP_B		(0x75)
#define	YAD1DD_COMTYPE_MEM_CP_C		(0x76)
#define	YAD1DD_COMTYPE_MEM_CP_E		(0x77)
#define	YAD1DD_COMTYPE_MEM_CP_F		(0x78)
#define	YAD1DD_COMTYPE_MEM_CP_ANA	(0x79)
#define	YAD1DD_COMTYPE_MEM_CP_CD	(0x7A)

#define	YAD1DD_MEMCTRL_WRITE		(0x00)
#define	YAD1DD_MEMCTRL_ADD			(0x01)
#define	YAD1DD_MEMCTRL_SUB			(0x02)
#define	YAD1DD_MEMCTRL_MUL			(0x03)
#define	YAD1DD_MEMCTRL_OR			(0x04)
#define	YAD1DD_MEMCTRL_AND			(0x05)
#define	YAD1DD_MEMCTRL_XOR			(0x06)

static const U08 au08testpath01_00[]={
        0x19, 0x39, 0x40, 0x40, 0x0a, 0x09, 0x01, 0x01, 0x0a, 0x01, 0x01, 0x09, 0x01, 0x01, 0x09, 0x01, 
        0x00, 0x09, 0x0a, 0x15, 0x09, 0x02, 0x13, 0x19, 0x39, 0x08, 0x08, 0x0a, 0x09, 0x02, 0x03, 0x19, 
        0x39, 0x10, 0x10, 0x0a, 0x09, 0x37, 0x41, 0x19, 0x39, 0x80, 0x80, 0x32, 0x09, 0x02, 0x02, 0x0a, 
        0x01, 0x01, 0x0a, 0x01, 0x00, 0x01, 0x02, 0x01, 0x01, 0x02, 0x00, 0x02, 0x0d, 0x01, 0x02, 0x0e, 
        0x02, 0x02, 0x0f, 0x03, 0x02, 0x10, 0x03, 0x02, 0x09, 0xa2, 0x02, 0x0a, 0x02, 0x02, 0x0b, 0x02, 
        0x02, 0x04, 0x2b, 0x02, 0x05, 0x3f, 0x02, 0x08, 0x00, 0x02, 0x06, 0x3f, 0x02, 0x11, 0x00, 0x02, 
        0x18, 0x05, 0x02, 0x19, 0x0d, 0x02, 0x1a, 0x00, 0x02, 0x1b, 0x4e, 0x02, 0x1c, 0x00, 0x02, 0x1d, 
        0x00, 0x02, 0x1e, 0x02, 0x02, 0x13, 0x10, 0x02, 0x12, 0x04, 0x02, 0x01, 0x06, 0x0a, 0x04, 0x03, 
        0x1f, 0x02, 0x02, 0x02, 0x7f, 0x1f, 0x02, 0x02, 0x02, 0x7e, 0x02, 0x02, 0x3e, 0x01, 0x03, 0xbf, 
        0x11, 0x03, 0x40, 0x00, 0x0a, 0x01, 0x03, 0xbb, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x16, 0x0a, 0x02, 
        0x00, 0x89, 0x03, 0x0d, 0x00, 0x00, 0x09, 0x18, 0x00, 0x09, 0x29, 0xfc, 0x09, 0x2c, 0x80, 0x09, 
        0x06, 0xcc, 0x09, 0x07, 0x80, 0x09, 0x3a, 0x10, 0x07, 0x00, 0x01, 0x07, 0x00, 0x00, 0x07, 0x02, 
        0x53, 0x07, 0x25, 0x02, 0x07, 0x26, 0x00, 0x07, 0x01, 0x42, 0x1a, 0x31, 0x08, 0x08, 0x64, 0x0a, 
        0x31, 0x08, 0x07, 0x02, 0x03, 0x09, 0x15, 0x03, 0x03, 0x44, 0x33, 0x03, 0x43, 0x81, 0x03, 0x47, 
        0x80, 0x04, 0x55, 0xa0, 0x1f, 0x4b,
};


/*
        [Path]
                Set Signal Path
 */
static const U08 au08testpath01_16[]={
        0x03, 0x23, 0x00, 0x03, 0x25, 0x00, 0x03, 0x27, 0x00, 0x03, 0x29, 0x00, 0x83, 0x03, 0x2b, 0x00, 
        0x01, 0x83, 0x03, 0x2f, 0x00, 0x10, 0x83, 0x03, 0x33, 0x00, 0x00, 0x83, 0x03, 0x37, 0x00, 0x00, 
        0x83, 0x03, 0x3b, 0x00, 0x10, 0x83, 0x03, 0x3f, 0x00, 0x10, 0x03, 0x20, 0x00, 0x03, 0x21, 0x00, 
        0x03, 0x22, 0x00, 0x89, 0x04, 0x30, 0x30, 0x30, 0x80, 0x83, 0x06, 0x50, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x04, 0x00, 0x40, 0x04, 0x10, 0x40, 0x04, 0x20, 0x00, 0x04, 0x30, 0x00, 0x04, 0x01, 0x00, 
        0x04, 0x11, 0x00, 0x04, 0x21, 0x00, 0x04, 0x31, 0x20, 0x04, 0x02, 0x24, 0x04, 0x03, 0x02, 0x04, 
        0x13, 0x02, 0x04, 0x23, 0x00, 0x04, 0x33, 0x00, 0x04, 0x04, 0x24, 0x04, 0x05, 0x00, 0x04, 0x15, 
        0x00, 0x04, 0x25, 0x00, 0x04, 0x35, 0x00, 0x04, 0x06, 0x44, 0x04, 0x16, 0x44, 0x04, 0x26, 0x44, 
        0x04, 0x36, 0x44, 0x04, 0x07, 0x00, 0x04, 0x17, 0x00, 0x04, 0x27, 0x00, 0x04, 0x08, 0x80, 0x04, 
        0x18, 0x80, 0x04, 0x28, 0x80, 0x04, 0x09, 0x80, 0x04, 0x19, 0x80, 0x04, 0x29, 0x80, 0x84, 0x03, 
        0x2b, 0x00, 0x00, 0x87, 0x07, 0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x08, 0x29, 0x01, 
        0x10, 0x00, 0x22, 0x00, 0x43, 0x00, 0x07, 0x30, 0x00, 0x07, 0x03, 0x00, 0x07, 0x12, 0x88, 0x87, 
        0x03, 0x15, 0x00, 0x00, 0x87, 0x04, 0x44, 0x00, 0x00, 0x00, 0x07, 0x08, 0x00, 0x87, 0x08, 0x1c, 
        0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0a, 0x02, 0x00,
};


/*
        [Start]
                Set Volume
 */
static const U08 au08testpath01_17[]={
        0x02, 0x04, 0x2b, 0x02, 0x05, 0x01, 0x02, 0x06, 0x03, 0x04, 0x0a, 0x0f, 0x04, 0x1a, 0x03, 0x04, 
        0x2a, 0x30, 0x04, 0x3a, 0x0a, 0x04, 0x2c, 0x00, 0x07, 0x48, 0x00, 0x89, 0x05, 0x1b, 0x21, 0x21, 
        0x00, 0x00, 0x89, 0x03, 0x19, 0x80, 0x00, 0x03, 0x06, 0x60, 0x03, 0x08, 0x00, 0x03, 0x0a, 0x00, 
        0x03, 0x0c, 0x00, 0x83, 0x03, 0x0e, 0x60, 0x60, 0x83, 0x03, 0x10, 0x00, 0x00, 0x83, 0x03, 0x12, 
        0x00, 0x00, 0x08, 0x20, 0x00, 0x05, 0x00, 0x00, 0x03, 0x14, 0x60, 0x03, 0x16, 0x60, 0x03, 0x18, 
        0x00, 0x03, 0x1a, 0x00, 0x03, 0x1c, 0x60, 0x03, 0x1e, 0x60, 0x89, 0x03, 0x24, 0xef, 0x6f, 0x89, 
        0x03, 0x26, 0xef, 0x6f, 0x1f, 0x0a, 0x09, 0x3a, 0x00,
};


/*
        [Stop]
                Mute Volume
 */
static const U08 au08testpath01_18[]={
	    0x09, 0x3a, 0x10, 0x03, 0x14, 0x00, 0x03, 0x16, 0x00, 0x03, 0x18, 0x00, 0x03, 0x1a, 0x00, 0x03, 
        0x1c, 0x00, 0x03, 0x1e, 0x00, 0x89, 0x03, 0x24, 0x80, 0x00, 0x89, 0x03, 0x26, 0x00, 0x00, 0x1f, 
        0x14, 0x08, 0x20, 0x00, 0x05, 0x00, 0x00, 0x07, 0x4d, 0x01, 0x89, 0x05, 0x1b, 0x00, 0x00, 0x00, 
        0x00, 0x89, 0x03, 0x19, 0x80, 0x00, 0x03, 0x06, 0x00, 0x03, 0x08, 0x00, 0x03, 0x0a, 0x00, 0x03, 
        0x0c, 0x00, 0x83, 0x03, 0x0e, 0x00, 0x00, 0x83, 0x03, 0x10, 0x00, 0x00, 0x83, 0x03, 0x12, 0x00, 
        0x00, 0x02, 0x04, 0x2b, 0x02, 0x05, 0x3f, 0x02, 0x06, 0x3f, 0x04, 0x0a, 0x30, 0x04, 0x1a, 0x30, 
        0x04, 0x2a, 0x30, 0x04, 0x3a, 0x0a, 0x04, 0x2c, 0x00, 0x07, 0x48, 0x00,
};

#endif