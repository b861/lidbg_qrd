#define PLATFORM_MSM8226 1
//#define PLATFORM_MSM8974 1
#define  RECOVERY_MODE_DIR "/sbin/recovery"


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
	{1, 50,  1344000,"1344000"},
	{51, 60,  1190400,"1190400"},
	{61, 80,  998400, "998400"},
	{81, 90, 787200, "787200"},
	{91,100, 600000, "600000"},
	{101,500, 300000, "300000"},
	{0,0, 0, "0"},//end flag

};

#elif defined(PLATFORM_MSM8974)

struct thermal_ctrl cpu_thermal[] = 
{
        //{1,  60,  2265600,"2265600"},
        {1, 70,  1728000,"1728000"},
        {71, 80,  1267200,"1267200"},
        {81, 90, 960000,"960000"},
        {91,100, 652800, "652800"},
        {101,150, 300000, "300000"},
		{0,0, 0, "0"}//end flag
};


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
		{
			int tmp;
			long temp;
			u32 max_freq;
			tmp = cpufreq_get(0); //cpufreq.c
			max_freq = get_scaling_max_freq();
			tsens_get_temp(&tsens_dev, &temp);//cpu0 temp
			lidbg("cpufreq=%d,maxfreq=%d,cpu0_temp = %ld\n", tmp,max_freq,temp);

			for(i = 0; i < SIZE_OF_ARRAY(cpu_thermal); i++)
			{
				if((cpu_thermal[i].temp_low == 0)||(cpu_thermal[i].temp_high == 0))
					break;
			
				if((temp >= cpu_thermal[i].temp_low ) && (temp <= cpu_thermal[i].temp_high ) 
					  && (max_freq != cpu_thermal[i].limit_freq))
				{

#ifdef PLATFORM_MSM8974
					if(!is_file_exist(RECOVERY_MODE_DIR))
					{
						break;
					}
#endif
					lidbg_readwrite_file(FREQ_MAX_NODE, NULL, cpu_thermal[i].limit_freq_string, strlen(cpu_thermal[i].limit_freq_string));
					lidbg("kernel:set max freq to: %d,temp:%ld\n", cpu_thermal[i].limit_freq,temp);
					ctrl_max_freq = cpu_thermal[i].limit_freq;
					cpufreq_update_policy(0);
					break;
				}
			}
		}
		count++;
		if(count >= 45*4)
		{
			ctrl_max_freq = 0;
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
		if(ctrl_max_freq != 0)
		{
			policy->max = ctrl_max_freq;
			lidbg("%s: mitigating cpu %d to freq max: %u min: %u\n",
			KBUILD_MODNAME, policy->cpu, policy->max, policy->min);
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


