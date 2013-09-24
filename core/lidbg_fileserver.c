
#include "lidbg.h"
/*
update log:
	1:[20130912 V5.1]/add separaror
	2:[20130913 V6.0]/save core,driver list to overwrite core.conf driver.conf
	3:[20130916 V7.0]/check if the conf file need to be updated
	4:[20130922] copy file (from to),set file[to] length to zero
*/

#define FS_WARN(fmt, args...) pr_info("[futengfei.fs]warn.%s: " fmt,__func__,##args)
#define FS_ERR(fmt, args...) pr_info("[futengfei.fs]err.%s: " fmt,__func__,##args)
#define FS_SUC(fmt, args...) pr_info("[futengfei.fs]suceed.%s: " fmt,__func__,##args)

//zone below [tools]
#define FS_VERSION "FS.VERSION:  [20130922]"
#define DEBUG_MEM_FILE "/data/fs_private.txt"
#define LIDBG_LOG_FILE_PATH "/data/lidbg_log.txt"
#define LIDBG_KMSG_FILE_PATH "/data/lidbg_kmsg.txt"
#define MACHINE_ID_FILE "/data/MIF.txt"
#define PRE_CONF_INFO_FILE "/data/conf_info.txt"
#define LIDBG_NODE "/dev/mlidbg0"
#define KMSG_NODE "/proc/kmsg"
#define FIFO_SIZE (1024)
#define MAX_FILE_LEN (MEM_SIZE_4_MB)
static struct kfifo log_fifo;
spinlock_t		fs_lock;
unsigned long flags;
static  char *driver_sd_path = "/data/drivers.txt";
static  char *driver_fly_path = "/flysystem/lib/out/drivers.conf";
static  char *driver_lidbg_path = "/system/lib/modules/out/drivers.conf";
static  char *core_sd_path = "/data/core.txt";
static  char *core_fly_path = "/flysystem/lib/out/core.conf";
static  char *core_lidbg_path = "/system/lib/modules/out/core.conf";
static  char *cmd_sd_path = "/data/cmd.txt";
static  char *cmd_fly_path = "/flysystem/lib/out/cmd.conf";
static  char *cmd_lidbg_path = "/system/lib/modules/out/cmd.conf";
static  char *state_sd_path = "/data/state.txt";
static  char *state_mem_path = "/dev/log/state.txt";
static  char *state_fly_path = "/flysystem/lib/out/state.conf";
static  char *state_lidbg_path = "/system/lib/modules/out/state.conf";
static struct task_struct *filelog_task;
static struct task_struct *filepoll_task;
static struct task_struct *fs_statetask;
static struct task_struct *fs_kmsgtask;
static struct task_struct *fs_fdetectask;
struct rtc_time precorefile_tm;
struct rtc_time predriverfile_tm;
struct rtc_time precmdfile_tm;
struct completion kmsg_wait;
static int g_dubug_mem = 0;
static int g_dubug_on = 0;
static int g_dubug_filedetec = 0;
static int g_clearlogfifo_ms = 30000;
static int g_pollfile_ms = 7000;
static int g_pollstate_ms = 1000;
static int g_pollkmsg_en = 0;
static int g_iskmsg_ready = 0;
static int g_filedetect_ms = 10000;
static int machine_id = 0;
unsigned char log_buffer[FIFO_SIZE];
unsigned char log_buffer2write[FIFO_SIZE];
LIST_HEAD(lidbg_drivers_list);
LIST_HEAD(lidbg_core_list);
LIST_HEAD(fs_state_list);
LIST_HEAD(fs_filedetec_list);
LIST_HEAD(kill_list_test);//for test
static struct task_struct *fileserver_test_task;
static struct task_struct *fileserver_test_task2;
static struct task_struct *fileserver_test_task3;
static int test_count = 0;
//zone end

//zone below [interface]
int fileserver_deal_cmd(struct list_head *client_list, enum string_dev_cmd cmd, char *lookfor, char *key, char **string, int *int_value, void (*callback)(char *key, char *value ));
int bfs_fill_list(char *filename, enum string_dev_cmd cmd, struct list_head *client_list);
int bfs_file_amend(char *file2amend, char *str_append);
int dump_kmsg(char *node, char *save_msg_file, int size, int *always);
int get_int_value(struct list_head *client_list, char *key, int *int_value, void (*callback)(char *key, char *value));
void save_list_to_file(struct list_head *client_list, char *filename);
void regist_filedetec(char *filename, void (*cb_filedetec)(char *filename ));
void file_separator(char *file2separator);
void update_file_tm(void);
bool copy_file(char *from, char *to);
bool is_file_exist(char *file);


void fs_save_list_to_file(void)
{
    save_list_to_file(&lidbg_core_list, core_sd_path);
    save_list_to_file(&lidbg_drivers_list, driver_sd_path);
    fs_copy_file(state_mem_path, state_sd_path);
    update_file_tm();
}
void fs_file_separator(char *file2separator)
{
    file_separator(file2separator);
}
void fs_regist_filedetec(char *filename, void (*cb_filedetec)(char *filename ))
{
    if(filename && cb_filedetec)
        regist_filedetec(filename, cb_filedetec);
    else
        FS_ERR("<filename||cb_filedetec:null?>\n");
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
int get_machine_id(void)
{
    return machine_id;
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
int fs_dump_kmsg(char *tag, int size )
{
    file_separator(LIDBG_KMSG_FILE_PATH);
    if(tag != NULL)
        fs_string2file(LIDBG_KMSG_FILE_PATH, "fs_dump_kmsg: %s\n", tag);
    return dump_kmsg(KMSG_NODE, LIDBG_KMSG_FILE_PATH, size, NULL);
}
void fs_save_state(void)
{
    fs_copy_file(state_mem_path, state_sd_path);
}
int fs_regist_state(char *key, int *value)
{
    return fileserver_deal_cmd( &fs_state_list, FS_CMD_LIST_SAVEINFO, NULL, key, NULL, value, NULL);
}
int fs_get_intvalue(struct list_head *client_list, char *key, int *int_value, void (*callback)(char *key, char *value))
{
    return get_int_value(client_list, key, int_value, callback);
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
    int fifo_len, ret;
    spin_lock_irqsave(&fs_lock, flags);
    fifo_len = kfifo_len(&log_fifo);
    ret = kfifo_out(&log_fifo, log_buffer2write, fifo_len);
    log_buffer2write[fifo_len > FIFO_SIZE - 1 ? FIFO_SIZE - 1 : fifo_len] = '\0';
    spin_unlock_irqrestore(&fs_lock, flags);
    bfs_file_amend(LIDBG_LOG_FILE_PATH, log_buffer2write);
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
    int ret = -1;
    if(list_empty(client_list))
        ret = bfs_fill_list(filename, cmd, client_list);
    else
        printk("[futengfei]err.fs_fill_list:<your list is not empty>\n");
    return ret;
}
bool fs_copy_file(char *from, char *to)
{
    return copy_file(from, to);
}
//zone end


//zone below [fileserver]
int get_int_value(struct list_head *client_list, char *key, int *int_value, void (*callback)(char *key, char *value))
{
    struct string_dev *pos;
    if (list_empty(client_list))
    {
        printk("[futengfei]err.fileserver_deal_cmd:<list_is_empty>\n");
        return -2;
    }
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (!strcmp(pos->yourkey, key) )
        {
            if(pos->int_value)
            {
                *int_value = *(pos->int_value);
                printk("[futengfei]succeed.find_key:<%s=%d>\n", key, *(pos->int_value) );
                return 1;
            }
            if(int_value && pos->yourvalue)
            {
                pos->int_value = int_value;
                *(pos->int_value) = simple_strtoul(pos->yourvalue, 0, 0);
                pos->yourvalue = NULL;
            }
            if(callback)
                pos->callback = callback;
            printk("[futengfei]succeed.find_key:<%s=%d>\n", key, *(pos->int_value) );
            return 1;
        }
    }
    printk("[futengfei]err.fail_find:<%s>\n", key);
    return -1;

}
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

    case FS_CMD_LIST_SAVEINFO:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (!strcmp(pos->yourkey, key))
            {
                if(int_value)
                    pos->int_value = int_value;
                if(callback)
                    pos->callback = callback;
            }
        }
        return 1;

    case FS_CMD_LIST_GETVALUE:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (!strcmp(pos->yourkey, key))
            {
                *string = pos->yourvalue;
                printk("[futengfei]succeed.find_key:<%s=%s>\n", key, *string);
                return 1;
            }
        }
        printk("[futengfei]err.fail_find:<%s>\n", key);
        return -1;

    case FS_CMD_LIST_SETVALUE:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if ( (!strcmp(pos->yourkey, key)) && pos->yourvalue && (strcmp(pos->yourvalue, *string)))
            {

                pos->yourvalue = *string;
                printk("[futengfei]succeed.set_key:<%s=%s>\n", key, pos->yourvalue);
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
            if((p = memchr(pos->yourkey, lookfor[0], strlen(pos->yourkey))))
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
    file_len = file_len + 2;


    if(file_len > MAX_FILE_LEN)
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

int bfs_fill_list(char *filename, enum string_dev_cmd cmd, struct list_head *client_list)
{
    struct file *filep;
    struct inode *inode = NULL;
    struct string_dev *add_new_dev;
    mm_segment_t old_fs;
    char *token, *file_ptr = NULL, *file_ptmp;
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

    if(g_dubug_mem)
        fs_string2file(DEBUG_MEM_FILE, "%s=%d \n", __func__, file_len);

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
    file_ptmp = file_ptr;
    while((token = strsep(&file_ptmp, "\n")) != NULL )
    {
        if( token[0] != '#' && token[0] != '\n'  && token[0] != 18 && token[0] != 13)//del char:er,dc2 look for ASCII
        {
            if(g_dubug_on)
                printk("%d[%s]\n", all_purpose, token);
            add_new_dev = kzalloc(sizeof(struct string_dev), GFP_KERNEL);
            if(g_dubug_mem)
                fs_string2file(DEBUG_MEM_FILE, "%s=%d \n", __func__, sizeof(struct string_dev));
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
void file_separator(char *file2separator)
{
    char buf[32];
    lidbg_get_current_time(buf, NULL);
    fs_string2file(file2separator, "------------------------%s------------------------\n", buf);
}
static int thread_log_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    while(!kthread_should_stop())
    {
        if(g_clearlogfifo_ms)
        {
            msleep(g_clearlogfifo_ms);
            if(!kfifo_is_empty(&log_fifo))
            {
                clearfifo_tofile();
            }
        }
        else
            ssleep(30);
    }
    return 1;
}

int update_list(const char *filename, struct list_head *client_list)
{
    struct file *filep;
    struct inode *inode = NULL;
    struct string_dev *pos;
    mm_segment_t old_fs;
    char *token, *file_ptr = NULL, *file_ptmp, *ptmp, *key, *value;
    int all_purpose, curren_intvalue;
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
    file_len = file_len + 2;

    file_ptr = (unsigned char *)kzalloc(file_len, GFP_KERNEL);
    if(file_ptr == NULL)
    {
        printk( "[futengfei]err.vmalloc:<cannot kzalloc memory!>\n");
        return -1;
    }

    if(g_dubug_mem)
        fs_string2file(DEBUG_MEM_FILE, "free.%s=%d \n", __func__, file_len);

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
    file_ptmp = file_ptr;
    while((token = strsep(&file_ptmp, "\n")) != NULL )
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
                {
                    //start
                    list_for_each_entry(pos, client_list, tmp_list)
                    {
                        if ( (!strcmp(pos->yourkey, key)) )//&& pos->yourvalue && (strcmp(pos->yourvalue, *string))
                        {
                            curren_intvalue = simple_strtoul(value, 0, 0);
                            if (pos->int_value && (*(pos->int_value) != curren_intvalue))
                            {
                                *(pos->int_value) = curren_intvalue;
                                if (pos->callback)
                                    pos->callback(key, value);
                                FS_SUC("<%s=%d>\n", key, *(pos->int_value));
                            }
                        }
                    }
                }//end

            }
            else if(g_dubug_on)
                printk("\ndroped[%s]\n", token);
        }
    }
    kfree(file_ptr);
    return 1;
}

int readwrite_file(const char *filename, char *wbuff, char *rbuff, int readlen)
{
    struct file *filep;
    struct inode *inodefrom = NULL;
    mm_segment_t old_fs;
    unsigned int file_len = 1;

    filep = filp_open(filename,  O_RDWR, 0);
    if(IS_ERR(filep) || !(filep->f_op) || !(filep->f_op->read) || !(filep->f_op->write))
        return -1;
    old_fs = get_fs();
    set_fs(get_ds());

    if(wbuff)
        filep->f_op->write(filep, wbuff, strlen(wbuff), &filep->f_pos);
    else
    {
        inodefrom = filep->f_dentry->d_inode;
        file_len = inodefrom->i_size;
        filep->f_op->llseek(filep, 0, 0);
        filep->f_op->read(filep, rbuff, file_len, &filep->f_pos);
        *(rbuff + (file_len < readlen ? file_len : readlen - 1)) = '\0';
        if(g_dubug_on)
            FS_WARN("%d,%s\n", file_len, rbuff);
    }
    set_fs(old_fs);
    filp_close(filep, 0);
    return file_len;
}

int launch_file_cmd(const char *filename)
{
    struct file *filep;
    struct inode *inode = NULL;
    mm_segment_t old_fs;
    char *token, *file_ptr = NULL, *file_ptmp;
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

    if(g_dubug_mem)
        fs_string2file(DEBUG_MEM_FILE, "free.%s=%d \n", __func__, file_len);

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
    file_ptmp = file_ptr;
    while((token = strsep(&file_ptmp, "\n")) != NULL )
    {
        if( token[0] != '#' && token[0] != '0' && token[1] == 'c' )
        {
            int loop;
            char p[2] ;
            p[0] = token[0];
            p[1] = '\0';
            loop = simple_strtoul(p, 0, 0);
            for(; loop > 0; loop--)
                readwrite_file(LIDBG_NODE, token + 1, NULL, 0);
        }
    }
    kfree(file_ptr);
    return 1;
}

bool set_file_len_to_zero(char *filename)
{
    struct file *filep;
    filep = filp_open(filename, O_CREAT | O_RDWR | O_TRUNC , 0777);
    if(IS_ERR(filep))
        return false;
    else
        filp_close(filep, 0);
    return true;
}
void show_tm(struct rtc_time *ptmp)
{
    FS_WARN("<file modify time:%d-%02d-%02d %02d:%02d:%02d>\n",
            ptmp->tm_year + 1900, ptmp->tm_mon + 1, ptmp->tm_mday, ptmp->tm_hour + 8, ptmp->tm_min, ptmp->tm_sec);
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

void new_filedetec_dev(char *filename, void (*cb_filedetec)(char *filename))
{
    struct string_dev *add_new_dev;
    add_new_dev = kzalloc(sizeof(struct string_dev), GFP_KERNEL);
    if(g_dubug_mem)
        fs_string2file(DEBUG_MEM_FILE, "%s=%d \n", __func__, sizeof(struct string_dev));

    add_new_dev->filedetec = filename;
    add_new_dev->cb_filedetec = cb_filedetec;
    list_add(&(add_new_dev->tmp_list), &fs_filedetec_list);
}
bool is_file_registerd(char *filename)
{
    struct string_dev *pos;
    struct list_head *client_list = &fs_filedetec_list;

    if(list_empty(client_list))
        return false;
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (!strcmp(pos->filedetec, filename))
            return true;
    }
    return false;
}
void show_filedetec_list(void)
{
    struct string_dev *pos;
    struct list_head *client_list = &fs_filedetec_list;

    if(list_empty(client_list))
        return ;
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->filedetec && pos->cb_filedetec)
            FS_WARN("<registerd_list:%s>\n", pos->filedetec);
    }
}
void call_filedetec_cb(void)
{
    struct string_dev *pos;
    struct list_head *client_list = &fs_filedetec_list;

    if(list_empty(client_list))
        return ;
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->filedetec && pos->cb_filedetec && is_file_exist(pos->filedetec) )
        {
            if(g_dubug_filedetec)
                FS_WARN("<should call :%s>\n", pos->filedetec);
            if (!pos->have_warned)
            {
                pos->cb_filedetec(pos->filedetec);
                if(g_dubug_filedetec)
                    FS_WARN("<have called :%s>\n", pos->filedetec);
            }
            pos->have_warned = true;
        }
        else
            pos->have_warned = false;
    }
}
void regist_filedetec(char *filename, void (*cb_filedetec)(char *filename))
{
    if(  !is_file_registerd(filename))
        new_filedetec_dev(filename, cb_filedetec);
    else
        FS_ERR("<existed :%s>\n", filename);
}
static int thread_filedetec_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    ssleep(20);
    FS_WARN("<thread start>\n");
    if(g_dubug_filedetec)
        show_filedetec_list();
    while(!kthread_should_stop())
    {
        if(g_filedetect_ms)
        {
            call_filedetec_cb();
            msleep(g_filedetect_ms);
        }
        else
        {
            ssleep(30);
        }
    }
    return 1;
}
void update_file_tm(void)
{
    get_file_mftime(core_sd_path, &precorefile_tm);
    get_file_mftime(driver_sd_path, &predriverfile_tm);
    get_file_mftime(cmd_sd_path, &precmdfile_tm);
}
static int thread_pollfile_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    ssleep(20);
    update_file_tm();
    g_iskmsg_ready = 1;
    if(g_pollkmsg_en)
        complete(&kmsg_wait);
    while(!kthread_should_stop())
    {
        if(g_pollfile_ms)
        {
            msleep(g_pollfile_ms);
            if(is_file_updated(core_sd_path, &precorefile_tm))
            {
                show_tm(&precorefile_tm);
                update_list(core_sd_path, &lidbg_core_list);
            }

            if(is_file_updated(driver_sd_path, &predriverfile_tm))
            {
                show_tm(&predriverfile_tm);
                update_list(driver_sd_path, &lidbg_drivers_list);
            }

            if(is_file_updated(cmd_sd_path, &precmdfile_tm))
            {
                show_tm(&precmdfile_tm);
                launch_file_cmd(cmd_sd_path);
            }

        }
        else
            ssleep(30);
    }
    return 1;
}
void save_list_to_file(struct list_head *client_list, char *filename)
{
    struct string_dev *pos;
    struct file *filep;
    mm_segment_t old_fs;
    char buff[100];
    filep = filp_open(filename, O_CREAT | O_RDWR | O_TRUNC , 0777);
    if(!IS_ERR(filep))
    {
        old_fs = get_fs();
        set_fs(get_ds());
        if(filep->f_op->write)
        {
            list_for_each_entry(pos, client_list, tmp_list)
            {
                memset(buff, '\0', sizeof(buff));
                if(pos->yourkey && pos->int_value)
                    sprintf(buff, "%s=%d\n", pos->yourkey, *(pos->int_value));
                else if(pos->yourkey && pos->yourvalue)
                    sprintf(buff, "%s=%s\n", pos->yourkey, pos->yourvalue);
                filep->f_op->write(filep, buff, strlen(buff), &filep->f_pos);
                filep->f_op->llseek(filep, 0, SEEK_END);
            }
        }
        set_fs(old_fs);
        filp_close(filep, 0);
    }
    else
        FS_ERR("<fail open:%s>\n", filename);
}
static int thread_pollstate_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    while(!kthread_should_stop())
    {
        if(g_pollstate_ms)
        {
            msleep(g_pollstate_ms);
            save_list_to_file(&fs_state_list, state_mem_path);
        }
        else
            ssleep(30);
    }
    return 1;
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

            if(g_dubug_mem)
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
    //set_freezable();
    while(!kthread_should_stop())
    {
        if( !wait_for_completion_interruptible(&kmsg_wait))
            dump_kmsg(KMSG_NODE, LIDBG_KMSG_FILE_PATH, 0, &g_pollkmsg_en);
    }
    return 1;
}

bool is_file_exist(char *file)
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
    char *string = NULL;
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
    pfileto = filp_open(to, O_CREAT | O_RDWR | O_TRUNC, 0777);
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

    if(g_dubug_mem)
        fs_string2file(DEBUG_MEM_FILE, "free.%s=%d \n", __func__, file_len);

    pfilefrom->f_op->llseek(pfilefrom, 0, 0);
    pfilefrom->f_op->read(pfilefrom, string, file_len, &pfilefrom->f_pos);
    set_fs(old_fs);
    filp_close(pfilefrom, 0);

    old_fs = get_fs();
    set_fs(get_ds());
    pfileto->f_op->llseek(pfileto, 0, 0);
    pfileto->f_op->write(pfileto, string, file_len, &pfileto->f_pos);
    set_fs(old_fs);
    filp_close(pfileto, 0);

    vfree(string);
    return true;
}

void set_machine_id(void)
{
    char string[64];
    if(is_file_exist(MACHINE_ID_FILE))
    {
        readwrite_file(MACHINE_ID_FILE, NULL, string, sizeof(string));
        machine_id = simple_strtoul(string, 0, 0);
    }
    else
    {
        get_random_bytes(&machine_id, sizeof(int));
        machine_id = ABS(machine_id);
        sprintf(string, "%d", machine_id);
        if(g_dubug_on)
            FS_WARN("%s\n", string);
        bfs_file_amend(MACHINE_ID_FILE, string);
    }
}

void cb_kv_pollkmsg(char *key, char *value)
{
    if(g_dubug_on)
        FS_WARN("<%s=%s>\n", key, value);
    if ( (!strcmp(key, "fs_kmsg_en" ))  &&  (strcmp(value, "0" )) )
        complete(&kmsg_wait);
}
void cb_kv_filedetecen(char *key, char *value)
{
    if(g_dubug_on)
        FS_WARN("<%s=%s>\n", key, value);
    if ( (!strcmp(key, "fs_dbg_file_detect" ))  &&  (strcmp(value, "0" )) )
        show_filedetec_list();
}
void cb_filedetec_dump_kmsg(char *filename )
{
    if(g_dubug_filedetec)
        FS_WARN("<callback belong::%s>\n", filename);
    fs_dump_kmsg((char *)__FUNCTION__, __LOG_BUF_LEN);
}
void cb_filedetec_test(char *filename )
{
    if(g_dubug_filedetec)
        FS_WARN("<callback belong::%s>\n", filename);
}

void copy_all_conf_file(void)
{
    if(is_file_exist(core_fly_path))
    {
        copy_file(core_fly_path, core_sd_path);
        copy_file(driver_fly_path, driver_sd_path) ;
        copy_file(state_fly_path, state_sd_path);
        copy_file(cmd_fly_path, cmd_sd_path);
    }
    else
    {
        copy_file(core_lidbg_path, core_sd_path);
        copy_file(driver_lidbg_path, driver_sd_path);
        copy_file(state_lidbg_path, state_sd_path);
        copy_file(cmd_lidbg_path, cmd_sd_path);
    }
}
bool get_file_tmstring(char *filename, char *tmstring)
{
    struct rtc_time tm;
    if(filename && tmstring && get_file_mftime(filename, &tm) )
    {
        sprintf(tmstring, "%d-%02d-%02d %02d:%02d:%02d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour + 8, tm.tm_min, tm.tm_sec);
        return true;
    }
    return false;
}
bool is_conf_updated(char *filename, char *infofile)
{
    char pres[64], news[64];
    if (get_file_tmstring(filename, news) && (readwrite_file(infofile, NULL, pres, sizeof(pres)) > 0) && (strcmp(news, pres)))
        return true;
    else
        return false;
}
void check_conf_file(char *filename)
{
    if(!is_file_exist(PRE_CONF_INFO_FILE) || is_conf_updated(filename, PRE_CONF_INFO_FILE))
    {
        char  news[64];

        FS_WARN("<overwrite all conf:push,update?>\n");
        copy_all_conf_file();
        get_file_tmstring(filename, news);
        set_file_len_to_zero(PRE_CONF_INFO_FILE);
        bfs_file_amend(PRE_CONF_INFO_FILE, news);
    }
}
void fileserverinit_once(void)
{
    char tbuff[100];

    if(is_file_exist(core_fly_path))
        check_conf_file(core_fly_path);
    else
        check_conf_file(core_lidbg_path);

    //search priority:sd_path>fly_path>lidbg_path
    if(!is_file_exist(driver_sd_path) || !is_file_exist(core_sd_path) || !is_file_exist(state_sd_path))
    {
        FS_WARN("<overwrite all conf:conf miss>\n");
        copy_all_conf_file();
    }
    fs_fill_list(driver_sd_path, FS_CMD_FILE_CONFIGMODE, &lidbg_drivers_list);
    fs_fill_list(core_sd_path, FS_CMD_FILE_CONFIGMODE, &lidbg_core_list);
    fs_fill_list(state_sd_path, FS_CMD_FILE_CONFIGMODE, &fs_state_list);

    init_completion(&kmsg_wait);
    spin_lock_init(&fs_lock);
    kfifo_init(&log_fifo, log_buffer, FIFO_SIZE);
    kfifo_reset(&log_fifo);
    lidbg_get_current_time(tbuff, NULL);
    set_machine_id();

    fs_file_log("\nBuild Time: %s, %s, %s\n", __FILE__, __DATE__, __TIME__);
    fs_file_log("%s\n", FS_VERSION );
    fs_file_log("machine_id:%d\n", get_machine_id());//save to log
    FS_WARN("machine_id:%d\n", get_machine_id());//sdve to uart
    fs_file_log("%s\n", tbuff);

    fs_get_intvalue(&lidbg_core_list, "fs_dbg_mem", &g_dubug_mem, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_dbg_enable", &g_dubug_on, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_clearlogfifo_ms", &g_clearlogfifo_ms, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_pollfile_ms", &g_pollfile_ms, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_updatestate_ms", &g_pollstate_ms, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_kmsg_en", &g_pollkmsg_en, cb_kv_pollkmsg);
    fs_get_intvalue(&lidbg_core_list, "fs_filedetect_ms", &g_filedetect_ms, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_dbg_file_detect", &g_dubug_filedetec, cb_kv_filedetecen);

    fs_regist_filedetec("/mnt/sdcard/dump_kmsg", cb_filedetec_dump_kmsg);
    fs_regist_filedetec("/mnt/usbdisk/dump_kmsg", cb_filedetec_dump_kmsg);
    fs_regist_filedetec("/mnt/sdcard/123.txt", cb_filedetec_test);


    printk("[futengfei]warn.fileserverinit_once:<g_dubug_on=%d;g_pollstate_ms=%d,g_pollkmsg_en=%d>\n", g_dubug_on, g_pollstate_ms, g_pollkmsg_en);
    filelog_task = kthread_run(thread_log_func, NULL, "ftf_clearlogfifo");
    filepoll_task = kthread_run(thread_pollfile_func, NULL, "ftf_filepolltask");
    fs_statetask = kthread_run(thread_pollstate_func, NULL, "ftf_statetask");
    fs_kmsgtask = kthread_run(thread_pollkmsg_func, NULL, "ftf_kmsgtask");
    fs_fdetectask = kthread_run(thread_filedetec_func, NULL, "ftf_fdetectask");
}
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
    printk("[futengfei]get key value:[%d]\n", ret );
    fs_get_intvalue(&lidbg_drivers_list, "mayanping", &ret, NULL);
    printk("[futengfei]get key value:[%d]\n", ret );
    fs_get_intvalue(&lidbg_core_list, "fs_updatestate_ms", &ret, NULL);
    printk("[futengfei]get key value:[%d]\n", ret );

    delay = "futengfei1";
    //fs_set_value(&lidbg_core_list, "fs_private_patch", delay);
    //fs_get_value(&lidbg_core_list, "fs_private_patch", &value);
    //printk("[futengfei]warn.test_fileserver_stability:<value=%s>\n", value);
    lidbg_get_current_time(tbuff, NULL);
    fs_file_log("%s\n", tbuff);

    fs_string2file("/data/fs_string2file.txt", "%s\n", tbuff);
    set_machine_id();

    is_file_updated(core_sd_path, &precorefile_tm);
    update_list(core_sd_path, &lidbg_core_list);
    is_file_updated(driver_sd_path, &predriverfile_tm);
    update_list(driver_sd_path, &lidbg_drivers_list);
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
    //set_freezable();
    fs_regist_state("ats", &test_count);
    //fs_regist_state("fs_pollfile_ms", &g_pollfile_ms);
    //fs_regist_state("fs_kmsg_en", &g_pollkmsg_en);
    //fs_regist_state("fs_updatestate_ms", &g_pollstate_ms);
    while(!kthread_should_stop())
    {
        test_fileserver_stability();
        ssleep(1);
        fs_log_sync();
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
        printk("[futengfei]======start_kthread_run1.2.3\n");
        fileserver_test_task3 = kthread_run(thread_fileserver_test, NULL, "ftf_fs_task3");
        msleep(88);
    case 2:
        printk("[futengfei]======start_kthread_run1.2\n");
        fileserver_test_task2 = kthread_run(thread_fileserver_test, NULL, "ftf_fs_task2");
        msleep(230);
    case 1:
        printk("[futengfei]======start_kthread_run1\n");
        fileserver_test_task = kthread_run(thread_fileserver_test, NULL, "ftf_fs_task");
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
    int cmd_para = 0;
    int thread_count = 0;
    if(argc < 3)
    {
        printk("[futengfei]err.lidbg_fileserver_main:echo \"c file 1 1 1\" > /dev/mlidbg0\n");
        return;
    }

    thread_count = simple_strtoul(argv[0], 0, 0);
    cmd = simple_strtoul(argv[1], 0, 0);
    cmd_para = simple_strtoul(argv[2], 0, 0);
    fileserver_thread_test(thread_count);
    switch (cmd)
    {
    case 1:
        fs_log_sync();
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

    case 7:
        if(cmd_para)
            lidbg_mkdir("/data/123");
        else
            lidbg_rmdir("/data/123");
        break;
    case 8:
        if(cmd_para)
            lidbg_touch("/data/123/4.txt");
        else
            lidbg_rm("/data/123/4.txt");
        break;
    case 9:
        if(cmd_para)
            lidbg_cp("/data/123/4.txt", "/data/4.txt");
         else
            lidbg_rm("/data/4.txt");
        break;
    case 10:
        if(cmd_para)
            lidbg_setprop("fly.ftf.test", "1");
        else
            lidbg_setprop("fly.ftf.test", "0");
        break;
    case 11:
        lidbg_chmod("/data");
        break;
    case 13:
        lidbg_reboot();
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
EXPORT_SYMBOL(get_machine_id);
EXPORT_SYMBOL(fs_save_list_to_file);
EXPORT_SYMBOL(fs_regist_filedetec);
EXPORT_SYMBOL(fs_file_separator);
EXPORT_SYMBOL(fs_enable_kmsg);
EXPORT_SYMBOL(fs_string2file);
EXPORT_SYMBOL(fs_save_state);
EXPORT_SYMBOL(fs_dump_kmsg);
EXPORT_SYMBOL(fs_regist_state);
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
