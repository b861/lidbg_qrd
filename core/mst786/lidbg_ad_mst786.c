
#include "lidbg.h"
extern int start_ADC(int ch, int mode);

u8 soc_ad_request_log[AD_LOG_NUM];

void  soc_ad_init(void)
{
    memset(soc_ad_request_log, 0xff, AD_LOG_NUM);//no use here
}

unsigned int  soc_ad_read(unsigned int channel)
{
    u32 value;
    int mode = 0;  //default freerun mode

    if(channel >= ADC_MAX_CH)
    {
        lidbg( "channel err!\n");
        return 0xffffffff;
    }
    value = start_adc(channel, mode);

    return value;
}


EXPORT_SYMBOL(soc_ad_read);


