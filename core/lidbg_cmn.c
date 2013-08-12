/* Copyright (c) 2012, swlee
 *
 */
#include "lidbg.h"

#define GET_INODE_FROM_FILEP(filp) ((filp)->f_path.dentry->d_inode)

int lidbg_readwrite_file(const char *filename, char *rbuf,
	const char *wbuf, size_t length)
{
	int ret = 0;
	struct file *filp = (struct file *)-ENOENT;
	mm_segment_t oldfs;
	oldfs = get_fs();
	set_fs(KERNEL_DS);

	do {
		int mode = (wbuf) ? O_RDWR : O_RDONLY;
		filp = filp_open(filename, mode, S_IRUSR);

		if (IS_ERR(filp) || !filp->f_op) {
			ret = -ENOENT;
			break;
		}

		if (!filp->f_op->write || !filp->f_op->read) {
			filp_close(filp, NULL);
			ret = -ENOENT;
			break;
		}

		if (length == 0) {
			/* Read the length of the file only */
			struct inode    *inode;

			inode = GET_INODE_FROM_FILEP(filp);
			if (!inode) {
				lidbg(
					"kernel_readwrite_file: Error 2\n");
				ret = -ENOENT;
				break;
			}
			ret = i_size_read(inode->i_mapping->host);
			break;
		}

		if (wbuf) {
			ret = filp->f_op->write(
				filp, wbuf, length, &filp->f_pos);
			if (ret < 0) {
				lidbg(
					"kernel_readwrite_file: Error 3\n");
				break;
			}
		} else {
			ret = filp->f_op->read(
				filp, rbuf, length, &filp->f_pos);
			if (ret < 0) {
				lidbg(
					"kernel_readwrite_file: Error 4\n");
				break;
			}
		}
	} while (0);

	if (!IS_ERR(filp))
		filp_close(filp, NULL);

	set_fs(oldfs);
	lidbg( "kernel_readwrite_file: ret=%d\n", ret);

	return ret;
}



//zone below [fileserver]
#define LIDBG_STATE_FILE_PATH "/data/lidbg_state"
#define LIDBG_CONFIG_FILE_PATH "/flysystem/lib/out/drivers.conf"
static int g_dubug_on = 0;
LIST_HEAD(lidbg_config_list);

int fileserver_deal_cmd(struct list_head *client_list, enum string_dev_cmd cmd, char *lookfor, char *key,char **string)
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
    case FS_CMD_LIST_SPLITKV:
        list_for_each_entry(pos, client_list, tmp_list)
        {
            p = memchr(pos->yourkey, '=', strlen(pos->yourkey));
            pos->yourvalue = p + 1;
            *p = '\0';
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
				*string=pos->yourvalue;
				printk("[futengfei]:suc find key[%s]=[%s]\n", key,*string);
				return 1;
			}
        }
        *string="-1";
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
    char *token;
    char *file_ptr;
    int all_purpose, file_len, flags;

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

    if(cmd == FS_CMD_FILE_APPENDMODE)
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
	all_purpose=0;
    while((token = strsep(&file_ptr, "\n")) != NULL )
    {
		if(strlen(token) > 2 && token[0] != '#')
		{
			if(g_dubug_on)
				printk("%d[%s]\n", all_purpose, token);
			add_new_dev = kzalloc(sizeof(struct string_dev), GFP_KERNEL);
			add_new_dev->yourkey = token;
			list_add(&(add_new_dev->tmp_list), client_list);
			all_purpose++;
		}
    }
    if(cmd == FS_CMD_FILE_CONFIGMODE)
        fileserver_deal_cmd(client_list, FS_CMD_LIST_SPLITKV, NULL, NULL,NULL);

    printk("[futengfei]=OUT===================fileserver_main\n");
    return 1;
}
void fileserver_initonce(void)
{	
	//note:your list can only be init once;I hope all your list init in the zone below
	char *enable;
	fileserver_main(LIDBG_CONFIG_FILE_PATH, FS_CMD_FILE_CONFIGMODE, NULL, &lidbg_config_list);
	fileserver_deal_cmd(&lidbg_config_list, FS_CMD_LIST_GETVALUE, NULL, "fs_dbg_enable",&enable);
	printk("[futengfei]==============fileserver_initonce[%s]\n", enable);
	if(simple_strtoul(enable, 0, 0) > 0)
		g_dubug_on = 1;
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


int lidbg_task_kill_exclude(char *exclude_process, u32 num)
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

        //lidbg( "process %d (%s)\n",p->pid, p->comm);
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


int lidbg_task_kill_select(char *task_name)
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
        //lidbg( "process %d (%s)\n",p->pid, p->comm);

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
    tmp1 = lidbg_get_ns_count();
    msleep(5);
    tmp2 = lidbg_get_ns_count();
    tmp3 = tmp2 - tmp1;
    lidbg ("tmp3=%x \n", tmp3);

}
#endif

u32 lidbg_get_ns_count(void)
{
    struct timespec t_now;
    getnstimeofday(&t_now);
    return t_now.tv_sec * 1000 + t_now.tv_nsec / 1000000;

}

// cmn_launch_user("/system/bin/insmod", "/system/lib/modules/wlan.ko");
// cmn_launch_user("/system/lidbg_servicer", NULL);
int  lidbg_launch_user( char bin_path[], char argv1[])
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
EXPORT_SYMBOL(lidbg_get_ns_count);
EXPORT_SYMBOL(lidbg_launch_user);
EXPORT_SYMBOL(lidbg_readwrite_file);
EXPORT_SYMBOL(lidbg_task_kill_select);

EXPORT_SYMBOL(fileserver_deal_cmd);
EXPORT_SYMBOL (fileserver_main);
EXPORT_SYMBOL (lidbg_config_list);

