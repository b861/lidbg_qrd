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
	//lidbg( "kernel_readwrite_file: ret=%d\n", ret);

	return ret;
}



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

void create_new_proc_entry(void)
{
    create_proc_read_entry("ps_list", 0, NULL, read_proc, NULL);
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

int lidbg_get_current_time(char *time_string,struct rtc_time *ptm)
{
    int  tlen=-1;
    struct timespec ts;
    struct rtc_time tm;
    getnstimeofday(&ts);
    rtc_time_to_tm(ts.tv_sec, &tm);
    if(time_string)
        tlen = sprintf(time_string, "%d-%02d-%02d %02d:%02d:%02d",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour , tm.tm_min, tm.tm_sec);
    if(ptm)
        *ptm = tm;
    return tlen;
}
int  lidbg_launch_user( char bin_path[], char argv1[],char argv2[],char argv3[],char argv4[],char argv5[],char argv6[])
{
    char *argv[] = { bin_path, argv1,argv2, argv3, argv4, argv5, argv6,NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin:/sbin", NULL };//tell me sh where it is;
    int ret;
    lidbg("%s ,%s\n", bin_path, argv1);
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_EXEC);
    //NOTE:  I test that:use UMH_NO_WAIT can't lunch the exe; UMH_WAIT_PROCwill block the ko,
    //UMH_WAIT_EXEC  is recommended.
    if (ret < 0)
        lidbg("lunch [%s %s] fail!\n", bin_path, argv1);
    //else
    //    lidbg("lunch [%s %s] success!\n", bin_path, argv1);
    return ret;
}

int  lidbg_exe(char path[])
{
    return lidbg_launch_user(path, NULL, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_mount(char path[])
{
    return lidbg_launch_user(MOUNT_PATH, "-o", "remount",path, NULL, NULL, NULL);
}
int  lidbg_insmod(char path[])
{
    return lidbg_launch_user(INSMOD_PATH, path, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_chmod(char path[])
{
    return lidbg_launch_user(CHMOD_PATH, "777", path, NULL, NULL, NULL, NULL);
}
int  lidbg_mv(char from[],char to[])
{
    return lidbg_launch_user(MV_PATH, from, to, NULL, NULL, NULL, NULL);
}
int  lidbg_rm(char path[])
{
    return lidbg_launch_user(RM_PATH, path, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_rmdir(char path[])
{
    return lidbg_launch_user(RMDIR_PATH, path, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_mkdir(char path[])
{
    return lidbg_launch_user(MKDIR_PATH, path, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_touch(char path[])
{
    return lidbg_launch_user(TOUCH_PATH, path, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_reboot(void)
{
    return lidbg_launch_user(REBOOT_PATH, NULL, NULL, NULL, NULL, NULL, NULL);
}
int  lidbg_setprop(char key[],char value[])
{
    return lidbg_launch_user(SETPROP_PATH, key, value, NULL, NULL, NULL, NULL);
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
            static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin:/sbin", NULL };
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
    LIDBG_MODULE_LOG;
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


EXPORT_SYMBOL(lidbg_exe);
EXPORT_SYMBOL(lidbg_mount);
EXPORT_SYMBOL(lidbg_insmod);
EXPORT_SYMBOL(lidbg_chmod);
EXPORT_SYMBOL(lidbg_mv);
EXPORT_SYMBOL(lidbg_rm);
EXPORT_SYMBOL(lidbg_rmdir);
EXPORT_SYMBOL(lidbg_mkdir);
EXPORT_SYMBOL(lidbg_touch);
EXPORT_SYMBOL(lidbg_reboot);
EXPORT_SYMBOL(lidbg_setprop);

EXPORT_SYMBOL(mod_cmn_main);
EXPORT_SYMBOL(lidbg_get_ns_count);
EXPORT_SYMBOL(lidbg_launch_user);
EXPORT_SYMBOL(lidbg_readwrite_file);
EXPORT_SYMBOL(lidbg_task_kill_select);
EXPORT_SYMBOL(lidbg_get_current_time);
