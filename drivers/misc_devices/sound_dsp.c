
#include "lidbg.h"

int thread_sound_dsp_init(void *data)
{
    DUMP_FUN;
    LPC_CMD_RADIORST_L;
    msleep(100);
    LPC_CMD_RADIORST_H;
    msleep(100);
    LPC_CMD_RADIO_INIT;
    msleep(3000);
    LPC_CMD_RADIO_SET;
    return 0;
}
