#include "lidbg.h"
#include "lidbg_fs.h"
#include "lidbg_cmn.h"
#include <linux/kfifo.h>

#define BUFF_SIZE 256
#define DEVICE_NAME "lidbg_mem_log"
#define LIDBG_FIFO_SIZE (5 * 1024 * 1024)

struct lidbg_msg_device
{
	char *name;
	struct kfifo fifo;
	struct cdev cdev;
	struct semaphore sem;
};

static struct lidbg_msg_device *dev;
static int lidbg_mem_log_ready;
static int max_file_len = 1;

static int lidbg_msg_open (struct inode *inode, struct file *filp)
{
	filp->private_data = dev;
	printk("lidbg_msg_open\n");
	return 0;
}

static ssize_t lidbg_msg_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk("lidbg_fifo_read\n");
	return 0;
}

static ssize_t lidbg_msg_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	//struct lidbg_fifo_device *dev = filp->private_data;
	int ret = 0;
	char *msg_write_buff; 

	msg_write_buff =kmalloc(count, GFP_KERNEL);

	if (copy_from_user( msg_write_buff, buf, count))
		printk("Lidbg msg copy_from_user ERR\n");

	if(kfifo_is_full(&dev->fifo))
		ret = kfifo_out(&dev->fifo, msg_write_buff, count);
	
	down(&dev->sem);
	ret = kfifo_in(&dev->fifo, msg_write_buff, count);
	up(&dev->sem);
	return ret;
}


static  struct file_operations lidbg_msg_fops =
{
	.owner = THIS_MODULE,
	.read = lidbg_msg_read,
	.write = lidbg_msg_write,
	.open = lidbg_msg_open,
};

static int mem_log_file_amend(char *file2amend, char *str_append)
{
    struct file *filep;
    struct inode *inode = NULL;
    mm_segment_t old_fs;
    int  flags, is_file_cleard = 0;
    unsigned int file_len;

    if(str_append == NULL)
    {
        printk("[futengfei]err.fileappend_mode:<str_append=null>\n");
        return -1;
    }
    flags = O_CREAT | O_RDWR | O_APPEND;

again:
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
    file_len = file_len + 2;


    if(file_len > max_file_len * MEM_SIZE_1_MB)
    {
        printk("[futengfei]warn.fileappend_mode:< file>8M.goto.again >\n");
        is_file_cleard = 1;
        flags = O_CREAT | O_RDWR | O_APPEND | O_TRUNC;
        set_fs(old_fs);
        filp_close(filep, 0);
        goto again;
    }
    filep->f_op->llseek(filep, 0, SEEK_END);//note:to the end to append;
    if(1 == is_file_cleard)
    {
        char *str_warn = "============have_cleard=============\n\n";
        is_file_cleard = 0;
        filep->f_op->write(filep, str_warn, strlen(str_warn), &filep->f_pos);
    }
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
		char msg_clean_buff[len];
 		kfifo_out(&dev->fifo, msg_clean_buff, len);
	}

}

int lidbg_msg_put( const char *fmt, ... )
{

	if(lidbg_mem_log_ready) {

		int ret, len;
		va_list args;
		char msg_in_buff[BUFF_SIZE];
		char buf[32];
		lidbg_get_curr_time(buf,NULL);
	
		len = strlen(buf);

		lidbg_msg_is_enough(len);
		down(&dev->sem);
		ret = kfifo_in(&dev->fifo, buf, len);
		up(&dev->sem);

		memset(msg_in_buff, '\0',sizeof(msg_in_buff));

		va_start ( args, fmt );
		ret = vsprintf (msg_in_buff, (const char *)fmt, args );
		va_end ( args );
		
		len = strlen(msg_in_buff);

		lidbg_msg_is_enough(len);

 		down(&dev->sem);	
		ret = kfifo_in(&dev->fifo, msg_in_buff, len);
		up(&dev->sem);
	}
	else
		printk("Lidbg mem log is not ready!\n");
	
	return 1;
}
EXPORT_SYMBOL(lidbg_msg_put);

int lidbg_msg_get(char *to_file, int out_mode )
{
	int len =0;
	unsigned int ret = 0;
	char *msg_out_buff; 

	
	down(&dev->sem);
	len = kfifo_len(&dev->fifo);
	up(&dev->sem);
		
	msg_out_buff =kmalloc(len, GFP_KERNEL);
		
	down(&dev->sem);
	ret = kfifo_out(&dev->fifo, msg_out_buff, len);
	up(&dev->sem);

	ret = mem_log_file_amend(to_file, msg_out_buff);
	if(ret != 1)
	{
		printk("ERR: lidbg msg out msg to file.\n");
		return -1;
	}
	
	return 1;
}
EXPORT_SYMBOL(lidbg_msg_get);

static int  lidbg_msg_probe(struct platform_device *pdev)
{
	int ret, err;
	int major_number = 0;
	dev_t dev_number;

	lidbg_mem_log_ready = 0;
	
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

	 sema_init(&dev->sem, 1);

	lidbg_mem_log_ready = 1;
	
	return 0;
}


static int  lidbg_msg_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver lidbg_string_driver =
{
    .probe		= lidbg_msg_probe,
    .remove     = lidbg_msg_remove,
    .driver         = {
        .name = "lidbg_mem_log",
        .owner = THIS_MODULE,
    },
};

static struct platform_device lidbg_string_device =
{
    .name               = "lidbg_mem_log",
    .id                 = -1,
};


static  int lidbg_msg_init(void)
{
	printk(" \nlidbg fifo driver init==\n");
	platform_device_register(&lidbg_string_device);

	return platform_driver_register(&lidbg_string_driver);

}

static void lidbg_msg_exit(void)
{
	printk(" \nlidbg msg driver exit\n");
	
	platform_device_unregister(&lidbg_string_device);
	platform_driver_unregister(&lidbg_string_driver);
}

module_init(lidbg_msg_init);
module_exit(lidbg_msg_exit);

MODULE_LICENSE("GPL");



