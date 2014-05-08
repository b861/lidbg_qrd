
#include "lidbg.h"
int temp_log_freq = 5;
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
    int cur_temp;
    DUMP_FUN;
    while(!kthread_should_stop())
    {
        msleep(1000);

        log_temp();
        cur_temp = soc_temp_get();
        //lidbg("MSM_THERM: %d *C\n",cur_temp);
    }
    return 0;
}
