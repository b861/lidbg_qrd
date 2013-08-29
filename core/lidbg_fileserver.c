
#include "lidbg.h"


//zone below [tools]
#define LIDBG_LOG_FILE_PATH "/mnt/sdcard/lidbg_log.txt"
#define LIDBG_STATE_FILE_PATH "/mnt/sdcard/lidbg_state.txt"
#define FIFO_SIZE (1024)
static struct kfifo log_fifo;
spinlock_t		fs_lock;
unsigned long flags;
static const char *driver_sd_path = "/mnt/sdcard/drivers.conf";
static const char *driver_fly_path = "/flysystem/lib/out/drivers.conf";
static const char *driver_lidbg_path = "/system/lib/modules/out/drivers.conf";
static const char *core_sd_path = "/mnt/sdcard/core.conf";
static const char *core_fly_path = "/flysystem/lib/out/core.conf";
static const char *core_lidbg_path = "/system/lib/modules/out/core.conf";
static struct task_struct *filelog_task;
static struct task_struct *filepoll_task;
struct rtc_time precorefile_tm;
struct rtc_time predriverfile_tm;
static int g_dubug_on = 0;
static int g_clearlogfifo_ms = 30000;
static int g_pollfile_ms = 10000;
unsigned char log_buffer[FIFO_SIZE];
unsigned char log_buffer2write[FIFO_SIZE];
LIST_HEAD(lidbg_drivers_list);
LIST_HEAD(lidbg_core_list);
LIST_HEAD(kill_list_test);//for test
static struct task_struct *fileserver_test_task;
static struct task_struct *fileserver_test_task2;
static struct task_struct *fileserver_test_task3;
//zone end

//zone below [interface]
int fileserver_deal_cmd(struct list_head *client_list, enum string_dev_cmd cmd, char *lookfor, char *key, char **string, int *int_value, void (*callback)(char *key, char *value ));
int fileserver_main(char *filename, enum string_dev_cmd cmd, char *str_append, struct list_head *client_list);
bool copy_file(char *from, char *to);

int fs_get_intvalue(struct list_head *client_list, char *key, int *int_value, void (*callback)(char *key, char *value))
{
    char *string;
    return  fileserver_deal_cmd(client_list, FS_CMD_LIST_GETVALUE, NULL, key, &string, int_value, callback);
}
int fs_get_value(struct list_head *client_list, char *key, char **string)
{
    return fileserver_deal_cmd( client_list, FS_CMD_LIST_GETVALUE, NULL, key, string, NULL, NULL);
}
int fs_set_value(struct list_head *client_list, char *key, char *string)
{
    return fileserver_deal_cmd( client_list, FS_CMD_LIST_SETVALUE, NULL, key, &string, NULL, NULL);
}
int fs_find_string(struct list_head *client_list, char *string)
{
    return fileserver_deal_cmd(client_list, FS_CMD_LIST_IS_STRINFILE, string, NULL, NULL, NULL, NULL);
}
int fs_show_list(struct list_head *client_list)
{
    return fileserver_deal_cmd(client_list, FS_CMD_LIST_SHOW, NULL, NULL, NULL, NULL, NULL);
}

void clearfifo_tofile(void)
{
    int fifo_len;
    spin_lock_irqsave(&fs_lock, flags);
    fifo_len = kfifo_len(&log_fifo);
    kfifo_out(&log_fifo, log_buffer2write, fifo_len);
    log_buffer2write[fifo_len > FIFO_SIZE - 1 ? FIFO_SIZE - 1 : fifo_len] = '\0';
    spin_unlock_irqrestore(&fs_lock, flags);
    fileserver_main(NULL, FS_CMD_FILE_APPENDMODE, log_buffer2write, NULL);
}
int fs_file_log( const char *fmt, ... )
{
    int len;
    va_list args;
    int n;
    char str_append[256];
    va_start ( args, fmt );
    n = vsprintf ( str_append, (const char *)fmt, args );
    va_end ( args );

    len = strlen(str_append);

    if(kfifo_is_full(&log_fifo) || kfifo_avail(&log_fifo) < len)
    {
        clearfifo_tofile();
    }
    spin_lock_irqsave(&fs_lock, flags);
    kfifo_in(&log_fifo, str_append, len);
    spin_unlock_irqrestore(&fs_lock, flags);
    return 1;
}
void fs_log_sync(void)
{
    clearfifo_tofile();
}
int fs_fill_list(char *filename, enum string_dev_cmd cmd, struct list_head *client_list)
{
    return fileserver_main(filename, cmd, NULL, client_list);
}
bool fs_copy_file(char *from, char *to)
{
    return copy_file(from, to);
}
//zone end


//zone below [fileserver]
int fileserver_deal_cmd(struct list_head *client_list, enum string_dev_cmd cmd, char *lookfor, char *key, char **string, int *int_value, void (*callback)(char *key, char *value))
{
    //note:you can add more func here,just copy one of the case as belows;
    struct string_dev *pos;
    char *p;
    if(g_dubug_on)
        printk("\n[futengfei]======fileserver_deal_cmd[%d]\n", cmd);
    if (list_empty(client_list))
    {
        printk("[futengfei]err.fileserver_deal_cmd:<list_is_empty>\n");
        return -2;
    }
    switch (cmd)
    {
    case FS_CMD_LIST_SPLITKV:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            p = memchr(pos->yourkey, '=', strlen(pos->yourkey));
            if(p != NULL)
            {
                pos->yourvalue = p + 1;
                *p = '\0';
            }
        }
        break;

    case FS_CMD_LIST_SHOW:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if(pos->yourvalue)
                printk("[%s]=[%s]\n", pos->yourkey, pos->yourvalue);
            else
                printk("[%s]\n", pos->yourkey);
        }
        break;

    case FS_CMD_LIST_GETVALUE:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (!strcmp(pos->yourkey, key))
            {
                *string = pos->yourvalue;
                if(int_value)
                {
                    pos->int_value = int_value;
                    *int_value = simple_strtoul(pos->yourvalue, 0, 0);
                }
                if(callback)
                    pos->callback = callback;
                printk("[futengfei]succeed.find_key:<%s=%s>\n", key, *string);
                return 1;
            }
        }
        printk("[futengfei]err.fail_find:<%s>\n", key);
        return -1;

    case FS_CMD_LIST_SETVALUE:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if ( (!strcmp(pos->yourkey, key))  &&  (strcmp(pos->yourvalue, *string)) )
            {
                p = *string;
                pos->yourvalue = kmalloc(strlen(p) + 1, GFP_KERNEL);
                if (pos->yourvalue  != NULL)
                {
                    strcpy(pos->yourvalue, p);
                    printk("[futengfei]succeed.set_key:<%s=%s>\n", key, pos->yourvalue);
                }
                if (pos->int_value)
                {
                    *(pos->int_value) = simple_strtoul(p, 0, 0);
                    printk("[futengfei]succeed.set_keyvalue:<%s=%d>\n", key, *(pos->int_value));
                }
                if (pos->callback)
                {
                    pos->callback(key, pos->yourvalue);
                    printk("[futengfei]succeed.pos->callback();\n");
                }
                return 1;
            }
        }
        if(g_dubug_on)
            printk("[futengfei]err.set_key:<%s>\n", key);
        return -1;
    case FS_CMD_LIST_IS_STRINFILE:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            int len = strlen(lookfor);
            if(p = memchr(pos->yourkey, lookfor[0], strlen(pos->yourkey)))
            {
                //note:you can use [strncmp()] to replace the code below;
                //I want to study the workflow;
                while(len)
                {
                    if( p[len - 1] == lookfor[len - 1])
                    {
                        if(g_dubug_on)
                            printk("[futengfei]======%d[%c][%c]\n", len, p[len - 1], lookfor[len - 1]);
                        len--;
                        if(len == 0)
                        {
                            printk("[futengfei]succeed.have_find:<%s>\n", lookfor);
                            return 1;
                        }
                    }
                    else
                        break;
                }
            }
        }
        printk("[futengfei]err.fail_find:<%s>\n", lookfor);
        return -1;
    default:
        printk("\n[futengfei]err.fileserver_deal_cmd:<unknown.cmd:%d\n", cmd);
        return -1;
    }
    return 1;
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
    //printk("[futengfei]warn.check:<fileappend_mode>\n");
    flags = O_CREAT | O_RDWR | O_APPEND;

again:
    filep = filp_open(file2amend, flags , 0600);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err.open:<%s>\n", file2amend);
        return -1;
    }
    //printk("[futengfei]succeed.open:<%s>\n", file2amend);

    old_fs = get_fs();
    set_fs(get_ds());

    inode = filep->f_dentry->d_inode;
    file_len = inode->i_size;
    // printk("[futengfei]warn.File_length:<%d>\n", file_len);
    file_len = file_len + 2;


    if(file_len > MEM_SIZE_8_MB)
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
    //printk("[futengfei]succeed.fileappend_mode:<write.over.return>\n");
    return 1;
}

int bfs_fill_list(char *filename, enum string_dev_cmd cmd, struct list_head *client_list)
{
    struct file *filep;
    struct inode *inode = NULL;
    struct string_dev *add_new_dev;
    mm_segment_t old_fs;
    char *token, *file_ptr;
    int all_purpose;
    unsigned int file_len;

    filep = filp_open(filename, O_RDWR , 0);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err.open:<%s>\n", filename);
        return -1;
    }
    printk("[futengfei]succeed.open:<%s>\n", filename);

    old_fs = get_fs();
    set_fs(get_ds());

    inode = filep->f_dentry->d_inode;
    file_len = inode->i_size;
    printk("[futengfei]warn.File_length:<%d>\n", file_len);
    file_len = file_len + 2;

    file_ptr = (unsigned char *)vmalloc(file_len);
    if(file_ptr == NULL)
    {
        printk( "[futengfei]err.vmalloc:<cannot malloc memory!>\n");
        return -1;
    }

    filep->f_op->llseek(filep, 0, 0);
    all_purpose = filep->f_op->read(filep, file_ptr, file_len, &filep->f_pos);
    if(all_purpose <= 0)
    {
        printk( "[futengfei]err.f_op->read:<read file data failed>\n");
        return -1;
    }
    set_fs(old_fs);
    filp_close(filep, 0);

    file_ptr[file_len - 1] = '\0';
    if(g_dubug_on)
        printk("%s\n", file_ptr);

    printk("[futengfei]warn.toke:<%s>\n", filename);
    all_purpose = 0;
    while((token = strsep(&file_ptr, "\n")) != NULL )
    {
        if( token[0] != '#' && token[0] != '\n' && token[0] != '\0')
        {
            if(g_dubug_on)
                printk("%d[%s]\n", all_purpose, token);
            add_new_dev = kzalloc(sizeof(struct string_dev), GFP_KERNEL);
            add_new_dev->yourkey = token;
            list_add(&(add_new_dev->tmp_list), client_list);
            all_purpose++;
        }
        else if(g_dubug_on)
            printk("\ndroped[%s]\n", token);
    }
    if(cmd == FS_CMD_FILE_CONFIGMODE)
        fileserver_deal_cmd(client_list, FS_CMD_LIST_SPLITKV, NULL, NULL, NULL, NULL, NULL);
    return 1;
}

int  fileserver_main(char *filename, enum string_dev_cmd cmd, char *str_append, struct list_head *client_list)
{
    //note: cmd tell me the file mode,if it is config file I will do more;
    int ret = -1;
    if(g_dubug_on)
        printk("\n[futengfei]==IN==fileserver_main\n");

    switch (cmd)
    {
    case FS_CMD_FILE_CONFIGMODE:
    case FS_CMD_FILE_LISTMODE:
        if(list_empty(client_list))
        {
            ret = bfs_fill_list(filename, cmd, client_list);
        }
        else
        {
            printk("[futengfei]err.fileserver_main:<your list is not empty>\n");
        }
        break;
    case FS_CMD_FILE_APPENDMODE:
        ret = bfs_file_amend(LIDBG_LOG_FILE_PATH, str_append);
        break;
    default:
        printk("\n[futengfei]err.fileserver_main:<unknown.cmd:%d>\n", cmd);
        break;
    }
    if(g_dubug_on)
        printk("[futengfei]==OUT==fileserver_main\n");
    return ret;
}

static int thread_log_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
	ssleep(20);
    while(!kthread_should_stop())
    {
        msleep(g_clearlogfifo_ms);
        if(!kfifo_is_empty(&log_fifo))
        {
            clearfifo_tofile();
        }
    }
    return 1;
}

int update_list(const char *filename, struct list_head *client_list)
{
    struct file *filep;
    struct inode *inode = NULL;
    struct string_dev *pos;
    mm_segment_t old_fs;
    char *token, *file_ptr, *ptmp, *key, *value;
    int all_purpose;
    unsigned int file_len;

    filep = filp_open(filename, O_RDWR , 0);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err.open:<%s>\n", filename);
        return -1;
    }
    printk("[futengfei]succeed.open:<%s>\n", filename);

    old_fs = get_fs();
    set_fs(get_ds());

    inode = filep->f_dentry->d_inode;
    file_len = inode->i_size;
    printk("[futengfei]warn.File_length:<%d>\n", file_len);
    file_len = file_len + 2;

    file_ptr = (unsigned char *)kzalloc(file_len, GFP_KERNEL);
    if(file_ptr == NULL)
    {
        printk( "[futengfei]err.vmalloc:<cannot kzalloc memory!>\n");
        return -1;
    }

    filep->f_op->llseek(filep, 0, 0);
    all_purpose = filep->f_op->read(filep, file_ptr, file_len, &filep->f_pos);
    if(all_purpose <= 0)
    {
        printk( "[futengfei]err.f_op->read:<read file data failed>\n");
        return -1;
    }
    set_fs(old_fs);
    filp_close(filep, 0);

    file_ptr[file_len - 1] = '\0';
    while((token = strsep(&file_ptr, "\n")) != NULL )
    {
        if( token[0] != '#' && strlen(token) > 2)
        {
            key = token;
            ptmp = memchr(key, '=', strlen(key));
            if(ptmp != NULL)
            {
                value = ptmp + 1;
                *ptmp = '\0';
                printk("<%s,%s>\n", key, value);
                fileserver_deal_cmd( client_list, FS_CMD_LIST_SETVALUE, NULL, key, &value, NULL, NULL);
            }
            else if(g_dubug_on)
                printk("\ndroped[%s]\n", token);
        }
    }
    kfree(file_ptr);
    return 1;
}

bool get_file_mftime(const char *filename, struct rtc_time *ptm)
{
    struct file *filep;
    struct inode *inode = NULL;
    struct timespec mtime;
    filep = filp_open(filename, O_RDWR , 0);
    if(IS_ERR(filep))
    {
        if(g_dubug_on)
            printk("[futengfei]err.open:<%s>\n", filename);
        return false;
    }
    inode = filep->f_dentry->d_inode;
    mtime = inode->i_mtime;
    rtc_time_to_tm(mtime.tv_sec, ptm);
    filp_close(filep, 0);
    return true;
}

bool is_tm_updated(struct rtc_time *pretm, struct rtc_time *newtm)
{
    if(pretm->tm_sec == newtm->tm_sec && pretm->tm_min == newtm->tm_min && pretm->tm_hour == newtm->tm_hour &&
            pretm->tm_mday == newtm->tm_mday && pretm->tm_mon == newtm->tm_mon && pretm->tm_year == newtm->tm_year )
        return false;
    else
        return true;
}

bool is_file_updated(const char *filename, struct rtc_time *pretm)
{
    struct rtc_time tm;
    if(get_file_mftime(filename, &tm) && is_tm_updated(pretm, &tm))
    {
        *pretm = tm; //get_file_mftime(filename, pretm);//give the new tm to pretm;
        return true;
    }
    else
        return false;
}

static int thread_filepoll_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    get_file_mftime(core_sd_path, &precorefile_tm);
    get_file_mftime(driver_sd_path, &predriverfile_tm);
	ssleep(50);
	if(!copy_file(driver_fly_path, driver_sd_path))
		copy_file(driver_lidbg_path, driver_sd_path);
	if(!copy_file(core_fly_path, core_sd_path))
		copy_file(core_lidbg_path, core_sd_path);
    while(!kthread_should_stop())
    {
        msleep(g_pollfile_ms);

        if(is_file_updated(core_sd_path, &precorefile_tm))
        {
            FS_WARN("<file modify time:%d-%02d-%02d %02d:%02d:%02d>\n",
                    precorefile_tm.tm_year + 1900, precorefile_tm.tm_mon + 1, precorefile_tm.tm_mday, precorefile_tm.tm_hour + 8, precorefile_tm.tm_min, precorefile_tm.tm_sec);
            update_list(core_sd_path, &lidbg_core_list);
        }

        if(is_file_updated(driver_sd_path, &predriverfile_tm))
        {
            FS_WARN("<file modify time:%d-%02d-%02d %02d:%02d:%02d>\n",
                    precorefile_tm.tm_year + 1900, precorefile_tm.tm_mon + 1, precorefile_tm.tm_mday, precorefile_tm.tm_hour + 8, precorefile_tm.tm_min, precorefile_tm.tm_sec);
            update_list(driver_sd_path, &lidbg_drivers_list);
        }
    }
    return 1;
}

void property_callback(char *key, char *value)
{
    FS_WARN("<%s=%s>\n", key, value);
}

bool  is_file_exist(char *file)
{
    struct file *filep;
    filep = filp_open(file, O_RDWR , 0);
    if(IS_ERR(filep))
        return false;
    else
        filp_close(filep, 0);
    return true;
}
bool copy_file(char *from, char *to)
{
    char *string;
    unsigned int file_len;
    struct file *pfilefrom;
    struct file *pfileto;
    struct inode *inodefrom = NULL;
    mm_segment_t old_fs;

    if(!is_file_exist(from))
    {
        FS_ERR("<file_miss:%s>\n", from);
        return false;
    }

    pfilefrom = filp_open(from, O_RDWR , 0);
    pfileto = filp_open(to, O_CREAT | O_RDWR , 0600);
    if(IS_ERR(pfileto))
    {
        FS_ERR("<%s>\n", to);
        filp_close(pfilefrom, 0);
        return false;
    }
    old_fs = get_fs();
    set_fs(get_ds());
    inodefrom = pfilefrom->f_dentry->d_inode;
    file_len = inodefrom->i_size;

    string = (unsigned char *)vmalloc(file_len);
    if(string == NULL)
        return false;
    pfilefrom->f_op->llseek(pfilefrom, 0, 0);
    pfilefrom->f_op->read(pfilefrom, string, file_len, &pfilefrom->f_pos);
    set_fs(old_fs);
    filp_close(pfilefrom, 0);

    old_fs = get_fs();
    set_fs(get_ds());
    pfileto->f_op->llseek(pfileto, 0, 0);
    pfileto->f_op->write(pfileto, string, file_len, &pfileto->f_pos);
    set_fs(old_fs);
    filp_close(pfilefrom, 0);

    vfree(string);
    return true;
}
void fileserverinit_once(void)
{
    //note:your list can only be init once
    char *enable;
    char tbuff[100];
    int ret;

    //search priority:sd_path>fly_path>lidbg_path
    if(is_file_exist(driver_sd_path) || copy_file(driver_fly_path, driver_sd_path) || copy_file(driver_lidbg_path, driver_sd_path))
        fs_fill_list(driver_sd_path, FS_CMD_FILE_CONFIGMODE, &lidbg_drivers_list);
    else
    {
        if(is_file_exist(driver_fly_path))
            fs_fill_list(driver_fly_path, FS_CMD_FILE_CONFIGMODE, &lidbg_drivers_list);
        else
            fs_fill_list(driver_lidbg_path, FS_CMD_FILE_CONFIGMODE, &lidbg_drivers_list);
    }

    if(is_file_exist(core_sd_path) || copy_file(core_fly_path, core_sd_path) || copy_file(core_lidbg_path, core_sd_path))
        fs_fill_list(core_sd_path, FS_CMD_FILE_CONFIGMODE, &lidbg_core_list);
    else
    {
        if(is_file_exist(core_fly_path))
            fs_fill_list(core_fly_path, FS_CMD_FILE_CONFIGMODE, &lidbg_core_list);
        else
            fs_fill_list(core_lidbg_path, FS_CMD_FILE_CONFIGMODE, &lidbg_core_list);
    }

    spin_lock_init(&fs_lock);
    kfifo_init(&log_fifo, log_buffer, FIFO_SIZE);
    kfifo_reset(&log_fifo);
    lidbg_get_current_time(tbuff);
    fs_file_log(tbuff);
    fs_get_intvalue(&lidbg_core_list, "fs_dbg_enable", &g_dubug_on, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_clearlogfifo_ms", &g_clearlogfifo_ms, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_pollfile_ms", &g_pollfile_ms, property_callback);
    printk("[futengfei]warn.fileserverinit_once:<g_dubug_on=%d;clearlogfifo_ms=%d,g_pollfile_ms=%d>\n", g_dubug_on, g_clearlogfifo_ms, g_pollfile_ms);
    filelog_task = kthread_run(thread_log_func, NULL, "ftf_clearlogfifo");
    filepoll_task = kthread_run(thread_filepoll_func, NULL, "ftf_filepolltask");
}
//zone end

//zone below [fileserver_test]
static int test_count = 0;
void test_fileserver_stability(void)
{
    char *delay, *value, tbuff[100];
    int ret = 0;

    //test_fileserver_stability
    fs_find_string(&kill_list_test, "cn.flyaudio.navigation");
    fs_find_string(&kill_list_test, "cn.flyaudio.navigationfutengfei");
    fs_get_intvalue(&lidbg_drivers_list, "futengfei", &ret, NULL);
    printk("[futengfei]get key value:[%d]\n", ret );
    fs_get_intvalue(&lidbg_drivers_list, "mayanping", &ret, NULL);
    printk("[futengfei]get key value:[%d]\n", ret );
    fs_get_intvalue(&lidbg_core_list, "fs_dbg_enable", &ret, NULL);
    printk("[futengfei]get key value:[%d]\n", ret );
    delay = "futengfei1";
    fs_set_value(&lidbg_core_list, "fs_dbg_enable", delay);
    fs_get_value(&lidbg_core_list, "fs_dbg_enable", &value);
    printk("[futengfei]warn.test_fileserver_stability:<value=%s>\n", value);
    lidbg_get_current_time(tbuff);
    fs_file_log(tbuff);


    is_file_updated(core_sd_path, &precorefile_tm);
    update_list(core_sd_path, &lidbg_core_list);
    is_file_updated(driver_sd_path, &predriverfile_tm);
    update_list(driver_sd_path, &lidbg_drivers_list);

    if(0)
    {
        sprintf(tbuff, "/mnt/sdcard/lidbg/lidbg_c%d.txt\0", ++test_count);
        fs_copy_file("/system/lib/modules/out/core.conf", tbuff);
        sprintf(tbuff, "/mnt/sdcard/lidbg/lidbg_d%d.txt\0", test_count);
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
static int fileserver_test_fk(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    while(!kthread_should_stop())
    {
        test_fileserver_stability();
        ssleep(1);
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
        printk("[futengfei]======start_kthread_run1.2.3\n");
        fileserver_test_task3 = kthread_run(fileserver_test_fk, NULL, "ftf_fs_task3");
        msleep(88);
    case 2:
        printk("[futengfei]======start_kthread_run1.2\n");
        fileserver_test_task2 = kthread_run(fileserver_test_fk, NULL, "ftf_fs_task2");
        msleep(230);
    case 1:
        printk("[futengfei]======start_kthread_run1\n");
        fileserver_test_task = kthread_run(fileserver_test_fk, NULL, "ftf_fs_task");
        break;
    case 0:
        printk("[futengfei]======stop_kthread_run1.2.3\n");
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
        printk("[futengfei]======thread_count:too much\n");
    }
}
//zone end

void lidbg_fileserver_main(int argc, char **argv)
{
    int cmd = 0;
    int thread_count = 0;
    if(argc < 2)
    {
        printk("[futengfei]err.lidbg_fileserver_main:echo \"c file 1 1\" > /dev/mlidbg0\n");
        return;
    }

    thread_count = simple_strtoul(argv[0], 0, 0);
    cmd = simple_strtoul(argv[1], 0, 0);
    fileserver_thread_test(thread_count);
    switch (cmd)
    {
    case 1:
        fs_log_sync();
        break;
    default:
        FS_ERR("<check you cmd:%d>\n", cmd);
        break;
    }
}


//zone below [EXPORT_SYMBOL]
EXPORT_SYMBOL(lidbg_fileserver_main);
EXPORT_SYMBOL(lidbg_drivers_list);
EXPORT_SYMBOL(lidbg_core_list);
EXPORT_SYMBOL(fs_get_intvalue);
EXPORT_SYMBOL(fs_get_value);
EXPORT_SYMBOL(fs_set_value);
EXPORT_SYMBOL(fs_find_string);
EXPORT_SYMBOL(fs_show_list);
EXPORT_SYMBOL(fs_file_log);
EXPORT_SYMBOL(fs_fill_list);
EXPORT_SYMBOL(fs_copy_file);
EXPORT_SYMBOL(fs_log_sync);
//zone end
