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


//#define RUN_FASTBOOT

static DECLARE_COMPLETION(suspend_start);
static DECLARE_COMPLETION(early_suspend_start);

static DECLARE_COMPLETION(resume_ok);
static DECLARE_COMPLETION(pwroff_start);

void fastboot_pwroff(void);


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

static spinlock_t kill_lock;
unsigned long flags_kill;


static struct task_struct *pwroff_task;
static struct task_struct *suspend_task;
static struct task_struct *resume_task;
static struct task_struct *pwroff_task;

bool ignore_wakelock = 0;

char *kill_exclude_process[] =
{
    "init",
    "kthreadd",
    //"ksoftirqd/0",
    //"kworker/0:0",
    //"kworker/u:0",
    //"kworker/u:1",
    //"kworker/0:1",
    //"kworker/u:2",
    //"kworker/0:2",
    //"kworker/u:4",

    //"migration/0",
    "khelper",
    "suspend_sys_syn",
    "suspend",
    "sync_supers",
    "bdi-default",
    "kblockd",
    "khubd",
    "l2cap",
    "a2mp",
    "modem_notifier",
    "smd_channel_clo",
    "smsm_cb_wq",
    "rpcrouter",
    "msm_adsp_probe",
    "rpcrotuer_smd_x",
    "krpcserversd",
    "rmt_storage",
    "voicememo_rpc",
    "kadspd",
    "kswapd0",
    "fsnotify_mark",
    "crypto",
    "mdp_dma_wq",
    "mdp_vsync_wq",
    "mdp_pipe_ctrl_w",

    "k_hsuart",
    "kgsl-3d0",
    //"mtdblock0",
    //"mtdblock1",
    //"mtdblock2",
    //"mtdblock3",
    //"mtdblock4",
    //"mtdblock5",
    //"mtdblock6",
    //"mtdblock7",
    //"mtdblock8",
    //"mtdblock9",
    //"mtdblock10",
    //"mtdblock11",
    //"mtdblock12",
    //"mtdblock13",
    //"mtdblock14",
    //"mtdblock15",
    //"mtdblock16",
    //"mtdblock17",
    "k_rmnet_mux_wor",
    "f_mtp",
    "file-storage",
    "diag_wq",
    "diag_cntl_wq",
    "krtcclntd",
    "krtcclntcbd",
    "kbatteryclntd",
    "kbatteryclntcbd",
    "battery_queue",
    "msm_adc",
    "dalrpc_rcv_DAL0",
    "iewq",
    "kinteractiveup",
    //"irq/298-msm-sdc",
    "binder",
    "koemrapiclientc",
    //"kworker/u:3",
    "krfcommd",
    "msm-cpufreq",
    "khsclntd",
    "rq_stats",
    "deferwq",
    "ueventd",
    //"yaffs-bg-1",
    //"flush-31:2",
    //"yaffs-bg-1",
    //"yaffs-bg-1",
    //"loop7",
    "servicemanager",
    "vold",
    "netd",
    "debuggerd",
    "rild",
    "surfaceflinger",
    "zygote",
    "drmserver",
    "mediaserver",
    "dbus-daemon",
    "installd",
    "keystore",
    "qlogd",
    "sh",
    "cnd",
    "wiperiface",
    "mm-pp-daemon",
    "logwrapper",
    "gpu_dcvsd",
    "k_gserial",
    "lidbg_servicer",
    "k_gsmd",
    "msg_task",
    "u2k_task",
    "pwroff_task",
    "suspend_task",
    "pwroff_task",
    "lpc_task",
    "led_task",
    "dev_init_task",
    "key_task",
    "k_rmnet_work",
    "bp_msg_task",
    "adbd",
    "qmuxd",
    "netmgrd",
    "sh",
	"mkdir",
    "chmod",
    //"irq/304-ft5x06_",
    //"system_server",
    "ATFWD-daemon",
    "mpdecision",
    //"flush-31:5",
    "audmgr_rpc",
    "sleep",
    "boadcastreceiver",
	"cfg80211",
	"ath6kl",
	"wpa_supplicant",
	"workqueue_trust",
	"logcat",
	"bootanimation",
	"fsck_msdos",
	"usb-storage",
	"sys.DeviceHealth",
	//"system",



    "mm-qcamera-daemon",
   // "com.android.qualcomm",
    //"com.android.settings",
   // "com.qualcomm.privinit",
    //"com.qualcomm.restore.airplanemode",
   // "com.qualcomm.cabl",
   // "com.innopath.activecare",
    "com.android.systemui",
   // "com.android.launcher",
    "com.qualcomm.fastboot",
    //"com.android.phone",
    "com.android.inputmethod.latin",
    //"com.qualcomm.stats",




    // name err
    "alljoyn-daemon",
    "oadcastreceiver",
    "mm-qcamera-daem",
    "putmethod.latin",
    //"m.android.phone",
    //"ndroid.launcher",
    //"ndroid.settings",
    "d.process.acore",
    //"ndroid.qualcomm",
   // "re.airplanemode",
    "ndroid.systemui",
   // "path.activecare",
   // "m.qualcomm.cabl",
   // "viders.calendar",
   // ".qualcomm.stats",
   // "alcomm.privinit",
    "alcomm.fastboot",
   // "android.smspush",
    //"ndroid.contacts",
    "d.process.media",
    //"qrd.simcontacts",
    //"droid.gallery3d",
   // "ndroid.exchange",
    //"m.android.email",
    //"oid.voicedialer",
   // "android.musicfx",
    "droid.deskclock",
    //"com.android.mms",
    //"m.android.music",
	//"omm.datamonitor",
	//"oid.sysinfo.pro",


//flyaudio	
	"roid.flyaudioui",
	"goodix_wq",
	//"io3.widget.time",
	//"lyaudio.Weather",
	//".flyaudio.media",
	//"ndroid.calendar",
	//"settings:remote",
	"kdmflush",
	"kcryptd",
	"kcryptd_io",
	"dhcpcd",
	"RilMessageDecod",
	"getprop",
	"setprop",


    //"flush-31:6",
    //"flush-31:3",
    //"flush-31:12",
    //"flush-7:7",
    "task_kill_exclude_end",

};



int fastboot_task_kill_select(char *task_name)
{
    struct task_struct *p;
    struct task_struct *selected = NULL;
    DUMP_FUN_ENTER;

    //read_lock(&tasklist_lock);
    for_each_process(p)
    {
        struct mm_struct *mm;
        struct signal_struct *sig;

        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        selected = p;
        //printk( "process %d (%s)\n",p->pid, p->comm);

        if(!strcmp(p->comm, task_name))
        {
            lidbg("find %s to kill\n", task_name);

            if (selected)
            {
                force_sig(SIGKILL, selected);
                return 1;
            }
        }
        //read_unlock(&tasklist_lock);
    }
    DUMP_FUN_LEAVE;
    return 0;
}



static void fastboot_task_kill_exclude(char *exclude_process[])
{


    //struct task_struct *kill_process[32];
    char kill_process[32][64];

    struct task_struct *p;
    struct mm_struct *mm;
    struct signal_struct *sig;
    u32 i, j = 0;
    bool safe_flag = 0;
    DUMP_FUN_ENTER;

    //memset(kill_process, NULL, 100);
    lidbg("-----------------------\n");


    //if(ptasklist_lock != NULL)
    //	read_lock(ptasklist_lock);


	
	spin_lock_irqsave(&kill_lock, flags_kill);

    for_each_process(p)
    {
        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        //lidbg( "process %d (%s)\n",p->pid, p->comm);
        safe_flag = 0;
        i = 0;


        if(
            (strncmp(p->comm, "flush", sizeof("flush") - 1) == 0) ||
            (strncmp(p->comm, "mtdblock", sizeof("mtdblock") - 1) == 0) ||
            (strncmp(p->comm, "kworker", sizeof("kworker") - 1) == 0) ||
            (strncmp(p->comm, "yaffs", sizeof("yaffs") - 1) == 0) ||
            (strncmp(p->comm, "irq", sizeof("irq") - 1) == 0) ||
            (strncmp(p->comm, "migration", sizeof("migration") - 1) == 0) ||
			(strncmp(p->comm, "mmcqd", sizeof("mmcqd") - 1) == 0) ||
			(strncmp(p->comm, "Fly", sizeof("Fly") - 1) == 0) ||
			(strncmp(p->comm, "flyaudio", sizeof("flyaudio") - 1) == 0) ||
			(strncmp(p->comm, "ksdioirqd", sizeof("ksdioirqd") - 1) == 0) ||
			(strncmp(p->comm, "jbd2", sizeof("jbd2") - 1) == 0) ||
			(strncmp(p->comm, "ext4", sizeof("ext4") - 1) == 0) ||
			(strncmp(p->comm, "scsi", sizeof("scsi") - 1) == 0) ||
			(strncmp(p->comm, "loop", sizeof("loop") - 1) == 0) ||
			(strncmp(p->comm, "ServiceHandler", sizeof("ServiceHandler") - 1) == 0) ||
			(strncmp(p->comm, "system", sizeof("system") - 1) == 0) ||
            (strncmp(p->comm, "ksoftirqd", sizeof("ksoftirqd") - 1) == 0)
        )
        {

            //lidbg("this safe,next1\n\n");

            continue;

        }

        while(1)
        {
            //lidbg( "---(%s)\n",exclude_process[i]);

            if(!strcmp(exclude_process[i], "task_kill_exclude_end"))
                break;

            if(strcmp(p->comm, exclude_process[i]) == 0)
            {
                safe_flag = 1;
                //lidbg("this safe,next2\n\n");
                break;
            }

            i++;

        }

        if(safe_flag == 0)
        {
            if (p)
            {
                //kill_process[j] = p;
               

               // lidbg("## find %s to kill ##\n", p->comm);
				
			    sprintf(kill_process[j++] , p->comm);
				force_sig(SIGKILL, p);
                //lidbg("+\n");
            }
        }

        //if(ptasklist_lock != NULL)
        //	read_unlock(ptasklist_lock);
    }//for_each_process
	spin_unlock_irqrestore(&kill_lock, flags_kill);


	lidbg("-----------------------\n\n");



    if(j == 0)
        lidbg("find nothing to kill\n");
    else
        for(i = 0; i < j; i++)
        {
        	//if(kill_process[i])				
        	{
           		lidbg("## find %s to kill ##\n", kill_process[i]);

        	}
        }
	//msleep(10000);
    DUMP_FUN_LEAVE;



}

int kill_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{


	fastboot_task_kill_exclude(kill_exclude_process);

    return 1;
}

void create_new_proc_entry()
{
    create_proc_read_entry("kill_task", 0, NULL, kill_proc, NULL);

}

int pwroff_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{

    if(PM_STATUS_LATE_RESUME_OK == fastboot_get_status())
		fastboot_pwroff();

    return 1;
}


void create_new_proc_entry2()
{
    create_proc_read_entry("fastboot_pwroff", 0, NULL, pwroff_proc, NULL);

}

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
                    if(time_count >= 10)
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




static int thread_fastboot_pwroff(void *data)
{

    while(1)
    {

        wait_for_completion(&pwroff_start);
		fastboot_pwroff();

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
                    if(time_count >= 15)
#endif
                    {
                        lidbgerr("thread_fastboot_suspend wait suspend timeout!\n");
#ifndef FLY_DEBUG

                       // SOC_Write_Servicer(LOG_DMESG);
                       // msleep(10000);//wait for write log finish
#endif

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

        //SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
        //SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);

        msleep(1000);
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
	fastboot_set_status(PM_STATUS_READY_TO_PWROFF);

#if 1 //ndef FLY_DEBUG
	SOC_Key_Report(KEY_PAUSECD, KEY_PRESSED_RELEASED);
	msleep(100);
#endif

   // fastboot_task_kill_exclude(kill_exclude_process);

    SOC_Dev_Suspend_Prepare();

//avoid mem leak
    fastboot_task_kill_select("mediaserver");
    fastboot_task_kill_select("void");


   //fastboot_set_status(PM_STATUS_READY_TO_PWROFF);


#ifdef FLY_DEBUG

#ifdef RUN_FASTBOOT
    SOC_Write_Servicer(CMD_FAST_POWER_OFF);
#else
	//SOC_Write_Servicer(SUSPEND_PREPARE);
	//msleep(500);
	SOC_Key_Report(KEY_POWER, KEY_PRESSED_RELEASED);
#endif

#endif

    complete(&early_suspend_start);


}



bool fastboot_is_ignore_wakelock(void)
{
    return ignore_wakelock;

}

void fastboot_go_pwroff(void)
{
    DUMP_FUN_ENTER;

	complete(&pwroff_start);

}



#ifndef SOC_COMPILE

static void set_func_tbl(void)
{

    //pwr

	
#ifdef FLY_DEBUG
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_go_pwroff;
#else
	plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_pwroff;
#endif	
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
#if 0 //for test

	while(1)
	{
		fastboot_task_kill_exclude(kill_exclude_process);
		msleep(1000);

	}
	
#endif


	fastboot_task_kill_exclude(kill_exclude_process);
#if 0
	ignore_wakelock = 1;
#endif
    wake_unlock(&(fb_data->flywakelock));
    complete(&suspend_start);


}

static void fastboot_late_resume(struct early_suspend *h)
{
    DUMP_FUN;
/*
#ifdef FLY_DEBUG
    SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
    SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);
    complete(&resume_ok);
#else
	SOC_Write_Servicer(WAKEUP_KERNEL);

    fastboot_set_status(PM_STATUS_LATE_RESUME_OK);


#endif
*/
#ifdef FLY_DEBUG
#ifndef RUN_FASTBOOT

//SOC_Write_Servicer(RESUME_PREPARE);

#endif
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

    INIT_COMPLETION(pwroff_start);
    pwroff_task = kthread_create(thread_fastboot_pwroff, NULL, "pwroff_task");
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

	spin_lock_init(&kill_lock);
    create_new_proc_entry();
    create_new_proc_entry2();

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
    //SOC_Write_Servicer(WAKEUP_KERNEL);
	complete(&resume_ok);

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


