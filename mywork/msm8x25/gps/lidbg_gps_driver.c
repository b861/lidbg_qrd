

#include "lidbg_def.h"
#include "lidbg_enter.h"
LIDBG_DEFINE;


#define DEVICE_NAME "ubloxgps"

struct gps_device
{
    char *name;
    unsigned int counter;
    wait_queue_head_t queue;
    struct semaphore sem;
    struct cdev cdev;
};

struct early_suspend early_suspend;

#define GPS_BUF_SIZE (1024*4)
u8 gps_data[GPS_BUF_SIZE];


#define JNI_BUF_SIZE (512)
u8 gps_data_for_jni[JNI_BUF_SIZE];


#define FIFO_SIZE (1024*4)
u8 fifo_buffer[FIFO_SIZE];
static struct kfifo gps_data_fifo;

bool work_en = 1;

static char  num_avi_gps_data[2] = { 0 };
static int    avi_gps_data_hl = 0;
int thread_gps_server(void *data);

bool debug_mask = 0;


//global variable
struct gps_device *dev;


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
#if 0
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
#else
ssize_t gps_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct gps_device *dev = filp->private_data;
    int read_len, fifo_len;

    if(kfifo_is_empty(&gps_data_fifo))
    {
        if(wait_event_interruptible(dev->queue, !kfifo_is_empty(&gps_data_fifo)))
            return -ERESTARTSYS;
    }


    down(&dev->sem);
    fifo_len = kfifo_len(&gps_data_fifo);

    if(fifo_len > JNI_BUF_SIZE)
        read_len = JNI_BUF_SIZE;
    else
        read_len = fifo_len;

    kfifo_out(&gps_data_fifo, &gps_data_for_jni, read_len);
    up(&dev->sem);

    copy_to_user(buf, gps_data_for_jni, read_len);

    if(fifo_len > JNI_BUF_SIZE)
        wake_up_interruptible(&dev->queue);

    if(debug_mask)
        printk("[ublox]fifo_len: %d, jni_read: %d\n", fifo_len, read_len);
    return read_len;
}


#endif

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

    //if (avi_gps_data_hl > 0 && avi_gps_data_hl <= 512)

    if(!kfifo_is_empty(&gps_data_fifo))
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


void clean_ublox_buf(void)
{
	int ret;
    DUMP_FUN_ENTER;

    ret=SOC_I2C_Rec(1, 0x42, 0xfd, num_avi_gps_data, 2);
	if (ret < 0)
		return;
	
    avi_gps_data_hl = (num_avi_gps_data[0] << 8) + num_avi_gps_data[1];

    SOC_I2C_Rec_Simple(1, 0x42, gps_data, avi_gps_data_hl);

    DUMP_FUN_LEAVE;

}



#if 0
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
                up(&dev->sem);
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

#else

int thread_gps_server(void *data)
{
    DUMP_FUN_ENTER;
    //clean_ublox_buf();
    while(1)
    {
        if(work_en == 0)
            goto do_nothing;

        SOC_I2C_Rec(1, 0x42, 0xfd, num_avi_gps_data, 2);

        avi_gps_data_hl = (num_avi_gps_data[0] << 8) + num_avi_gps_data[1];
        if(debug_mask)
            printk("[ublox]ublox_buf_len: ===========================%d\n", avi_gps_data_hl);

        if(avi_gps_data_hl > 0)
        {
            if(avi_gps_data_hl <= GPS_BUF_SIZE)
            {
                SOC_I2C_Rec_Simple(1, 0x42, gps_data, avi_gps_data_hl);
                //printk("[ublox]%s\n",gps_data);

                down(&dev->sem);
                if(kfifo_is_full(&gps_data_fifo))
                {
                    kfifo_reset(&gps_data_fifo);
                    printk("[ublox]kfifo_reset!!!!!\n");
                }
                kfifo_in(&gps_data_fifo, gps_data, avi_gps_data_hl);
                up(&dev->sem);
            }
            else
            {
                printk("[ublox]err!!!!!!1\n");
                goto do_nothing;
            }
        }
        else
        {
            goto do_nothing;

        }
        wake_up_interruptible(&dev->queue);
        if(debug_mask)
        {
            gps_data[avi_gps_data_hl ] = '\0';
            printk("%s\n", gps_data);
        }

do_nothing:
        msleep(500);

    }
    DUMP_FUN_LEAVE;
    return 0;
}



#endif


static int major_number = 0;
static struct task_struct *gps_server_task;
static struct class *class_install;



int read_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
    printk("enable ublox print\n");
    debug_mask = 1;

    return 1;
}

void create_new_proc_entry()
{
    create_proc_read_entry("ublox_dbg", 0, NULL, read_proc, NULL);
    // /cat proc/ublox_dbg
}

#ifdef CONFIG_HAS_EARLYSUSPEND

static void gps_early_suspend(struct early_suspend *h)
{
    DUMP_FUN;
    work_en = 0;

}

static void gps_late_resume(struct early_suspend *h)
{
    DUMP_FUN;
    clean_ublox_buf();
    work_en = 1;

}

#endif

int is_ublox_exist(void)
{
    int exist, retry;
    for(retry = 0; retry < 10; retry++)
    {
        exist = SOC_I2C_Rec_Simple(1, 0x42, gps_data, 1 );
        if (exist < 0)
        {
        	msleep(100);
            continue;
        }
        else
            return 1;
    }
    return -1;
}

static int  gps_probe(struct platform_device *pdev)
{
    int ret, err, result;

    DUMP_FUN;
    if(is_ublox_exist() < 0)
    {
        printk("[futengfei]=======================ublox.miss\n\n");
        return 0;
    }
    else
    {
        printk("[futengfei]=======================ublox.exist\n\n");
    }

#ifdef CONFIG_HAS_EARLYSUSPEND  //  enable/disable the gps thread 
    early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    early_suspend.suspend = gps_early_suspend;
    early_suspend.resume = gps_late_resume;
    register_early_suspend(&early_suspend);
#endif

    create_new_proc_entry(); //cat /proc/ublox_dbg to enable gps_debug

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
    gps_server_task = kthread_create(thread_gps_server, NULL, "fly_gps_server");
    if(IS_ERR(gps_server_task))
    {
        lidbg("Unable to start kernel thread.gps_server_task\n");
    }
    else wake_up_process(gps_server_task);


    //33init all the tools
    init_waitqueue_head(&dev->queue);
    sema_init(&dev->sem, 1);
    kfifo_init(&gps_data_fifo, fifo_buffer, FIFO_SIZE);



    return 0;
}



static int  gps_remove(struct platform_device *pdev)
{
    return 0;
}



#ifdef CONFIG_PM
static int gps_resume(struct device *dev)
{
    DUMP_FUN_ENTER;
    //work_en = 1;
    return 0;

}

static int gps_suspend(struct device *dev)
{
    DUMP_FUN_ENTER;
    //work_en = 0;
    return 0;

}

static struct dev_pm_ops gps_pm_ops =
{
    .suspend	= gps_suspend,
    .resume		= gps_resume,
};
#endif

static struct platform_driver gps_driver =
{
    .probe		= gps_probe,
    .remove     = gps_remove,
    .driver         = {
        .name = "lidbg_gps",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &gps_pm_ops,
#endif
    },
};

static struct platform_device lidbg_gps_device =
{
    .name               = "lidbg_gps",
    .id                 = -1,
};


static  int gps_server_driver_init(void)
{

    printk(" \n[futengfei] ==IN=================gps_server_driver_init==========500======0126=\n");

#ifndef SOC_COMPILE
    LIDBG_GET;
#endif


    platform_device_register(&lidbg_gps_device);

    return platform_driver_register(&gps_driver);

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

