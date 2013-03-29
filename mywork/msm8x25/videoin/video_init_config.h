#ifndef __VIDEO_INIT_CONFIG_H__
#define __VIDEO_INIT_CONFIG_H__
#include "i2c_io.h"
#include "tw9912.h"
#include "TC358746XBG.h"
#if 0
#define video_config_debug(msg...)  do { printk( KERN_CRIT "flyvideo: " msg); }while(0)
#else
#define video_config_debug(msg...)  do {}while(0)
#endif

void video_init_config_in(Vedio_Format config_pramat);
void video_io_i2c_init_in(void);
int flyVideoInitall_in(u8 Channel);
Vedio_Format flyVideoTestSignalPin_in(u8 Channel);
Vedio_Format camera_open_video_signal_test_in(void);
int flyVideoImageQualityConfig_in(Vedio_Effect cmd , u8 valu);
void Video_Show_Output_Color(void);
int read_chips_signal_status(u8 cmd);
int IfInputSignalNotStable(void);
#endif

