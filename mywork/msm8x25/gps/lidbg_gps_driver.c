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

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define DEVICE_NAME "uboxserver"

struct test_input_dev
{
    char *name;
    unsigned int counter;
    struct miscdevice *miscdev;
};

typedef struct touch
{
    int x;
    int y;
    int pressed;
} touch_t;
static touch_t touch = {0, 0, 0};
static char  gps_data[255] = { 0 }; 
static char  num_avi_gps_data[2] = { 0 }; 
static int    avi_gps_data_hl=0; 
int thread_gps_server(void *data);

void set_touch_pos(touch_t *t)
{
    memcpy( &touch, t, sizeof(touch_t) );
}

//global variable
struct test_input_dev *dev;
int fenzhi_major;
int data = 8;


int gps_open (struct inode *inode, struct file *filp)
{
    //do nothing
    	printk("[futengfei]==================gps_open\n");

    return 0;          /* success */
}


/*
 * Data management: read and write
 */
ssize_t gps_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct test_input_dev *dev = filp->private_data;

   // if(sizeof(touch_t) > count)return 0;
   //copy_to_user(buf, &touch, sizeof(touch_t));
	printk("[futengfei]==================gps_read\n");
   
    return count;
}



ssize_t gps_write (struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
    struct test_input_dev *dev = filp->private_data;
	printk("[futengfei]==================gps_write\n");

	int i =0;
	SOC_I2C_Rec(1, 0x42, 0x00, gps_data, 255);
	//SOC_I2C_Rec_Simple(1, 0x42, gps_data, 255);
	
	for(i=0;i<254;i++)
		{
			printk("[futengfei] gps_data=>0x%x]\n",gps_data[i]);
		}
	for(i=0;i<254;i++)
		{
			printk("[futengfei] gps_data=>%c]\n",gps_data[i]);
		}
	
    return count;
}

struct file_operations gps_fops =
{
    .owner =	THIS_MODULE,
    .read =		gps_read,
    .write =	gps_write,
    .open =	gps_open,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &gps_fops,
};
int thread_gps_server(void *data)
{
   int i = 0;
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
		//my thread item
	if(1)	
        	{
			SOC_I2C_Rec(1, 0x42, 0xfd, num_avi_gps_data, 2);
			avi_gps_data_hl=(num_avi_gps_data[0]<<8)+num_avi_gps_data[1];
			printk("[futengfei] gps_data[%d]  REG:0xfd 0xfe =>0x%x 0x%x ]\n",avi_gps_data_hl,num_avi_gps_data[0],num_avi_gps_data[1]);
			if (avi_gps_data_hl>0) 
				{
					
					//SOC_I2C_Rec(1, 0x42, 0x00, gps_data, 40);
					SOC_I2C_Rec_Simple(1, 0x42, gps_data, avi_gps_data_hl);
					for(i=0;i<40;i++)
					{
					//printk("[futengfei] gps_data[%d]=>0x%x]\n",avi_gps_data_hl,gps_data[i]);
					}

					printk("[futengfei] gps_data=>%s",gps_data);
					
					printk("\n\n");
				}
			msleep(800);
		}
	else
		ssleep(30);
    }
    return 0;
}

static struct task_struct *gps_server_task;
static  int gps_server_driver_init(void)
{
    int ret;

    printk(" \n[futengfei] ==IN=================gps_server_driver_init================0126=\n");

#ifndef SOC_COMPILE
LIDBG_GET;
#endif

    dev = (struct test_input_dev *)kmalloc( sizeof(struct test_input_dev), GFP_KERNEL );
    if (dev == NULL)
    {
        ret = -ENOMEM;
        printk(KERN_ERR "%s: Failed to allocate input device\n", __func__);
        return ret;
    }

    //miscdev
    dev->miscdev = &misc;
    ret = misc_register(&misc);
    if(ret < 0 )
    {
        kfree(dev);
        return ret;
    }


    gps_server_task = kthread_create(thread_gps_server, NULL, "ftf_gps_server");
    if(IS_ERR(gps_server_task))
    {
        lidbg("Unable to start kernel thread.gps_server_task\n");
    }
    else wake_up_process(gps_server_task);

    return 0;

}

//void __exit
static void gps_server_driver_exit(void)
{
    misc_deregister(&misc);
    kfree(dev);
}

module_init(gps_server_driver_init);
module_exit(gps_server_driver_exit);

MODULE_LICENSE("GPL");

