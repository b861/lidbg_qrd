#ifndef __SAF7741_H__
#define __SAF7741_H__

#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/syscalls.h>
#include <asm/system.h>

#include <linux/workqueue.h>

#if 1
#define BYTE u8
#define UINT u32
#define UINT32 u32
#define BOOL bool
#define ULONG u32

#define FALSE 0
#define TRUE 1
#endif

#ifdef SOC_msm8x26
#define I2C_API_XFER_MODE_SEND 1
#define I2C_API_XFER_MODE_RECV 2
#define I2C_API_XFER_MODE_RECV_SUBADDR_2BYTES 3

#define 	SAF7741_GPIO_RST          (LPC)  //no use

#define 	SAF7741_I2C_BUS  		  (5)
#define 	SAF7741_I2C_ADDR	      0x1c

#define     SAF7741_ADDR_W			  0x38
#define     SAF7741_ADDR_R		      0x39

#define SAF7741_SILICON_125     111
#define SAF7741_SILICON_140     222
#define SAF7741_SILICON_SEL     SAF7741_SILICON_125

enum audio_source{Init=0,MediaCD,CDC,RADIO,AUX,IPOD,TV,MediaMP3,SRADIO,A2DP,EXT_TEL,GR_AUDIO,MediaSystem,BACK,GPS,BT_RING,BT,DVR,VAP,FRONT_CAMERA,SYSTEM_RING,AUDIO_CHANNEL_MAX};

#define efIsExboxChannel(iChn)			(TV == iChn || DVR == iChn)

#define BALANCE_LEVEL_COUNT		21

#define FADER_LEVEL_COUNT		21

#define BASS_FREQ_COUNT		    3
#define BASS_LEVEL_COUNT	    11

#define MID_FREQ_COUNT		    3
#define MID_LEVEL_COUNT		    11

#define M_FREQ_500              0
#define M_FREQ_1000             1 
#define M_FREQ_1500             2

#define TREB_FREQ_COUNT		    2
#define TREB_LEVEL_COUNT	    11

#define T_FREQ_10K              0
#define T_FREQ_12K              1
#define T_FREQ_15K              2
#define T_FREQ_17K5             3


#else  //8974

#endif


#endif  //__SAF7741_H__

