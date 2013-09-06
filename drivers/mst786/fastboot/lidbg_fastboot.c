#include "lidbg.h"

LIDBG_DEFINE;

//#include <mach/proc_comm.h>
//#include <mach/clk.h>
//#include <mach/socinfo.h>
//#include <mach/proc_comm.h>
//#include <clock.h>
//#include <clock-pcom.h>
#define RUN_FASTBOOT


static LIST_HEAD(fastboot_kill_list);

struct fastboot_data
{
    int suspend_pending;
    u32 resume_count;
    struct mutex lock;
	int kill_task_en;
	int haslock_resume_times;
	int max_wait_unlock_time;
	int clk_block_suspend;
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct wake_lock flywakelock;
    struct early_suspend early_suspend;
#endif
};
struct fastboot_data *fb_data;

static spinlock_t kill_lock;
unsigned long flags_kill;

static void fastboot_task_kill_exclude(char *exclude_process[])
{
    char kill_process[32][64];

    struct task_struct *p;
    struct mm_struct *mm;
    struct signal_struct *sig;
    u32 i, j = 0;
    bool safe_flag = 0;
    DUMP_FUN_ENTER;

    lidbg("1.-----------------------\n");

#if 0
if(ptasklist_lock != NULL)
    {
        lidbg("read_lock+\n");
        read_lock(ptasklist_lock);
    }
    //else
#endif

    lidbg(" before .spin_lock_irqsave-----------------------\n");
	spin_lock_irqsave(&kill_lock, flags_kill);

    for_each_process(p)
    {
        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);
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
            (strncmp(p->comm, "fly", sizeof("fly") - 1) == 0) ||
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
            continue;
        }

	struct string_dev *pos; 	
	list_for_each_entry(pos, &fastboot_kill_list, tmp_list)
	{
		if(strncmp(p->comm, pos->yourkey, strlen(p->comm)) == 0)
		{
           		safe_flag = 1;
			//lidbg("nokill:%s\n", pos->yourkey);
            		break;
        	}
		else
		{
			//lidbg("kill:%s\n", pos->yourkey);
		}
	}

        if(safe_flag == 0)
        {
            if (p)
            {
                sprintf(kill_process[j++] , p->comm);
                force_sig(SIGKILL, p);
            }
        }
    }//for_each_process

  //  if(ptasklist_lock != NULL)
     //   read_unlock(ptasklist_lock);
   // else
        spin_unlock_irqrestore(&kill_lock, flags_kill);

    lidbg("-----------------------\n\n");


    if(j == 0)
        lidbg("find nothing to kill\n");
    else
        for(i = 0; i < j; i++)
        {
            {
                lidbg("## find %s to kill ##\n", kill_process[i]);
            }
        }
    DUMP_FUN_LEAVE;

}


int kill_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{

	fastboot_task_kill_exclude(NULL);
    return 1;
}

void create_new_proc_entry(void)
{
    create_proc_read_entry("kill_task", 0, NULL, kill_proc, NULL);
}

int pwroff_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
	DUMP_FUN_ENTER;
   // if(PM_STATUS_LATE_RESUME_OK == fastboot_get_status())
        fastboot_pwroff();
       //list_active_locks();
    return 0;
}


void create_new_proc_entry2(void)
{
    create_proc_read_entry("fastboot_pwroff", 0, NULL, pwroff_proc, NULL);
}




#ifndef SOC_COMPILE
static void set_func_tbl(void)
{
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_pwroff;
}
#endif



void fastboot_pwroff(void)
{
    u32 err_count = 0;
    DUMP_FUN_ENTER;

    //avoid mem leak
    //    fastboot_task_kill_select("mediaserver");
    //    fastboot_task_kill_select("vold");

#if 1//def FLY_DEBUG
	fastboot_task_kill_exclude(NULL);
	msleep(1000);

#ifdef RUN_FASTBOOT
    lidbg("before SOC_Key_Report \n");
    SOC_Key_Report(KEY_POWER, KEY_PRESSED_RELEASED);
#endif

#endif

}



#ifdef CONFIG_PM
static int fastboot_suspend(struct device *dev)
{
    lidbg("into fastboot_suspend\n");

    return 0;
}

static int fastboot_resume(struct device *dev)
{
	lidbg("fastboot_resume:%d\n", ++fb_data->resume_count);
	return 0;
}
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void fastboot_early_suspend(struct early_suspend *h)
{

	lidbg("fastboot_early_suspend:%d\n", fb_data->resume_count);	

	fastboot_task_kill_exclude(NULL);
	msleep(1000);
	
	//complete(&suspend_start);
}

static void fastboot_late_resume(struct early_suspend *h)
{
    DUMP_FUN;
}
#endif


static int  fastboot_remove(struct platform_device *pdev)
{

#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&fb_data->early_suspend);
#endif

    kfree(fb_data);

    return 0;
}

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
	fb_data->resume_count = 0;

#ifdef CONFIG_HAS_EARLYSUSPEND
	fb_data->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 5; //the later the better
	fb_data->early_suspend.suspend = fastboot_early_suspend;
	fb_data->early_suspend.resume = fastboot_late_resume;
	register_early_suspend(&fb_data->early_suspend);
#endif

	spin_lock_init(&kill_lock);

	create_new_proc_entry();
        create_new_proc_entry2();

	if(fs_fill_list("/mnt/sdcard/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &fastboot_kill_list)<0)
		fs_fill_list("/system/lib/modules/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &fastboot_kill_list);
	DUMP_FUN_LEAVE;
	lidbg("fastboot_early_suspend:%d\n", fb_data->resume_count);	


	return 0;


fail_mem:
	return ret;
}

static struct dev_pm_ops fastboot_pm_ops =
{
    .suspend	= fastboot_suspend,
    .resume	= fastboot_resume,
};


static struct platform_driver fastboot_driver=
{
	.probe                = fastboot_probe,
	.remove             = fastboot_remove,
	.driver                ={
		.name = "lidbg_fastboot",
		.owner = THIS_MODULE,
#ifdef CONFIG_PM
       		.pm = &fastboot_pm_ops,
#endif
	},
};


static struct platform_device lidbg_fastboot_device=
{
	.name               = "lidbg_fastboot",
	.id                     = -1,
};



static int __init fastboot_init(void)
{
	int ret;

	LIDBG_GET;
	set_func_tbl();


	platform_device_register(&lidbg_fastboot_device);
	ret = platform_driver_register(&fastboot_driver);

	return ret;
}

static void __exit fastboot_exit(void)
{
	platform_driver_unregister(&fastboot_driver);
}



module_init(fastboot_init);
module_exit(fastboot_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("mstar lidbg fastboot driver");
