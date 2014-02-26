#include "lidbg.h"

static struct task_struct *reboot_task;
static int logcat_en;
static int reboot_delay_s = 0;
static int cp_data_to_udisk_en = 0;
static int update_lidbg_out_dir_en = 0;
static int delete_out_dir_after_update = 1;
static int dump_mem_log = 0;
static int loop_warning_en = 0;

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
void cb_password_call_apk(char *password )
{
    fs_mem_log("<called:%s>\n", __func__ );
    fs_call_apk();
}
void cb_password_remove_apk(char *password )
{
    fs_mem_log("<called:%s>\n", __func__ );
    fs_remove_apk();
}
void cb_password_clean_all(char *password )
{
    fs_mem_log("<called:%s>\n", __func__ );
    fs_clean_all();
}
void cb_password_update(char *password )
{
    analysis_copylist("/mnt/usbdisk/conf/copylist.conf");

    if(fs_is_file_exist("/mnt/usbdisk/out/release"))
    {
        TE_WARN("<===============UPDATE_INFO =================>\n" );
        if( fs_update("/mnt/usbdisk/out/release", "/mnt/usbdisk/out", "/flysystem/lib/out") >= 0)
        {
            if(delete_out_dir_after_update)
                lidbg_rmdir("/mnt/usbdisk/out");

            //lidbg_chmod("/flysystem/lib/out");
            lidbg_launch_user(CHMOD_PATH, "777", "/flysystem/lib/out", "-R", NULL, NULL, NULL);
            lidbg_reboot();
        }
    }
    else
    {
        TE_ERR("<up>\n" );
    }
}
void update_lidbg_out_dir(char *key, char *value )
{
    cb_password_update(NULL);
}

void cb_password_gui_kmsg(char *password )
{
    if(lidbg_exe("/flysystem/lib/out/lidbg_gui", "/proc/kmsg", "1", NULL, NULL, NULL, NULL) < 0)
        TE_ERR("Exe lidbg_kmsg failed !\n");
}

void cb_password_gui_state(char *password )
{
    if(lidbg_exe("/flysystem/lib/out/lidbg_gui", "/dev/log/state.txt", "1", NULL, NULL, NULL, NULL) < 0)
        TE_ERR("Exe status failed !\n");
}

void cb_password_mem_log(char *password )
{
    lidbg_msg_get(LIDBG_LOG_DIR"lidbg_mem_log.txt", 0);
}
void cb_int_mem_log(char *key, char *value )
{
    cb_password_mem_log(NULL);
}
static int thread_reboot(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);

    ssleep(reboot_delay_s);
    if(!te_is_ts_touched())
        lidbg_reboot();
    else
        lidbg("<reb.exit:%d,%d>\n", reboot_delay_s, te_is_ts_touched());
    return 1;
}

void logcat_lunch(char *key, char *value )
{
    k2u_write(LOG_LOGCAT);
}

void cp_data_to_udisk(char *key, char *value )
{
    struct list_head *client_list = &fs_filename_list;
    if(!list_empty(client_list))
    {
        int index = 0;
        char dir[128], tbuff[128];
        int copy_delay = 300;
        struct fs_filename_item *pos;

        memset(dir, '\0', sizeof(dir));
        memset(tbuff, '\0', sizeof(tbuff));

        lidbg_get_current_time(tbuff, NULL);
        sprintf(dir, "/mnt/usbdisk/ID%d-%s", get_machine_id(), tbuff);
        lidbg_mkdir(dir);
        msleep(1000);

        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (pos->filename && pos->copy_en)
            {
                char *file = strrchr(pos->filename, '/');
                if(file)
                {
                    index++;
                    memset(tbuff, '\0', sizeof(tbuff));
                    sprintf(tbuff, "%s/%s", dir, ++file);
                    fs_copy_file(pos->filename, tbuff);
                    msleep(copy_delay);
                }
            }
        }
        lidbg_rm("/data/kmsg.txt");
        lidbg_domineering_ack();
    }
    else
        LIDBG_ERR("<nobody_register>\n");
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

char *cmd_string[] =
{
    "chmod",
    "rmdir",
    "mount",
    "echo",
};
int parse_cmd_type(char *what)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(cmd_string); i++)
    {
        if (!strcmp(what, cmd_string[i]))
            return i + 1;
    }
    return 0;
}
void cb_kv_cmd(char *key, char *value)
{
    if(value)
    {
        char *param[8];
        int num = lidbg_token_string(value, ",", param) ;
        if(num > 1)
        {
            switch (parse_cmd_type(param[0]))
            {
            case  1:
                lidbg_chmod(param[1]);
                break;
            case  2:
                lidbg_rmdir(param[1]);
                break;
            case  3:
                lidbg_mount(param[1]);
                break;
            case  4:
                if(num >= 3)
                    fs_readwrite_file(param[2], param[1], NULL, 0);
                else
                    fs_mem_log("echo err:%d", num);
                break;

            default:
                break;
            }
        }
        fs_mem_log("test1:%s\n", value);
    }
}
int misc_init(void *data)
{
    TE_WARN("<==IN==>\n");

    te_regist_password("001100", cb_password_remove_apk);
    te_regist_password("001101", cb_password_upload);
    te_regist_password("001102", cb_password_call_apk);
    te_regist_password("001110", cb_password_clean_all);
    te_regist_password("001111", cb_password_chmod);
    te_regist_password("001112", cb_password_update);
    te_regist_password("001120", cb_password_gui_kmsg);
    te_regist_password("001121", cb_password_gui_state);
    te_regist_password("011200", cb_password_mem_log);

    FS_REGISTER_INT(dump_mem_log, "dump_mem_log", 0, cb_int_mem_log);
    FS_REGISTER_INT(logcat_en, "logcat_en", 0, logcat_lunch);
    FS_REGISTER_INT(reboot_delay_s, "reboot_delay_s", 0, NULL);
    FS_REGISTER_INT(cp_data_to_udisk_en, "cp_data_to_udisk_en", 0, cp_data_to_udisk);
    FS_REGISTER_INT(update_lidbg_out_dir_en, "update_lidbg_out_dir_en", 0, update_lidbg_out_dir);
    FS_REGISTER_INT(delete_out_dir_after_update, "delete_out_dir_after_update", 0, NULL);
    FS_REGISTER_INT(loop_warning_en, "loop_warning_en", 0, NULL);

    FS_REGISTER_KEY( "cmdstring", cb_kv_cmd);

    fs_register_filename_list("/data/kmsg.txt", true);
    fs_register_filename_list(LIDBG_LOG_DIR"lidbg_mem_log.txt", true);

    if(1 == logcat_en)
        logcat_lunch(NULL, NULL);

    if(reboot_delay_s)
        reboot_task = kthread_run(thread_reboot, NULL, "ftf_misc_reb");
    else
        lidbg("<reb.exit.%d>\n", reboot_delay_s);

    TE_WARN("<==OUT==>\n\n");

    return 0;


}


static int __init lidbg_misc_init(void)
{

    DUMP_FUN;
    CREATE_KTHREAD(misc_init, NULL);
    return 0;
}

static void __exit lidbg_misc_exit(void)
{
}

module_init(lidbg_misc_init);
module_exit(lidbg_misc_exit);

EXPORT_SYMBOL(lidbg_loop_warning);


MODULE_AUTHOR("futengfei");
MODULE_DESCRIPTION("misc zone");
MODULE_LICENSE("GPL");

