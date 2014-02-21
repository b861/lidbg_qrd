#include "lidbg.h"

#define BUFF_SIZE (256)
#define DEVICE_NAME "lidbg_mem_log"
#define LIDBG_FIFO_SIZE (4 * 1024 * 1024)
#define MEM_LOG_EN

struct lidbg_msg_device
{
	char *name;
	struct kfifo fifo;
	struct cdev cdev;
	struct semaphore sem;
};

static struct lidbg_msg_device *dev;
int lidbg_mem_log_ready = 0;


static int lidbg_get_curr_time(char *time_string, struct rtc_time *ptm);
static void lidbg_msg_is_enough(int len);

static int lidbg_msg_open (struct inode *inode, struct file *filp)
{
	filp->private_data = dev;
	DUMP_FUN;
	return 0;
}

static ssize_t lidbg_msg_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	DUMP_FUN;
	return 0;
}

static ssize_t lidbg_msg_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret = 0;

	if(count > 0) 
	{
		int len;
		char msg_write_buff[BUFF_SIZE]; 

		lidbg_get_curr_time(msg_write_buff,NULL);
		len = strlen(msg_write_buff);

		lidbg_msg_is_enough(len);
		down(&dev->sem);
		ret = kfifo_in(&dev->fifo, msg_write_buff, len);

		memset(msg_write_buff, '\0', sizeof(msg_write_buff));	

		if (copy_from_user( msg_write_buff, buf, count))
			printk("Lidbg msg copy_from_user ERR\n");
		
		len = strlen(msg_write_buff);

		lidbg_msg_is_enough(len);

		ret = kfifo_in(&dev->fifo, msg_write_buff, len);
		up(&dev->sem);
		
		ret = 1;
	}
	else
		ret = 0;
	
	return ret;
}

int lidbg_msg_release(struct inode *inode, struct file *filp)
{
    return 0;
}


static  struct file_operations lidbg_msg_fops =
{
	.owner = THIS_MODULE,
	.read = lidbg_msg_read,
	.write = lidbg_msg_write,
	.open = lidbg_msg_open,
    .release = lidbg_msg_release,
};

static int mem_log_file_amend(char *file2amend, char *str_append)
{
    struct file *filep;
    struct inode *inode = NULL;
    mm_segment_t old_fs;
    int  flags = 0;
    unsigned int file_len;

    if(str_append == NULL)
    {
        printk("[futengfei]err.fileappend_mode:<str_append=null>\n");
        return -1;
    }
    flags = O_CREAT | O_RDWR | O_APPEND;

    filep = filp_open(file2amend, flags , 0777);
    if(IS_ERR(filep))
    {
        printk("[futengfei]err.open:<%s>\n", file2amend);
        return -1;
    }

    old_fs = get_fs();
    set_fs(get_ds());

    inode = filep->f_dentry->d_inode;
    file_len = inode->i_size;
    file_len = file_len + 1;

    filep->f_op->llseek(filep, 0, SEEK_END);//note:to the end to append;

    filep->f_op->write(filep, str_append, strlen(str_append), &filep->f_pos);
    set_fs(old_fs);
    filp_close(filep, 0);
    return 1;
}

static int lidbg_get_curr_time(char *time_string, struct rtc_time *ptm)
{
    int  tlen = -1;
    struct timespec ts;
    struct rtc_time tm;
    getnstimeofday(&ts);
    rtc_time_to_tm(ts.tv_sec, &tm);
    if(time_string)
        tlen = sprintf(time_string, "%d-%02d-%02d_%02d:%02d:%02d ",
                       tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour , tm.tm_min, tm.tm_sec);
    if(ptm)
        *ptm = tm;
    return tlen;
}

static void lidbg_msg_is_enough(int len)
{
	if(kfifo_is_full(&dev->fifo) || (kfifo_avail(&dev->fifo) < len))
	{	
		int ret =0;
		char msg_clean_buff[len];
 		ret = kfifo_out(&dev->fifo, msg_clean_buff, len);
	}

}

int lidbg_msg_put( const char *fmt, ... )
{
	int ret;
	if(lidbg_mem_log_ready) 
	{

		int len;
		va_list args;
		char msg_in_buff[BUFF_SIZE];
		
		lidbg_get_curr_time(msg_in_buff,NULL);
	
		len = strlen(msg_in_buff);

		lidbg_msg_is_enough(len);
		down(&dev->sem);
		ret = kfifo_in(&dev->fifo, msg_in_buff, len);

		memset(msg_in_buff, '\0', sizeof(msg_in_buff));

		va_start ( args, fmt );
		ret = vsprintf (msg_in_buff, (const char *)fmt, args );
		va_end ( args );
		
		len = strlen(msg_in_buff);

		lidbg_msg_is_enough(len);

		ret = kfifo_in(&dev->fifo, msg_in_buff, len);
		up(&dev->sem);
		
		ret = 1;
	}
	else
	{
		//printk("Lidbg mem log is not ready!\n");
		ret = 0;
	}
	
	return ret;
}
EXPORT_SYMBOL(lidbg_msg_put);

int lidbg_msg_get(char *to_file, int out_mode )
{
	int len =0;
	unsigned int ret = 1;
	char *msg_out_buff = NULL; 

	if(lidbg_mem_log_ready)
	{
		down(&dev->sem);
		len = kfifo_len(&dev->fifo);
		up(&dev->sem);

		printk("lidbg_msg_get kfifo_len=%d\n",len);
		
		msg_out_buff = kmalloc(len, GFP_KERNEL);
		if (msg_out_buff == NULL)
		{
			ret = -1;
			printk("lidbg_msg_get kmalloc err \n");
			return ret;
		}

		memset(msg_out_buff, '\0', sizeof(msg_out_buff));
		down(&dev->sem);
		ret = kfifo_out(&dev->fifo, msg_out_buff, len);
		up(&dev->sem);

		ret = mem_log_file_amend(to_file, msg_out_buff);
		if(ret != 1)
		{
			printk("ERR: lidbg msg out msg to file.\n");
			return -1;
		}

		kfree(msg_out_buff);
	}
	return ret;
}
EXPORT_SYMBOL(lidbg_msg_get);

static int  lidbg_msg_probe(struct platform_device *pdev)
{
	int ret, err;
	int major_number = 0;
	dev_t dev_number;
	static struct class *class_install;
	
#ifndef MEM_LOG_EN
	return 0;
#endif
	dev_number = MKDEV(major_number, 0);
	dev = (struct lidbg_msg_device *)kmalloc( sizeof(struct lidbg_msg_device), GFP_KERNEL);
	if (dev == NULL)
	{
       	 ret = -ENOMEM;
        	printk("Kmalloc space for lidbg msg failed.\n");
        	return ret;
	}

	
	if(major_number)
	{
        ret = register_chrdev_region(dev_number, 1, DEVICE_NAME);
	}
	else
	{
		ret = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
		major_number = MAJOR(dev_number);
	}

	cdev_init(&dev->cdev, &lidbg_msg_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &lidbg_msg_fops;

	ret = kfifo_alloc(&dev->fifo, LIDBG_FIFO_SIZE, GFP_KERNEL);
	if(ret)
	{
		printk("Alloc kfifo foer lidbg dev failed.");
		return ret;
	}

	err = cdev_add(&dev->cdev, dev_number, 1);
	if (err)
		printk( "Add cdev error.\n");

	class_install = class_create(THIS_MODULE, "trace_msg");
	if(IS_ERR(class_install))
	{
    		printk( "lidbg mem log class_create err\n");
		return -1;
	}
	device_create(class_install, NULL, dev_number, NULL, "%s%d", DEVICE_NAME, 0);
	
	 sema_init(&dev->sem, 1);

	lidbg_mem_log_ready = 1;
	
	return 0;
}


static int  lidbg_msg_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver lidbg_mem_log_driver =
{
    .probe		= lidbg_msg_probe,
    .remove     = lidbg_msg_remove,
    .driver         = {
        .name = "lidbg_mem_log",
        .owner = THIS_MODULE,
    },
};

static struct platform_device lidbg_mem_log_device =
{
    .name               = "lidbg_mem_log",
    .id                 = -1,
};


static  int lidbg_msg_init(void)
{
	DUMP_BUILD_TIME;
	platform_device_register(&lidbg_mem_log_device);

	return platform_driver_register(&lidbg_mem_log_driver);

}

static void lidbg_msg_exit(void)
{
	DUMP_FUN;
	
	platform_device_unregister(&lidbg_mem_log_device);
	platform_driver_unregister(&lidbg_mem_log_driver);
}


void mem_log_main(int argc, char **argv)
{

    if(!strcmp(argv[0], "dump"))
    {
    	lidbg("dump mem log\n");
		lidbg_msg_get("/data/lidbg/lidbg_mem_log.txt", 0);
    }

}
EXPORT_SYMBOL(mem_log_main);

module_init(lidbg_msg_init);
module_exit(lidbg_msg_exit);

MODULE_LICENSE("GPL");



