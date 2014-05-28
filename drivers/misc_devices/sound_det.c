#include "lidbg.h"

static int sound_detect_dbg = 0;
static int sound_detect_delay_ms = 2300;
static int is_system_sound_on = 0;
static int has_called_user_on = 0;

int gps_status = -1;

struct completion GPS_status_sem;
void GPS_sound_init(void)
{
}
int  GPS_sound_status(void)
{
    wait_for_completion(&GPS_status_sem);
    return gps_status;
}

int SOC_Get_System_Sound_Status_func(void *para, int length)
{
    is_system_sound_on = 1;
    sound_detect_delay_ms = 2000 * length / 32768 ;
    if(sound_detect_dbg)
        printk(KERN_CRIT"*%d,%d\n " , length, sound_detect_delay_ms);
    sound_detect_delay_ms = sound_detect_delay_ms + sound_detect_delay_ms / 5;
    return 1;
}

int thread_sound_detect(void *data)
{
    init_completion(&GPS_status_sem);
    FS_REGISTER_INT(sound_detect_dbg, "sound_detect_dbg", 0, NULL);
    lidbg("System_Sound_Status.start\n");
    while(1)
    {
        if(is_system_sound_on == 1)
        {
            is_system_sound_on = 0;
            if(!has_called_user_on)
            {
                has_called_user_on = 1;
                lidbg("music_start\n");
                gps_status = 1;
                complete(&GPS_status_sem);
            }
        }
        else if(has_called_user_on)
        {
            has_called_user_on = 0;
            lidbg("music_stop\n");
            gps_status = 2;
            complete(&GPS_status_sem);
        }
        if(sound_detect_dbg)
            printk(KERN_CRIT"msleep=%d\n " ,  sound_detect_delay_ms);
        msleep(sound_detect_delay_ms);
    }
    return 0;
}

EXPORT_SYMBOL(GPS_sound_init);
EXPORT_SYMBOL(GPS_sound_status);

