
#include "lidbg.h"
int temp_log_freq = 10;
//static int fan_onoff_temp;
static int cpu_temp_time_minute = 20;
static bool is_cpu_temp_enabled = false;

#define FREQ_MAX_NODE    "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define TEMP_LOG_PATH 	 LIDBG_LOG_DIR"log_ct.txt"
#define TEMP_FREQ_TEST_RESULT LIDBG_LOG_DIR"lidbg_temp_freq.txt"
#define TEMP_FREQ_COUNTER LIDBG_LOG_DIR"freq_tmp.txt"

int get_file_int(char *file)
{
    char cpu_temp[3];
    int temp = -1;
    fs_file_read(file, cpu_temp, sizeof(cpu_temp));
    temp = simple_strtoul(cpu_temp, 0, 0);
    return temp;
}

int soc_temp_get(void)
{
	return get_file_int(CPU_TEMP_PATH);
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
    int cur_frq, cur_temp;
    DUMP_FUN;
    cur_frq = FREQ_STEP0;
    temp_init();

	if(g_var.recovery_mode == 1)
	{
		int cpu_freq;
		while(1)
		{
			lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_RECOVERY_STRING, sizeof(FREQ_RECOVERY_STRING) - 1);
			set_cpu_governor(0);
			ssleep(5);
			cpu_freq = SOC_Get_CpuFreq();
			cur_temp = soc_temp_get();
			lidbg("cpufreq=%d,temp=%d\n", cpu_freq,cur_temp);
		}
	}

    while(is_cpu_temp_enabled)
    {
        lidbg("set max freq to: disabled\n");
        ssleep(10);
    }

    while(!kthread_should_stop())
    {
        msleep(1000);

        log_temp();
        cur_temp = soc_temp_get();
        //lidbg("MSM_THERM: %d\n",cur_temp);
        
        if     ((cur_temp > THRESHOLDS_STEP0 ) && (cur_temp <= THRESHOLDS_STEP1 ) && (cur_frq != FREQ_STEP0))
        {
            lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_STEP0_STRING, sizeof(FREQ_STEP0_STRING) - 1);
            lidbg("set max freq to: %d,temp:%d\n", FREQ_STEP0,cur_temp);
            cur_frq = FREQ_STEP0;
        }
        else if((cur_temp > THRESHOLDS_STEP1 ) && (cur_temp <= THRESHOLDS_STEP2 ) && (cur_frq != FREQ_STEP1))
        {
            lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_STEP1_STRING, sizeof(FREQ_STEP1_STRING) - 1);
            lidbg("set max freq to: %d,temp:%d\n", FREQ_STEP1,cur_temp);
            cur_frq = FREQ_STEP1;
        }
        else if((cur_temp > THRESHOLDS_STEP2 ) && (cur_temp <= THRESHOLDS_STEP3 ) && (cur_frq != FREQ_STEP2))
        {
            lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_STEP2_STRING, sizeof(FREQ_STEP2_STRING) - 1);
            lidbg("set max freq to: %d,temp:%d\n", FREQ_STEP2,cur_temp);
            cur_frq = FREQ_STEP2;
        }
		else if((cur_temp > THRESHOLDS_STEP3 ) && (cur_frq != FREQ_STEP3))
        {
            lidbg_readwrite_file(FREQ_MAX_NODE, NULL, FREQ_STEP3_STRING, sizeof(FREQ_STEP3_STRING) - 1);
            lidbg("set max freq to: %d,temp:%d\n", FREQ_STEP3,cur_temp);
            cur_frq = FREQ_STEP3;
        }

    }
    return 0;
}

static char temp_freq_test_str[256];
int thread_start_cpu_tmp_test(void *data)
{
    char *group[15] = {NULL}, buff[56] = {0};
    int group_num  = 0, freq_pos = 0, int_time_count = 0;

    ssleep(60);//waritting for system boot complete
    strcpy(temp_freq_test_str, TEMP_FREQ_TEST_STR);
    group_num = lidbg_token_string(temp_freq_test_str, ",", group) ;

    if((freq_pos = get_file_int(TEMP_FREQ_COUNTER)) < 0)
        freq_pos = 0;
    if(freq_pos >= group_num)
        goto err;
    fs_clear_file(TEMP_FREQ_COUNTER);
    fs_string2file(0, TEMP_FREQ_COUNTER, "%d", freq_pos + 1);

	freq_pos = group_num - freq_pos - 1;

    lidbg("%d,start_cpu_tmp_test: %s\n", cpu_temp_time_minute, TEMP_FREQ_TEST_STR);

    lidbg_shell_cmd("am start -n com.into.stability/com.into.stability.Run");
    ssleep(2);
    lidbg_shell_cmd("am start -n com.into.stability/.TestClassic");

    lidbg_readwrite_file(FREQ_MAX_NODE, NULL, group[freq_pos], strlen(group[freq_pos]));
    fs_file_separator(TEMP_FREQ_TEST_RESULT);

    sprintf(buff, "[%s]%d,%d", group[freq_pos], freq_pos, group_num);
    lidbg_toast_show(buff, 0);

    while(1)
    {
        int cur_temp;
        int_time_count++;
        cur_temp = soc_temp_get();
        fs_string2file(100, TEMP_FREQ_TEST_RESULT, "%d,temp=%d,time=%d,freq=%s:\n", freq_pos, cur_temp, int_time_count * 2, group[freq_pos]);
        lidbg("%d,temp=%d,time=%d,freq=%s:\n", freq_pos, cur_temp, int_time_count * 2, group[freq_pos]);
        ssleep(2);
        if(int_time_count * 2 > cpu_temp_time_minute * 60)
            lidbg_reboot();
    }
    return 0;

err:
    is_cpu_temp_enabled = false;
    sprintf(buff, "cpu_tmp_test.stop:%d,%d", freq_pos, group_num);
    lidbg_toast_show(buff, 0);
    lidbg_rm(TEMP_FREQ_COUNTER);
    fs_string2file(100, TEMP_FREQ_TEST_RESULT, "-------stop-------\n");
    return 0;
}
void cb_kv_cpu_temp_test(char *key, char *value)
{
    if(value && *value == '1')
    {
        lidbg_rm(TEMP_FREQ_TEST_RESULT);
        fs_file_write(TEMP_FREQ_COUNTER, "0");
        ssleep(1);
        lidbg_reboot();
    }
    else
        fs_mem_log("cb_kv_cpu_temp_test:fail,%s\n", value);
}
void temp_init(void)
{
    FS_REGISTER_KEY( "cpu_temp_test", cb_kv_cpu_temp_test);
    fs_register_filename_list(TEMP_FREQ_TEST_RESULT, true);
    FS_REGISTER_INT(cpu_temp_time_minute, "cpu_temp_time_minute", 20, NULL);

    if(fs_is_file_exist(TEMP_FREQ_COUNTER))
    {
        is_cpu_temp_enabled = true;
        CREATE_KTHREAD(thread_start_cpu_tmp_test, NULL);
    }

    fs_register_filename_list(TEMP_LOG_PATH, true);
    fs_regist_state("cpu_temp", &(g_var.temp));

    //FS_REGISTER_INT(fan_onoff_temp, "fan_onoff_temp", 65, NULL);
}


