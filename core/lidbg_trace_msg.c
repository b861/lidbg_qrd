
#include "lidbg.h"
#include "lidbg_fs.h"

#define DEVICE_NAME "lidbg_trace_msg"
#define LIDBG_TRACE_MSG_FIFO_SIZE		(32  * 1024)
#define LIDBG_TRACE_MSG_PATH "/data/lidbg/lidbg_trace_msg.txt"

static LIST_HEAD(lidbg_trace_msg_string_list);

struct lidbg_trace_message_device
{
	char *name;
	int disable_flag;
	struct kfifo fifo;
	struct cdev cdev;
	struct semaphore sem;
};

struct lidbg_trace_key_words
{
	struct list_head tmp_list;
	char *key_words;
};

static struct lidbg_trace_message_device *pdev;

void lidbg_trace_msg_disable(int flag)
{
	pdev->disable_flag = flag;
}
EXPORT_SYMBOL(lidbg_trace_msg_disable);

static void lidbg_trace_key_word(char *string)
{
	char *ret = NULL;
	struct string_dev *pmsg;

	//down(&pdev->sem);
	list_for_each_entry(pmsg, &lidbg_trace_msg_string_list, tmp_list)
	{
		if(strlen(pmsg->yourkey))
			ret = strstr(string, pmsg->yourkey);
		if(ret) {
			bfs_file_amend(LIDBG_TRACE_MSG_PATH, string);
			ret =NULL;
		}
	
	}
	//up(&pdev->sem);
}

static void lidbg_trace_msg_is_enough(int len)
{
	if(kfifo_is_full(&pdev->fifo) || (kfifo_avail(&pdev->fifo) < len))
	{
		int ret;
		char msg_clean_buff[len];
		
		down(&pdev->sem);
 		ret = kfifo_out(&pdev->fifo, msg_clean_buff, len);
		up(&pdev->sem);

	}

}

static int thread_trace_msg_in(void *data)
{
	int len;
	char buff[256];
	struct file *filep;

	filep = filp_open("/proc/kmsg", O_RDONLY, 0644);
	if(filep < 0)
		printk("Open /proc/kmsg failed !\n");

	memset(buff, '\0', sizeof(buff));
	
	while(1)
	{
		if(!pdev->disable_flag) {
			len = filep->f_op->read(filep, buff, 256, &filep->f_pos);

			lidbg_trace_msg_is_enough(len);

			down(&pdev->sem);
			kfifo_in(&pdev->fifo, buff, len);
			up(&pdev->sem);

			msleep(50);
			memset(buff, '\0', sizeof(buff));
		}
	}

	return 0;
}

static int thread_trace_msg_out(void *data)
{
	int i = 0;
	int len;
	char buff[256];

	memset(buff, '\0', sizeof(buff));
	while(1)
	{
		if(kfifo_is_empty(&pdev->fifo))
		{
			msleep(50);
			continue;
		}

		down(&pdev->sem);
		len = kfifo_out(&pdev->fifo, &buff[i], 1);
		up(&pdev->sem);
		
		if(buff[i] == '\n')
		{
			i = 0;
			lidbg_trace_key_word(buff);
			memset(buff, '\0', sizeof(buff));
			continue;
		}

		i++;
	}
	
	return 0;

}

static int lidbg_trace_msg_open (struct inode *inode, struct file *filp)
{
	DUMP_FUN;
	filp->private_data = pdev;
	return 0;
}

static ssize_t lidbg_trace_msg_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	DUMP_FUN;
	return 0;
}

static ssize_t lidbg_trace_msg_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int ret = 0;
	char pbuff[256];

	memset(pbuff, '\0', sizeof(pbuff));

	if (copy_from_user( pbuff, buf, count))
		printk("Lidbg msg copy_from_user ERR\n");

	lidbg_trace_msg_is_enough(count);

	down(&pdev->sem);
	kfifo_in(&pdev->fifo, pbuff, count);
	up(&pdev->sem);
	
	return ret;
}

int lidbg_trace_msg_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static  struct file_operations lidbg_trace_msg_fops =
{
	.owner = THIS_MODULE,
	.read = lidbg_trace_msg_read,
	.write = lidbg_trace_msg_write,
	.open = lidbg_trace_msg_open,
	.release = lidbg_trace_msg_release,
};

static int  lidbg_trace_msg_probe(struct platform_device *ppdev)
{
	int ret, err;
	int major_number = 0;
	dev_t dev_number;

	DUMP_FUN;
	dev_number = MKDEV(major_number, 0);
	pdev = (struct lidbg_trace_message_device *)kmalloc( sizeof(struct lidbg_trace_message_device), GFP_KERNEL);
	if (pdev == NULL)
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

	cdev_init(&pdev->cdev, &lidbg_trace_msg_fops);
	pdev->cdev.owner = THIS_MODULE;
	pdev->cdev.ops = &lidbg_trace_msg_fops;
	pdev->disable_flag = 1;
	
	ret = kfifo_alloc(&pdev->fifo, LIDBG_TRACE_MSG_FIFO_SIZE, GFP_KERNEL);
	if(ret)
	{
		printk("Alloc kfifo for lidbg dev failed.\n");
		return ret;
	}

	err = cdev_add(&pdev->cdev, dev_number, 1);
	if (err)
		printk( "Add cdev error.\n");

	sema_init(&pdev->sem, 1);

	if(fs_fill_list("/flysystem/lib/out/lidbg_trace_msg_string_list.conf", FS_CMD_FILE_LISTMODE, &lidbg_trace_msg_string_list) < 0)
		fs_fill_list("/system/lib/modules/out/lidbg_trace_msg_string_list.conf", FS_CMD_FILE_LISTMODE, &lidbg_trace_msg_string_list);

	fs_file_separator(LIDBG_TRACE_MSG_PATH);
	FS_REGISTER_INT(pdev->disable_flag, "trace_msg_disable", 1, NULL);

	CREATE_KTHREAD(thread_trace_msg_in, NULL);
	CREATE_KTHREAD(thread_trace_msg_out, NULL);

	return 0;
}


static int  lidbg_trace_msg_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver lidbg_trace_msg_driver =
{
    .probe		= lidbg_trace_msg_probe,
    .remove     = lidbg_trace_msg_remove,
    .driver         = {
        .name = "lidbg_trace_msg",
        .owner = THIS_MODULE,
    },
};

static struct platform_device lidbg_trace_msg_device =
{
    .name               = "lidbg_trace_msg",
    .id                 = -1,
};

static int __init lidbg_trace_msg_init(void)
{
	DUMP_BUILD_TIME;
	platform_device_register(&lidbg_trace_msg_device);

	return platform_driver_register(&lidbg_trace_msg_driver);
}

static void __exit lidbg_trace_msg_exit(void)
{
	DUMP_FUN;
	
	platform_device_unregister(&lidbg_trace_msg_device);
	platform_driver_unregister(&lidbg_trace_msg_driver);
}

module_init(lidbg_trace_msg_init);
module_exit(lidbg_trace_msg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");


