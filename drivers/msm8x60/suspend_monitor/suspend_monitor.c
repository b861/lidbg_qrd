

#include "lidbg.h"

#define DEVICE_NAME "suspend_monitor"

#define LED_FLASH_FAST (500)
#define LED_FLASH_SLOW (2000)

char const*const BUTTON_FILE
        = "/sys/class/leds/button-backlight/brightness";
int suspend_times = 0;

void led_ctrl(bool on)
{
	int len;
	char buf[4];
	len = sprintf(buf, "%d", on);
	lidbg_readwrite_file(BUTTON_FILE, NULL, buf, len);
}

int sleep_time;
void led_tigger()
{
    static int status = 0;
	status = status % 2;
	led_ctrl(status);
	if(status==1)
		msleep(sleep_time/10);
	else
		msleep(sleep_time);
	
	status++;
}

int suspend_monitor_open (struct inode *inode, struct file *filp)
{
    return 0;
}


ssize_t suspend_monitor_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}


ssize_t suspend_monitor_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}



static  struct file_operations suspend_monitor_fops =
{
    .owner = THIS_MODULE,
    .read = suspend_monitor_read,
    .write = suspend_monitor_write,
    .open = suspend_monitor_open,
};


#ifdef CONFIG_HAS_EARLYSUSPEND
static void suspend_monitor_early_suspend(struct early_suspend *h)
{
	sleep_time = LED_FLASH_FAST;
    DUMP_FUN;
}

static void suspend_monitor_late_resume(struct early_suspend *h)
{
	sleep_time = LED_FLASH_SLOW;
    DUMP_FUN;
}

#endif


int thread_led(void *data)
{
	led_tigger();
}

static struct task_struct *led_task;
static int  suspend_monitor_probe(struct platform_device *pdev)
{

	
    fs_regist_state("suspend_times", &suspend_times);

	sleep_time = LED_FLASH_SLOW;
    led_task = kthread_create(thread_led, NULL, "fly_gps_server");
    if(IS_ERR(led_task))
    {
        lidbg("Unable to start kernel thread.gps_server_task\n");
    }
    else wake_up_process(led_task);

    return 0;
}


static int  suspend_monitor_remove(struct platform_device *pdev)
{
    return 0;
}


#ifdef CONFIG_PM
static int suspend_monitor_resume(struct device *dev)
{
    DUMP_FUN_ENTER;
	suspend_times ++;
    return 0;

}

static int suspend_monitor_suspend(struct device *dev)
{
    DUMP_FUN_ENTER;
    return 0;

}

static struct dev_pm_ops suspend_monitor_pm_ops =
{
    .suspend	= suspend_monitor_suspend,
    .resume		= suspend_monitor_resume,
};
#endif

static struct platform_driver suspend_monitor_driver =
{
    .probe		= suspend_monitor_probe,
    .remove     = suspend_monitor_remove,
    .driver         = {
        .name = "lidbg_suspend_monitor",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &suspend_monitor_pm_ops,
#endif
    },
};

static struct platform_device lidbg_suspend_monitor_device =
{
    .name               = "lidbg_suspend_monitor",
    .id                 = -1,
};


static  int suspend_monitor_init(void)
{
    platform_device_register(&lidbg_suspend_monitor_device);
    return platform_driver_register(&suspend_monitor_driver);

}

static void suspend_monitor_exit(void)
{

}

module_init(suspend_monitor_init);
module_exit(suspend_monitor_exit);

MODULE_LICENSE("GPL");


