#include "lidbg.h"
LIDBG_DEFINE;
//#define CONTROL_PM_IO_BY_BP

#define PM_DIR LIDBG_LOG_DIR"pm_info/"
#define PM_FILE_INFO PM_DIR"pm_info.txt"
#define PM_FILE_INFO_SIZE (5)

static DECLARE_COMPLETION(sleep_observer_wait);
static atomic_t is_in_sleep = ATOMIC_INIT(-1);
static int sleep_counter = 0;
void observer_start(void);
void observer_stop(void);

bool is_safety_apk(char *apkname)
{
    if(strncmp(apkname, "com.fly.flybootservice", sizeof("com.fly.flybootservice") - 1) == 0)
        return true;
    else
        return false;
}

/*	0:show wakelock		1:kill has lock apk		2:save has lock apk package*/
void userspace_wakelock_action(int action_enum, char *file_path)
{
    int index = 0;
    char *p1 = NULL, *p2 = NULL, *kill = NULL;
    struct wakelock_item *pos;
    struct list_head *client_list ;

    client_list = &lidbg_wakelock_list;
    if(!list_empty(client_list))
    {
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (pos->name && pos->cunt)
            {
                index++;
                printk(KERN_CRIT"[ftf_pm.wl]%d,MAX%d<THE%d:[%d,%d][%s][%s,%s]>\n", pos->cunt, pos->cunt_max, index, pos->pid, pos->uid, lock_type(pos->is_count_wakelock), pos->name, pos->package_name);
                switch (action_enum)
                {
                case 0:
                    break;
                case 1:
                {
                    p1 = strchr(pos->package_name, ',');
                    p2 = strchr(pos->package_name, '.');
                    kill = NULL;

                    if(p1)
                        kill = p1 + 1;
                    else if(p2)
                        kill = pos->package_name;
                    if(kill && !is_safety_apk(kill))
                        lidbg_force_stop_apk(kill);
                }
                break;
                case 2:
                    if (file_path && pos->is_count_wakelock)
                        fs_string2file(PM_FILE_INFO_SIZE, file_path, "[J]%d,[%s,%s]>\n", index, pos->name, pos->package_name);
                    break;

                default:
                    lidbg("<lidbg_show_wakelock.err:%d>\n", action_enum );
                    break;
                }

            }
        }
    }
    else
        lidbg("<err.lidbg_show_wakelock:nobody_register>\n");
}


int kernel_wakelock_print(char *info)
{
    struct wakeup_source *ws;
    int list_count = 0;
    if(g_var.ws_lh == NULL)
    {
        PM_ERR("g_var.ws_lh==NULL\n");
        return -1;
    }
    PM_WARN("<%s>\n", info);
    rcu_read_lock();
    list_for_each_entry_rcu(ws, g_var.ws_lh, entry)
    {
        spin_lock_irq(&ws->lock);
        if (ws->active)
        {
            PM_WARN("[%s]:%d:%s,ps.%lld,ac.%lu,rc%lu,wc.%lu\n", info, list_count, ws->name, ktime_to_ms(ws->prevent_sleep_time), ws->active_count, ws->relax_count, ws->wakeup_count);
            list_count++;
        }
        spin_unlock_irq(&ws->lock);
    }
    rcu_read_unlock();
    return 1;
}

int kernel_wakelock_force_unlock(char *info)
{
    struct wakeup_source *ws;
    int list_count = 0;
    if(g_var.ws_lh == NULL)
    {
        PM_ERR("g_var.ws_lh==NULL\n");
        return -1;
    }
    PM_WARN("<%s>\n", info);
    rcu_read_lock();
    list_for_each_entry_rcu(ws, g_var.ws_lh, entry)
    {
        if ( ws->active)
        {
            PM_WARN("[%s]:%d:%s,ps.%lld,ac.%lu,rc%lu,wc.%lu\n", info, list_count, ws->name, ktime_to_ms(ws->prevent_sleep_time), ws->active_count, ws->relax_count, ws->wakeup_count);
            __pm_relax(ws);
            list_count++;
        }
    }
    rcu_read_unlock();
    return 1;
}

int kernel_wakelock_save_wakelock(char *info, char *path)
{
    struct wakeup_source *ws;
    int list_count = 0;
    if(g_var.ws_lh == NULL)
    {
        PM_ERR("g_var.ws_lh==NULL\n");
        return -1;
    }
    PM_WARN("<%s>\n", info);
    rcu_read_lock();
    list_for_each_entry_rcu(ws, g_var.ws_lh, entry)
    {
        if (ws->active)
        {
            rcu_read_unlock();
            fs_string2file(PM_FILE_INFO_SIZE, path, "%s[K].%d:%s\n", info, list_count, ws->name);
            list_count++;
            rcu_read_lock();
        }
    }
    rcu_read_unlock();
    return 1;
}

struct wakeup_source *kernel_wakelock_find_wakelock(char *info)
{
    struct wakeup_source *ws, *tmp = NULL;
    int exist = 0;
    if(g_var.ws_lh == NULL)
    {
        PM_ERR("g_var.ws_lh==NULL\n");
        return NULL;
    }
    rcu_read_lock();
    list_for_each_entry_rcu(ws, g_var.ws_lh, entry)
    {
        spin_lock_irq(&ws->lock);
        if (ws->active && !strcmp(ws->name, info))
            exist = 1;
        spin_unlock_irq(&ws->lock);

        if(exist == 1)
        {
            tmp = ws;
            break;
        }
    }
    rcu_read_unlock();
    return tmp;
}

void lidbg_pm_step_call(fly_pm_stat_step step, void *data)
{
    switch (step)
    {
    case PM_AUTOSLEEP_STORE1:
    {
        char *buff = data;
        if(!strcmp(buff, "mem"))
        {
        }
        else if(!strcmp(buff, "off"))
        {
            observer_stop();
        }
        PM_WARN("PM_AUTOSLEEP_STORE1:[%d,%s,%d]\n", sleep_counter, buff, atomic_read(&is_in_sleep));
    }
    break;
    case PM_AUTOSLEEP_SET_STATE2:
        break;
    case PM_QUEUE_UP_SUSPEND_WORK3:
        break;
    case PM_TRY_TO_SUSPEND4:
        //   kernel_wakelock_print("start1:");
        //   userspace_wakelock_action(0);
        break;
    case PM_TRY_TO_SUSPEND4P1:
    case PM_TRY_TO_SUSPEND4P2:
    case PM_TRY_TO_SUSPEND4P3:
    case PM_TRY_TO_SUSPEND4P4:
    case PM_TRY_TO_SUSPEND4P5:
        //   kernel_wakelock_print("stop1:");
        //   userspace_wakelock_action(0);
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
        //suspend_ops->enter.in
        break;
    case PM_SUSPEND_ENTER8:
		g_var.system_status = FLY_KERNEL_DOWN;
		MCU_WP_GPIO_OFF;
		sleep_counter++;
        PM_SLEEP_DBG("SLEEP8.suspend_enter.sleep.SOC_IO_Output(0, 35, 1);sleep_count:%d,%d\n",(*(int *)data),sleep_counter);
        break;
    case PM_SUSPEMD_OPS_ENTER9:
        break;
    case PM_SUSPEMD_OPS_ENTER9P1:
		g_var.system_status = FLY_KERNEL_UP;
		MCU_WP_GPIO_ON;
		PM_SLEEP_DBG("WAKEUP9.1.suspend_enter.wakeup.SOC_IO_Output(0, 35, 0);\n");
        //suspend_ops->enter.out
        break;
    case PM_NULL:
        PM_ERR("PM_NULL\n");
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
        PM_ERR("LTL_TRANSFER_NULL\n");
        break;
    default:
        break;
    }
    return 1;
}
void usb_disk_enable(bool enable)
{
    PM_WARN("[%s]\n", enable ? "usb_enable" : "usb_disable");
    if(enable)
        USB_WORK_ENABLE;
    else
        USB_WORK_DISENABLE;
}
static int thread_usb_disk_enable_delay(void *data)
{
    msleep(200);
    usb_disk_enable(true);
    return 1;
}
static int thread_usb_disk_disable_delay(void *data)
{
    usb_disk_enable(false);
    return 1;
}
static int thread_gpio_app_status_delay(void *data)
{
    ssleep(30);
    MCU_APP_GPIO_ON;
#ifdef CONTROL_PM_IO_BY_BP
		MCU_SET_APP_GPIO_SUSPEND;
#endif

    PM_WARN("<set MCU_APP_GPIO_ON >\n");
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
    PM_WARN("-----FLYSTEP------------------[%s]---\n", cmd_buf);

    cmd_num = lidbg_token_string(cmd_buf, " ", cmd) ;

    //flyaudio logic
    if(!strcmp(cmd[0], "flyaudio"))
    {
        if(!strcmp(cmd[1], "screen_off"))
        {
            lidbg("******into screen_off********\n");
			g_var.system_status = FLY_SCREEN_OFF;
            lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_SCREEN_OFF));
            if(SOC_Hal_Acc_Callback)
            {
                lidbg("hal callback 0\n");
                SOC_Hal_Acc_Callback(0);
            }
            if(!g_var.is_fly && fs_is_file_exist("/system/app/NfcNci.apk"))
                lidbg_rm("/system/app/NfcNci.apk");
			LPC_PRINT(true,sleep_counter,"PM:screen_off");
        }
        if(!strcmp(cmd[1], "screen_on"))
        {
            lidbg("******into screen_on********\n");
			g_var.system_status = FLY_SCREEN_ON;
			if(!g_var.is_fly)
			    LCD_ON;
			if(SOC_Hal_Acc_Callback)
			{
			    lidbg("hal callback 1\n");
			    SOC_Hal_Acc_Callback(1);
			}
            lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_SCREEN_ON));
			LPC_PRINT(true,sleep_counter,"PM:screen_on");
        }
        else  if(!strcmp(cmd[1], "android_up"))
        {
			g_var.system_status = FLY_ANDROID_UP;
			MCU_APP_GPIO_ON;
        }
        else  if(!strcmp(cmd[1], "android_down"))
        {
        	
			g_var.system_status = FLY_ANDROID_DOWN;
			MCU_APP_GPIO_OFF;
        }
        else  if(!strcmp(cmd[1], "gotosleep"))
        {
        	
			g_var.system_status = FLY_GOTO_SLEEP;
            observer_start();
            CREATE_KTHREAD(thread_usb_disk_disable_delay, NULL);
        }
        else if(!strcmp(cmd[1], "devices_up"))
        {
        	
			g_var.system_status = FLY_DEVICE_UP;
            CREATE_KTHREAD(thread_usb_disk_enable_delay, NULL);

            lidbg("******into suspend_on********\n");
            lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_SUSPEND_UNPREPARE));
            if(SOC_Hal_Acc_Callback)
            {
                lidbg("hal callback 2\n");
                SOC_Hal_Acc_Callback(2);
            }
        }
        else if(!strcmp(cmd[1], "devices_down"))
        {
            lidbg("******into suspend_off********\n");
			
			g_var.system_status = FLY_DEVICE_DOWN;
            lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_ACC_EVENT, NOTIFIER_MINOR_SUSPEND_PREPARE));
            if(SOC_Hal_Acc_Callback)
            {
                lidbg("hal callback 3\n");
                SOC_Hal_Acc_Callback(3);
            }
        }

    }

    //pm debug
    if(!strcmp(cmd[0], "ws"))
    {
        if(!strcmp(cmd[1], "chmod"))
            lidbg_chmod(cmd[2]);
        else if(!strcmp(cmd[1], "rm"))
            lidbg_rm(cmd[2]);
        else  if(!strcmp(cmd[1], "list"))
        {
            int  ws_action_type = simple_strtoul(cmd[2], 0, 0);
            /*	0:show wakelock		1:force unlock		2:kill has lock apk		3:save lock		4:find wakelock*/
            switch (ws_action_type)
            {
            case 0:
                kernel_wakelock_print("test:");
                userspace_wakelock_action(0, NULL);
                break;
            case 1:
                kernel_wakelock_force_unlock("test:");
                userspace_wakelock_action(1, NULL);
                break;
            case 2:
                kernel_wakelock_print("test:");
                userspace_wakelock_action(1, NULL);
                break;
            case 3:
                fs_clear_file(LIDBG_OSD_DIR"pm_info.txt");
                kernel_wakelock_save_wakelock("test:", LIDBG_OSD_DIR"pm_info.txt");
                userspace_wakelock_action(2, LIDBG_OSD_DIR"pm_info.txt");
                break;
            case 4:
                if(cmd[3] && kernel_wakelock_find_wakelock(cmd[3]) != NULL)
                    PM_WARN("<find:%s>\n", cmd[3]);
                userspace_wakelock_action(0, NULL);
                break;

            default:
                break;
            }

        }
        else  if(!strcmp(cmd[1], "pm"))
        {
            int pm_observer_mode = simple_strtoul(cmd[2], 0, 0);
            if(pm_observer_mode)
                observer_start();
            else
                observer_stop();
        }
        else  if(!strcmp(cmd[1], "reb"))
        {
            int  ws_action_type = simple_strtoul(cmd[2], 0, 0);
            msleep(100);
            /*	0:normal reboot		1:reboot recovery		2:reboot bootloader	*/
            switch (ws_action_type)
            {
            case 0:
                lidbg_shell_cmd("reboot");
                break;
            case 1:
                lidbg_shell_cmd("reboot recovery");
                break;
            case 2:
                lidbg_shell_cmd("reboot bootloader");
                break;
            default:
                break;
            }
        }
        else  if(!strcmp(cmd[1], "apk"))
        {
            lidbg_pm_install_dir(cmd[2]);
        }
        else  if(!strcmp(cmd[1], "kill"))
        {
            lidbg_force_stop_apk(cmd[2]);
        }
        else  if(!strcmp(cmd[1], "toast"))
        {
            int  enum_value = simple_strtoul(cmd[3], 0, 0);
            lidbg_toast_show(cmd[2], enum_value);
        }
        else  if(!strcmp(cmd[1], "udisk_reset"))
        {
            usb_disk_enable(false);
            ssleep(2);
            usb_disk_enable(true);
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

void log_resume_times(int sleep_counter)
{
    static char dmesg_file_name[32] = {0};
    static char time_buf[64] = {0};
    static bool flag = 0;
    if(flag == 0)
    {
        lidbg_mkdir(PM_DIR);
        ssleep(2);
        lidbg_get_current_time(time_buf, NULL);
        sprintf(dmesg_file_name, PM_DIR"last_acc%s.txt", time_buf);
        flag = 1;
    }
    fs_clear_file(dmesg_file_name);
    fs_string2file(0, dmesg_file_name, "%d", sleep_counter);
}
static int thread_save_acc_times(void *data)
{
    log_resume_times(sleep_counter);
    return 1;
}
#ifdef CONFIG_PM
static int pm_suspend(struct device *dev)
{
    DUMP_FUN;
    return 0;
}
static int pm_resume(struct device *dev)
{
    DUMP_FUN;
    CREATE_KTHREAD(thread_save_acc_times, NULL);
    return 0;
}
static struct dev_pm_ops lidbg_pm_ops =
{
    .suspend	= pm_suspend,
    .resume		= pm_resume,
};
#endif

void observer_prepare(void)
{
    char cmd[128] = {0};
    lidbg_chmod("/sys/module/msm_show_resume_irq/parameters/debug_mask");
    ssleep(1);
    fs_file_write("/sys/module/msm_show_resume_irq/parameters/debug_mask",false, "1",0,strlen("1"));
    sprintf(cmd, "cat /proc/interrupts > %sinterrupts.txt &", LIDBG_LOG_DIR);
    lidbg_shell_cmd(cmd);
    fs_register_filename_list(LIDBG_LOG_DIR"interrupts.txt", true);
    fs_register_filename_list(PM_FILE_INFO, true);
}
void observer_start(void)
{
    atomic_set(&is_in_sleep, 1);
    complete(&sleep_observer_wait);
}
void observer_stop(void)
{
    atomic_set(&is_in_sleep, 0);
}
static int thread_observer(void *data)
{
    int have_triggerd_sleep_S = 0;
    char when[64] = {0};

    observer_prepare();

    while(!kthread_should_stop())
    {
        have_triggerd_sleep_S = 0;
        if( !wait_for_completion_interruptible(&sleep_observer_wait))
        {
            kernel_wakelock_print("start:");
            userspace_wakelock_action(0, NULL);
            while((atomic_read(&is_in_sleep) == 1))
            {
                ssleep(1);
                if(atomic_read(&is_in_sleep) != 1)
                    break;
                have_triggerd_sleep_S++;
                switch (have_triggerd_sleep_S)
                {
                case 60:
                    sprintf(when, "unlock:%d:", sleep_counter);
                    kernel_wakelock_save_wakelock(when, PM_FILE_INFO);
                    kernel_wakelock_force_unlock(when);
                    userspace_wakelock_action(2, PM_FILE_INFO);
                    break;

                default:
                    if(have_triggerd_sleep_S >= 5 && !(have_triggerd_sleep_S % 5) && (atomic_read(&is_in_sleep) == 1))
                    {
                        sprintf(when, "start%d:", have_triggerd_sleep_S);
                        kernel_wakelock_print(when);
                        userspace_wakelock_action(0, NULL);
                    }
                    break;
                }
            }
            PM_WARN("\n<stop>\n");
        }
    }
    return 1;
}

static int  lidbg_pm_probe(struct platform_device *pdev)
{
    DUMP_FUN;
    PM_WARN("<==IN==>\n");
    fs_file_separator(PM_FILE_INFO);
    lidbg_new_cdev(&pm_nod_fops, "lidbg_pm");
    kthread_run(thread_observer, NULL, "ftf_pmtask");
    LIDBG_MODULE_LOG;

    PM_WARN("<==OUT==>\n");
    return 0;
}
static struct platform_device lidbg_pm =
{
    .name               = "lidbg_pm",
    .id                 = -1,
};

static struct platform_driver lidbg_pm_driver =
{
    .probe		= lidbg_pm_probe,
    .driver         = {
        .name = "lidbg_pm",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &lidbg_pm_ops,
#endif
    },
};

static void set_func_tbl(void)
{
    plidbg_dev->soc_func_tbl.pfnLINUX_TO_LIDBG_TRANSFER = linux_to_lidbg_receiver;
    plidbg_dev->soc_func_tbl.pfnSOC_PM_STEP = lidbg_pm_step_call;
    plidbg_dev->soc_func_tbl.pfnHal_Acc_Callback = NULL;
}

static int __init lidbg_pm_init(void)
{
    DUMP_FUN;
    LIDBG_GET;
    set_func_tbl();

    lidbg_mkdir(PM_DIR);

    if(is_out_updated)
        lidbg_shell_cmd("rm -r "PM_DIR"*");

	MCU_WP_GPIO_ON;
#ifdef CONTROL_PM_IO_BY_BP
	MCU_SET_WP_GPIO_SUSPEND;
#endif
    PM_WARN("<set MCU_WP_GPIO_ON>\n");
    CREATE_KTHREAD(thread_gpio_app_status_delay, NULL);
    lidbg_shell_cmd("echo 8  > /proc/sys/kernel/printk");
    platform_device_register(&lidbg_pm);
    platform_driver_register(&lidbg_pm_driver);
    return 0;
}

static void __exit lidbg_pm_exit(void)
{
}

module_init(lidbg_pm_init);
module_exit(lidbg_pm_exit);

MODULE_DESCRIPTION("lidbg.powermanager2014.04");
MODULE_LICENSE("GPL");


