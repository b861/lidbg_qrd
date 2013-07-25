/* Copyright (c) 2012, swlee
 *
 */
#include "lidbg.h"

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
void cmn_launch_user( char bin_path[], char argv1[])
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

