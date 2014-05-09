
#include "lidbg.h"
LIDBG_DEFINE;

static struct task_struct *Thermal_task = NULL;
//static int fan_onoff_temp;
int is_key_scan_en = 1;
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
            is_key_scan_en = 1;
        else if (*blank == FB_BLANK_POWERDOWN)
            is_key_scan_en = 0;
    }

    return 0;
}
#endif

static int soc_dev_probe(struct platform_device *pdev)
{
    //lidbg("=====soc_dev_probe====\n");

#if defined(CONFIG_FB)
    devices_notif.notifier_call = devices_notifier_callback;
    fb_register_client(&devices_notif);
#endif

    fs_register_filename_list(TEMP_LOG_PATH, true);
    fs_regist_state("cpu_temp", &(g_var.temp));
    CREATE_KTHREAD(thread_button_init, NULL);
    CREATE_KTHREAD(thread_led, NULL);
    CREATE_KTHREAD(thread_key, NULL);
    Thermal_task =  kthread_run(thread_thermal, NULL, "flythermalthread");
    //FS_REGISTER_INT(fan_onoff_temp, "fan_onoff_temp", 65, NULL);

    //register_lidbg_notifier(&lidbg_notifier);
    LCD_ON;
    return 0;

}
static int soc_dev_remove(struct platform_device *pdev)
{
    lidbg("soc_dev_remove\n");
    return 0;

}
static int  soc_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
    lidbg("soc_dev_suspend\n");
    button_suspend();
    return 0;

}
static int soc_dev_resume(struct platform_device *pdev)
{
    lidbg("soc_dev_resume\n");
	button_resume();
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
int dev_init(void)
{
    lidbg("=======misc_dev_init========\n");
    LIDBG_GET;
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
MODULE_AUTHOR("lsw, <sw.lee.g2@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);

