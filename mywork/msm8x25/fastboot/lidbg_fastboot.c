/* Copyright (c) 2012, swlee
 *
 */

//#define SOC_COMPILE


#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"

#include "lidbg_enter.h"

LIDBG_DEFINE;
#endif


#include "lidbg_fastboot.h"

static DECLARE_COMPLETION(suspend_start);
static DECLARE_COMPLETION(early_suspend_start);

static DECLARE_COMPLETION(resume_ok);



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

struct fastboot_data *fb_data;



static struct task_struct *pwroff_task;
static struct task_struct *suspend_task;
static struct task_struct *resume_task;

bool ignore_wakelock = 0;



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
            wait_for_completion(&early_suspend_start);
            while(1)
            {
                msleep(1000);
                time_count++;
                if(fastboot_get_status() == PM_STATUS_READY_TO_PWROFF)
                {
#ifdef FLY_DEBUG
                    if(time_count >= 10)
#else
					if(time_count >= 20)
#endif
                    {
                        lidbgerr("thread_pwroff wait early suspend timeout!\n");
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



static int thread_fastboot_suspend(void *data)
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
#ifdef FLY_DEBUG
					if(time_count >= 15)
#else
					if(time_count >= 25)
#endif
                    {
                        lidbgerr("thread_fastboot_suspend wait suspend timeout!\n");
                        //SOC_Write_Servicer(LOG_DMESG);
						//msleep(10000);//wait for write log finish
						ignore_wakelock = 1;
						wake_lock(&(fb_data->flywakelock));
						wake_unlock(&(fb_data->flywakelock));
                        break;
                    }
                }
                else
                {
                    lidbg("thread_fastboot_suspend wait time_count=%d\n", time_count);
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






void fastboot_set_status(LIDBG_FAST_PWROFF_STATUS status);

static int thread_fastboot_resume(void *data)
{
    while(1)
    {
       // set_current_state(TASK_UNINTERRUPTIBLE);
       // if(kthread_should_stop()) break;
		
		wait_for_completion(&resume_ok);
	    DUMP_FUN_ENTER;
	    msleep(3000);
		SOC_Write_Servicer(WAKEUP_KERNEL);

		SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
		SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);

		msleep(2000);
		fastboot_set_status(PM_STATUS_LATE_RESUME_OK);
	    DUMP_FUN_LEAVE;
    }
    return 0;
}





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
	u32 err_count = 0;
    DUMP_FUN_ENTER;
	
    while(PM_STATUS_LATE_RESUME_OK != fastboot_get_status())
    {
        lidbgerr("Call SOC_PWR_ShutDown when suspend_pending != PM_STATUS_LATE_RESUME_OK :%d\n", fastboot_get_status());
		err_count++;
		
		if(err_count > 50)//10s
		{
        	lidbgerr("err_count > 50,force fastboot_pwroff!\n");
			break;
		}
		msleep(200);

    }
	
	
	SOC_Dev_Suspend_Prepare();

	
    fastboot_set_status(PM_STATUS_READY_TO_PWROFF);

	
#ifdef FLY_DEBUG
    SOC_Write_Servicer(CMD_FAST_POWER_OFF);
#endif

	complete(&early_suspend_start);


}



bool fastboot_is_ignore_wakelock(void)
{
	return ignore_wakelock;

}





#ifndef SOC_COMPILE

static void set_func_tbl(void)
{

    //pwr
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_pwroff;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_GetStatus = fastboot_get_status;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_SetStatus = fastboot_set_status;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_Ignore_Wakelock = fastboot_is_ignore_wakelock;


}
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void fastboot_early_suspend(struct early_suspend *h)
{
	
    lidbg("fastboot_early_suspend:%d\n", fb_data->resume_count);
    if(PM_STATUS_EARLY_SUSPEND_PENDING != fastboot_get_status())
    {
        lidbgerr("Call devices_early_suspend when suspend_pending != PM_STATUS_READY_TO_PWROFF\n");

    }
    wake_unlock(&(fb_data->flywakelock));
	complete(&suspend_start);


}

static void fastboot_late_resume(struct early_suspend *h)
{
    DUMP_FUN;

#ifdef FLY_DEBUG
	SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
	SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);
	complete(&resume_ok);
#else
	fastboot_set_status(PM_STATUS_LATE_RESUME_OK);
#endif	




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


    INIT_COMPLETION(early_suspend_start);
    pwroff_task = kthread_create(thread_pwroff, NULL, "pwroff_task");
    if(IS_ERR(pwroff_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(pwroff_task);



	INIT_COMPLETION(suspend_start);
    suspend_task = kthread_create(thread_fastboot_suspend, NULL, "suspend_task");
    if(IS_ERR(suspend_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(suspend_task);


	
	INIT_COMPLETION(resume_ok);
    resume_task = kthread_create(thread_fastboot_resume, NULL, "pwroff_task");
    if(IS_ERR(resume_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(resume_task);


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
        lidbgerr("Call fastboot_suspend when suspend_pending != PM_STATUS_SUSPEND_PENDING\n");

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

	ignore_wakelock = 0;

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


