#include "lidbg.h"

LIDBG_DEFINE;

#define RUN_ACCBOOT
#define DEVICE_NAME "lidbg_acc"

static DECLARE_COMPLETION(acc_ready);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
DECLARE_MUTEX(lidbg_acc_sem);
#else
DEFINE_SEMAPHORE(lidbg_acc_sem);
#endif

static struct task_struct *acc_task;
static int thread_acc(void *data);


typedef struct
{
	unsigned int  acc_flag;
} lidbg_acc;

lidbg_acc *plidbg_acc = NULL;


void acc_pwroff(void)
{
	DUMP_FUN_ENTER;

#ifdef RUN_ACCBOOT
	lidbg("send CMD_FAST_POWER_OFF  to lidbg_server\n");
	SOC_Write_Servicer(CMD_FAST_POWER_OFF);
#endif
}



int thread_acc(void *data)
{
    lidbg("thread_acc.\n");
    plidbg_acc = ( lidbg_acc *)kmalloc(sizeof(lidbg_acc), GFP_KERNEL);
    memset(plidbg_acc, 0, sizeof(*plidbg_acc));

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            	wait_for_completion(&acc_ready);
		acc_pwroff();
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


ssize_t  acc_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
    return size;
}

ssize_t  acc_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{

	lidbg("acc_write.\n");
	down(&lidbg_acc_sem);

	/*memset(&(plidbg_acc->acc_flag), 0, sizeof(plidbg_acc->acc_flag));
	if(copy_from_user(&(plidbg_acc->acc_flag), buffer, size))
	{
		lidbg("copy_from_user ERR\n");
	}
	lidbg("acc write %s\n", plidbg_acc->acc_flag)*/

	up(&lidbg_acc_sem);

	complete(&acc_ready);

	return size;
}


int acc_open(struct inode *inode, struct file *filp)
{
    //down(&lidbg_msg_sem);

    return 0;
}

int acc_release(struct inode *inode, struct file *filp)
{
    //up(&lidbg_msg_sem);
    return 0;
}




static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   =   acc_open,
    .read   =   acc_read,
    .write  =   acc_write,
    .release =  acc_release,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};


static int __init acc_init(void)
{
	int ret;
	LIDBG_GET;

	INIT_COMPLETION(acc_ready);
	ret = misc_register(&misc);

	acc_task = kthread_create(thread_acc, NULL, "acc_task");
	if(IS_ERR(acc_task))
	{
		 lidbg("Unable to start kernel thread.\n");
	}
	else wake_up_process(acc_task);

	lidbg_chmod("/dev/lidbg_acc");
	
	lidbg (DEVICE_NAME"acc  dev_init\n");

	return ret;
}

static void __exit acc_exit(void)
{
	misc_deregister(&misc);
    	lidbg (DEVICE_NAME"acc  dev_exit\n");
}



module_init(acc_init);
module_exit(acc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("mstar lidbg_acc driver");
