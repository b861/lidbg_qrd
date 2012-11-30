

#ifndef __SOC_CS42L52_
#define __SOC_CS42L52_

#define CS42L52_RESET  do{    SOC_IO_Output(6, 33, 0); \
    							msleep(100); \
    							SOC_IO_Output(6, 33, 1);\
    							msleep(100);}while(0)

int audio_codec_init(void);


#endif
