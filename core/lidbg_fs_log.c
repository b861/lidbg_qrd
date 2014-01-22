
#include "lidbg.h"


//zone below [fs.log.tools]
int max_file_len = 1;
int g_iskmsg_ready = 1;
int g_pollkmsg_en = 0;
static struct task_struct *fs_kmsgtask;
static struct completion kmsg_wait;
//zone end


//zone below [fs.log.driver]
bool upload_machine_log(void)
{
    if(lidbg_exe("/flysystem/lib/out/client_mobile", NULL, NULL, NULL, NULL, NULL, NULL) < 0)
        lidbg_exe("/system/lib/modules/out/client_mobile", NULL, NULL, NULL, NULL, NULL, NULL);
    return true;
}
void remount_system(void)
{
	static int g_is_remountd_system = 0;
    if(!g_is_remountd_system)
    {
        g_is_remountd_system = 1;
        lidbg_chmod("/system/bin/mount");
        lidbg_mount("/system");
        lidbg_mount("/flysystem");
        msleep(200);
    }
}
void chmod_for_apk(void)
{
    remount_system();
    lidbg_chmod("/system/bin/mount");
    lidbg_mount("/system");
    lidbg_chmod("/data");
    lidbg_chmod("/system/app");
    lidbg_chmod("/flysystem/lib/out/fileserver.apk");
    lidbg_chmod("/system/lib/modules/out/fileserver.apk");
}
void call_apk(void)
{
    chmod_for_apk();
    if(!fs_copy_file("/flysystem/lib/out/fileserver.apk", "/system/app/fileserver.apk"))
        fs_copy_file("/system/lib/modules/out/fileserver.apk", "/system/app/fileserver.apk");
}
int bfs_file_amend(char *file2amend, char *str_append)
{
    struct file *filep;
    struct inode *inode = NULL;
    mm_segment_t old_fs;
    int  flags, is_file_cleard = 0;
    unsigned int file_len;

    if(str_append == NULL)
    {
        printk("[futengfei]err.fileappend_mode:<str_append=null>\n");
        return -1;
    }
    flags = O_CREAT | O_RDWR | O_APPEND;

again:
    filep = filp_open(file2amend, flags , 0777);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err.open:<%s>\n", file2amend);
        return -1;
    }

    old_fs = get_fs();
    set_fs(get_ds());

    inode = filep->f_dentry->d_inode;
    file_len = inode->i_size;
    file_len = file_len + 1;


    if(file_len > max_file_len * MEM_SIZE_1_MB)
    {
        printk("[futengfei]warn.fileappend_mode:< file>8M.goto.again >\n");
        is_file_cleard = 1;
        flags = O_CREAT | O_RDWR | O_APPEND | O_TRUNC;
        set_fs(old_fs);
        filp_close(filep, 0);
        goto again;
    }
    filep->f_op->llseek(filep, 0, SEEK_END);//note:to the end to append;
    if(1 == is_file_cleard)
    {
        char *str_warn = "============have_cleard=============\n\n";
        is_file_cleard = 0;
        filep->f_op->write(filep, str_warn, strlen(str_warn), &filep->f_pos);
    }
    filep->f_op->write(filep, str_append, strlen(str_append), &filep->f_pos);
    set_fs(old_fs);
    filp_close(filep, 0);
    return 1;
}
void file_separator(char *file2separator)
{
    char buf[32];
    lidbg_get_current_time(buf, NULL);
    fs_string2file(file2separator, "------%s------\n", buf);
}
int dump_kmsg(char *node, char *save_msg_file, int size, int *always)
{
    struct file *filep;
    mm_segment_t old_fs;

    int  ret = -1;
    if(!size && !always)
    {
        FS_ERR("<size_k=null&&always=null>\n");
        return -1;
    }
    filep = filp_open(node,  O_RDONLY , 0);
    if(!IS_ERR(filep))
    {
        old_fs = get_fs();
        set_fs(get_ds());

        if(size)
        {
            char *psize = NULL;
            psize = (unsigned char *)vmalloc(size);
            if(psize == NULL)
            {
                FS_ERR("<cannot malloc memory!>\n");
                return ret;
            }

            if(g_mem_dbg)
                fs_string2file(DEBUG_MEM_FILE, "free.%s=%d \n", __func__, size);

            ret = filep->f_op->read(filep, psize, size - 1, &filep->f_pos);
            if(ret > 0)
            {
                psize[ret] = '\0';
                bfs_file_amend(save_msg_file, psize);
            }
            vfree(psize);
        }
        else
        {
            char buff[512];
            memset(buff, 0, sizeof(buff));
            while( (!always ? 0 : *always) )
            {
                ret = filep->f_op->read(filep, buff, 512 - 1, &filep->f_pos);
                if(ret > 0)
                {
                    buff[ret] = '\0';
                    bfs_file_amend(save_msg_file, buff);
                }
            }
        }

        set_fs(old_fs);
        filp_close(filep, 0);
    }
    return ret;
}
static int thread_pollkmsg_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    while(!kthread_should_stop())
    {
        if( !wait_for_completion_interruptible(&kmsg_wait))
            dump_kmsg(KMSG_NODE, LIDBG_KMSG_FILE_PATH, 0, &g_pollkmsg_en);
    }
    return 1;
}
void cb_kv_pollkmsg(char *key, char *value)
{
    FS_WARN("<%s=%s>\n", key, value);
    if ( (!strcmp(key, "fs_kmsg_en" ))  &&  (strcmp(value, "0" )) )
        complete(&kmsg_wait);
}
//zone end


//zone below [fs.log.interface]
void fs_file_separator(char *file2separator)
{
    file_separator(file2separator);
}
int fs_dump_kmsg(char *tag, int size )
{
    file_separator(LIDBG_KMSG_FILE_PATH);
    if(tag != NULL)
        fs_string2file(LIDBG_KMSG_FILE_PATH, "fs_dump_kmsg: %s\n", tag);
    return dump_kmsg(KMSG_NODE, LIDBG_KMSG_FILE_PATH, size, NULL);
}
void fs_enable_kmsg( bool enable )
{
    if(enable)
    {
        g_pollkmsg_en = 1;
        if(g_iskmsg_ready)
            complete(&kmsg_wait);
    }
    else
        g_pollkmsg_en = 0;
}
int fs_string2file(char *filename, const char *fmt, ... )
{
    va_list args;
    int n;
    char str_append[256];
    va_start ( args, fmt );
    n = vsprintf ( str_append, (const char *)fmt, args );
    va_end ( args );

    return bfs_file_amend(filename, str_append);
}
int fs_mem_log( const char *fmt, ... )
{
    int len;
    va_list args;
    int n;
    char str_append[256];
    va_start ( args, fmt );
    n = vsprintf ( str_append, (const char *)fmt, args );
    va_end ( args );

    len = strlen(str_append);

    bfs_file_amend(LIDBG_MEM_LOG_FILE, str_append);

    return 1;
}
bool fs_upload_machine_log(void)
{
    lidbg_rm("/dev/log/mobile.txt");
    remount_system();
    return upload_machine_log();
}
int thread_call_apk_init(void *data)
{
    call_apk();
    return 0;
}
void fs_call_apk(void)
{
    CREATE_KTHREAD(thread_call_apk_init, NULL);
}
void fs_remove_apk(void)
{
    lidbg_rm("/system/app/fileserver.apk");
}
void fs_remount_system(void)
{
    remount_system();
}
//zone end



void lidbg_fs_log_init(void)
{

    init_completion(&kmsg_wait);

    FS_REGISTER_INT(g_pollkmsg_en, "fs_kmsg_en", 0, cb_kv_pollkmsg);
    if(g_pollkmsg_en == 1)
        complete(&kmsg_wait);
    FS_REGISTER_INT(max_file_len, "fs_max_file_len", 1, NULL);

    fs_kmsgtask = kthread_run(thread_pollkmsg_func, NULL, "ftf_kmsgtask");
}

EXPORT_SYMBOL(fs_file_separator);
EXPORT_SYMBOL(fs_dump_kmsg);
EXPORT_SYMBOL(fs_enable_kmsg);
EXPORT_SYMBOL(fs_string2file);
EXPORT_SYMBOL(fs_mem_log);
EXPORT_SYMBOL(fs_upload_machine_log);
EXPORT_SYMBOL(fs_call_apk);
EXPORT_SYMBOL(fs_remove_apk);
EXPORT_SYMBOL(fs_remount_system);
EXPORT_SYMBOL(bfs_file_amend);

