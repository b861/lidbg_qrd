#include "lidbg.h"
#if (defined(BOARD_V1) || defined(BOARD_V2))
#include <proc_comm.h>
#else
#include <mach/proc_comm.h>
#endif
#include <mach/clk.h>
#include <mach/socinfo.h>
#include <clock.h>
#include <clock-pcom.h>

LIDBG_DEFINE;

#define RUN_ACCBOOT
#define DEVICE_NAME "lidbg_acc"
#define HAL_SO "/flysystem/lib/hw/flyfa.default.so"
#define FASTBOOT_LOG_PATH LIDBG_LOG_DIR"log_fb.txt"


int suspend_state = PM_STATUS_LATE_RESUME_OK;

static DECLARE_COMPLETION(acc_ready);
static DECLARE_COMPLETION(suspend_start);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
DECLARE_MUTEX(lidbg_acc_sem);
#else
DEFINE_SEMAPHORE(lidbg_acc_sem);
#endif

static struct task_struct *acc_task;
static struct task_struct *resume_task;

static spinlock_t  active_wakelock_list_lock;
static spinlock_t kill_lock;
static LIST_HEAD(fastboot_kill_list);

bool ignore_wakelock = 0;


#ifdef CONFIG_HAS_EARLYSUSPEND
static void acc_early_suspend(struct early_suspend *handler);
static void acc_late_resume(struct early_suspend *handler);
struct early_suspend early_suspend;
#endif

typedef struct
{
    unsigned int  acc_flag;
    u32 resume_count;
    u32 poweroff_count;
    u32 accoff_count;
} lidbg_acc;

lidbg_acc *plidbg_acc = NULL;
static struct task_struct *suspend_task;

static int pc_clk_is_enabled(int id)
{
    if (msm_proc_comm(PCOM_CLKCTL_RPC_ENABLED, &id, NULL))
        return 0;
    else
        return id;
}


int check_all_clk_disable(void)
{
    int i = P_NR_CLKS - 1;
    int ret = 0;
    DUMP_FUN;
    while(i >= 0)
    {
        if (pc_clk_is_enabled(i))
        {
            lidbg("pc_clk_is_enabled:%3d\n", i);
            ret++;
        }
        i--;
    }
    return ret;
}


//53 109 34

int safe_clk[] = {113, 106, 105, 104, 103, 102, 97 ,95, 51, 32, 31, 24, 22, 20, 16, 15, 12, 10, 8, 4, 3, 1};

bool find_unsafe_clk(void)
{
    int j, i = P_NR_CLKS - 1;
    int ret = 0;
    bool is_safe = 0;
    while(i >= 0)
    {
        if (pc_clk_is_enabled(i))
        {
            is_safe = 0;
            for(j = 0; j < sizeof(safe_clk); j++)
            {
                if(i == safe_clk[j] )
                {
                    is_safe = 1;
                    break;
                }
            }
            if(is_safe == 0)
            {
                lidbg_fs_log(FASTBOOT_LOG_PATH, "block unsafe clk:%d\n", i);
                ret = 1;
                //return ret;
            }
        }
        i--;
    }
    return ret;

}


bool is_ignore_wakelock(void)
{
    return ignore_wakelock;
}


static void fastboot_task_kill_exclude()
{
    static char kill_process[32][25];
    unsigned long flags_kill;

    struct task_struct *p;
    struct mm_struct *mm;
    struct signal_struct *sig;
    u32 i, j = 0;
    bool safe_flag = 0;
    DUMP_FUN_ENTER;

    lidbg("-----------------------\n");
    if(ptasklist_lock != NULL)
    {
        lidbg("read_lock+\n");
        read_lock(ptasklist_lock);
    }
    else
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
            (strncmp(p->comm, "ksoftirqd", sizeof("ksoftirqd") - 1) == 0) ||
            (strncmp(p->comm, "ftf", sizeof("ftf") - 1) == 0)
        )
        {
            continue;
        }


        {
            struct string_dev *pos;
            list_for_each_entry(pos, &fastboot_kill_list, tmp_list)
            {
                if(strcmp(p->comm, pos->yourkey) == 0)
                {
                    safe_flag = 1;
                    //lidbg("nokill:%s\n", pos->yourkey);
                    break;
                }
                else
                {

                }
            }
        }

        if(safe_flag == 0)
        {
            if (p)
            {
                sprintf(kill_process[j++] , p->comm);
                //force_sig(SIGKILL, p);
            }
        }
    }//for_each_process

    if(ptasklist_lock != NULL)
        read_unlock(ptasklist_lock);
    else
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





int task_kill_select(char *task_name)
{
    struct task_struct *p;
    struct task_struct *selected = NULL;
    unsigned long flags_kill;
    DUMP_FUN_ENTER;

    if(ptasklist_lock != NULL)
    {
        lidbg("read_lock+\n");
        read_lock(ptasklist_lock);
    }
    else
        spin_lock_irqsave(&kill_lock, flags_kill);

    for_each_process(p)
    {
        struct mm_struct *mm;
        struct signal_struct *sig;

        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        selected = p;

        if(!strcmp(p->comm, task_name))
        {
            lidbg("find %s to kill\n", task_name);

            if (selected)
            {
                force_sig(SIGKILL, selected);
                break;
            }
        }
    }

    if(ptasklist_lock != NULL)
        read_unlock(ptasklist_lock);
    else
        spin_unlock_irqrestore(&kill_lock, flags_kill);

    DUMP_FUN_LEAVE;
    return 0;
}






int task_find_by_pid(int pid)
{
    struct task_struct *p;
    struct task_struct *selected = NULL;
    unsigned long flags_kill;
    DUMP_FUN_ENTER;

    if(ptasklist_lock != NULL)
    {
        lidbg("read_lock+\n");
        read_lock(ptasklist_lock);
    }
    else
        spin_lock_irqsave(&kill_lock, flags_kill);

    for_each_process(p)
    {
        struct mm_struct *mm;
        struct signal_struct *sig;

        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        selected = p;

        if (p->pid == pid)
        {
           // lidbg("find %s by pid-%d\n", p->comm, pid);
            lidbg_fs_log(FASTBOOT_LOG_PATH,"find %s by pid-%d\n", p->comm, pid);

            //if (selected)
            {
                //force_sig(SIGKILL, selected);
                break;
            }
        }
    }

    if(ptasklist_lock != NULL)
        read_unlock(ptasklist_lock);
    else
        spin_unlock_irqrestore(&kill_lock, flags_kill);

    DUMP_FUN_LEAVE;
    return 0;
}


void show_wakelock(bool file_log)
{
    int index = 0;
    struct wakelock_item *pos;
    struct list_head *client_list = &lidbg_wakelock_list;

    if(list_empty(client_list))
        lidbg("<err.lidbg_show_wakelock:nobody_register>\n");
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->name && pos->cunt > 0)
        {
            if(!strcmp(pos->name, "mmc0_detect") || !strcmp(pos->name, "mmc1_detect") || !strcmp(pos->name, "mmc2_detect")
                    || !strcmp(pos->name, "msm_serial_hs_rx") || !strcmp(pos->name, "msm_serial_hs_dma")
              )
                continue;

            index++;
            lidbg("<THE%d:[%d,%d][%s][%s]>,%d,MAX:%d\n",  index, pos->pid, pos->uid, lock_type(pos->is_count_wakelock), pos->name,pos->cunt, pos->cunt_max );
            if(file_log)
            {
                lidbg_fs_log(FASTBOOT_LOG_PATH, "block wakelock %s\n", pos->name);
				
				if(pos->pid != 0)
					task_find_by_pid(pos->pid);
            }
        }
    }
}

struct list_head *active_wake_locks = NULL;
void get_wake_locks(struct list_head *p)
{
    active_wake_locks = p;
}

static void list_active_locks(void)
{
    struct wake_lock *lock;
    int type = 0;
    unsigned long irq_flags;
    if(active_wake_locks == NULL) return;
    DUMP_FUN;
    spin_lock_irqsave(&active_wakelock_list_lock, irq_flags);
    list_for_each_entry(lock, &active_wake_locks[type], link)
    {
        lidbg("%s\n", lock->name);
    }
    spin_unlock_irqrestore(&active_wakelock_list_lock, irq_flags);
}

static void set_func_tbl(void)
{
    plidbg_dev->soc_func_tbl.pfnSOC_Get_WakeLock = get_wake_locks;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_Ignore_Wakelock = is_ignore_wakelock;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void acc_early_suspend(struct early_suspend *handler)
{
    lidbg("acc_early_suspend:%d\n", plidbg_acc->resume_count);

    suspend_state = PM_STATUS_EARLY_SUSPEND_PENDING;
    check_all_clk_disable();

    if(find_unsafe_clk()) {}

    fastboot_task_kill_exclude();
	
	//task_kill_select("tencent.qqmusic");
	//task_kill_select(".flyaudio.media");
}

static void acc_late_resume(struct early_suspend *handler)
{
    DUMP_FUN_ENTER;
    suspend_state = PM_STATUS_LATE_RESUME_OK;

}
#endif


static int thread_acc_suspend(void *data)
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

                if(suspend_state == PM_STATUS_EARLY_SUSPEND_PENDING)    //if suspend state always in early suspend
                {
                    if(time_count >= 10)
                    {
                        lidbgerr("thread_acc_suspend wait suspend timeout!\n");
						
						//task_kill_select("tencent.qqmusic");
						//task_kill_select(".flyaudio.media");

                        if(time_count % 5 == 0)
                        {
                        	show_wakelock(0);
                        	//list_active_locks();
                        }
                        if(time_count % 10 == 0)
                        {
							fastboot_task_kill_exclude();
                        }

                        #if 0
                        if(time_count % 30 == 0)
                        {
							task_kill_select("tencent.qqmusic");
							task_kill_select(".flyaudio.media");
                        }
						#endif
                        if(time_count >= 120)
                        {
                            show_wakelock(1);
                            if(suspend_state == PM_STATUS_EARLY_SUSPEND_PENDING)
                                ignore_wakelock = 1;
                        }
                        //break;
                    }
                }
                else
                    break;
            }
        }
    }
    return 0;
}

ssize_t  acc_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
    return size;
}

ssize_t  acc_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{

    char data_rec[32];
    char *p = NULL;
    int len = count;

    lidbg("acc_write.\n");

    if (copy_from_user( data_rec, buf, count))
    {
        printk("copy_from_user ERR\n");
    }

    if((p = memchr(data_rec, '\n', count)))
    {
        len = p - data_rec;
        *p = '\0';
    }
    else
        data_rec[len] =  '\0';

    printk("acc_nod_write:==%d====[%s]\n", len, data_rec);

    // processing data
    if(!strcmp(data_rec, "acc_on"))
    {
        printk("******bp:goto acc_on********\n");
        SOC_Write_Servicer(CMD_ACC_ON);
        lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_ACC_ON));
    }
    else if(!strcmp(data_rec, "acc_off"))
    {
        printk("******bp:goto acc_off********\n");
        plidbg_acc->accoff_count ++;
        SOC_Write_Servicer(CMD_ACC_OFF);
        lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_ACC_OFF));
    }
    else if(!strcmp(data_rec, "power"))
    {
        printk("******bp:goto fastboot********\n");
        lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_POWER_OFF));
        SOC_Write_Servicer(CMD_FAST_POWER_OFF);
        plidbg_acc->poweroff_count++;
        if((!g_var.is_fly)||(STRICT_SUSPEND == 0xff))
        {
            SOC_Write_Servicer(LOG_LOGCAT);
            //SOC_Write_Servicer(LOG_DMESG);
        }
        msleep(5000);
        complete(&suspend_start);
    }

    return count;
}


int acc_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int acc_release(struct inode *inode, struct file *filp)
{
    return 0;
}

void cb_password_poweroff(char *password )
{
    SOC_Write_Servicer(CMD_FAST_POWER_OFF);
}

static int  acc_probe(struct platform_device *pdev)
{
    int ret;
    DUMP_FUN_ENTER;
    plidbg_acc = kmalloc(sizeof(lidbg_acc), GFP_KERNEL);
    if (!plidbg_acc)
    {
        ret = -ENODEV;
        goto fail_mem;
    }

    plidbg_acc->resume_count = 0;
    plidbg_acc->accoff_count = 0;
    plidbg_acc->poweroff_count = 0;

    if(!fs_is_file_exist(HAL_SO))
    {
        FORCE_LOGIC_ACC;
    }

    spin_lock_init(&active_wakelock_list_lock);

    fs_regist_state("acc_times", (int *)&plidbg_acc->resume_count);
    te_regist_password("001200", cb_password_poweroff);

    fs_file_separator(FASTBOOT_LOG_PATH);

    if(fs_fill_list("/flysystem/lib/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &fastboot_kill_list) < 0)
        fs_fill_list("/system/lib/modules/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &fastboot_kill_list);

    spin_lock_init(&kill_lock);

    return 0;

fail_mem:
    return ret;
}

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   =   acc_open,
    .read   =   acc_read,
    .write  =   acc_write,
    .release =  acc_release,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};

static int  acc_remove(struct platform_device *pdev)
{
    return 0;
}

#ifdef CONFIG_PM
static int acc_resume(struct device *dev)
{
    DUMP_FUN_ENTER;
    ignore_wakelock = 0;
    suspend_state = PM_STATUS_RESUME_OK;

    lidbg("fastboot_resume:%d\n", ++plidbg_acc->resume_count);
    if(plidbg_acc->poweroff_count != plidbg_acc->resume_count)
    {
        lidbg("err:poweroff_count:%d\n", plidbg_acc->poweroff_count);

    }
    return 0;

}

static int acc_suspend(struct device *dev)
{
    DUMP_FUN_ENTER;
    suspend_state = PM_STATUS_SUSPEND_PENDING;
    return 0;

}

static struct dev_pm_ops acc_pm_ops =
{
    .suspend	= acc_suspend,
    .resume		= acc_resume,
};
#endif

static struct platform_driver acc_driver =
{
    .probe		= acc_probe,
    .remove     = acc_remove,
    .driver         = {
        .name = "lidbg_acc1",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &acc_pm_ops,
#endif
    },
};

static struct platform_device lidbg_acc_device =
{
    .name               = "lidbg_acc1",
    .id                 = -1,
};


static int __init acc_init(void)
{
    int ret;
    LIDBG_GET;
    set_func_tbl();
    platform_device_register(&lidbg_acc_device);

    platform_driver_register(&acc_driver);

    INIT_COMPLETION(acc_ready);
    ret = misc_register(&misc);

#ifdef CONFIG_HAS_EARLYSUSPEND
    {
        early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 5; //the later the better
        early_suspend.suspend = acc_early_suspend;
        early_suspend.resume = acc_late_resume;
        register_early_suspend(&early_suspend);
    }
#endif

    INIT_COMPLETION(suspend_start);
    suspend_task = kthread_create(thread_acc_suspend, NULL, "suspend_task");
    if(IS_ERR(suspend_task))
    {
        lidbg("Unable to start kernel suspend_task.\n");

    }
    else wake_up_process(suspend_task);

    lidbg_chmod("/dev/lidbg_acc");
    lidbg (DEVICE_NAME"acc dev_init\n");
    fs_register_filename_list(FASTBOOT_LOG_PATH, true);

    return ret;
}

static void __exit acc_exit(void)
{
    misc_deregister(&misc);
    lidbg (DEVICE_NAME"acc dev_exit\n");
}

module_init(acc_init);
module_exit(acc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("lidbg_acc driver");

