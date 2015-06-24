#define PLATFORM_MSM8226 1
//#define PLATFORM_MSM8974 1
#define ANDROID_5_LATER

#define  RECOVERY_MODE_DIR "/sbin/recovery"


bool is_recovery_mode = 0;
bool ctrl_en = 1;

struct thermal_ctrl
{
	int temp_low;
	int temp_high;
	u32 limit_freq;
	char* limit_freq_string;
};

#ifdef PLATFORM_MSM8226
struct thermal_ctrl cpu_thermal[] = 
{
	//{1,  60,  1401600,"1401600"},
	{-500, 50,  1344000,"1344000"},
	{51,55,  1190400,"1190400"},
	{56,60,  998400, "998400"},
	{61,70,  600000, "600000"},
	{71,500, 300000, "300000"},
	{0,0, 0, "0"},//end flag

};

#elif defined(PLATFORM_MSM8974)
#ifdef ANDROID_5_LATER
struct thermal_ctrl cpu_thermal[] = 
{
        {-500,  50,  2265600,"2265600"},
		{61, 65,  1958400,"1958400"},
        {66, 70,  1728000,"1728000"},
        {71, 75,  1497600,"1497600"},
        {76, 80,  1267200,"1267200"},
		{81, 85,  1190400,"1190400"},
		{86, 90,  960000, "960000"},
        {91, 95,  729600, "729600"},
		{96, 500, 300000, "300000"},
		{0,0, 0, "0"}//end flag
};
#else
struct thermal_ctrl cpu_thermal[] = 
{
        {-500,  50,  2265600,"2265600"},
		{51, 55,  1958400,"1958400"},
        {56, 60,  1728000,"1728000"},
        {61, 65,  1497600,"1497600"},
        {66, 70,  1267200,"1267200"},
		{71, 75,  1190400,"1190400"},
		{76, 80,  960000, "960000"},
        {81, 90,  729600, "729600"},
		{91, 500, 300000, "300000"},
		{0,0, 0, "0"}//end flag
};
#endif

#endif


#define FREQ_MAX_NODE "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"

int ctrl_max_freq = 0;

u32 get_scaling_max_freq(void)
{
	static char max_freq[32];
	static u32 tmp;
	lidbg_readwrite_file(FREQ_MAX_NODE, max_freq, NULL, 32);
    tmp = simple_strtoul(max_freq, 0, 0);
	//lidbg("scaling_max_freq=%d,%s\n", tmp,max_freq);
	return tmp;
}


static int thread_freq_limit(void *data)
{
	int count = 0;
	int i = 0;
	
		struct tsens_device tsens_dev;
	//cpu0 temp_sensor_id
	tsens_dev.sensor_num = 5;

	lidbg("create kthread \n");
	
	while(1)
	{
			int tmp;
			long temp;
			u32 max_freq = 0;
			tmp = cpufreq_get(0); //cpufreq.c
#ifndef ANDROID_5_LATER
			max_freq = get_scaling_max_freq();
#endif
			tsens_get_temp(&tsens_dev, &temp);//cpu0 temp
			lidbg("cpufreq=%d,maxfreq=%d,cpu0_temp = %ld\n", tmp,max_freq,temp);
			for(i = 0; i < SIZE_OF_ARRAY(cpu_thermal); i++)
			{
				if((cpu_thermal[i].temp_low == 0)||(cpu_thermal[i].temp_high == 0))
					break;
			
				if((temp >= cpu_thermal[i].temp_low ) && (temp <= cpu_thermal[i].temp_high ) 
					  && (max_freq != cpu_thermal[i].limit_freq))
				{
#ifndef ANDROID_5_LATER
					lidbg_readwrite_file(FREQ_MAX_NODE, NULL, cpu_thermal[i].limit_freq_string, strlen(cpu_thermal[i].limit_freq_string));
					lidbg("kernel:set max freq to: %d,temp:%ld\n", cpu_thermal[i].limit_freq,temp);
#endif
					ctrl_max_freq = cpu_thermal[i].limit_freq;
					cpufreq_update_policy(0);
					break;
				}
			}

		count++;
		
		if(count >= 20*4)
			is_recovery_mode = is_file_exist(RECOVERY_MODE_DIR);
		
		if((count >= 90*4) && (temp < 100))
		{
			if(is_recovery_mode == 0)
				is_recovery_mode = is_file_exist(RECOVERY_MODE_DIR);
			if(is_recovery_mode == 1)
			{
				msleep(1000);
				continue;
			}
#ifdef PLATFORM_MSM8226
			//lidbg_readwrite_file(FREQ_MAX_NODE, NULL, "1593600", strlen("1593600"));
			ctrl_max_freq = 300000;
			cpufreq_update_policy(0);
#else
	#ifndef ANDROID_5_LATER
			lidbg_readwrite_file(FREQ_MAX_NODE, NULL, "2265600", strlen("2265600"));
	#endif			
			if(temp > 100)
				ctrl_max_freq = 300000;
			else
				ctrl_max_freq = 2265600;
			cpufreq_update_policy(0);
#endif

			ctrl_en = 0;
			lidbg("thread_freq_limit stoped\n");
			return 1;
		}
		msleep(250);

	}
    return 1;
}


static int  cpufreq_callback(struct notifier_block *nfb,
		unsigned long event, void *data)
{
	struct cpufreq_policy *policy = data;

	switch (event) {
	case CPUFREQ_NOTIFY:
		if((ctrl_max_freq != 0) && (ctrl_en))
		{
			policy->max = ctrl_max_freq;
			policy->min = 300000;

#ifdef ANDROID_5_LATER
	#ifdef PLATFORM_MSM8974
			if(is_recovery_mode == 1)
				policy->min = ctrl_max_freq;
	#endif
#endif

			//lidbg("%s: mitigating cpu %d to freq max: %u min: %u\n",
			//KBUILD_MODNAME, policy->cpu, policy->max, policy->min);
			break;
		}
		
	}
	return NOTIFY_OK;
}
static struct notifier_block cpufreq_notifier = {
	.notifier_call = cpufreq_callback,
};

void freq_ctrl_start(void)
{
	struct task_struct *task;

	int ret = 0;
	is_recovery_mode = 0;

	ret = cpufreq_register_notifier(&cpufreq_notifier,
			CPUFREQ_POLICY_NOTIFIER);
	if (ret)
		pr_err("%s: cannot register cpufreq notifier\n",
			KBUILD_MODNAME);

	task = kthread_create(thread_freq_limit, NULL, "thread_freq_limit");
	if(IS_ERR(task))
	{
		lidbg("Unable to start thread.\n");
	}
	else wake_up_process(task);
	

}


