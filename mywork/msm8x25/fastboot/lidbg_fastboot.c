/* Copyright (c) 2012, swlee
 *
 */

//#define SOC_COMPILE


#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"
#define LIDBG_FIRST_DEF

#include "lidbg_enter.h"

LIDBG_DEFINE;
#endif


#include "lidbg_fastboot.h"

static DECLARE_COMPLETION(suspend_start);

static struct task_struct *pwroff_task;

static int thread_pwroff(void *data)
{
    int time_count;

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            time_count = 0;
            wait_for_completion(&suspend_start);
            while(1)
            {
                msleep(1000);
                time_count++;
                if(fastboot_get_status() == PM_STATUS_EARLY_SUSPEND_PENDING)
                {
                    if(time_count >= 10)
                    {
                        lidbgerr("thread_pwroff wait suspend timeout!\n");
                        SOC_Write_Servicer(SUSPEND_KERNEL);
                        break;
                    }
                }
                else
                {
                    lidbg("thread_pwroff wait time_count=%d\n", time_count);
                    break;

                }
            }
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


struct fastboot_data
{
    int suspend_pending;
    u32 resume_count;
    struct mutex lock;
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct wake_lock flywakelock;
    struct early_suspend early_suspend;
#endif

};


struct fastboot_data *fb_data ;

int fastboot_get_status(void)
{
    return fb_data->suspend_pending;

}


void fastboot_set_status(LIDBG_FAST_PWROFF_STATUS status)
{
    lidbg("fastboot_set_status:%d\n", status);
    fb_data->suspend_pending = status;
}

void fastboot_pwroff(void)
{
    DUMP_FUN_ENTER;

    if(PM_STATUS_LATE_RESUME_OK != fastboot_get_status())
    {
        lidbgerr("Call SOC_PWR_ShutDown when suspend_pending != PM_STATUS_LATE_RESUME_OK :%d\n", fastboot_get_status());

    }

    fastboot_set_status(PM_STATUS_EARLY_SUSPEND_PENDING);

#ifdef  FLY_DEBUG

    SOC_Write_Servicer(CMD_FAST_POWER_OFF);

#endif
    complete(&suspend_start);

}


#ifndef SOC_COMPILE

static void set_func_tbl(void)
{

    //pwr
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_pwroff;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_GetStatus = fastboot_get_status;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_SetStatus = fastboot_set_status;


}
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void fastboot_early_suspend(struct early_suspend *h)
{
    DUMP_FUN;
    if(PM_STATUS_EARLY_SUSPEND_PENDING != fastboot_get_status())
    {
        lidbgerr("Call devices_early_suspend when suspend_pending != PM_STATUS_EARLY_SUSPEND_PENDING\n");

    }
    wake_unlock(&(fb_data->flywakelock));


}

static void fastboot_late_resume(struct early_suspend *h)
{
    DUMP_FUN;
    fastboot_set_status(PM_STATUS_LATE_RESUME_OK);

}
#endif

static int  fastboot_probe(struct platform_device *pdev)
{
    int ret;
    DUMP_FUN_ENTER;

    fb_data = kmalloc(sizeof(struct fastboot_data), GFP_KERNEL);
    if (!fb_data)
    {
        ret = -ENODEV;
        goto fail_mem;
    }
    mutex_init(&fb_data->lock);
    wake_lock_init(&(fb_data->flywakelock), WAKE_LOCK_SUSPEND, "lidbg_wake_lock");

    fastboot_set_status(PM_STATUS_LATE_RESUME_OK);
    fb_data->resume_count = 0;

#ifdef CONFIG_HAS_EARLYSUSPEND
    fb_data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    fb_data->early_suspend.suspend = fastboot_early_suspend;
    fb_data->early_suspend.resume = fastboot_late_resume;
    register_early_suspend(&fb_data->early_suspend);
    wake_lock(&(fb_data->flywakelock));
#endif


    INIT_COMPLETION(suspend_start);

    pwroff_task = kthread_create(thread_pwroff, NULL, "pwroff_task");
    if(IS_ERR(pwroff_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(pwroff_task);

    DUMP_FUN_LEAVE;


    return 0;

fail_mem:
    return ret;
}

static int  fastboot_remove(struct platform_device *pdev)
{

#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&fb_data->early_suspend);
#endif

    kfree(fb_data);

    return 0;
}

#ifdef CONFIG_PM
static int fastboot_suspend(struct device *dev)
{
    DUMP_FUN;
    if(PM_STATUS_SUSPEND_PENDING != fastboot_get_status())
    {
        lidbgerr("Call fastboot_suspend when suspend_pending != PM_STATUS_EARLY_SUSPEND_PENDING\n");

    }

    return 0;
}

static int fastboot_resume(struct device *dev)
{
    DUMP_FUN;
    lidbg("fastboot_resume:%d\n", ++fb_data->resume_count);
    lidbg("call kernel on!\n");

    fastboot_set_status(PM_STATUS_RESUME_OK);
    wake_lock(&(fb_data->flywakelock));
    SOC_Write_Servicer(WAKEUP_KERNEL);


    return 0;
}

static struct dev_pm_ops fastboot_pm_ops =
{
    .suspend	= fastboot_suspend,
    .resume		= fastboot_resume,
};
#endif

static struct platform_driver fastboot_driver =
{
    .probe		= fastboot_probe,
    .remove     = fastboot_remove,
    .driver         = {
        .name = "lidbg_fastboot",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &fastboot_pm_ops,
#endif
    },
};

static struct platform_device lidbg_fastboot_device =
{
    .name               = "lidbg_fastboot",
    .id                 = -1,
};

static int __init fastboot_init(void)
{
    DUMP_BUILD_TIME;

#ifndef SOC_COMPILE
    LIDBG_GET;
    set_func_tbl();
#endif

    platform_device_register(&lidbg_fastboot_device);

    return platform_driver_register(&fastboot_driver);
}

static void __exit fastboot_exit(void)
{
    platform_driver_unregister(&fastboot_driver);
}

module_init(fastboot_init);
module_exit(fastboot_exit);



MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("lidbg fastboot driver");


