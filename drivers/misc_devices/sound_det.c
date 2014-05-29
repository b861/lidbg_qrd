#include "lidbg.h"

static int sound_detect_dbg = 0;
static int sound_detect_delay_ms = 200;
static int is_system_sound_on = 0;
static int has_called_user_on = 0;
int g_length;

int gps_status = -1;

struct completion GPS_status_sem;

int  iGPS_sound_status(void)
{
    wait_for_completion(&GPS_status_sem);
    return gps_status;
}

int SOC_Get_System_Sound_Status_func(void *para, int length)
{
	int i;
	char *p = (char *)para;
	
    if(sound_detect_dbg)
        lidbg("%d\n " , length);

	for(i=0;i<20;i++)
	{
		if(p[i] != 0)
			goto data_not_all_zero;
	}

	for(i=1;i<20;i++)
	{
		if(p[length - i] != 0)
			goto data_not_all_zero;
	}

	//is_system_sound_on = 0;
	g_length = 0;
	if(sound_detect_dbg)lidbg("data_all_zero\n");
	
	return 0;


data_not_all_zero:	
    is_system_sound_on = 1;
    g_length = length;
    return 1;
}

int thread_sound_detect(void *data)
{
	int i;
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
                lidbg("music_start,len=%d\n",g_length);
                gps_status = 1;
                complete(&GPS_status_sem);
            }
        }
        else if(has_called_user_on)
        {
            has_called_user_on = 0;
            lidbg("music_stop,len=%d\n",g_length);
            gps_status = 2;
            complete(&GPS_status_sem);
        }

		if(0)
		{
			int len;
			len = g_length;
			sound_detect_delay_ms = (2000 * g_length / 32768) / 50 ;
	        if(sound_detect_dbg)
	            lidbg("msleep=%d times\n " ,  sound_detect_delay_ms);
			
			for(i = 0; i < sound_detect_delay_ms ; i++)
			{
				if(g_length != len)
					break;
	        	msleep(50);
			}
		}
		else
		{
			msleep(sound_detect_delay_ms);
		}
		
   }
    return 0;
}


