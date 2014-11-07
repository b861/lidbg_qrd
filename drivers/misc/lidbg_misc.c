#include "lidbg.h"
LIDBG_DEFINE;

static int logcat_en;
static int reboot_delay_s = 0;
static int cp_data_to_udisk_en = 0;
static int update_lidbg_out_dir_en = 0;
static int delete_out_dir_after_update = 1;
static int dump_mem_log = 0;
static int loop_warning_en = 0;

#include "system_switch.c"


void lidbg_enable_logcat(void)
{
    char cmd[128] = {0};
    char logcat_file_name[256] = {0};
    char time_buf[32] = {0};

    lidbg("\n\n\nthread_enable_logcat:logcat+\n");

    lidbg_get_current_time(time_buf, NULL);
    sprintf(logcat_file_name, "logcat_%d_%s.txt", get_machine_id(), time_buf);

    sprintf(cmd, "date >/data/%s", logcat_file_name);
    lidbg_shell_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    ssleep(1);
    lidbg_shell_cmd("chmod 777 /data/logcat*");
    ssleep(1);
    sprintf(cmd, "logcat  -v time>> /data/%s &", logcat_file_name);
    lidbg_shell_cmd(cmd);
    lidbg("logcat-\n");

}

void lidbg_enable_kmsg(void)
{
    char cmd[128] = {0};
    char dmesg_file_name[256] = {0};
    char time_buf[32] = {0};

    lidbg("\n\n\nthread_enable_dmesg:kmsg+\n");

    lidbg_trace_msg_disable(1);
    lidbg_get_current_time(time_buf, NULL);
    sprintf(dmesg_file_name, "kmsg_%d_%s.txt", get_machine_id(), time_buf);

    sprintf(cmd, "date >/data/%s", dmesg_file_name);
    lidbg_shell_cmd(cmd);
    memset(cmd, '\0', sizeof(cmd));
    ssleep(1);
    lidbg_shell_cmd("chmod 777 /data/kmsg*");
    ssleep(1);
    sprintf(cmd, "cat /proc/kmsg >> /data/%s &", dmesg_file_name);
    lidbg_shell_cmd(cmd);
    lidbg("kmsg-\n");
}
void cb_password_chmod(char *password )
{
    fs_mem_log("<called:%s>\n", __func__ );

    lidbg_chmod("/system/bin/mount");
    lidbg_mount("/system");
    lidbg_chmod("/data");
}
void cb_password_upload(char *password )
{
    fs_mem_log("<called:%s>\n", __func__ );
    fs_upload_machine_log();
}
void cb_password_clean_all(char *password )
{
    fs_mem_log("<called:%s>\n", __func__ );
    fs_clean_all();
}
void cb_password_update(char *password )
{
    LIDBG_WARN("<===============UPDATE_INFO =================>\n" );
    fs_slient_level = 4;
    analysis_copylist(USB_MOUNT_POINT"/conf/copylist.conf");

    if(fs_is_file_exist(USB_MOUNT_POINT"/out/release"))
    {
        LIDBG_WARN("use:release\n" );
        if( fs_update(USB_MOUNT_POINT"/out/release", USB_MOUNT_POINT"/out", "/flysystem/lib/out") >= 0)
        {
            if(delete_out_dir_after_update)
                lidbg_rmdir(USB_MOUNT_POINT"/out");
            lidbg_launch_user(CHMOD_PATH, "777", "/flysystem/lib/out", "-R", NULL, NULL, NULL);
            lidbg_reboot();
        }
    }
    else
        LIDBG_ERR("<skip>\n" );
}

void update_lidbg_out_dir(char *key, char *value )
{
    cb_password_update(NULL);
}

void cb_password_gui_kmsg(char *password )
{
    if(lidbg_exe("/flysystem/lib/out/lidbg_gui", "/proc/kmsg", "1", NULL, NULL, NULL, NULL) < 0)
        LIDBG_ERR("Exe lidbg_kmsg failed !\n");
}

void cb_password_gui_state(char *password )
{
    if(lidbg_exe("/flysystem/lib/out/lidbg_gui", "/dev/log/state.txt", "1", NULL, NULL, NULL, NULL) < 0)
        LIDBG_ERR("Exe status failed !\n");
}

void cb_password_mem_log(char *password )
{
    lidbg_fifo_get(glidbg_msg_fifo, LIDBG_LOG_DIR"lidbg_mem_log.txt", 0);
}
void cb_int_mem_log(char *key, char *value )
{
    if(dump_mem_log != 0)
        cb_password_mem_log(NULL);
}
int thread_reboot(void *data)
{
    bool volume_find;
    if(!reboot_delay_s)
    {
        lidbg("<reb.exit.%d>\n", reboot_delay_s);
        return 0;
    }
    ssleep(reboot_delay_s);
    volume_find = !!find_mounted_volume_by_mount_point(USB_MOUNT_POINT) ;
    if(volume_find && !te_is_ts_touched())
    {
        lidbg("<lidbg:thread_reboot,call reboot,%d>\n", te_is_ts_touched());
        msleep(100);
        kernel_restart(NULL);
    }
    return 0;
}

void logcat_lunch(char *key, char *value )
{
#ifdef SOC_msm8x25
    k2u_write(LOG_LOGCAT);
#else
    lidbg_enable_logcat();
#endif
}
void cb_cp_data_to_udisk(char *key, char *value )
{
    char shell_cmd[128] = {0}, tbuff[128] = {0};
#ifdef SOC_msm8x25
    fs_cp_data_to_udisk(false);
#else
    ssleep(7);
    lidbg_get_current_time(tbuff, NULL);
    sprintf(shell_cmd, "mkdir "USB_MOUNT_POINT"/ID-%d-%s", get_machine_id() , tbuff);
    lidbg_shell_cmd(shell_cmd);
    sprintf(shell_cmd, "cp -rf "LIDBG_LOG_DIR"* "USB_MOUNT_POINT"/ID-%d-%s", get_machine_id() , tbuff);
    lidbg_shell_cmd(shell_cmd);
    sprintf(shell_cmd, "cp  /data/*.txt "USB_MOUNT_POINT"/ID-%d-%s", get_machine_id() , tbuff);
    lidbg_shell_cmd(shell_cmd);
    sprintf(shell_cmd, "cp  "LIDBG_MEM_DIR"*.txt "USB_MOUNT_POINT"/ID-%d-%s", get_machine_id() , tbuff);
    lidbg_shell_cmd(shell_cmd);
    ssleep(2);
#endif
}
int loop_warnning(void *data)
{
    while(1)
    {
        lidbg_notifier_call_chain(NOTIFIER_VALUE(NOTIFIER_MAJOR_SIGNAL_EVENT, NOTIFIER_MINOR_SIGNAL_BAKLIGHT_ACK));
        msleep(1000);
    }
    return 0;
}

void lidbg_loop_warning(void)
{
    if(loop_warning_en)
    {
        DUMP_FUN;
        CREATE_KTHREAD(loop_warnning, NULL);
    }
}
void cb_kv_app_install(char *key, char *value)
{
    if(value && *value == '1')
        lidbg_pm_install_dir("/mnt/media_rw/udisk/apps");
    else
        fs_mem_log("cb_kv_app_install:fail,%s\n", value);
}
void cb_kv_reboot_recovery(char *key, char *value)
{
    if(value && *value == '1')
        lidbg_shell_cmd("reboot recovery");
    else
        fs_mem_log("cb_kv_reboot_recovery:fail,%s\n", value);
}

void cb_kv_cmd(char *key, char *value)
{
    if(value)
    {
        lidbg_shell_cmd(value);
        fs_mem_log("cb_kv_cmd:%s\n", value);
    }
}

static struct completion udisk_misc_wait;
static int thread_udisk_misc(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    while(!kthread_should_stop())
    {
        if(!wait_for_completion_interruptible(&udisk_misc_wait))
        {
            ssleep(5);

            if(fs_is_file_exist(USB_MOUNT_POINT"/conf/lidbg_udisk_shell.conf"))
            {
                LIST_HEAD(lidbg_udisk_shell_list);
                LIDBG_WARN("use:conf/lidbg_udisk_shell.conf\n" );
                fs_fill_list(USB_MOUNT_POINT"/conf/lidbg_udisk_shell.conf", FS_CMD_FILE_LISTMODE, &lidbg_udisk_shell_list);
                if(analyze_list_cmd(&lidbg_udisk_shell_list))
                    LIDBG_WARN("exe success\n" );
            }

        }
    }
    return 1;
}
static int usb_nb_misc_func(struct notifier_block *nb, unsigned long action, void *data)
{
    switch (action)
    {
    case USB_DEVICE_ADD:
        complete(&udisk_misc_wait);
        break;
    case USB_DEVICE_REMOVE:
        break;
    }
    return NOTIFY_OK;
}
static struct notifier_block usb_nb_misc =
{
    .notifier_call = usb_nb_misc_func,
};

int misc_open (struct inode *inode, struct file *filp)
{
    return 0;
}
ssize_t misc_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    char cmd_buf[512];
    int argc = 0;
    char *argv[32] = {NULL};
    memset(cmd_buf, '\0', 512);

    if(copy_from_user(cmd_buf, buf, size))
    {
        lidbg("copy_from_user ERR\n");
    }
    if(cmd_buf[size - 1] == '\n')
        cmd_buf[size - 1] = '\0';

    argc = lidbg_token_string(cmd_buf, "*", argv);
    if(argc >= 2 && argv[1] != NULL && (!strcmp(argv[0], "apk")))
    {
        lidbg_shell_cmd(argv[1]);
    }
    else
        LIDBG_ERR("%d\n", argc);
    return size;
}

static  struct file_operations misc_nod_fops =
{
    .owner = THIS_MODULE,
    .write = misc_write,
    .open = misc_open,
};
int misc_init(void *data)
{
    LIDBG_WARN("<==IN==>\n");
    init_completion(&udisk_misc_wait);

    system_switch_init();

    te_regist_password("001101", cb_password_upload);
    te_regist_password("001110", cb_password_clean_all);
    te_regist_password("001111", cb_password_chmod);
    te_regist_password("001112", cb_password_update);
    te_regist_password("001120", cb_password_gui_kmsg);
    te_regist_password("001121", cb_password_gui_state);
    te_regist_password("011200", cb_password_mem_log);

    FS_REGISTER_INT(dump_mem_log, "dump_mem_log", 0, cb_int_mem_log);
    FS_REGISTER_INT(logcat_en, "logcat_en", 0, logcat_lunch);
    FS_REGISTER_INT(reboot_delay_s, "reboot_delay_s", 0, NULL);
    FS_REGISTER_INT(cp_data_to_udisk_en, "cp_data_to_udisk_en", 0, cb_cp_data_to_udisk);
    FS_REGISTER_INT(update_lidbg_out_dir_en, "update_lidbg_out_dir_en", 0, update_lidbg_out_dir);
    FS_REGISTER_INT(delete_out_dir_after_update, "delete_out_dir_after_update", 0, NULL);
    FS_REGISTER_INT(loop_warning_en, "loop_warning_en", 0, NULL);

    FS_REGISTER_KEY( "cmdstring", cb_kv_cmd);
    FS_REGISTER_KEY( "app_install_en", cb_kv_app_install);
    FS_REGISTER_KEY( "reboot_recovery", cb_kv_reboot_recovery);
    FS_REGISTER_KEY( "lidbg_origin_system", cb_kv_lidbg_origin_system);

    fs_register_filename_list("/data/kmsg.txt", true);
    fs_register_filename_list("/data/top.txt", true);
    fs_register_filename_list("/data/ps.txt", true);
    fs_register_filename_list("/data/df.txt", true);
    fs_register_filename_list("/data/machine.txt", true);
    fs_register_filename_list("/data/dumpsys.txt", true);
    fs_register_filename_list("/data/screenshot.png", true);
    fs_register_filename_list(LIDBG_LOG_DIR"lidbg_mem_log.txt", true);

    CREATE_KTHREAD(thread_reboot, NULL);

    CREATE_KTHREAD(thread_udisk_misc, NULL);
    usb_register_notify(&usb_nb_misc);

    LIDBG_WARN("<==OUT==>\n\n");
    LIDBG_MODULE_LOG;

    if(1 == logcat_en)
        logcat_lunch(NULL, NULL);
    lidbg_new_cdev(&misc_nod_fops, "lidbg_misc");
    return 0;
}


static int __init lidbg_misc_init(void)
{
    DUMP_FUN;
    LIDBG_GET;
    CREATE_KTHREAD(misc_init, NULL);
    return 0;
}

static void __exit lidbg_misc_exit(void)
{
}

module_init(lidbg_misc_init);
module_exit(lidbg_misc_exit);

EXPORT_SYMBOL(lidbg_loop_warning);
EXPORT_SYMBOL(lidbg_enable_logcat);
EXPORT_SYMBOL(lidbg_enable_kmsg);
EXPORT_SYMBOL(lidbg_system_switch);


MODULE_AUTHOR("futengfei");
MODULE_DESCRIPTION("misc zone");
MODULE_LICENSE("GPL");


