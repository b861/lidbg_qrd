/* Copyright (c) 2012, swlee
 *
 */



#include "lidbg.h"
#define DEVICE_NAME "mlidbg_cmn"

#ifdef _LIGDBG_SHARE__
LIDBG_SHARE_DEFINE;
#endif



int read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    int len = 0;
    struct task_struct *task_list;


    for_each_process(task_list)
    {

        len  += sprintf(buf + len, "%s %d \n", task_list->comm, task_list->pid);
		//msleep(100);//for test
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

u32 GetNsCount(void)
{
    struct timespec t_now;
    getnstimeofday(&t_now);
    return t_now.tv_sec * 1000 + t_now.tv_nsec / 1000000;

}


ssize_t  cmn_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{


    return size;
}

ssize_t  cmn_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{

    return size;
}


int cmn_open(struct inode *inode, struct file *filp)
{

    return 0;
}

#if 0
static int cmn_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{

    return 1;
}
#endif

void mod_cmn_main(int argc, char **argv)
{


    return;
}

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
#if 0
    .ioctl	=	cmn_ioctl,
#endif
    .open   = cmn_open,
    .read   =   cmn_read,
    .write  =  cmn_write,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};




static void share_set_func_tbl(void)
{
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnmod_cmn_main = mod_cmn_main;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfncmn_task_kill_select = cmn_task_kill_select;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfncmn_task_kill_exclude = cmn_task_kill_exclude;

    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_mem_main = lidbg_mem_main;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnwrite_phy_addr = write_phy_addr;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnwrite_phy_addr_bit = write_phy_addr_bit;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnread_phy_addr = read_phy_addr;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnread_phy_addr_bit = read_phy_addr_bit;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnread_virt_addr = read_virt_addr;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnwrite_virt_addr = write_virt_addr;

    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_display_main = lidbg_display_main;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_get_screen_res = soc_get_screen_res;


}






static int __init cmn_init(void)
{
    int ret;
    DUMP_BUILD_TIME;

    //下面的代码可以自动生成设备节点，但是该节点在/dev目录下，而不在/dev/misc目录下
    //其实misc_register就是用主设备号10调用register_chrdev()的 misc设备其实也就是特殊的字符设备。
    //注册驱动程序时采用misc_register函数注册，此函数中会自动创建设备节点，即设备文件。无需mknod指令创建设备文件。因为misc_register()会调用class_device_create()或者device_create()。
#ifdef _LIGDBG_SHARE__
    LIDBG_SHARE_GET;
    share_set_func_tbl();
#endif

    ret = misc_register(&misc);
    dbg (DEVICE_NAME"cmn dev_init\n");
#if 0
    {
        int tmp1, tmp2, tmp3;
        tmp1 = GetNsCount();
        msleep(5);
        tmp2 = GetNsCount();
        tmp3 = tmp2 - tmp1;
        dbg ("tmp3=%x \n", tmp3);

    }
#endif
    create_new_proc_entry();


    return ret;
}

static void __exit cmn_exit(void)
{
    misc_deregister(&misc);

    remove_proc_entry("proc_entry", NULL);
    dbg (DEVICE_NAME"cmn  dev_exit\n");
}

module_init(cmn_init);
module_exit(cmn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");

#ifndef _LIGDBG_SHARE__

EXPORT_SYMBOL(mod_cmn_main);
EXPORT_SYMBOL(GetNsCount);

#endif

