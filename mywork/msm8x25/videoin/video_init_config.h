#ifndef __VIDEO_INIT_CONFIG_H__
#define __VIDEO_INIT_CONFIG_H__
#include "i2c_io.h"
#include "tw9912.h"
#include "TC358746XBG.h"

void video_init_config_in(Vedio_Format config_pramat);
void video_io_i2c_init_in(void);
int flyVideoInitall_in(u8 Channel);
int flyVideoTestSignalPin_in(u8 Channel);
int flyVideoImageQualityConfig_in(u8 cmd ,u8 valu);
#endif

