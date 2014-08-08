
#define BOOT_LIMIT_FREQ (600000)
//#define BOOT_LIMIT_FREQ (652800)    //8974

int ctrl_max_freq = BOOT_LIMIT_FREQ;


static int thread_freq_limit(void *data)
{
	int count = 0;
	
		struct tsens_device tsens_dev;
	//cpu0 temp_sensor_id
	tsens_dev.sensor_num = 5;

	lidbg("create kthread \n");
	
	while(1)
	{
		ctrl_max_freq = BOOT_LIMIT_FREQ;
		{
			int tmp;
			long temp;
			tmp = cpufreq_get(0); //cpufreq.c
			tsens_get_temp(&tsens_dev, &temp);//cpu0 temp
			lidbg("cpufreq=%d,cpu0_temp = %ld\n", tmp,temp);
		}
		count++;
		if(count >= 45)
		{
			ctrl_max_freq = 0;
			lidbg("thread_freq_limit stoped\n");
			return 1;
		}
		msleep(1000);

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


