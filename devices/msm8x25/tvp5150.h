

#ifndef __SOC_TVP5150__
#define __SOC_TVP5150__


int video_codec_init(void);
void tvp5150_set_high_z(bool status);


#if defined(FLY_BOARD_2ST) || defined(FLY_BOARD_3ST)
#define TVP5150_POWER_UP  do{SOC_IO_Output(5, 33, 1);}while(0)
#define TVP5150_POWER_DOWN  do{SOC_IO_Output(5, 33, 0);}while(0)
#else
#define TVP5150_POWER_UP  //do{SOC_IO_Output(5, 11, 1);}while(0)
#define TVP5150_POWER_DOWN  //do{SOC_IO_Output(5, 11, 0);}while(0)
#endif


#define TVP5150_RESET_LOW  do{    SOC_IO_Output(4, 33, 0);}while(0)
#define TVP5150_RESET_HIGH  do{    SOC_IO_Output(4, 33, 1);}while(0)

#define TVP5150_RESET  do{    TVP5150_RESET_LOW; \
    							msleep(100); \
    							TVP5150_RESET_HIGH;\
    							msleep(100);}while(0)




#endif
