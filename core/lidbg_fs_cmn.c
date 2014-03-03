
#include "lidbg.h"
//zone below [fs.cmn.tools]
LIST_HEAD(fs_filedetec_list);
LIST_HEAD(fs_filename_list);
static struct task_struct *fs_fdetectask;
static spinlock_t  new_item_lock;
int g_filedetec_dbg = 0;
int g_filedetect_ms = 10000;
//zone end

//zone below [fs.fs_filename_list]
struct fs_filename_item *get_filename_list_item(struct list_head *client_list, const char *filename)
{
    struct fs_filename_item *pos;
    unsigned long flags;

    spin_lock_irqsave(&new_item_lock, flags);
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (!strcmp(pos->filename, filename))
        {
            spin_unlock_irqrestore(&new_item_lock, flags);
            return pos;
        }
    }
    spin_unlock_irqrestore(&new_item_lock, flags);
    return NULL;
}
bool new_filename_list_item(struct list_head *client_list, char *filename, bool copy_en)
{
    struct fs_filename_item *add_new_item;
    unsigned long flags;

    add_new_item = kmalloc(sizeof(struct fs_filename_item), GFP_ATOMIC);
    if(add_new_item == NULL)
    {
        FS_ERR("<kmalloc.fs_filename_item>\n");
        return false;
    }

    add_new_item->filename = kmalloc(strlen(filename) + 1, GFP_ATOMIC);
    if(add_new_item->filename == NULL)
    {
        FS_ERR("<kmalloc.strlen(name)>\n");
        return false;
    }

    strcpy(add_new_item->filename, filename);
    add_new_item->copy_en = copy_en;

    spin_lock_irqsave(&new_item_lock, flags);
    list_add(&(add_new_item->tmp_list), client_list);
    spin_unlock_irqrestore(&new_item_lock, flags);
    //FS_SUC("<NEW:[%s,%d]>\n",filename,copy_en);
    return true;
}

bool register_filename_list(struct list_head *client_list, char *filename, bool copy_en)
{
    struct fs_filename_item *pos = get_filename_list_item(client_list, filename);
    if(pos)
    {
        FS_ERR("<EEXIST.%s>\n", filename);
        return false;
    }
    else
        return new_filename_list_item(client_list, filename, copy_en);
}
void show_filename_list(struct list_head *client_list)
{
    int index = 0;
    struct fs_filename_item *pos;

    if(!list_empty(client_list))
    {
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (pos->filename)
            {
                index++;
                FS_WARN("<%d.%dINFO:[%s]>\n", pos->copy_en, index, pos->filename);
            }
        }
    }
    else
        FS_ERR("<nobody_register>\n");
}
//zone end

//zone below [fs.cmn.driver]
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
        if(0)
            FS_WARN("%d,%s\n", file_len, rbuff);
    }
    set_fs(old_fs);
    filp_close(filep, 0);
    return file_len;
}
bool clear_file(char *filename)
{
    struct file *filep;
    filep = filp_open(filename, O_CREAT | O_RDWR | O_TRUNC , 0777);
    if(IS_ERR(filep))
        return false;
    else
        filp_close(filep, 0);
    return true;
}
bool get_file_mftime(const char *filename, struct rtc_time *ptm)
{
    struct file *filep;
    struct inode *inode = NULL;
    struct timespec mtime;
    filep = filp_open(filename, O_RDONLY , 0);
    if(IS_ERR(filep))
    {
        lidbg("[futengfei]err.open:<%s>\n", filename);
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
bool is_file_tm_updated(const char *filename, struct rtc_time *pretm)
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
    if(g_mem_dbg)
        fs_string2file(0,DEBUG_MEM_FILE, "%s=%d \n", __func__, sizeof(struct string_dev));

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
        if (pos->filedetec && pos->cb_filedetec && fs_is_file_exist(pos->filedetec) )
        {
            if(g_filedetec_dbg)
                FS_WARN("<should call :%s>\n", pos->filedetec);
            if (!pos->is_warned)
            {
                pos->cb_filedetec(pos->filedetec);
                if(g_filedetec_dbg)
                    FS_WARN("<have called :%s>\n", pos->filedetec);
            }
            pos->is_warned = true;
        }
        else
            pos->is_warned = false;
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
    FS_WARN("<thread start>\n");
    if(g_filedetec_dbg)
        show_filedetec_list();
    while(!kthread_should_stop())
    {
        if(g_filedetect_ms && is_fs_work_enable)
        {
            call_filedetec_cb();
            msleep(g_filedetect_ms);
        }
        else
        {
            ssleep(1);
        }
    }
    return 1;
}
bool is_file_exist(char *file)
{
    struct file *filep;
    filep = filp_open(file, O_RDONLY , 0);
    if(IS_ERR(filep))
        return false;
    else
    {
        filp_close(filep, 0);
        return true;
    }
}
int fs_get_file_size(char *file)
{
    struct file *filep;
    struct inode *inodefrom = NULL;
    unsigned int file_len;
    filep = filp_open(file, O_RDONLY , 0);
    if(IS_ERR(filep))
        return -1;
    else
    {
        inodefrom = filep->f_dentry->d_inode;
        file_len = inodefrom->i_size;
        filp_close(filep, 0);
        return file_len;
    }
}
bool copy_file(char *from, char *to)
{
    char *string = NULL;
    unsigned int file_len;
    struct file *pfilefrom;
    struct file *pfileto;
    struct inode *inodefrom = NULL;
    mm_segment_t old_fs;

    if(!fs_is_file_exist(from))
    {
        FS_ERR("<file_miss:%s>\n", from);
        return false;
    }

    pfilefrom = filp_open(from, O_RDONLY , 0);
    pfileto = filp_open(to, O_CREAT | O_RDWR | O_TRUNC, 0777);
    if(IS_ERR(pfileto))
    {
        lidbg_rm(to);
        msleep(500);
        pfileto = filp_open(to, O_CREAT | O_RDWR | O_TRUNC, 0777);
        if(IS_ERR(pfileto))
        {
            FS_ERR("fail to open <%s>\n", to);
            filp_close(pfilefrom, 0);
            return false;
        }
    }
    old_fs = get_fs();
    set_fs(get_ds());
    inodefrom = pfilefrom->f_dentry->d_inode;
    file_len = inodefrom->i_size;

    string = (unsigned char *)vmalloc(file_len);
    if(string == NULL)
        return false;

    if(g_mem_dbg)
        fs_string2file(0,DEBUG_MEM_FILE, "free.%s=%d \n", __func__, file_len);

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
bool fs_is_file_updated(char *filename, char *infofile)
{
    char pres[64], news[64];
    memset(pres, '\0', sizeof(pres));
    memset(news, '\0', sizeof(news));

    get_file_tmstring(filename, news);

    if(fs_get_file_size(infofile) > 0)
    {
        if ( (readwrite_file(infofile, NULL, pres, sizeof(pres)) > 0) && (!strcmp(news, pres)))
            return false;
    }
    fs_clear_file(infofile);
    bfs_file_amend(infofile, news,0);
    return true;
}
void cb_kv_filedetecen(char *key, char *value)
{
    FS_WARN("<%s=%s>\n", key, value);
    if ( (!strcmp(key, "fs_dbg_file_detect" ))  &&  (strcmp(value, "0" )) )
        show_filedetec_list();
}
void cp_data_to_udisk(void)
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
        //lidbg_rm("/data/kmsg.txt");
        lidbg_domineering_ack();
    }
    else
        FS_ERR("<nobody_register>\n");
}

//zone end



//zone below [fs.cmn.interface]
void fs_regist_filedetec(char *filename, void (*cb_filedetec)(char *filename ))
{
    if(filename && cb_filedetec)
        regist_filedetec(filename, cb_filedetec);
    else
        FS_ERR("<filename||cb_filedetec:null?>\n");
}
bool fs_copy_file(char *from, char *to)
{
    return copy_file(from, to);
}
bool fs_is_file_exist(char *file)
{
    return is_file_exist(file);
}
bool fs_clear_file(char *filename)
{
    return clear_file(filename);
}
int fs_readwrite_file(const char *filename, char *wbuff, char *rbuff, int readlen)
{
    return readwrite_file(filename, wbuff, rbuff, readlen);
}
int  lidbg_cp(char from[], char to[])
{
    return fs_copy_file(from, to);
}
bool fs_register_filename_list(char *filename, bool copy_en)
{
    if(filename)
        return register_filename_list(&fs_filename_list, filename, copy_en);
    else
    {
        FS_ERR("<filename.null>\n");
        return false;
    }
}
void fs_show_filename_list(void)
{
    show_filename_list(&fs_filename_list);
}
int thread_cp_data_to_udiskt(void *data)
{
    cp_data_to_udisk();
    return 0;
}
void fs_cp_data_to_udisk(void)
{
    CREATE_KTHREAD(thread_cp_data_to_udiskt, NULL);
}
//zone end



void lidbg_fs_cmn_init(void)
{
    spin_lock_init(&new_item_lock);
    fs_get_intvalue(&lidbg_core_list, "fs_filedetect_ms", &g_filedetect_ms, NULL);
    fs_get_intvalue(&lidbg_core_list, "fs_filedetect_dbg", &g_filedetec_dbg, cb_kv_filedetecen);
    fs_fdetectask = kthread_run(thread_filedetec_func, NULL, "ftf_fdetectask");
}


EXPORT_SYMBOL(lidbg_cp);
EXPORT_SYMBOL(fs_cp_data_to_udisk);
EXPORT_SYMBOL(fs_readwrite_file);
EXPORT_SYMBOL(fs_regist_filedetec);
EXPORT_SYMBOL(fs_copy_file);
EXPORT_SYMBOL(fs_is_file_exist);
EXPORT_SYMBOL(fs_is_file_updated);
EXPORT_SYMBOL(fs_clear_file);
EXPORT_SYMBOL(fs_filename_list);
EXPORT_SYMBOL(fs_register_filename_list);
EXPORT_SYMBOL(fs_show_filename_list);
EXPORT_SYMBOL(fs_get_file_size);


