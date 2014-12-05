
#include "lidbg.h"
LIDBG_DEFINE;


#if defined(CONFIG_FB)
struct notifier_block devices_notif;
static int devices_notifier_callback(struct notifier_block *self,
                                     unsigned long event, void *data)
{
    struct fb_event *evdata = data;
    int *blank;

    if (evdata && evdata->data && event == FB_EVENT_BLANK)
    {
        blank = evdata->data;
        if (*blank == FB_BLANK_UNBLANK)
            g_var.fb_on = 1;
        else if (*blank == FB_BLANK_POWERDOWN)
            g_var.fb_on = 0;
    }

    return 0;
}
#endif

void usb_disk_enable(bool enable)
{
    lidbg("[%s]\n", enable ? "usb_enable" : "usb_disable");
    if(enable)
        USB_WORK_ENABLE;
    else
        USB_WORK_DISENABLE;
}
static int thread_usb_disk_enable_delay(void *data)
{
    msleep(3000);
    usb_disk_enable(true);
    return 1;
}
static int thread_usb_disk_disable_delay(void *data)
{
    msleep(3000);
    usb_disk_enable(false);
    return 1;
}

static int lidbg_event(struct notifier_block *this,
                       unsigned long event, void *ptr)
{
    DUMP_FUN;

    switch (event)
    {
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_SCREEN_OFF):
        break;

    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_DEVICE_DOWN):
		#ifdef PLATFORM_msm8226
		MSM_GPS_POWER_OFF;
		#endif
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_ANDROID_DOWN):
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_GOTO_SLEEP):
		CREATE_KTHREAD(thread_usb_disk_disable_delay, NULL);
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_KERNEL_DOWN):
		break;
	case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_KERNEL_UP):
		break;
	case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_ANDROID_UP):
		break;
	case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_DEVICE_UP):
		#ifdef PLATFORM_msm8226
		MSM_GPS_POWER_ON;
		#endif
		CREATE_KTHREAD(thread_usb_disk_enable_delay, NULL);
		break;
	case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_SCREEN_ON):
		if(!g_var.is_fly)
    		LCD_ON;
		break;

    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SIGNAL_EVENT, NOTIFIER_MINOR_SIGNAL_BAKLIGHT_ACK):
        LCD_OFF;
        msleep(100);
        LCD_ON;
        break;
    default:
        break;
    }

    return NOTIFY_DONE;
}


static struct notifier_block lidbg_notifier =
{
    .notifier_call = lidbg_event,
};

int dev_open(struct inode *inode, struct file *filp)
{
    return 0;
}
int dev_close(struct inode *inode, struct file *filp)
{
    return 0;
}

static void parse_cmd(char *pt)
{
    int argc = 0;
    char *argv[32] = {NULL};

    lidbg("%s\n", pt);
    argc = lidbg_token_string(pt, " ", argv);

    if (!strcmp(argv[0], "sound"))
    {
        bool enable;
        enable = simple_strtoul(argv[1], 0, 0);
        sound_detect_event(enable);
    }
	else if (!strcmp(argv[0], "lcd_on"))
    {
		LCD_ON;
    }
    else if (!strcmp(argv[0], "lcd_off"))
    {
		LCD_OFF;
    }
	else if (!strcmp(argv[0], "performance_high"))
    {
		set_system_performance(3);
    }
	else if (!strcmp(argv[0], "performance_middle"))
    {
		set_system_performance(2);
    }
	else if (!strcmp(argv[0], "performance_low"))
    {
		set_system_performance(1);
    }
		
}


static ssize_t dev_write(struct file *filp, const char __user *buf,
                         size_t size, loff_t *ppos)
{
    char *p = NULL;
    int len = size;
    char tmp[size + 1];//C99 variable length array
    char *mem = tmp;

    memset(mem, '\0', size + 1);

    if(copy_from_user(mem, buf, size))
    {
        lidbg("copy_from_user ERR\n");
    }

    if((p = memchr(mem, '\n', size)))
    {
        len = p - mem;
        *p = '\0';
    }
    else
        mem[len] =  '\0';

    parse_cmd(mem);

    return size;//warn:don't forget it;
}

static struct file_operations dev_fops =
{
    .owner = THIS_MODULE,
    .open = dev_open,
    .write = dev_write,
    .release = dev_close,
};

static int soc_dev_probe(struct platform_device *pdev)
{
#if defined(CONFIG_FB)
    devices_notif.notifier_call = devices_notifier_callback;
    fb_register_client(&devices_notif);
#endif

    register_lidbg_notifier(&lidbg_notifier);

	CREATE_KTHREAD(thread_led, NULL);
	CREATE_KTHREAD(thread_thermal, NULL);
	sound_detect_init();
	
	if((g_var.is_fly == 0) || (g_var.recovery_mode == 1))
	{
	    CREATE_KTHREAD(thread_button_init, NULL);
	    CREATE_KTHREAD(thread_key, NULL);
		
	    LCD_ON;
	}
	USB_WORK_ENABLE;
	SET_USB_ID_SUSPEND;
#ifdef SOC_msm8x26
	MSM_GPS_POWER_ON;
#endif
	lidbg_new_cdev(&dev_fops, "flydev");
    return 0;

}
static int soc_dev_remove(struct platform_device *pdev)
{
    lidbg("soc_dev_remove\n");
	
	if(!g_var.is_fly){}

	return 0;

}
static int  soc_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
    lidbg("soc_dev_suspend\n");
	if(!g_var.is_fly)
	{
    	button_suspend();
	}

    return 0;

}
static int soc_dev_resume(struct platform_device *pdev)
{
    lidbg("soc_dev_resume\n");
	
	if(!g_var.is_fly)
	{
		button_resume();
		led_resume();
	}

    return 0;
}
struct platform_device soc_devices =
{
    .name			= "misc_devices",
    .id 			= 0,
};
static struct platform_driver soc_devices_driver =
{
    .probe = soc_dev_probe,
    .remove = soc_dev_remove,
    .suspend = soc_dev_suspend,
    .resume = soc_dev_resume,
    .driver = {
        .name = "misc_devices",
        .owner = THIS_MODULE,
    },
};
static void set_func_tbl(void)
{
    plidbg_dev->soc_func_tbl.pfnSOC_Get_System_Sound_Status = SOC_Get_System_Sound_Status_func;
	plidbg_dev->soc_func_tbl.pfnGPS_sound_status = iGPS_sound_status;
}



int dev_init(void)
{
    lidbg("=======misc_dev_init========\n");
    LIDBG_GET;
	set_func_tbl();
	platform_device_register(&soc_devices);
	platform_driver_register(&soc_devices_driver);
    return 0;
}

void dev_exit(void)
{
    lidbg("dev_exit\n");
}

void lidbg_device_main(int argc, char **argv)
{
    lidbg("lidbg_device_main\n");

    if(argc < 1)
    {
        lidbg("Usage:\n");
        return;
    }
}
EXPORT_SYMBOL(lidbg_device_main);
MODULE_AUTHOR("fly, <fly@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);

