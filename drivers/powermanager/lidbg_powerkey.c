#include "lidbg.h"

#define DEV_NAME	 "/dev/lidbg_pm0"
#define SCREEN_ON    "flyaudio screen_on"
#define SCREEN_OFF   "flyaudio screen_off"
#define DEVICES_ON   "flyaudio devices_up"
#define DEVICES_DOWN "flyaudio devices_down"
#define ANDROID_UP	 "flyaudio android_up"
#define ANDROID_DOWN "flyaudio android_down"
#define GOTO_SLEEP   "flyaudio gotosleep"
#define POWER_SUSPEND_TIME   (jiffies + 10*HZ)//delay between screenoff->devicedown->androiddown
#define AirplanMode_TIME     (jiffies + 10*HZ)//wakelock hold time between androiddown->wakelock release
#define POWERKEY_DELAY_TIME     (jiffies + 0*HZ)//wakelock hold time between androiddown->wakelock release
#define POWERKEY_FIFO_SIZE (512)

static struct notifier_block fb_notif;
static struct timer_list timer;
static atomic_t status = ATOMIC_INIT(FLY_SCREEN_ON);
static DECLARE_COMPLETION (sleep_powerkey_wait);
static DECLARE_COMPLETION (read_powerkey_wait);
static struct semaphore powerkey_sem;
static struct kfifo powerkey_state_fifo;
static unsigned int *powerkey_state_buffer;

void powerkey_fifo_in(void)
{
	down(&powerkey_sem);
	kfifo_in(&powerkey_state_fifo, (const void *)&atomic_read(&status), 4);
	up(&powerkey_sem);
}

static int thread_powerkey_func(void *data)
{
	while(1)
	{
		wait_for_completion(&sleep_powerkey_wait);
		lidbg("%s = %d\n", __FUNCTION__,atomic_read(&status));
		powerkey_fifo_in();
		complete(&read_powerkey_wait);
		switch(atomic_read(&status))
		{
			case FLY_SCREEN_OFF:
				fs_file_write(DEV_NAME, false, SCREEN_OFF, 0, strlen(SCREEN_OFF));
				mod_timer(&timer,POWER_SUSPEND_TIME);
				break;
			case FLY_DEVICE_DOWN:
				fs_file_write(DEV_NAME, false, DEVICES_DOWN, 0, strlen(DEVICES_DOWN));
				mod_timer(&timer,POWER_SUSPEND_TIME);
				break;
			case FLY_ANDROID_DOWN:
				fs_file_write(DEV_NAME, false, ANDROID_DOWN, 0, strlen(ANDROID_DOWN));
				mod_timer(&timer,AirplanMode_TIME);
				break;
			case FLY_GOTO_SLEEP:
				fs_file_write(DEV_NAME, false, GOTO_SLEEP, 0, strlen(GOTO_SLEEP));
				break;
			case FLY_ANDROID_UP:
				fs_file_write(DEV_NAME, false, ANDROID_UP, 0, strlen(ANDROID_UP));
				mod_timer(&timer,POWERKEY_DELAY_TIME);
				break;
			case FLY_DEVICE_UP:
				fs_file_write(DEV_NAME, false, DEVICES_ON, 0, strlen(DEVICES_ON));
				mod_timer(&timer,POWERKEY_DELAY_TIME);
				break;
			case FLY_SCREEN_ON:
				fs_file_write(DEV_NAME, false, SCREEN_ON, 0, strlen(SCREEN_ON));
				mod_timer(&timer,POWERKEY_DELAY_TIME);
				break;
			default:
				break;
		}
	}
	return 0;
}

static void powerkey_suspend_timer(unsigned long data)
{
	if(atomic_read(&status) != FLY_SCREEN_ON)
	{
		atomic_inc(&status);
		complete(&sleep_powerkey_wait);
	}
	return;
}

static void powerkey_suspend(void)
{
	lidbg("powerkey_suspend status = %d\n", atomic_read(&status));

	if(atomic_read(&status) == FLY_SCREEN_ON )
	{
		atomic_set(&status, FLY_SCREEN_OFF);
		complete(&sleep_powerkey_wait);
	}
	return;
}

static void powerkey_resume(void)
{
	lidbg("powerkey_resume status = %d\n", atomic_read(&status));

	if(atomic_read(&status) == FLY_SCREEN_OFF)
	{
		del_timer(&timer);
		atomic_set(&status, FLY_SCREEN_ON);
	}
	else if(atomic_read(&status) == FLY_DEVICE_DOWN)
	{
		del_timer(&timer);
		atomic_set(&status, FLY_DEVICE_UP);
	}
	else if(atomic_read(&status) == FLY_ANDROID_DOWN)
	{
		return;
	}
	else
	{
		atomic_set(&status, FLY_ANDROID_UP);
	}
	complete(&sleep_powerkey_wait);
	return;
}


static int fb_notifier_callback(struct notifier_block *self,
				 unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int *blank;
	if (evdata && evdata->data && event == FB_EVENT_BLANK)
	{
		blank = evdata->data;
		if (*blank == FB_BLANK_UNBLANK)
			powerkey_resume();
		else if (*blank == FB_BLANK_POWERDOWN)
			powerkey_suspend();
	}

	return 0;
}
int powerkey_open (struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t  powerkey_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
	int bytes;
	int powerkey_state;
	if(atomic_read(&status) != FLY_GOTO_SLEEP)
		wait_for_completion(&read_powerkey_wait);
	else
	{
	       lidbg("kfifo_len=%d\n",kfifo_len(&powerkey_state_fifo));
		powerkey_fifo_in();
	}

	if( kfifo_len(&powerkey_state_fifo) == 0)
	{
	    lidbg("copy_to_user ERR\n");
	    return -1;
	}
	down(&powerkey_sem);
	bytes = kfifo_out(&powerkey_state_fifo, &powerkey_state, 4);
	up(&powerkey_sem);

	if (copy_to_user(buffer, &powerkey_state,  4))
	{
		lidbg("copy_to_user ERR\n");
	}
	return size;
}

ssize_t powerkey_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}

static  struct file_operations powerkey_fops =
{
    .owner = THIS_MODULE,
    .open = powerkey_open,
    .read = powerkey_read,
    .write = powerkey_write,
};

static int lidbg_powerkey_probe(struct platform_device *pdev)
{
	int ret;

	init_timer(&timer);
	timer.function = &powerkey_suspend_timer;
	timer.data = 0;
	timer.expires = 0;

	fb_notif.notifier_call = fb_notifier_callback;
	ret = fb_register_client(&fb_notif);
	if (ret)
		PM_ERR("Unable to register fb_notifier: %d\n",ret);

	powerkey_state_buffer = (unsigned int *)kmalloc(POWERKEY_FIFO_SIZE, GFP_KERNEL);
	if(powerkey_state_buffer == NULL)
	{
	    lidbg("powerkey kmalloc state buffer error.\n");
	    return 0;
	}

	sema_init(&powerkey_sem, 1);
	kfifo_init(&powerkey_state_fifo, powerkey_state_buffer, POWERKEY_FIFO_SIZE);
	lidbg_new_cdev(&powerkey_fops, "flyaudio_pm");
	CREATE_KTHREAD(thread_powerkey_func, NULL);
	return 0;
}

static struct platform_device lidbg_powerkey =
{
    .name               = "lidbg_powerkey",
    .id                 = -1,
};

static struct platform_driver lidbg_powerkey_driver =
{
    .probe		= lidbg_powerkey_probe,
    .driver     = {
        .name = "lidbg_powerkey",
        .owner = THIS_MODULE,
    },
};

static int __init lidbg_powerkey_init(void)
{
    DUMP_FUN;
    platform_device_register(&lidbg_powerkey);
    platform_driver_register(&lidbg_powerkey_driver);
    return 0;
}

static void __exit lidbg_powerkey_exit(void)
{
}

module_init(lidbg_powerkey_init);
module_exit(lidbg_powerkey_exit);

MODULE_DESCRIPTION("lidbg.powerkey");
MODULE_LICENSE("GPL");
