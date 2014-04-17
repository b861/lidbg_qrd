#include "lidbg.h"
LIDBG_DEFINE;

#define BUTTON_LED_NODE "/sys/class/leds/button-backlight/brightness"
static int is_sleep_mode = 0;
static int list_count ;
static ktime_t last_ktime_sleep;
static struct completion sleep_observer_wait;
typedef enum
{
    WS_LIST_ACTION_PRINT,
    WS_LIST_ACTION_PRINT_UNLOCK,
    WS_LIST_ACTION_NULL
} ws_list_action_types;

void pm_observer_wakeup(void);
void pm_observer_sleep(void);

bool set_factory_reset_mode(bool on)
{
    PM_WARN("%d\n", on);
    lidbg_uevent_shell("am broadcast -a android.intent.action.MASTER_CLEAR");
    return true;
}

static void fastboot_task_kill_exclude(void)
{
    static char kill_process[32][25];
    struct task_struct *p;
    struct mm_struct *mm;
    struct signal_struct *sig;
    u32 i, j = 0;
    bool safe_flag = 0;
    DUMP_FUN_ENTER;

    PM_WARN("-----------------------\n");
    rcu_read_lock();
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
            (strncmp(p->comm, "ftf", sizeof("ftf") - 1) == 0) ||
            (strncmp(p->comm, "lidbg_", sizeof("lidbg_") - 1) == 0)
        )
        {
            continue;
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
    rcu_read_unlock();

    PM_WARN("-----------------------\n\n");


    if(j == 0)
        PM_WARN("find nothing to kill\n");
    else
        for(i = 0; i < j; i++)
        {
            {
                PM_WARN("## find %s to kill ##\n", kill_process[i]);

            }
        }
    DUMP_FUN_LEAVE;

}

static int ws_func_print(char *info, struct wakeup_source *ws)
{
    spin_lock_irq(&ws->lock);
    if (ws->active)
    {
        PM_WARN("[%s]:%d:%s,ps.%lld,ac.%lu,rc%lu,wc.%lu\n", info, list_count, ws->name, ktime_to_ms(ws->prevent_sleep_time), ws->active_count, ws->relax_count, ws->wakeup_count);
        list_count++;
    }
    spin_unlock_irq(&ws->lock);
    return 1;
}
static int ws_func_print_unlock(char *info, struct wakeup_source *ws)
{
    if ( ws->active)
    {
        PM_WARN("[%s]:%d:%s,ps.%lld,ac.%lu,rc%lu,wc.%lu\n", info, list_count, ws->name, ktime_to_ms(ws->prevent_sleep_time), ws->active_count, ws->relax_count, ws->wakeup_count);
        __pm_relax(ws);
        list_count++;
    }
    return 1;
}
static int (*wl_list_action_func[]) (char *info, struct wakeup_source *ws) =
{
    [WS_LIST_ACTION_PRINT] = ws_func_print,
    [WS_LIST_ACTION_PRINT_UNLOCK] = ws_func_print_unlock,
};
void ws_list_action_entry(char *info, ws_list_action_types type)
{
    struct wakeup_source *ws;
    list_count = 0;
    PM_WARN("%s,%d\n", info, type);
    lidbg_show_wakelock();
    if(g_var.ws_lh == NULL || type >= WS_LIST_ACTION_NULL)
    {
        PM_ERR("g_var.ws_lh==NULL||type.%d\n", type);
        return ;
    }
    rcu_read_lock();
    list_for_each_entry_rcu(ws, g_var.ws_lh, entry)
    {
        wl_list_action_func[type] (info, ws);
    }
    rcu_read_unlock();
}

void pm_suspend_marker1(char *annotation)
{
    struct timespec ts;
    struct rtc_time tm;

    getnstimeofday(&ts);
    rtc_time_to_tm(ts.tv_sec, &tm);
    pr_info("PM: suspend %s %d-%02d-%02d %02d:%02d:%02d.%09lu UTC\n",
            annotation, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec);
}

void set_sleep_time(ktime_t ktime)
{
    last_ktime_sleep = ktime;
}
int get_sleep_time(void)
{
    ktime_t now, sub;
    now = ktime_get();
    sub = ktime_sub(now, last_ktime_sleep);
    return (int)ktime_to_ms(sub) / 1000;
}

void lidbg_pm_step_call(fly_pm_stat_step step, void *data)
{
    switch (step)
    {
    case PM_AUTOSLEEP_STORE1:
    {
        char *buff = data;

        if(!strcmp(buff, "mem"))
            pm_observer_wakeup();
        else if(!strcmp(buff, "off"))
            pm_observer_sleep();

        PM_WARN("PM_AUTOSLEEP_STORE1:[%s,%d]\n", buff, is_sleep_mode);
    }
    break;
    case PM_AUTOSLEEP_SET_STATE2:
        break;
    case PM_QUEUE_UP_SUSPEND_WORK3:
        break;
    case PM_TRY_TO_SUSPEND4:
        // ws_list_action_entry("start1",WS_LIST_ACTION_PRINT);
        break;
    case PM_TRY_TO_SUSPEND4P1:
    case PM_TRY_TO_SUSPEND4P2:
    case PM_TRY_TO_SUSPEND4P3:
    case PM_TRY_TO_SUSPEND4P4:
    case PM_TRY_TO_SUSPEND4P5:
        //ws_list_action_entry("stop1",WS_LIST_ACTION_PRINT);
        break;
    case PM_SUSPEND5:
        break;
    case PM_ENTER_STATE6:
        break;
    case PM_ENTER_STATE6P1:
        break;
    case PM_ENTER_STATE6P2:
        break;
    case PM_SUSPEND_DEVICES_AND_ENTER7:
        pm_suspend_marker1("[ftf_pm1].suspend_ops->enter.in");
        set_sleep_time(ktime_get());
        break;
    case PM_SUSPEND_ENTER8:
        fs_file_write(BUTTON_LED_NODE, "0");
        break;
    case PM_SUSPEMD_OPS_ENTER9:
        break;
    case PM_SUSPEMD_OPS_ENTER9P1:
        pm_suspend_marker1("[ftf_pm1].suspend_ops->enter.out");
        PM_WARN("sleeped:[%d] S\n", get_sleep_time());
        fs_file_write(BUTTON_LED_NODE, "255");
        break;
    case PM_NULL:
        ws_list_action_entry("test", WS_LIST_ACTION_PRINT_UNLOCK);
        break;
    default:
        break;
    }
}

int linux_to_lidbg_receiver(linux_to_lidbg_transfer_t _enum, void *data)
{
    switch (_enum)
    {
    case LTL_TRANSFER_RTC:
        break;
    case LTL_TRANSFER_NULL:
        break;
    default:
        break;
    }
    return 1;
}

void pm_observer_wakeup(void)
{
    is_sleep_mode = true;
    complete(&sleep_observer_wait);
}
void pm_observer_sleep(void)
{
    is_sleep_mode = false;
}
void pm_observer_prepare(void)
{
    char cmd[128] = {0};
    lidbg_chmod("/sys/module/msm_show_resume_irq/parameters/debug_mask");
    ssleep(1);
    fs_file_write("/sys/module/msm_show_resume_irq/parameters/debug_mask", "1");
    ssleep(1);
    lidbg_chmod("/dev/lidbg*");
    sprintf(cmd, "cat /proc/interrupts > %sinterrupts.txt &", LIDBG_LOG_DIR);
    lidbg_uevent_shell(cmd);
    fs_register_filename_list(LIDBG_LOG_DIR"interrupts.txt", true);
}
void chmod_for_app(bool on)
{
    lidbg_chmod("/sys/power/*");
}
static int thread_pm_observer(void *data)
{
    ssleep(10);
    pm_observer_prepare();
    if(!g_var.is_fly)
        chmod_for_app(true);
    while(!kthread_should_stop())
    {
        if( !wait_for_completion_interruptible(&sleep_observer_wait))
        {
            ws_list_action_entry("start1", WS_LIST_ACTION_PRINT);
            while(0)
            {
                ssleep(5);
                ws_list_action_entry("stop1", WS_LIST_ACTION_PRINT);//WS_LIST_ACTION_PRINT_UNLOCK  //fastboot_task_kill_exclude();
            }
            PM_WARN("stop\n");
        }
    }
    return 1;
}

static LIST_HEAD(pm_list_3);
void pm_list_kill_each(void)
{
    struct string_dev *pos;
    char *p = NULL;
    list_for_each_entry(pos, &pm_list_3, tmp_list)
    {
        if(pos->yourkey && (p = strchr(pos->yourkey, ':')))
        {
            ++p;
            lidbg_force_stop_apk(p);
            msleep(1200);
        }
        p = NULL;
    }
}
static int thread_am_kill_apk(void *data)
{
    pm_list_kill_each();
    return 1;
}
static int thread_pm_led_monitor(void *data)
{
    ssleep(8);
    while(!kthread_should_stop())
    {
        fs_file_write(BUTTON_LED_NODE, "0");
        ssleep(1);
        fs_file_write(BUTTON_LED_NODE, "255");
        ssleep(1);
    }
    PM_WARN("stop\n");
    return 1;
}

int pm_open (struct inode *inode, struct file *filp)
{
    return 0;
}
ssize_t pm_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    char *cmd[8] = {NULL};
    int cmd_num  = 0;
    char cmd_buf[512];
    memset(cmd_buf, '\0', 512);

    if(copy_from_user(cmd_buf, buf, size))
    {
        PM_ERR("copy_from_user ERR\n");
    }
    if(cmd_buf[size - 1] == '\n')
        cmd_buf[size - 1] = '\0';
    PM_WARN("[%d,%s]\n", size, cmd_buf);

    cmd_num = lidbg_token_string(cmd_buf, " ", cmd) ;

    //just test
    if(!strcmp(cmd[0], "ws"))
    {
        if(!strcmp(cmd[1], "chmod"))
            lidbg_chmod(cmd[2]);
        else if(!strcmp(cmd[1], "rm"))
            lidbg_rm(cmd[2]);
        else  if(!strcmp(cmd[1], "list"))
        {
            int  ws_action_type = simple_strtoul(cmd[2], 0, 0);
            ws_list_action_entry("app", (ws_list_action_types)ws_action_type);
        }
        else  if(!strcmp(cmd[1], "pm"))
        {
            int pm_observer_mode = simple_strtoul(cmd[2], 0, 0);
            if(pm_observer_mode)
                pm_observer_wakeup();
            else
                pm_observer_sleep();
        }
        else  if(!strcmp(cmd[1], "fac"))
        {
            set_factory_reset_mode(true);
        }
        else  if(!strcmp(cmd[1], "reb"))
        {
            msleep(100);
            lidbg_reboot();
        }
        else  if(!strcmp(cmd[1], "apk"))
        {
            lidbg_pm_install_dir(cmd[2]);
        }
        else  if(!strcmp(cmd[1], "kill3"))
        {
            CREATE_KTHREAD(thread_am_kill_apk, NULL);
        }
    }
    return size;
}
static  struct file_operations pm_nod_fops =
{
    .owner = THIS_MODULE,
    .write = pm_write,
    .open = pm_open,
};


static void set_func_tbl(void)
{
    plidbg_dev->soc_func_tbl.pfnLINUX_TO_LIDBG_TRANSFER = linux_to_lidbg_receiver;
    plidbg_dev->soc_func_tbl.pfnSOC_PM_STEP = lidbg_pm_step_call;
}
int thread_pm_init(void *data)
{
    PM_WARN("<==IN==>\n");
    lidbg_new_cdev(&pm_nod_fops, "lidbg_pm");

    if(!g_var.is_fly)
        CREATE_KTHREAD(thread_pm_led_monitor, NULL);

    PM_WARN("<==OUT==>\n\n");
    LIDBG_MODULE_LOG;
    ssleep(10);
    lidbg_uevent_shell("pm list packages -3 > /data/lidbg/pm_3.txt");
    ssleep(5);
    fs_fill_list(LIDBG_LOG_DIR"pm_3.txt", FS_CMD_FILE_LISTMODE, &pm_list_3);
    fs_register_filename_list(LIDBG_LOG_DIR"pm_3.txt", true);

    return 0;
}
static int __init lidbg_pm_init(void)
{
    DUMP_FUN;
    LIDBG_GET;
    set_func_tbl();
    init_completion(&sleep_observer_wait);
    set_sleep_time(ktime_set(0, 0));
    CREATE_KTHREAD(thread_pm_init, NULL);
    kthread_run(thread_pm_observer, NULL, "ftf_pmtask");
    return 0;
}

static void __exit lidbg_pm_exit(void)
{
}

module_init(lidbg_pm_init);
module_exit(lidbg_pm_exit);

MODULE_DESCRIPTION("lidbg.powermanager");
MODULE_LICENSE("GPL");


