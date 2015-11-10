#include "lidbg.h"

#define DEV_NAME	 "/dev/lidbg_pm0"
#define SCREEN_ON    "flyaudio screen_on"
#define SCREEN_OFF   "flyaudio screen_off"
#define DEVICES_ON   "flyaudio devices_up"
#define DEVICES_DOWN "flyaudio devices_down"
#define ANDROID_UP	 "flyaudio android_up"
#define ANDROID_DOWN "flyaudio android_down"
#define GOTO_SLEEP   "flyaudio gotosleep"
#define POWER_SUSPEND_TIME   (jiffies + 4*HZ)  /* 30s */
#define AirplanMode_TIME     (jiffies + 6*HZ)  /* 6s */
static struct notifier_block fb_notif;
static struct timer_list timer;
static FLY_SYSTEM_STATUS status;
static DECLARE_COMPLETION (sleep_powerkey_wait);

static int thread_powerkey_func(void *data)
{
    while(1)
    {
        wait_for_completion(&sleep_powerkey_wait);
        switch(status)
        {
        case FLY_SCREEN_OFF:
            fs_file_write(DEV_NAME, false, SCREEN_OFF, 0, strlen(SCREEN_OFF));
            mod_timer(&timer, POWER_SUSPEND_TIME);
            break;
        case FLY_DEVICE_DOWN:
            fs_file_write(DEV_NAME, false, DEVICES_DOWN, 0, strlen(DEVICES_DOWN));
            mod_timer(&timer, POWER_SUSPEND_TIME);
            break;
        case FLY_ANDROID_DOWN:
            fs_file_write(DEV_NAME, false, ANDROID_DOWN, 0, strlen(ANDROID_DOWN));
            //AirplanMode
            //kill process
            mod_timer(&timer, AirplanMode_TIME);
            break;
        case FLY_GOTO_SLEEP:
            lidbg_key_report(KEY_POWER, KEY_PRESSED);
            mdelay(100);
            fs_file_write(DEV_NAME, false, GOTO_SLEEP, 0, strlen(GOTO_SLEEP));
            break;
        case FLY_ANDROID_UP:
            fs_file_write(DEV_NAME, false, ANDROID_UP, 0, strlen(ANDROID_UP));
        case FLY_DEVICE_UP:
            fs_file_write(DEV_NAME, false, DEVICES_ON, 0, strlen(DEVICES_ON));
        case FLY_SCREEN_ON:
            fs_file_write(DEV_NAME, false, SCREEN_ON, 0, strlen(SCREEN_ON));
            status = FLY_SCREEN_ON;
        default:
            break;
        }
    }
    return 0;
}

static void powerkey_suspend_timer(unsigned long data)
{
    status = (int)status + 1;
    complete(&sleep_powerkey_wait);
    return;
}

static void powerkey_suspend(void)
{
    lidbg("powerkey_suspend status = %d\n", status);

    if(status == FLY_GOTO_SLEEP)
        return;
    else
        status = FLY_SCREEN_OFF;

    complete(&sleep_powerkey_wait);
    return;
}

static void powerkey_resume(void)
{
    lidbg("powerkey_resume status = %d\n", status);

    if(status == FLY_SCREEN_OFF)
    {
        del_timer(&timer);
        status = FLY_SCREEN_ON;
    }
    else if(status == FLY_DEVICE_DOWN)
    {
        del_timer(&timer);
        status = FLY_DEVICE_UP;
    }
    else if(status == FLY_ANDROID_DOWN)
    {
        return;
    }
    else
    {
        status = FLY_ANDROID_UP;
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


static int lidbg_powerkey_probe(struct platform_device *pdev)
{
    int ret;

    init_timer(&timer);
    timer.function = &powerkey_suspend_timer;
    timer.data = 0;
    timer.expires = 0;

    CREATE_KTHREAD(thread_powerkey_func, NULL);
    fb_notif.notifier_call = fb_notifier_callback;
    ret = fb_register_client(&fb_notif);
    if (ret)
        PM_ERR("Unable to register fb_notifier: %d\n", ret);
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

