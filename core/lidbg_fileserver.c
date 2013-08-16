
#include "lidbg.h"


//zone below [fileserver]
#define LIDBG_STATE_FILE_PATH "/mnt/sdcard/lidbg_state.txt"
static int g_dubug_on = 0;
LIST_HEAD(lidbg_drivers_list);
LIST_HEAD(lidbg_core_list);

int fileserver_deal_cmd(struct list_head *client_list, enum string_dev_cmd cmd, char *lookfor, char *key, char **string)
{
    //note:you can add more func here,just copy one of the case as belows;
    struct string_dev *pos;
    char *p;
    int find_it = 0;
    if(g_dubug_on)
        printk("\n[futengfei]======fileserver_deal_cmd[%d]\n", cmd);
    if (list_empty(client_list))
    {
        printk("[futengfei]err====list_empty_return(-2);\n");
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
                printk("[futengfei]:suc find key[%s]=[%s]\n", key, *string);
                return 1;
            }
        }
        printk("[futengfei]:err find key[%s]\n", key);
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
                            printk("[futengfei]fileserver:have_find[%s]\n", lookfor);
                            return 1;
                        }
                    }
                    else
                        break;
                }
            }
        }
        printk("[futengfei]fileserver:err_find[%s]\n", lookfor);
        return -1;// break;
    default:
        printk("\n[futengfei]err cmd======fileserver_deal_cmd[%d]\n", cmd);
    }

}
int  fileserver_main(char *filename, enum string_dev_cmd cmd, char *str_append, struct list_head *client_list)
{
    //note: cmd tell me the file mode,if it is config file I will do more;
    struct file *filep;
    struct inode *inode = NULL;
    struct string_dev *add_new_dev;
    mm_segment_t old_fs;
    loff_t fsize;
    umode_t fmode;
    char *token;
    char *file_ptr;
    int all_purpose, flags, is_file_cleard = 0;
    unsigned int file_len;

    printk("\n[futengfei]=IN===================fileserver_main\n");
    if(cmd == FS_CMD_FILE_APPENDMODE)
    {
        if(str_append == NULL)
        {
            printk("[futengfei]err.fileappend_mode========str_append:null_return\n");
            return -1;
        }
        printk("[futengfei]warn:check========fileappend_mode\n");
        filename = LIDBG_STATE_FILE_PATH;
        flags = O_CREAT | O_RDWR | O_APPEND | O_LARGEFILE;
        fmode = 0600;
    }
    else
    {
        flags = O_RDWR;
        fmode = 0;
    }

again:
    filep = filp_open(filename, flags , fmode);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err   open [%s] \n", filename);
        return -1;
    }
    printk("[futengfei]suess  open[%s] \n", filename);

    old_fs = get_fs();
    set_fs(get_ds());

    //two ways to get the fsize
    file_len = filep->f_op->llseek(filep, 0, SEEK_END);
    if(file_len < 0)
        file_len = 0;
    file_len = file_len + 2;
    printk("[futengfei]File length: %d\n", file_len);

    inode = filep->f_dentry->d_inode;
    fsize = inode->i_size;
    printk("[futengfei]File fsize: %d\n", (int)fsize);

    if(cmd == FS_CMD_FILE_APPENDMODE)
    {
        if(file_len > MEM_SIZE_8_MB)
        {
            printk("[futengfei]fileappend_mode========overlow.goto.again\n");
            is_file_cleard = 1;
            flags = O_CREAT | O_RDWR | O_APPEND | O_LARGEFILE | O_TRUNC;
            set_fs(old_fs);
            filp_close(filep, 0);
            goto again;
        }
        filep->f_op->llseek(filep, 0, SEEK_END);//note:to the end to append;
        
        if(1 == is_file_cleard)
        {
            is_file_cleard = 0;
            char *str_warn = "============have_cleard=============\n\n";
            filep->f_op->write(filep, str_warn, strlen(str_warn), &filep->f_pos);
        }
        filep->f_op->write(filep, str_append, strlen(str_append), &filep->f_pos);
        set_fs(old_fs);
        filp_close(filep, 0);
        printk("[futengfei]fileappend_mode========write.over.return\n");
        return 1;
    }

    file_ptr = (unsigned char *)vmalloc(file_len);
    if(file_ptr == NULL)
    {
        printk( "cannot malloc memory!\n");
        return -1;
    }

    filep->f_op->llseek(filep, 0, 0);
    all_purpose = filep->f_op->read(filep, file_ptr, file_len, &filep->f_pos);
    if(all_purpose <= 0)
    {
        printk( "read file data failed\n");
        return -1;
    }
    set_fs(old_fs);
    filp_close(filep, 0);

    file_ptr[file_len - 1] = '\0';
    if(g_dubug_on)
        printk("%s\n", file_ptr);

    printk("[futengfei]start to toke[%s]\n", filename);
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
            printk("droped[%s]\n", token);
    }
    if(cmd == FS_CMD_FILE_CONFIGMODE)
        fileserver_deal_cmd(client_list, FS_CMD_LIST_SPLITKV, NULL, NULL, NULL);

    printk("[futengfei]=OUT===================fileserver_main\n");
    return 1;
}
void fileserverinit_once(void)
{
    //note:your list can only be init once
    char *enable;
    char tbuff[100];
    int ret;
    if(fileserver_main("/flysystem/lib/out/drivers.conf", FS_CMD_FILE_CONFIGMODE, NULL, &lidbg_drivers_list) < 0)
        fileserver_main("/system/lib/modules/out/drivers.conf", FS_CMD_FILE_CONFIGMODE, NULL, &lidbg_drivers_list);
    if(fileserver_main("/flysystem/lib/out/core.conf", FS_CMD_FILE_CONFIGMODE, NULL, &lidbg_core_list) < 0)
        fileserver_main("/system/lib/modules/out/core.conf", FS_CMD_FILE_CONFIGMODE, NULL, &lidbg_core_list);
    lidbg_get_current_time(tbuff);
    fileserver_main(NULL, FS_CMD_FILE_APPENDMODE, tbuff, NULL);
    ret = fileserver_deal_cmd(&lidbg_core_list, FS_CMD_LIST_GETVALUE, NULL, "fs_dbg_enable", &enable);
    if(ret > 0)    g_dubug_on = simple_strtoul(enable, 0, 0);
    printk("[futengfei]==============fileserverinit_once[%d]\n", g_dubug_on);
}
//zone end

//zone below [fileserver_test]
#define LIDBG_KILL_LIST_PATH "/flysystem/lib/out/fastboot_not_kill_list.conf"
LIST_HEAD(kill_list_test);
void test_fileserver_stability(void)
{
    char *delay;
    //test_fileserver_stability
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_SHOW, NULL, NULL, NULL);
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_IS_STRINFILE, "cn.flyaudio.navigation", NULL, NULL);
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_IS_STRINFILE, "cn.flyaudio.navigationfutengfei", NULL, NULL);
    fileserver_deal_cmd(&lidbg_drivers_list, FS_CMD_LIST_SHOW, NULL, NULL, NULL);
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_SHOW, NULL, NULL, NULL);
    fileserver_deal_cmd(&lidbg_drivers_list, FS_CMD_LIST_SHOW, NULL, NULL, NULL);
    fileserver_deal_cmd(&lidbg_drivers_list, FS_CMD_LIST_GETVALUE, NULL, "futengfei", &delay);
    printk("[futengfei]get key value:[%d]\n", simple_strtoul(delay, 0, 0) );
    fileserver_deal_cmd(&lidbg_drivers_list, FS_CMD_LIST_GETVALUE, NULL, "mayanping", &delay);
    printk("[futengfei]get key value:[%d]\n", simple_strtoul(delay, 0, 0) );
    fileserver_deal_cmd(&lidbg_core_list, FS_CMD_LIST_GETVALUE, NULL, "fs_dbg_enable", &delay);
    printk("[futengfei]get key value:[%d]\n", simple_strtoul(delay, 0, 0) );
    fileserver_main(NULL, FS_CMD_FILE_APPENDMODE, "\n###save some state###\n ts=gt801\n", NULL);
    ssleep(3);
}
static struct task_struct *fileserver_test_task;
static int fileserver_test_fk(void *data)
{
    int ret;
    ret = fileserver_main(LIDBG_KILL_LIST_PATH, FS_CMD_FILE_LISTMODE, NULL, &kill_list_test);
    while(ret > 0)
    {
        test_fileserver_stability();
        ssleep(1);
    };
    return 1;
}
void fileserver_thread_test(int zero_return)
{
    printk("[futengfei].%s========%s[sizeof(mysr_dev[%d])=%d]\n", (zero_return ? "on" : "off"), __func__);
    if(!zero_return)
        return ;
    fileserver_test_task = kthread_run(fileserver_test_fk, NULL, "ftf_fileserver_task");
}
//zone end

void lidbg_fileserver_main(int argc, char **argv)
{

    fileserver_thread_test(1);
}


//zone below [EXPORT_SYMBOL]
EXPORT_SYMBOL(lidbg_fileserver_main);
EXPORT_SYMBOL(fileserver_deal_cmd);
EXPORT_SYMBOL (fileserver_main);
EXPORT_SYMBOL (lidbg_drivers_list);
EXPORT_SYMBOL (lidbg_core_list);
//zone end
