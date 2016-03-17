
#define DETECT_THRESHOLD	(1000)			//a = 10.00
#define MCONVERT_PARA		981 / 1024		//g = 9.81
static int x_data_bak = 0;
static int y_data_bak = 0;
static int z_data_bak = 0;
static int cnt_exceed_threshold = -1;
static DECLARE_COMPLETION (completion_for_notifier);

#define NOTIFIER_MAJOR_GSENSOR_STATUS_CHANGE	(130)
#define NOTIFIER_MINOR_EXCEED_THRESHOLD 		(10)

static int thread_notifier_func(void *data)
{
	while(1)
	{
		wait_for_completion(&completion_for_notifier);
		lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_GSENSOR_STATUS_CHANGE, NOTIFIER_MINOR_EXCEED_THRESHOLD));
	}
	return 0;
}

static void get_gsensor_data(int x, int y, int z)
{
	pr_debug("gsensor data, x = %-4d , y = %-4d, z = %-4d\n",
		-x * MCONVERT_PARA,
		-y * MCONVERT_PARA,
		-z * MCONVERT_PARA);
	pr_debug("gsensor delta data---------------------------, x = %-4ld , y = %-4ld, z = %-4ld\n",
			abs(x - x_data_bak)* MCONVERT_PARA,
			abs(y - y_data_bak)* MCONVERT_PARA,
			abs(z - z_data_bak)* MCONVERT_PARA);

	if(cnt_exceed_threshold == -1)
	{
		cnt_exceed_threshold = 0;
		x_data_bak = x;
		y_data_bak = y;
		z_data_bak = z;
	}
	else
	{
		if ((abs(x - x_data_bak)* MCONVERT_PARA > DETECT_THRESHOLD) ||
			(abs(y - y_data_bak)* MCONVERT_PARA > DETECT_THRESHOLD) ||
			(abs(z - z_data_bak)* MCONVERT_PARA > DETECT_THRESHOLD))
		{
			cnt_exceed_threshold++;
			fs_mem_log("gsensor previous data, x = %d , y = %d, z = %d\n", -x * MCONVERT_PARA, -y * MCONVERT_PARA, -z * MCONVERT_PARA);
			fs_mem_log("gsensor current  data, x = %d , y = %d, z = %d\n", -x_data_bak * MCONVERT_PARA, -y_data_bak * MCONVERT_PARA, -z_data_bak * MCONVERT_PARA);
			fs_mem_log("gsensor delta    data, x = %d , y = %d, z = %d, cnt = %d\n\n",
						abs(x - x_data_bak)* MCONVERT_PARA,
						abs(y - y_data_bak)* MCONVERT_PARA,
						abs(z - z_data_bak)* MCONVERT_PARA,
						cnt_exceed_threshold);

			complete(&completion_for_notifier);
		}
		x_data_bak = x;
		y_data_bak = y;
		z_data_bak = z;
	}
	return;
}

static void crash_detect_init(void)
{
	CREATE_KTHREAD(thread_notifier_func, NULL);
}

