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


void show_some_msg()
{
	printk("current: %s ,%d\n",current->comm, current->pid);
}
void creat_file(char *filename, char *txtbuff)
{
    struct file *filep;
    mm_segment_t old_fs;
    char redtxt[100];
    int ret;

    //S1
    filep = filp_open(filename, O_CREAT | O_RDWR, 0);
    if(IS_ERR(filep))
    {
    printk("[futengfei]filp_open[%s] err\n",filename);
        return -1;
    }
    printk("[futengfei]filp_open[%s] suess\n",filename);

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

void launch_exe( int on_off,char bin_path[], char argv1[])
{

    printk("[futengfei].confirm:lunch [%s %s] ===========%s\n",bin_path, argv1, (on_off?"on":"off"));
    if(on_off==0)
    	{
		return ;
	}
		
    char *argv[] = { bin_path, argv1, NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin", NULL };//tell me sh where it is;
    int ret;
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_EXEC);
    //NOTE:  I test that:use UMH_NO_WAIT can't lunch the exe; UMH_WAIT_PROCwill block the ko,
    //UMH_WAIT_EXEC  is recommended.
    if (ret < 0)
                printk("[futengfei].lunch [%s %s] =====%d======fail\n",bin_path, argv1, on_off);
    else
                printk("[futengfei].lunch [%s %s] =====%d======sucess\n",bin_path, argv1, on_off);

}
struct kobject *mstate_kobj;
#define mstate_attr(_name) \
static struct kobj_attribute _name##_attr = {	\
	.attr	= {				\
		.name = __stringify(_name),	\
		.mode = 0644,			\
	},					\
	.show	= lidbg_show,			\
	.store	= lidbg_store,		\
}
	//.show	= _name##_show,	
	//.store	= _name##_store,
static int g_count =0 ;
static ssize_t lidbg_show(struct kobject *kobj, struct kobj_attribute *attr,
			  char *buf)
{
	
	static char *health_text[] = {
	"Unknown", "Good", "Overheat", "Dead", "Over voltage",
	"Unspecified failure", "Cold",
	};
	printk("[futengfei]===========%d[lidbg_show]\n",ARRAY_SIZE(health_text));
	
	g_count++;
	if (g_count<ARRAY_SIZE(health_text))
		g_count=g_count;
	else 
		g_count=0;
	
	return sprintf(buf, "%s\n", health_text[g_count]);
}

static ssize_t lidbg_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t n)
{
	printk("[futengfei]===========[lidbg_store].%d[%s]\n",n,buf);
	return n;
}
mstate_attr(lidbg);
mstate_attr(lidbg2);
static struct attribute *g[] = {
	&lidbg_attr.attr,
	&lidbg2_attr.attr,
	NULL,
};
static struct attribute_group attr_group = {
	.attrs = g,
};
void kernel_creat_sysgroup()
{
	mstate_kobj = kobject_create_and_add("mstate", NULL);
	if (!mstate_kobj)
		{
			printk("[futengfei]===========err=kobject_create[mstate]\n");
			return -ENOMEM;
		}
	printk("[futengfei]===========suc=kobject_create[mstate]\n");
	return sysfs_create_group(mstate_kobj, &attr_group);
}
int kernel_creat_file_init(void)
{
    printk("[futengfei]===IN===============kernel_creat_file_init\n");
    //wait a moment
    ssleep(3);
    show_some_msg();
    creat_file("/mnt/sdcard/love2.txt", "ceshideceshihaishiceshi."); //creat file
    launch_exe(0,"/system/bin/insmod", "/system/lib/modules/wlan.ko"); //lunch userspace exe ,test ok
    // ssleep(2);
    launch_exe(0,"/system/lidbg_servicer", NULL); //lunch userspace exe,test ok
    
    kernel_creat_sysgroup();





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







