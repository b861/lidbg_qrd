
#include "lidbg.h"
int temp_log_freq = 5;
//static int fan_onoff_temp;

#define FREQ_MAX_NODE    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define TEMP_LOG_PATH 	 LIDBG_LOG_DIR"log_ct.txt"


int soc_temp_get(void)
{
    char cpu_temp[3];
    int temp = -1;
    fs_file_read(CPU_TEMP_PATH, cpu_temp, sizeof(cpu_temp));
    temp = simple_strtoul(cpu_temp, 0, 0);
    return temp;
}

void log_temp(void)
{
    static int old_temp, cur_temp;
    int tmp;
    g_var.temp = cur_temp = soc_temp_get();
    tmp = cur_temp - old_temp;
    if((temp_log_freq != 0) && (ABS(tmp) >= temp_log_freq))
    {
        lidbg_fs_log(TEMP_LOG_PATH, "%d\n", cur_temp);
        old_temp = cur_temp;
    }
}
 int thread_thermal(void *data)
{
    int cur_frq,cur_temp;
    DUMP_FUN;
	cur_frq = FREQ_MAX;
	temp_init();

    while(!kthread_should_stop())
    {
        msleep(1000);

        log_temp();
        cur_temp = soc_temp_get();
        //lidbg("MSM_THERM: %d\n",cur_temp);
		
		if((cur_temp > THRESHOLDS_STEP1 ) && (cur_temp =< THRESHOLDS_STEP2 ) && (cur_frq != FREQ_STEP1))
		{
			lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_STEP1_STRING, sizeof(FREQ_STEP1_STRING) - 1);
			lidbg("set max freq to: %d\n",FREQ_STEP1);
		    cur_frq = FREQ_STEP1;
		}
		else if((cur_temp > THRESHOLDS_STEP2 ) && (cur_frq != FREQ_STEP2))
		{
			lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_STEP2_STRING, sizeof(FREQ_STEP2_STRING) - 1);
			lidbg("set max freq to: %d\n",FREQ_STEP2);
		    cur_frq = FREQ_STEP2;
		}
		else if ((cur_temp <= THRESHOLDS_STEP1 ) && (cur_frq != FREQ_MAX))
		{
			lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_MAX_STRING, sizeof(FREQ_MAX_STRING) - 1);
			lidbg("set max freq to: %d\n",FREQ_MAX);
			cur_frq = FREQ_MAX;
		}
			
    }
    return 0;
}


void temp_init(void)
{
    fs_register_filename_list(TEMP_LOG_PATH, true);
    fs_regist_state("cpu_temp", &(g_var.temp));
    //FS_REGISTER_INT(fan_onoff_temp, "fan_onoff_temp", 65, NULL);
}

 
