#include "lidbg.h"

#define TAG "sound:"

static int sound_detect_dbg = 0;
int gps_status = -1;

struct completion GPS_status_sem;

int sound_detect_event(bool started)
{
    if(started)
    {
        lidbg(TAG"music_start,%d\n", started);
        gps_status = 1;
        complete(&GPS_status_sem);
    }
    else
    {
        lidbg(TAG"music_stop,%d\n", started);
        gps_status = 2;
        complete(&GPS_status_sem);
    }
	return 1;
}

void cb_sound_detect(char *key, char *value )
{
	lidbg_setprop("persist.lidbg.sound.dbg", value);
	lidbg(TAG"persist.lidbg.sound.dbg=%s\n", value);
}
int sound_detect_init(void)
{
    init_completion(&GPS_status_sem);
    FS_REGISTER_INT(sound_detect_dbg, "sound_detect_dbg", 0, cb_sound_detect);
    lidbg(TAG"sound_detect_init\n");

	return 1;
}

int  iGPS_sound_status(void)
{
    wait_for_completion(&GPS_status_sem);
    return gps_status;
}

int SOC_Get_System_Sound_Status_func(void *para, int length)
{
    return 1;
}



