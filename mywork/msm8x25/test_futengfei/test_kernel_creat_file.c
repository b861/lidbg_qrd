#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/memory.h>
#include <linux/memory_hotplug.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include<linux/init.h>
#include<linux/string.h>
#include<linux/init.h>
#include<linux/sched.h>
#include <linux/crc-ccitt.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/parport.h>
#include <linux/if_arp.h>
#include <linux/hdlcdrv.h>
#include <linux/baycom.h>
#include <linux/jiffies.h>
#include <linux/random.h>
#include <linux/kmod.h>
#include <net/ax25.h>
#include<asm/uaccess.h> /* get_fs(),set_fs(),get_ds() */

void creat_file(char *filename,char *txtbuff)
{
    struct file *filep;
    mm_segment_t old_fs;
    char redtxt[100];
    int ret;

    printk("[futengfei]start\n");
    //S1
    filep = filp_open(filename, O_CREAT | O_RDWR, 0);
    if(IS_ERR(filep))
    {
        printk("[futengfei]filp_open err\n");
        return -1;
    }
    printk("[futengfei]filp_open suess\n");

    //S2
    old_fs = get_fs();
    set_fs(get_ds());
    //S3
    filep->f_op->llseek(filep, 0, 0);//note,i wil fugai
    filep->f_op->write(filep, txtbuff, strlen(txtbuff), &filep->f_pos);
    filep->f_op->llseek(filep, 0, 0);
    ret = filep->f_op->read(filep, redtxt, strlen(txtbuff), &filep->f_pos);
    //S4
    set_fs(old_fs);
    //S5
    filp_close(filep, 0);

    //over
    if(ret > 0)
    {
        redtxt[ret] = '\0';
        printk("redtxt:%s\n", redtxt);
    }
    else if(ret == 0)
        printk("read nothing.............\n");
    else
    {
        printk("read error\n");
        return -1;
    }
}

void launch_exe( char bin_path[], char argv1[])
{
    char *argv[] = { bin_path, argv1, NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin", NULL };//tell me sh where it is;
    int ret;
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_EXEC);
    //NOTE:  I test that:use UMH_NO_WAIT can't lunch the exe; UMH_WAIT_PROCwill block the ko,
    //UMH_WAIT_EXEC  is recommended.
    if (ret < 0)
        printk("[futengfei].lunch [%s %s] ===========fail\n", bin_path, argv1);
    else
        printk("[futengfei].lunch [%s %s] ===========success\n", bin_path, argv1);

}

int kernel_creat_file_init(void)
{
    printk("[futengfei]===IN===============kernel_creat_file_init\n");
    //wait
    ssleep(3);
    creat_file("/mnt/sdcard/love2.txt","ceshideceshihaishiceshi.");//creat file
    launch_exe("/system/bin/insmod", "/system/lib/modules/wlan.ko"); //lunch userspace exe ,test ok
    ssleep(2);
    launch_exe("/system/lidbg_servicer", NULL); //lunch userspace exe,test ok

    printk("[futengfei]===OUT==============kernel_creat_file_init\n");

    return 0;
}
void kernel_creat_file_exit(void)
{


}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("futengfei 2013.4.18.");
module_init(kernel_creat_file_init);
module_exit(kernel_creat_file_exit);







