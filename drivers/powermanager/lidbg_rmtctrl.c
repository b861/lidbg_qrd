#include "lidbg.h"

#define DEV_NAME	 "/dev/lidbg_pm0"
#define rmtctrl_FIFO_SIZE (512)
#define MCU_ACC_STATE_IO GPIO_FASTBOOT_REQUEST

#define SCREE_OFF_JIFF (5)
#define SCREE_OFF_TIME_S (jiffies + SCREE_OFF_JIFF*HZ)

#define GOTO_SLEEP_JIFF (5)
#define GOTO_SLEEP_TIME_S (jiffies + GOTO_SLEEP_JIFF*HZ)

#define AUTO_SLEEP_JIFF (5)
#define AUTO_SLEEP_TIME_S (jiffies + AUTO_SLEEP_JIFF*HZ)

#define SCREEN_ON    "flyaudio screen_on"
#define SCREEN_OFF   "flyaudio screen_off"
#define DEVICES_ON   "flyaudio devices_up"
#define DEVICES_DOWN "flyaudio devices_down"
#define REQUEST_FASTBOOT "flyaudio request_fastboot"
#define ANDROID_UP	 "flyaudio android_up"
#define ANDROID_DOWN "flyaudio android_down"
#define GOTO_SLEEP   "flyaudio gotosleep"

static struct notifier_block fb_notif;
static atomic_t status = ATOMIC_INIT(FLY_SCREEN_ON);
static wait_queue_head_t wait_queue;
static struct semaphore rmtctrl_sem;
static struct kfifo rmtctrl_state_fifo;
static unsigned int *rmtctrl_state_buffer;
struct work_struct acc_state_work;
static struct timer_list rmtctrl_timer;
static struct wake_lock rmtctrl_wakelock;

typedef enum
{
	FLY_ACC_OFF,
	FLY_ACC_ON,
}FLY_ACC_STATUS;


FLY_ACC_STATUS acc_io_state = FLY_ACC_ON;

void rmtctrl_fifo_in(void)
{
	down(&rmtctrl_sem);
	kfifo_in(&rmtctrl_state_fifo, (const void *)&atomic_read(&status), 4);
	up(&rmtctrl_sem);
}

irqreturn_t acc_state_isr(int irq, void *dev_id)
{
	lidbg(">>>>> Acc state irq is coming =======>>>\n");
    if(!work_pending(&acc_state_work))
        schedule_work(&acc_state_work);

    return IRQ_HANDLED;
}
static void send_app_status(FLY_SYSTEM_STATUS state)
{
		atomic_set(&status, state);
		rmtctrl_fifo_in();
		wake_up_interruptible(&wait_queue);
}
static void rmtctrl_timer_func(unsigned long data)
{
	if(acc_io_state == FLY_ACC_OFF){
       lidbg("rmtctrl_timer_func: goto_sleep %ds later...\n", GOTO_SLEEP_JIFF);
       send_app_status(FLY_GOTO_SLEEP);
       mod_timer(&rmtctrl_timer,GOTO_SLEEP_TIME_S);
	}
	return;
}

void acc_status_handle(FLY_ACC_STATUS val)
{
	if(val == FLY_ACC_ON){
		lidbg("acc_state_work_func: FLY_ACC_ON\n");
		g_var.acc_flag = 1;

		lidbg("*** Set acc.status to 0\n");
		lidbg_shell_cmd("setprop persist.lidbg.acc.status 0");
		lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, NOTIFIER_MINOR_ACC_ON));
		acc_io_state = FLY_ACC_ON;
		wake_lock(&rmtctrl_wakelock);
		send_app_status(FLY_KERNEL_UP);
		send_app_status(FLY_SCREEN_ON);
		fs_file_write(DEV_NAME, false, SCREEN_ON, 0, strlen(SCREEN_ON));
		//USB_WORK_ENABLE;
		//LCD_ON;

		lidbg("acc_state_work_func: FLY_ACC_ON del rmtctrl timer.\n");
		del_timer(&rmtctrl_timer);
	}else{
		lidbg("acc_state_work_func: FLY_ACC_OFF\n");
		g_var.acc_flag = 0;

		lidbg("*** Set acc.status to 1\n");
		lidbg_shell_cmd("setprop persist.lidbg.acc.status 1");
		//USB_WORK_DISENABLE;
		//LCD_OFF;
		lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, NOTIFIER_MINOR_ACC_OFF));
		acc_io_state = FLY_ACC_OFF;
		wake_unlock(&rmtctrl_wakelock);	//ensure KERNEL_UP FB be called before sleep
//		send_app_status(FLY_GOTO_SLEEP);
		send_app_status(FLY_SCREEN_OFF);
		fs_file_write(DEV_NAME, false, SCREEN_OFF, 0, strlen(SCREEN_OFF));
		lidbg("acc_state_work_func: FLY_ACC_OFF, add rmtctrl timer.\n");
		mod_timer(&rmtctrl_timer,SCREE_OFF_TIME_S);
	}
}

static void acc_state_work_func(struct work_struct *work)
{
	int val = -1;

	val = SOC_IO_Input(MCU_ACC_STATE_IO, MCU_ACC_STATE_IO, GPIO_CFG_PULL_UP);
	acc_status_handle(val);
}

static void rmtctrl_suspend(void)
{
	lidbg("rmtctrl_suspend set MCU_APP_GPIO_OFF\n");

	lidbg("acc_state_work_func: FLY_ACC_OFF, set prop AccWakedupState false\n");
	lidbg_shell_cmd("setprop persist.lidbg.AccWakedupState false");

//	lidbg("acc_state_work_func: FLY_ACC_OFF, disable mobiledata.\n");
//	lidbg_shell_cmd("iptables -t filter -P OUTPUT DROP");
//	lidbg_shell_cmd("iptables -t filter -P FORWARD DROP");
//	lidbg_shell_cmd("iptables -t filter -P INPUT DROP");

	send_app_status(FLY_DEVICE_DOWN);
	fs_file_write(DEV_NAME, false, DEVICES_DOWN, 0, strlen(DEVICES_DOWN));
	send_app_status(FLY_ANDROID_DOWN);
	fs_file_write(DEV_NAME, false, ANDROID_DOWN, 0, strlen(ANDROID_DOWN));

	return;
}

static void rmtctrl_resume(void)
{
	lidbg("rmtctrl_resume set MCU_APP_GPIO_ON\n");

	lidbg("acc_state_work_func: FLY_ACC_ON, set prop AccWakedupState true\n");
	lidbg_shell_cmd("setprop persist.lidbg.AccWakedupState true"); //prop for stopping kill_process when ACC_ON

//	lidbg("acc_state_work_func: FLY_ACC_OFF, enable mobiledata.\n");
//	lidbg_shell_cmd("iptables -t filter -P OUTPUT ACCEPT");
//	lidbg_shell_cmd("iptables -t filter -P FORWARD ACCEPT");
//	lidbg_shell_cmd("iptables -t filter -P INPUT ACCEPT");

	send_app_status(FLY_ANDROID_UP);
	fs_file_write(DEV_NAME, false, ANDROID_UP, 0, strlen(ANDROID_UP));
	send_app_status(FLY_DEVICE_UP);
	fs_file_write(DEV_NAME, false, DEVICES_ON, 0, strlen(DEVICES_ON));

	if(acc_io_state == FLY_ACC_OFF){
		lidbg("rmtctrl_resume, acc_io_state is FLY_ACC_OFF, add rmtctrl timer.\n");
		mod_timer(&rmtctrl_timer,AUTO_SLEEP_TIME_S);
	}
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
			rmtctrl_resume();
		else if (*blank == FB_BLANK_POWERDOWN)
			rmtctrl_suspend();
	}

	return 0;
}
int rmtctrl_open (struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t  rmtctrl_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
	int bytes;
	int rmtctrl_state;
	
	lidbg("rmtctrl_read\n");
	if(kfifo_is_empty(&rmtctrl_state_fifo))
	{
	    if(wait_event_interruptible(wait_queue, !kfifo_is_empty(&rmtctrl_state_fifo)))
	        return -ERESTARTSYS;
	}

	if( kfifo_len(&rmtctrl_state_fifo) == 0)
	{
	    lidbg("copy_to_user ERR\n");
	    return -1;
	}
	down(&rmtctrl_sem);
	bytes = kfifo_out(&rmtctrl_state_fifo, &rmtctrl_state, 4);
	up(&rmtctrl_sem);

	if (copy_to_user(buffer, &rmtctrl_state,  4))
	{
		lidbg("copy_to_user ERR\n");
	}
	return size;
}

ssize_t rmtctrl_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}

static int lidbg_rmtctrl_event(struct notifier_block *this,
                       unsigned long event, void *ptr)
{
    DUMP_FUN;

    switch (event)
    {
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_SLEEP_TIMEOUT):
		send_app_status(FLY_SLEEP_TIMEOUT);
        break;
    default:
        break;
    }

    return NOTIFY_DONE;
}

static struct notifier_block lidbg_rmtctrl_notifier =
{
    .notifier_call = lidbg_rmtctrl_event,
};

static  struct file_operations rmtctrl_fops =
{
    .owner = THIS_MODULE,
    .open = rmtctrl_open,
    .read = rmtctrl_read,
    .write = rmtctrl_write,
};

static int lidbg_rmtctrl_probe(struct platform_device *pdev)
{
	int ret;

	fb_notif.notifier_call = fb_notifier_callback;
	ret = fb_register_client(&fb_notif);
	if (ret)
		PM_ERR("Unable to register fb_notifier: %d\n",ret);

	rmtctrl_state_buffer = (unsigned int *)kmalloc(rmtctrl_FIFO_SIZE, GFP_KERNEL);
	if(rmtctrl_state_buffer == NULL)
	{
	    lidbg("rmtctrl kmalloc state buffer error.\n");
	    return 0;
	}
	INIT_WORK(&acc_state_work, acc_state_work_func);

	MCU_APP_GPIO_ON;
	lidbg("rmtctrl probe: MCU_APP_GPIO_ON\n");

	init_timer(&rmtctrl_timer);
	rmtctrl_timer.function = &rmtctrl_timer_func;
	rmtctrl_timer.data = 0;
	rmtctrl_timer.expires = 0;

	register_lidbg_notifier(&lidbg_rmtctrl_notifier);

	lidbg_shell_cmd("setprop persist.lidbg.acc.status 0");
	lidbg("rmtctrl probe: init prop AccWakedupState\n");
	lidbg_shell_cmd("setprop persist.lidbg.AccWakedupState false");

	wake_lock_init(&rmtctrl_wakelock, WAKE_LOCK_SUSPEND, "lidbg_rmtctrl");

	acc_io_state = SOC_IO_Input(MCU_ACC_STATE_IO, MCU_ACC_STATE_IO, GPIO_CFG_PULL_UP);
	SOC_IO_ISR_Add(MCU_ACC_STATE_IO, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, acc_state_isr, NULL);
	lidbg("rmtctrl probe: enable_irq_wake %d\n",GPIO_TO_INT(MCU_ACC_STATE_IO));
	enable_irq_wake(GPIO_TO_INT(MCU_ACC_STATE_IO));

	//lidbg_shell_cmd("svc data disable &");
	init_waitqueue_head(&wait_queue);
	sema_init(&rmtctrl_sem, 1);
	kfifo_init(&rmtctrl_state_fifo, rmtctrl_state_buffer, rmtctrl_FIFO_SIZE);
	lidbg_new_cdev(&rmtctrl_fops, "flyaudio_pm");
	lidbg_chmod("/dev/flyaudio_pm0");

	// boot when acc off
	if(acc_io_state == FLY_ACC_OFF)
		acc_status_handle(acc_io_state);
		
	return 0;
}

#ifdef CONFIG_PM
static int rmtctrl_pm_suspend(struct device *dev)
{
    DUMP_FUN;

    return 0;
}
static int rmtctrl_pm_resume(struct device *dev)
{
    DUMP_FUN;
//	if(g_var.system_status == FLY_KERNEL_DOWN)
//		send_app_status(FLY_KERNEL_UP);
	if(acc_io_state == FLY_ACC_OFF){
		lidbg("rmtctrl_pm_resume, acc_io_state is FLY_ACC_OFF, add rmtctrl timer.\n");
		mod_timer(&rmtctrl_timer,AUTO_SLEEP_TIME_S);
	}
    return 0;
}
static struct dev_pm_ops lidbg_rmtctrl_ops =
{
    .suspend	= rmtctrl_pm_suspend,
    .resume	= rmtctrl_pm_resume,
};
#endif

static struct platform_device lidbg_rmtctrl_device =
{
    .name               = "lidbg_rmtctrl",
    .id                 = -1,
};

static struct platform_driver lidbg_rmtctrl_driver =
{
    .probe		= lidbg_rmtctrl_probe,
    .driver     = {
        .name = "lidbg_rmtctrl",
        .owner = THIS_MODULE,
 #ifdef CONFIG_PM
        .pm = &lidbg_rmtctrl_ops,
 #endif
    },
};

static int __init lidbg_rmtctrl_init(void)
{
    DUMP_BUILD_TIME;
    DUMP_FUN;
    platform_device_register(&lidbg_rmtctrl_device);
    platform_driver_register(&lidbg_rmtctrl_driver);
    return 0;
}
  
static void __exit lidbg_rmtctrl_exit(void)
{
}

module_init(lidbg_rmtctrl_init);
module_exit(lidbg_rmtctrl_exit);

MODULE_DESCRIPTION("lidbg.rmtctrl");
MODULE_LICENSE("GPL");
