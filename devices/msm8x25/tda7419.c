#include "lidbg.h"

//#define tda7419_MASTER

u8 tda7419_config[] =
{

    0x34, 0x5f,


    0xFF		// end flag

};


int audio_7419_init(void)
{
#if 0
    u32 i = 0;
    u8 data = 0;

    lidbg("tda7419 init+\n");

    SOC_IO_Output(6, 14, 0);//reset
    msleep(100);
    SOC_IO_Output(6, 14, 1);
    msleep(100);

    while(tda7419_config[i*2] != 0xff)
    {
        SOC_I2C_Send(1, 0x4a, &tda7419_config[i*2] , 2);
        i++;
    }

    i = 0;
    while(i < 0x35)
    {
        data = 0xff;
        SOC_I2C_Rec(1, 0x4a, i, &data , 1);
        lidbg("%x %x\n", i, data);

        i++;
    }

    lidbg("tda7419 init-\n");
#endif
    return 1;
}
//EXPORT_SYMBOL(audio_codec_init);


