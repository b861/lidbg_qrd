/* Copyright (c) 2012, swlee
 *
 */
#include "lidbg.h"



//zone below [fileserver]
#define LIDBG_KILL_LIST_PATH "/flysystem/lib/out/lidbg_kill_list"
#define LIDBG_STATE_FILE_PATH "/data/lidbg_state"
#define LIDBG_STRING_DEV_MAX 500
struct string_dev mysr_dev[LIDBG_STRING_DEV_MAX];//sizeof(mysr_dev[10000])=16?
static int dev_pre_pos = 0;
static int g_dubug_on = 0;
LIST_HEAD(kill_list_test);
LIST_HEAD(lidbg_config_list);

static int atoi(const char *name)
{
    int val = 0;
    for (;; name++)
    {
        switch (*name)
        {
        case '0' ... '9':
            val = 10 * val + (*name - '0');
            break;
        default:
            return val;
        }
    }
}

int fileserver_deal_cmd(struct list_head *client_list, enum string_dev_cmd cmd, char *lookfor, char *key)
{

    //note:you can add more func here,just copy one of the case as belows;
    printk("\n[futengfei]======fileserver_deal_cmd[%d]\n", cmd);
    struct string_dev *pos;
    char *p;
    int find_it = 0;
	if (list_empty(client_list))
	{
		printk("[futengfei]err====list_empty_return(-2);\n");
		return -2;
	}
    switch (cmd)
    {
    case fs_cmd_list_splitkv:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            p = memchr(pos->yourkey, '=', strlen(pos->yourkey));
            pos->yourvalue = p + 1;
            *p = '\0';
        }

        break;
    case fs_cmd_list_show:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if(pos->yourvalue)
                printk("[%s]=[%s]\n", pos->yourkey, pos->yourvalue);
            else
                printk("[%s]\n", pos->yourkey);
        }

        break;
    case fs_cmd_list_getvalue:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            if (!strcmp(pos->yourkey, key))
            {
                printk("[futengfei]:suc find key[%s]=[%s]\n", key, pos->yourvalue);
                return atoi(pos->yourvalue);
            }
        }
        printk("[futengfei]:err find key[%s]\n", key);
        return -1;
    case fs_cmd_list_is_strinfile:
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
    mm_segment_t old_fs;
    loff_t fsize;
    char *token;
    char *file_ptr;
    int all_purpose, file_len, flags;

    printk("\n[futengfei]=IN===================fileserver_main\n");
    if(cmd == fs_cmd_file_appendmode)
    {
        if(str_append == NULL)
        {
            printk("[futengfei]err.fileappend_mode========str_append:null_return\n");
            return -1;
        }
        printk("[futengfei]warn:check========fileappend_mode\n");
        filename = LIDBG_STATE_FILE_PATH;
        flags = O_CREAT | O_RDWR;//|O_APPEND
    }
    else
        flags = O_RDWR;

    filep = filp_open(filename, flags , 0);
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

    if(cmd == fs_cmd_file_appendmode)
    {
        filep->f_op->llseek(filep, 0, SEEK_END);//note:to the end to append;
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
    while((token = strsep(&file_ptr, "\n")) != NULL )
    {
        if(strlen(token) > 2 && token[0] != '#')
        {
            if(g_dubug_on)
                printk("%d[%s]---[%d][%d]\n", dev_pre_pos, token, sizeof(token), strlen(token));
            mysr_dev[dev_pre_pos].yourkey = token;
            list_add(&(mysr_dev[dev_pre_pos].tmp_list), client_list);
            dev_pre_pos++;
        }
    }
    if(cmd == fs_cmd_file_configmode)
        fileserver_deal_cmd(client_list, fs_cmd_list_splitkv, NULL, NULL);

    printk("[futengfei]=OUT===================fileserver_main\n");
    return 1;
}
void fileserver_initonce(void)
{	
	//note:your list can only be init once;I hope all your list init in the zone below
	printk("[futengfei]==============fileserver_initonce\n");
//	fileserver_main(LIDBG_KILL_LIST_PATH, fs_cmd_file_listmode, NULL, &kill_list_test);
	fileserver_main(LIDBG_CONFIG_FILE_PATH, fs_cmd_file_configmode, NULL, &lidbg_config_list);
}
void fileserver_example(void)
{

    //fileserver_example
    fileserver_deal_cmd(&kill_list_test, fs_cmd_list_show, NULL, NULL);
    fileserver_deal_cmd(&kill_list_test, fs_cmd_list_is_strinfile, "cn.flyaudio.navigation,", NULL);
    fileserver_deal_cmd(&kill_list_test, fs_cmd_list_is_strinfile, "cn.flyaudio.navigation,futengfei", NULL);
    fileserver_deal_cmd(&lidbg_config_list, fs_cmd_list_show, NULL, NULL);
    fileserver_deal_cmd(&kill_list_test, fs_cmd_list_show, NULL, NULL);
    fileserver_deal_cmd(&lidbg_config_list, fs_cmd_list_show, NULL, NULL);
    printk("[futengfei]get key value:[%d]\n", fileserver_deal_cmd(&lidbg_config_list, fs_cmd_list_getvalue, NULL, "futengfei"));
    printk("[futengfei]get key value:[%d]\n", fileserver_deal_cmd(&lidbg_config_list, fs_cmd_list_getvalue, NULL, "mayanping"));

    //append a string to a file to save some state;
    fileserver_main(NULL, fs_cmd_file_appendmode, "\n###save some state###\n ts=gt801\n", NULL);

}
static struct task_struct *fileserver_test_task;
static int fileserver_test_fk(void *data)
{
 ssleep(80);
    while(1)
    {
        fileserver_example();
        ssleep(1);
    };
    return 1;
}
void fileserver_thread_test(int zero_return)
{
    printk("[futengfei].%s========%s[sizeof(mysr_dev[%d])=%d]\n", (zero_return ? "on" : "off"), __func__,LIDBG_STRING_DEV_MAX,sizeof(mysr_dev[LIDBG_STRING_DEV_MAX]));
    if(!zero_return)
        return ;
    fileserver_test_task = kthread_run(fileserver_test_fk, NULL, "ftf_fileserver_task");
}
//zone end

int read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len = 0;
    struct task_struct *task_list;

    for_each_process(task_list)
    {
        len  += sprintf(buf + len, "%s %d \n", task_list->comm, task_list->pid);
    }
    return len;
}

void create_new_proc_entry()
{
    create_proc_read_entry("ps_list", 0, NULL, read_proc, NULL);

}


int cmn_task_kill_exclude(char *exclude_process, u32 num)
{
    struct task_struct *p;
    struct mm_struct *mm;
    struct signal_struct *sig;
    u32 i;
    bool safe_flag = 0;
    DUMP_FUN_ENTER;

    lidbg("exclude_process_num = %d\n", num);

    //read_lock(&tasklist_lock);
    for_each_process(p)
    {
        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        //printk( "process %d (%s)\n",p->pid, p->comm);
        safe_flag = 0;

        for(i = 0; i < num; i++)
        {
            if(!strcmp(p->comm, exclude_process[i]))
            {
                safe_flag = 1;
                break;
            }
        }
        if(safe_flag == 0)
        {
            lidbg("find %s to kill\n", p->comm);

            if (p)
            {
                force_sig(SIGKILL, p);
            }
        }
        //read_unlock(&tasklist_lock);
    }
    DUMP_FUN_LEAVE;
    return 1;

}


int cmn_task_kill_select(char *task_name)
{
    struct task_struct *p;
    struct task_struct *selected = NULL;
    DUMP_FUN_ENTER;

    //read_lock(&tasklist_lock);
    for_each_process(p)
    {
        struct mm_struct *mm;
        struct signal_struct *sig;

        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        selected = p;
        //printk( "process %d (%s)\n",p->pid, p->comm);

        if(!strcmp(p->comm, task_name))
        {
            lidbg("find %s to kill\n", task_name);

            if (selected)
            {
                force_sig(SIGKILL, selected);
                return 1;
            }
        }
        //read_unlock(&tasklist_lock);
    }
    DUMP_FUN_LEAVE;
    return 0;
}

#if 0
{
    int tmp1, tmp2, tmp3;
    tmp1 = GetNsCount();
    msleep(5);
    tmp2 = GetNsCount();
    tmp3 = tmp2 - tmp1;
    lidbg ("tmp3=%x \n", tmp3);

}
#endif

u32 GetNsCount(void)
{
    struct timespec t_now;
    getnstimeofday(&t_now);
    return t_now.tv_sec * 1000 + t_now.tv_nsec / 1000000;

}

// cmn_launch_user("/system/bin/insmod", "/system/lib/modules/wlan.ko");
// cmn_launch_user("/system/lidbg_servicer", NULL);
int  cmn_launch_user( char bin_path[], char argv1[])
{
    char *argv[] = { bin_path, argv1, NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin", NULL };//tell me sh where it is;
    int ret;
    lidbg("%s ,%s\n", bin_path, argv1);
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_EXEC);
    //NOTE:  I test that:use UMH_NO_WAIT can't lunch the exe; UMH_WAIT_PROCwill block the ko,
    //UMH_WAIT_EXEC  is recommended.
    if (ret < 0)
        lidbg("lunch [%s %s] fail!\n", bin_path, argv1);
    else
        lidbg("lunch [%s %s] success!\n", bin_path, argv1);
    return ret;
}

void mod_cmn_main(int argc, char **argv)
{

    if(!strcmp(argv[0], "user"))
    {

        if(argc < 2)
        {
            lidbg("Usage:\n");
            lidbg("bin_path\n");
            lidbg("bin_path argv1\n");
            return;

        }

        {
            char *argv2[] = { argv[1], argv[2], argv[3], argv[4], NULL };
            static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin", NULL };
            int ret;
            lidbg("%s ,%s ,%s ,%s\n", argv[1], argv[2], argv[3], argv[4]);

            ret = call_usermodehelper(argv[1], argv2, envp, UMH_WAIT_PROC);
            if (ret < 0)
                lidbg("lunch fail!\n");
            else
                lidbg("lunch  success!\n");
        }

    }

    return;
}


static int __init cmn_init(void)
{
	fileserver_initonce();
    fileserver_thread_test(0);
    DUMP_BUILD_TIME;
    create_new_proc_entry();
    return 0;
}

static void __exit cmn_exit(void)
{
    remove_proc_entry("proc_entry", NULL);
}

module_init(cmn_init);
module_exit(cmn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");


EXPORT_SYMBOL(mod_cmn_main);
EXPORT_SYMBOL(GetNsCount);
EXPORT_SYMBOL(cmn_launch_user);
EXPORT_SYMBOL(fileserver_deal_cmd);
EXPORT_SYMBOL (fileserver_main);
EXPORT_SYMBOL (lidbg_config_list);

