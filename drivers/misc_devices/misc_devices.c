
#include "lidbg.h"
#define DISABLE_USB_WHEN_DEVICE_DOWN
//#define DISABLE_USB_WHEN_ANDROID_DOWN
//#define FORCE_UMOUNT_UDISK

LIDBG_DEFINE;

int udisk_stability_test = 0;


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
        {
            if(g_var.system_status >= FLY_KERNEL_UP)
                if(g_var.led_hal_status & g_var.led_app_status)
                    LCD_ON;
            g_var.fb_on = 1;
        }
        else if (*blank == FB_BLANK_POWERDOWN)
        {
            LCD_OFF;
            g_var.fb_on = 0;
        }
    }

    return 0;
}
#endif

void usb_disk_enable(bool enable)
{
    lidbg("60.[%s]\n", enable ? "usb_enable" : "usb_disable");
    if(enable)
        USB_WORK_ENABLE;
    else
    {
#ifdef FORCE_UMOUNT_UDISK
        lidbg("call lidbg_umount\n");
        lidbg_shell_cmd("/system/lib/modules/out/lidbg_umount &");
        lidbg_shell_cmd("/flysystem/lib/out/lidbg_umount &");
        msleep(4000);
#else
        lidbg("USB_WORK_DISENABLE.200.unmount\n");
        lidbg_shell_cmd("umount /storage/udisk");
        msleep(200);
#endif
        USB_WORK_DISENABLE;
    }
}
static int thread_usb_disk_enable_delay(void *data)
{
//    msleep(4000);
    usb_disk_enable(true);
    return 1;
}


static int thread_usb_disk_disable_delay(void *data)
{
#ifdef DISABLE_USB_WHEN_DEVICE_DOWN
    //msleep(2000);
#else
    //msleep(1000);
#endif
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
        //if(!g_var.is_fly)
    {
        LCD_OFF;
        //lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_APP_OFF));
        //lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_HAL_OFF));
    }
    break;

    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_DEVICE_DOWN):
#ifdef DISABLE_USB_WHEN_DEVICE_DOWN
        CREATE_KTHREAD(thread_usb_disk_disable_delay, NULL);
#endif
#if 0//def VENDOR_QCOM
        lidbg("set uart to gpio\n");
        SOC_IO_Output_Ext(0, g_hw.gpio_dvd_tx, 1, GPIOMUX_PULL_NONE, GPIOMUX_DRV_8MA);
        SOC_IO_Output_Ext(0, g_hw.gpio_dvd_rx, 1, GPIOMUX_PULL_NONE, GPIOMUX_DRV_8MA);
        SOC_IO_Output_Ext(0, g_hw.gpio_bt_tx, 1, GPIOMUX_PULL_NONE, GPIOMUX_DRV_8MA);
        SOC_IO_Output_Ext(0, g_hw.gpio_bt_rx, 1, GPIOMUX_PULL_NONE, GPIOMUX_DRV_8MA);
#endif
        GPS_POWER_OFF;
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_ANDROID_DOWN):
        MSM_DSI83_DISABLE;
#ifdef DISABLE_USB_WHEN_ANDROID_DOWN
        CREATE_KTHREAD(thread_usb_disk_disable_delay, NULL);
#endif
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_GOTO_SLEEP):
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_KERNEL_DOWN):
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_KERNEL_UP):
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_ANDROID_UP):
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_DEVICE_UP):
#if 0//def VENDOR_QCOM
        lidbg("set gpio to uart\n");
        SOC_IO_Config(g_hw.gpio_dvd_tx, GPIOMUX_FUNC_2, GPIOMUX_OUT_HIGH, GPIOMUX_PULL_NONE, GPIOMUX_DRV_16MA);
        SOC_IO_Config(g_hw.gpio_dvd_rx, GPIOMUX_FUNC_2, GPIOMUX_OUT_HIGH, GPIOMUX_PULL_NONE, GPIOMUX_DRV_16MA);
        SOC_IO_Config(g_hw.gpio_bt_tx, GPIOMUX_FUNC_2, GPIOMUX_OUT_HIGH, GPIOMUX_PULL_NONE, GPIOMUX_DRV_16MA);
        SOC_IO_Config(g_hw.gpio_bt_rx, GPIOMUX_FUNC_2, GPIOMUX_OUT_HIGH, GPIOMUX_PULL_NONE, GPIOMUX_DRV_16MA);
#endif
        CREATE_KTHREAD(thread_usb_disk_enable_delay, NULL);
        GPS_POWER_ON;
        break;
    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, FLY_SCREEN_ON):
        //if(!g_var.is_fly)
    {
        if(g_var.led_hal_status & g_var.led_app_status)
            LCD_ON;
        //lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_APP_ON));
        //lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_HAL_ON));
    }
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

    if (!strcmp(argv[0], "lcd_on"))
    {
        lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_APP_ON));
    }
    else if (!strcmp(argv[0], "lcd_off"))
    {
        lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_APP_OFF));
    }
    else if (!strcmp(argv[0], "performance_high"))
    {
#ifdef SOC_msm8x26
        set_system_performance(3);
#endif
    }
    else if (!strcmp(argv[0], "performance_middle"))
    {
#ifdef SOC_msm8x26
        set_system_performance(2);
#endif
    }
    else if (!strcmp(argv[0], "performance_low"))
    {
#ifdef SOC_msm8x26
        set_system_performance(1);
#endif
    }
    else if (!strcmp(argv[0], "acc_debug_mode"))
    {
        lidbg("acc_debug_mode enable!");
        g_var.is_debug_mode = 1;
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

int usb_enumerate_limit = 5;
void usb_enumerate_monitor(char *key_word, void *data)
{
    DUMP_FUN;
    if(!g_var.is_udisk_needreset || usb_enumerate_limit <= 0)
    {
        lidbg("find key word.return %d,%d\n", !g_var.is_udisk_needreset, usb_enumerate_limit);
        return;
    }
    lidbg("find key word.in %d\n", usb_enumerate_limit);
    g_var.is_udisk_needreset = 0;
    usb_enumerate_limit--;
    if(g_var.system_status >= FLY_ANDROID_UP)
    {
        usb_disk_enable(0);
        ssleep(2);
        usb_disk_enable(1);
    }
}

int thread_udisk_stability_test(void *data)
{
    u32 cnt = 0;
    ssleep(30);
    while(1)
    {
        usb_disk_enable(1);
        ssleep(15);
        usb_disk_enable(0);
        ssleep(5);
        cnt++;
        lidbg("udisk_stability_test times=%d\n", cnt);
    }

}

#ifdef PLATFORM_ID_7
static bool  is_udisk_added = 0;
static int usb_nb_misc_func(struct notifier_block *nb, unsigned long action, void *data)
{
    if(!is_udisk_added)
        lidbg("stop.thread_udisk_en\n");
    is_udisk_added = 1;
    return NOTIFY_OK;
}
static struct notifier_block usb_nb_misc =
{
    .notifier_call = usb_nb_misc_func,
};
int thread_udisk_en(void *data)
{
    int loop = 0;
    DUMP_FUN_ENTER;
    ssleep(3);
    while(!is_udisk_added)
    {
        lidbg("thread_udisk_en====%d\n", ++loop);
        IO_CONFIG_OUTPUT(0, 73);
        soc_io_output(0, 73, 0);
        ssleep(2);
    }
    lidbg("thread_udisk_en===exit=%d\n", loop);
    return 0;
}
#endif

static int soc_dev_probe(struct platform_device *pdev)
{
#if defined(CONFIG_FB)
    devices_notif.notifier_call = devices_notifier_callback;
    fb_register_client(&devices_notif);
#endif

    register_lidbg_notifier(&lidbg_notifier);

    CREATE_KTHREAD(thread_led, NULL);
    //CREATE_KTHREAD(thread_thermal, NULL);

    if((g_var.is_fly == 0) || (g_var.recovery_mode == 1))
    {
        // CREATE_KTHREAD(thread_button_init, NULL);
        //CREATE_KTHREAD(thread_key, NULL);

        //LCD_ON;
    }
    LCD_ON;
    //lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_APP_ON));
    //lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_BL_LCD_STATUS_CHANGE, NOTIFIER_MINOR_BL_HAL_ON));
    USB_WORK_ENABLE;

    SET_USB_ID_SUSPEND;
    GPS_POWER_ON;
    lidbg_new_cdev(&dev_fops, "flydev");

    lidbg_trace_msg_cb_register("unable to enumerate USB device", NULL, usb_enumerate_monitor);

    FS_REGISTER_INT(udisk_stability_test, "udisk_stability_test", 0, NULL);

    if(udisk_stability_test == 1)
    {
        CREATE_KTHREAD(thread_udisk_stability_test, NULL);
    }

#ifdef PLATFORM_ID_7
    if(0)
    {
        usb_register_notify(&usb_nb_misc);
        CREATE_KTHREAD(thread_udisk_en, NULL);
    }
#endif
#ifdef PLATFORM_ID_11
            lidbg("-------disable logcat server -----");
            lidbg_shell_cmd("setprop ctl.stop logd");
#endif
    return 0;

}
static int soc_dev_remove(struct platform_device *pdev)
{
    lidbg("soc_dev_remove\n");

    if(!g_var.is_fly) {}

    return 0;

}
static int  soc_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
    lidbg("soc_dev_suspend\n");

    if(!g_var.is_fly)
    {
        //button_suspend();
    }

    return 0;

}
static int soc_dev_resume(struct platform_device *pdev)
{
    lidbg("soc_dev_resume \n");
    if(!g_var.is_fly)
    {
        //button_resume();
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
    return;
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

