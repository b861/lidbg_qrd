
#include "lidbg.h"

//zone below [tools]
#define FS_VERSION "FS.VERSION:  [20131031]"
LIST_HEAD(kill_list_test);
static struct task_struct *fileserver_test_task;
static struct task_struct *fileserver_test_task2;
static struct task_struct *fileserver_test_task3;
bool is_out_updated = false;
bool is_fs_work_enable = false;
int g_mem_dbg = 0;
static int test_count = 0;
//zone end


//zone below [fileserver_test]
void test_fileserver_stability(void)
{
    char *delay, tbuff[100];
    int ret = 0;

    //test_fileserver_stability
    fs_find_string(&kill_list_test, "cn.flyaudio.navigation");
    fs_find_string(&kill_list_test, "cn.flyaudio.navigationfutengfei");
    fs_get_intvalue(&lidbg_drivers_list, "futengfei", &ret, NULL);
    lidbg("[futengfei]get key value:[%d]\n", ret );
    fs_get_intvalue(&lidbg_drivers_list, "mayanping", &ret, NULL);
    lidbg("[futengfei]get key value:[%d]\n", ret );
    fs_get_intvalue(&lidbg_core_list, "fs_updatestate_ms", &ret, NULL);
    lidbg("[futengfei]get key value:[%d]\n", ret );

    delay = "futengfei1";
    //fs_set_value(&lidbg_core_list, "fs_private_patch", delay);
    //fs_get_value(&lidbg_core_list, "fs_private_patch", &value);
    //lidbg("[futengfei]warn.test_fileserver_stability:<value=%s>\n", value);
    lidbg_get_current_time(tbuff, NULL);
    fs_mem_log("%s\n", tbuff);

    fs_string2file(LIDBG_LOG_DIR"fs_string2file.txt", "%s\n", tbuff);

    test_count++;
    if(0)
    {
        sprintf(tbuff, "/mnt/sdcard/lidbg/lidbg_c%d.txt", test_count);
        fs_copy_file("/system/lib/modules/out/core.conf", tbuff);
        sprintf(tbuff, "/mnt/sdcard/lidbg/lidbg_d%d.txt", test_count);
        fs_copy_file("/system/lib/modules/out/drivers.conf", tbuff);
    }
    if(0)
    {
        fs_show_list(&kill_list_test);
        fs_show_list(&lidbg_drivers_list);
        fs_show_list(&kill_list_test);
        fs_show_list(&lidbg_drivers_list);
    }
}
static int thread_fileserver_test(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    fs_regist_state("ats", &test_count);
    while(!kthread_should_stop())
    {
        test_fileserver_stability();
        ssleep(1);
        //fs_save_list_to_file();
    };
    return 1;
}
void fileserver_thread_test(int zero_return)
{

    if(fs_fill_list("/flysystem/lib/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &kill_list_test) < 0)
        fs_fill_list("/system/lib/modules/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &kill_list_test);
    switch (zero_return)
    {
    case 3:
        lidbg("[futengfei]======start_kthread_run1.2.3\n");
        fileserver_test_task3 = kthread_run(thread_fileserver_test, NULL, "ftf_fs_task3");
        msleep(88);
    case 2:
        lidbg("[futengfei]======start_kthread_run1.2\n");
        fileserver_test_task2 = kthread_run(thread_fileserver_test, NULL, "ftf_fs_task2");
        msleep(230);
    case 1:
        lidbg("[futengfei]======start_kthread_run1\n");
        fileserver_test_task = kthread_run(thread_fileserver_test, NULL, "ftf_fs_task");
        break;
    case 0:
        lidbg("[futengfei]======stop_kthread_run1.2.3\n");
        if (fileserver_test_task)
            kthread_stop(fileserver_test_task);
        if (fileserver_test_task2)
            kthread_stop(fileserver_test_task2);
        if (fileserver_test_task3)
            kthread_stop(fileserver_test_task3);
        fileserver_test_task = NULL;
        fileserver_test_task2 = NULL;
        fileserver_test_task3 = NULL;
        break;
    default:
        lidbg("[futengfei]======thread_count:too much\n");
    }
}
//zone end

void lidbg_fileserver_main(int argc, char **argv)
{
    int cmd = 0;
    int cmd_para = 0;
    int thread_count = 0;
    if(argc < 3)
    {
        lidbg("[futengfei]err.lidbg_fileserver_main:echo \"c file 1 1 1\" > /dev/mlidbg0\n");
        return;
    }

    thread_count = simple_strtoul(argv[0], 0, 0);
    cmd = simple_strtoul(argv[1], 0, 0);
    cmd_para = simple_strtoul(argv[2], 0, 0);

    if(thread_count)
        fileserver_thread_test(thread_count);

    switch (cmd)
    {
    case 1:
        break;
    case 2:
        fs_enable_kmsg(cmd_para);
        break;
    case 3:
        fs_dump_kmsg((char *)__FUNCTION__, cmd_para * 1024);
        break;
    case 4:
        FS_WARN("machine_id:%d\n", get_machine_id());
        break;
    case 5:
        fs_save_list_to_file();
        break;
    case 6:
        bfs_file_amend(LIDBG_MEM_LOG_FILE, "<chmod_for_apk>\n");
        chmod_for_apk();
        break;
    case 7:
        bfs_file_amend(LIDBG_MEM_LOG_FILE, "<fs_upload_machine_log>\n");
        fs_upload_machine_log();
        break;
    case 8:
        bfs_file_amend(LIDBG_MEM_LOG_FILE, "<fs_clean_all>\n");
        fs_clean_all();
        break;
    case 9:
        FS_WARN("<lidbg_mount>\n");
        lidbg_mount("/system");
        break;
    case 10:
        FS_WARN("<fs_show_filename_list>\n");
        fs_show_filename_list();
        break;
    default:
        FS_ERR("<check you cmd:%d>\n", cmd);
        break;
    }
}


//zone below [fileserver]
bool is_fly_system(void)
{
    if(fs_is_file_exist(build_time_fly_path))
        return true;
    else
        return false;
}
void copy_all_conf_file(void)
{
    if(is_fly_system())
    {
        fs_copy_file(build_time_fly_path, build_time_sd_path);
        fs_copy_file(core_fly_path, core_sd_path);
        fs_copy_file(driver_fly_path, driver_sd_path) ;
        fs_copy_file(state_fly_path, state_sd_path);
        fs_copy_file(cmd_fly_path, cmd_sd_path);
    }
    else
    {
        fs_copy_file(build_time_lidbg_path, build_time_sd_path);
        fs_copy_file(core_lidbg_path, core_sd_path);
        fs_copy_file(driver_lidbg_path, driver_sd_path);
        fs_copy_file(state_lidbg_path, state_sd_path);
        fs_copy_file(cmd_lidbg_path, cmd_sd_path);
    }
}
void check_conf_file(void)
{
    int size[6];

    //tmp:test
    size[0] = fs_is_file_updated(build_time_fly_path, PRE_CONF_INFO_FILE);
    size[1] = fs_get_file_size(driver_sd_path);
    size[2] = fs_get_file_size(core_sd_path);
    size[3] = fs_get_file_size(state_sd_path);
    size[4] = fs_get_file_size(PRE_CONF_INFO_FILE);
    fs_string2file(LIDBG_MEM_LOG_FILE, "<check_conf_file:%d,%d,%d,%d,%d>\n", size[0], size[1], size[2], size[3], size[4]);

    if(size[0] || size[1] < 1 || size[2] < 1 || size[3] < 1 || size[4] < 1)
    {
        fs_remount_system();
        FS_WARN("<overwrite:push,update?>\n");
        fs_string2file(LIDBG_MEM_LOG_FILE, "<overwrite:push,update?>\n");
        is_out_updated = true;
        analysis_copylist("/flysystem/lib/out/copylist.conf");
        copy_all_conf_file();
        lidbg_rm("/data/kmsg.txt");
    }

}


static int get_parameters(char *buf, char **param1)
{
    char *token;
    int pos = 0;
    while((token = strsep(&buf, " ")) != NULL )
    {
        *param1 = token;
        param1++;
        pos++;
    }
    return pos;
}
int fs_nod_open (struct inode *inode, struct file *filp)
{
    return 0;
}
ssize_t fs_nod_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int pos = 0;
    char *tmp, *param[20];
    tmp = memdup_user(buf, count);
    if (IS_ERR(tmp))
    {
        FS_ERR("<memdup_user>\n");
        return PTR_ERR(tmp);
    }

    pos = get_parameters(tmp, param);

    if(pos < 5)
    {
        FS_ERR("echo \"c file tw 1 1\" > /dev/fs_node0\n");
        goto out;
    }
    //zone start [dosomething]
    if(!strcmp(param[2], "tw") )
    {
    }
    //zone end
out:
    kfree(tmp);
    return count;
}

static  struct file_operations fs_nod_fops =
{
    .owner = THIS_MODULE,
    .write = fs_nod_write,
    .open = fs_nod_open,
};
void lidbg_fileserver_main_prepare(void)
{

    FS_WARN("<%s>\n", FS_VERSION);

    lidbg_mkdir(LIDBG_LOG_DIR);
    lidbg_mkdir(LIDBG_OSD_DIR);

    check_conf_file();

    set_machine_id();
    FS_WARN("machine_id:%d\n", get_machine_id());

    fs_fill_list(driver_sd_path, FS_CMD_FILE_CONFIGMODE, &lidbg_drivers_list);
    fs_fill_list(core_sd_path, FS_CMD_FILE_CONFIGMODE, &lidbg_core_list);
    fs_fill_list(state_sd_path, FS_CMD_FILE_CONFIGMODE, &fs_state_list);

    fs_copy_file(build_time_fly_path, LIDBG_MEM_DIR"build_time.txt");

    if(0)
        lidbg_new_cdev(&fs_nod_fops, "fs_node");
}
void lidbg_fileserver_main_init(void)
{
    fs_get_intvalue(&lidbg_core_list, "fs_mem_dbg", &g_mem_dbg, NULL);
    fs_string2file(LIDBG_MEM_LOG_FILE, "%s\n", FS_VERSION );
    fs_register_filename_list(LIDBG_MEM_LOG_FILE, true);
    fs_register_filename_list(build_time_sd_path, true);
    fs_register_filename_list(LIDBG_KMSG_FILE_PATH, true);
    fs_register_filename_list(MACHINE_ID_FILE, true);
    fs_register_filename_list(driver_sd_path, true);
    fs_register_filename_list(core_sd_path, true);
}
//zone end

static int __init lidbg_fileserver_init(void)
{
    FS_WARN("<==IN==>\n");

    lidbg_fileserver_main_prepare();

    lidbg_fs_keyvalue_init();
    lidbg_fs_log_init();
    lidbg_fs_update_init();
    lidbg_fs_conf_init();
    lidbg_fs_cmn_init();

    lidbg_fileserver_main_init();//note,put it in the end.

    FS_WARN("<==OUT==>\n\n");
    return 0;
}

static void __exit lidbg_fileserver_exit(void)
{
}


EXPORT_SYMBOL(lidbg_fileserver_main);
EXPORT_SYMBOL(is_out_updated);
EXPORT_SYMBOL(is_fs_work_enable);


module_init(lidbg_fileserver_init);
module_exit(lidbg_fileserver_exit);

MODULE_AUTHOR("futengfei");
MODULE_DESCRIPTION("fileserver.entry");
MODULE_LICENSE("GPL");

