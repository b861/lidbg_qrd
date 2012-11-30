


#include "lidbg.h"
#define DEVICE_NAME "lidbg_servicer"

#define FIFO_SIZE (16)
struct kfifo k2u_fifo;
struct kfifo u2k_fifo;

int k2u_fifo_buffer[FIFO_SIZE];
int u2k_fifo_buffer[FIFO_SIZE];

static spinlock_t fifo_k2u_lock;
static spinlock_t fifo_u2k_lock;

unsigned long flags_k2u;
unsigned long flags_u2k;

//http://blog.csdn.net/yjzl1911/article/details/5654893
static struct fasync_struct *fasync_queue;

static struct task_struct *u2k_task;

//static wait_queue_head_t k2u_wait;

static DECLARE_WAIT_QUEUE_HEAD(k2u_wait);

static DECLARE_COMPLETION(u2k_com);


int thread_u2k(void *data)

{

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;

        wait_for_completion(&u2k_com);
        dbg ("wait_for_completion ok\n");



    }
    return 0;
}





static int servicer_fasync(int fd, struct file *filp, int on)
/*fasync\u65b9\u6cd5\u7684\u5b9e\u73b0*/
{
    int retval;
    retval = fasync_helper(fd, filp, on, &fasync_queue);
    /*\u5c06\u8be5\u8bbe\u5907\u767b\u8bb0\u5230fasync_queue\u961f\u5217\u4e2d\u53bb*/
    if(retval < 0)
        return retval;
    return 0;
}



//int cmd2app = SERVICER_DONOTHING;
void k2u_write(int cmd)
{

    if(cmd != SERVICER_DONOTHING)
    {
        dbg ("k2u_write=%d\n", cmd);

        spin_lock_irqsave(&fifo_k2u_lock, flags_k2u);
        //cmd2app = cmd;
        kfifo_in(&k2u_fifo, &cmd, sizeof(int));
        spin_unlock_irqrestore(&fifo_k2u_lock, flags_k2u);

        wake_up(&k2u_wait);

        if (fasync_queue)
            kill_fasync(&fasync_queue, SIGIO, POLL_IN);
    }
}



// read from kernel to user
int k2u_read(void)
{

    int ret;

    spin_lock_irqsave(&fifo_k2u_lock, flags_k2u);
    //ret=cmd2app;
    kfifo_out(&k2u_fifo, &ret, sizeof(int));
    spin_unlock_irqrestore(&fifo_k2u_lock, flags_k2u);
    return ret;

}



// read from  user to  kernel

int u2k_read(void)
{

    int ret;

    spin_lock_irqsave(&fifo_k2u_lock, flags_k2u);
    kfifo_out(&u2k_fifo, &ret, sizeof(int));
    spin_unlock_irqrestore(&fifo_k2u_lock, flags_k2u);
    return ret;

}

/*
int fifo_check_k2u()
{
	int tmp;
	spin_lock_irqsave(&fifo_k2u_lock,flags_k2u);
	tmp = kfifo_is_empty(k2u_fifo);
	spin_unlock_irqrestore(&fifo_k2u_lock,flags_k2u);
	return tmp;

}
*/

#if 0
ssize_t  servicer_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{

    int cmd ;

    spin_lock_irqsave(&fifo_k2u_lock, flags_k2u);

    while(kfifo_is_empty(&k2u_fifo))
    {
        spin_unlock_irqrestore(&fifo_k2u_lock, flags_k2u);
        if(filp->f_flags & O_NONBLOCK)
        {
            return SERVICER_DONOTHING;

        }
        wait_event(k2u_wait, !kfifo_is_empty(&k2u_fifo));

        spin_lock_irqsave(&fifo_k2u_lock, flags_k2u);


    }

    spin_unlock_irqrestore(&fifo_k2u_lock, flags_k2u);


    cmd = k2u_read();


    //lidbg("cmd = %d\n",cmd);
    if (copy_to_user(buffer, &cmd, 4))
    {
        // ret =  - EFAULT;
        lidbg("copy_to_user ERR\n");
    }
    /*
        if(cmd!=SERVICER_DONOTHING)
        {
    		k2u_write(SERVICER_DONOTHING);
    		 //lidbg("cmd2app reset 0\n");
        }
    */
    return size;


}
#else

ssize_t  servicer_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{

    int cmd ;
    cmd = k2u_read();
    //lidbg("cmd = %d\n",cmd);
    if (copy_to_user(buffer, &cmd, 4))
    {
        // ret =  - EFAULT;
        lidbg("copy_to_user ERR\n");
    }
    /*
        if(cmd!=SERVICER_DONOTHING)
        {
    		k2u_write(SERVICER_DONOTHING);
    		 //lidbg("cmd2app reset 0\n");
        }
    */
    return size;


}


#endif

//u2k_write
ssize_t  servicer_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{
    int cmd ;

    if (copy_from_user( &cmd, buffer, 4))
    {
        lidbg("copy_from_user ERR\n");
    }

    spin_lock_irqsave(&fifo_k2u_lock, flags_k2u);
    kfifo_in(&u2k_fifo, &cmd, sizeof(int));
    spin_unlock_irqrestore(&fifo_k2u_lock, flags_k2u);

    complete(&u2k_com);

    return size;
}


int servicer_open(struct inode *inode, struct file *filp)
{
    dbg ("servicer_open\n");

    return 0;
}

#if 0
static int servicer_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{

    return 1;
}
#endif


void lidbg_servicer_main(int argc, char **argv)
{
    u32 cmd = 0;

    if(!strcmp(argv[0], "dmesg"))
    {
        cmd = LOG_DMESG ;
    }
    else if(!strcmp(argv[0], "logcat"))
    {
        cmd = LOG_LOGCAT ;
    }

    k2u_write(cmd);

}


static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
#if 0
    .ioctl	=	log_ioctl,
#endif
    .open   = servicer_open,
    .read   =   servicer_read,
    .write  =  servicer_write,
    .fasync = servicer_fasync,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};
static int __init dev_init(void)
{
    int ret;

    //下面的代码可以自动生成设备节点，但是该节点在/dev目录下，而不在/dev/misc目录下
    //其实misc_register就是用主设备号10调用register_chrdev()的 misc设备其实也就是特殊的字符设备。
    //注册驱动程序时采用misc_register函数注册，此函数中会自动创建设备节点，即设备文件。无需mknod指令创建设备文件。因为misc_register()会调用class_device_create()或者device_create()。

    ret = misc_register(&misc);
    dbg (DEVICE_NAME"servicer dev_init\n");
    DUMP_BUILD_TIME;
    //DECLARE_KFIFO(cmd_fifo);
    //INIT_KFIFO(cmd_fifo);
    dbg ("kfifo_init,FIFO_SIZE=%d\n", FIFO_SIZE);
    kfifo_init(&k2u_fifo, k2u_fifo_buffer, FIFO_SIZE);
    kfifo_init(&u2k_fifo, u2k_fifo_buffer, FIFO_SIZE);
    spin_lock_init(&fifo_k2u_lock);



    u2k_task = kthread_create(thread_u2k, NULL, "u2k_task");
    if(IS_ERR(u2k_task))
    {
        lidbg("Unable to start kernel thread.\n");
        u2k_task = NULL;

    }
    wake_up_process(u2k_task);


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
    return ret;
}

static void __exit dev_exit(void)
{
    misc_deregister(&misc);
    dbg (DEVICE_NAME"servicer  dev_exit\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");


EXPORT_SYMBOL(lidbg_servicer_main);
EXPORT_SYMBOL(k2u_write);
EXPORT_SYMBOL(k2u_read);
EXPORT_SYMBOL(u2k_read);





