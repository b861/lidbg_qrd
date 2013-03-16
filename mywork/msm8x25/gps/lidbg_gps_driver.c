#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"

#include "lidbg_enter.h"

LIDBG_DEFINE;
#endif

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/spinlock.h>
#include <linux/i2c.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/semaphore.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define DEVICE_NAME "ubloxgps"

struct gps_device
{
    char *name;
    unsigned int counter;
    wait_queue_head_t queue;
    struct semaphore sem;
    struct cdev cdev;
};


static char  gps_data[512] = { 0 };
static char  num_avi_gps_data[2] = { 0 };
static int    avi_gps_data_hl = 0;
int thread_gps_server(void *data);


//global variable
struct gps_device *dev;
int data = 8;


int gps_open (struct inode *inode, struct file *filp)
{
    //do nothing
    filp->private_data = dev;
    printk("[futengfei]==================gps_open\n");

    return 0;          /* success */
}


/*
 * Data management: read and write
 */
ssize_t gps_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct gps_device *dev = filp->private_data;
    int ret;

    //printk("[futengfei]==================gps_read %d,%d\n", count, avi_gps_data_hl);

    if (avi_gps_data_hl == 0)
    {
        if(wait_event_interruptible(dev->queue, avi_gps_data_hl > 0))
            return -ERESTARTSYS;
    }

    down(&dev->sem);
    if (avi_gps_data_hl > 0)
    {
        copy_to_user(buf, gps_data, avi_gps_data_hl);
        ret = avi_gps_data_hl;
        avi_gps_data_hl = 0;
    }
    else
    {
        avi_gps_data_hl = 0;
        ret =  -1;
    }
    up(&dev->sem);
out:
    printk("gps_read:%dreturn %d byte\n", count, ret);
    return ret;
}



ssize_t gps_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct gps_device *dev = filp->private_data;
    printk("[futengfei]==================gps_write\n");

    printk("[futengfei] gps_data=>%s\n", gps_data);

    return count;
}

static unsigned int gps_poll(struct file *filp, struct poll_table_struct *wait)
{
    struct gps_device *dev = filp->private_data;
    unsigned int mask = 0;

    down(&dev->sem);
    poll_wait(filp, &dev->queue, wait);

    if (avi_gps_data_hl > 0 && avi_gps_data_hl <= 512)
        mask |= POLLIN | POLLRDNORM;

    up(&dev->sem);
    return mask;
}

static  struct file_operations gps_fops =
{
    .owner = THIS_MODULE,
    .read = gps_read,
    .write = gps_write,
    .poll = gps_poll,
    .open = gps_open,
};
int thread_gps_server(void *data)
{
    int i = 0;
    //purpse to clear the ublox buffer
again1:
    SOC_I2C_Rec(1, 0x42, 0xfd, num_avi_gps_data, 2);
    avi_gps_data_hl = (num_avi_gps_data[0] << 8) + num_avi_gps_data[1];
    if (avi_gps_data_hl > 512)
    {
        avi_gps_data_hl = 512;
        SOC_I2C_Rec_Simple(1, 0x42, gps_data, 512);
        goto again1;
    }
    SOC_I2C_Rec_Simple(1, 0x42, gps_data, avi_gps_data_hl);
    printk("[futengfei]================go to while(1)\n");
    while(1)
    {
        // set_current_state(TASK_UNINTERRUPTIBLE);
        //  if(kthread_should_stop()) break;
        //my thread item

        SOC_I2C_Rec(1, 0x42, 0xfd, num_avi_gps_data, 2);
        down(&dev->sem);
        avi_gps_data_hl = (num_avi_gps_data[0] << 8) + num_avi_gps_data[1];
        printk("[futengfei]================avi_gps_data_hl[%d]\n", avi_gps_data_hl);

        //printk("[futengfei] gps_data[%d]  REG:0xfd 0xfe =>0x%x 0x%x ]\n", avi_gps_data_hl, num_avi_gps_data[0], num_avi_gps_data[1]);
        if (avi_gps_data_hl > 0 )
        {
            if (avi_gps_data_hl > 512)
            {
                SOC_I2C_Rec_Simple(1, 0x42, gps_data, 512);
                continue;
            }
            SOC_I2C_Rec_Simple(1, 0x42, gps_data, avi_gps_data_hl);
            wake_up_interruptible(&dev->queue);
        }
        up(&dev->sem);
        msleep(500);

    }
    return 0;
}

static int major_number = 0;
static struct task_struct *gps_server_task;
static struct class *class_install;

static  int gps_server_driver_init(void)
{
    int ret, err, result;

    printk(" \n[futengfei] ==IN=================gps_server_driver_init================0126=\n");

#ifndef SOC_COMPILE
    LIDBG_GET;
#endif


    //11creat cdev
    dev = (struct gps_device *)kmalloc( sizeof(struct gps_device), GFP_KERNEL );
    if (dev == NULL)
    {
        ret = -ENOMEM;
        printk("[futengfei]===========gps_server_driver_init:kmalloc err \n");
        return ret;
    }

    dev_t dev_number = MKDEV(major_number, 0);
    if(major_number)
    {
        result = register_chrdev_region(dev_number, 1, DEVICE_NAME);
    }
    else
    {
        result = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
        major_number = MAJOR(dev_number);
    }
    printk("[futengfei]===========alloc_chrdev_region result:%d \n", result);

    cdev_init(&dev->cdev, &gps_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &gps_fops;
    err = cdev_add(&dev->cdev, dev_number, 1);
    if (err)
        printk( "[futengfei]===========Error cdev_add\n");

    //cread cdev node in /dev
    class_install = class_create(THIS_MODULE, "gpsclass");
    if(IS_ERR(class_install))
    {
        printk( "[futengfei]=======class_create err\n");
        return -1;
    }
    device_create(class_install, NULL, dev_number, NULL, "%s%d", DEVICE_NAME, 0);



    //22creat thread
    gps_server_task = kthread_create(thread_gps_server, NULL, "ftf_gps_server");
    if(IS_ERR(gps_server_task))
    {
        lidbg("Unable to start kernel thread.gps_server_task\n");
    }
    else wake_up_process(gps_server_task);


    //33init all the tools
    init_waitqueue_head(&dev->queue);
    sema_init(&dev->sem, 1);

    //return
    return 0;

}

//void __exit
static void gps_server_driver_exit(void)
{
    printk(" \n[futengfei] ==IN=================gps_server_driver_exit================0126=\n");

    // misc_deregister(&misc);
    // kfree(dev);
}

module_init(gps_server_driver_init);
module_exit(gps_server_driver_exit);

MODULE_LICENSE("GPL");

