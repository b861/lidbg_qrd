#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35) 
#include <linux/semaphore.h> 
#define init_MUTEX(LOCKNAME) 		sema_init(LOCKNAME,1)
#define init_MUTEX_LOCKED(LOCKNAME) sema_init(LOCKNAME,0)
#endif 


#include "semcmd.h"



#define DEVICE_NAME	"flysemdriver"

typedef struct sem_dev{
	unsigned short java_page_id;
	unsigned short c_page_id;
	struct semaphore dev_sem;  //�����������õĲ������Ƶ��ź���
	struct semaphore user_sem;  //�ṩ���û��ӿڵĲ������Ƶ��ź���
	
	struct semaphore java_read_sem;  //java���ӿ�������
	struct semaphore c_read_sem;  	//c���ӿ�������
	
	struct semaphore java_mutex_sem;  //java�ӿڻ�����
	struct semaphore c_mutex_sem;  	//c�ӿڻ�����
}sem_dev;

sem_dev *sem_devp;

int flysemdriver_open (struct inode *inode, struct file *filp)
{
	filp->private_data = sem_devp;
	return 0;          /* success */
}

ssize_t flysemdriver_read (struct file *filp,char __user *buf, size_t size, loff_t *ppos)
{
	//unsigned long p = *ppos;
	//unsigned int count = size;
	int ret = 0;
	struct sem_dev *dev = filp->private_data;
	
	printk(KERN_INFO "flysemdriver_read begin\n");
	printk(KERN_INFO "wait the dev_sem ing...\n");
	if(down_interruptible(&dev->dev_sem)) //��ȡ�ź���1
    {
    	printk(KERN_INFO "get the signal return\n");
      	return -ERESTARTSYS;
    }
    printk(KERN_INFO "dev_sem get the lock\n");
	//if (copy_to_user(buf,(void *)(dev->mem + p),count))
	up(&dev->dev_sem);   //�ͷ��ź���1
	printk(KERN_INFO "dev_sem unlock\n");
	printk(KERN_INFO "flysemdriver_read end\n");
	return ret;
}

ssize_t flysemdriver_write (struct file *filp, const char __user *buf,size_t size, loff_t *ppos)
{
	
	struct sem_dev *dev = filp->private_data;
	
	
	//unsigned long p = *ppos;
	//unsigned int count = size; 
	int ret = 0;
	printk(KERN_INFO "flysemdriver_write begin\n");
		
	printk(KERN_INFO "wait the dev_sem ing...\n");
	if(down_interruptible(&dev->dev_sem)) //��ȡ�ź���1
    {
    	printk(KERN_INFO "get the signal return\n");
      	return -ERESTARTSYS;
    }
    printk(KERN_INFO "dev_sem get the lock\n");
	//if (copy_from_user(dev->mem + p,buf, count))

	up(&dev->dev_sem);   //�ͷ��ź���1
	printk(KERN_INFO "dev_sem unlock\n");
	printk(KERN_INFO "flysemdriver_write end\n");
	return ret;
}

//�ļ��ͷź���
int flysemdriver_release(struct inode *inode, struct file *filp)
{
	return 0;
}

//flysemdriver_ioctl����
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35) 
static long flysemdriver_ioctl(struct file *filp,unsigned int cmd, unsigned long arg)
#else
static int flysemdriver_ioctl(struct inode *inodep, struct file *filp,unsigned int cmd, unsigned long arg)
#endif
{
	int ret = 0;
	struct sem_dev *dev = filp->private_data;
	printk(KERN_INFO "cmd11:%d,pageid_or_add:%lx\n",cmd,arg);
	
	switch (cmd)
	{
		case SEM_LOCK://�û�����
			printk(KERN_INFO "user lock begin\n");
			if(down_interruptible(&dev->user_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}
        	printk(KERN_INFO "user get lock\n");
			break;
		case SEM_UNLOCK://�û�����
			printk(KERN_INFO "user unlock\n");
			up(&dev->user_sem);   
			break;
		
		//java��д�ӿ�
		case READ_PAGEID_FORJAVA://
			printk(KERN_INFO "READ_PAGEID_FORJAVA\n");
			if(down_interruptible(&dev->java_read_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}
        	/*
			if(down_interruptible(&dev->c_mutex_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}  	   
        	*/
        	if (copy_to_user((void *)arg,(void *)&(dev->c_page_id),sizeof(dev->c_page_id)))
        	{
        		printk(KERN_INFO "copy_to_user error\n");
				ret = -EFAULT;
			}
			else
			{
				printk(KERN_INFO "copy_to_user:%x to %lx value:%x\n",dev->c_page_id,arg,*(int*)arg);
			}
			up(&dev->c_mutex_sem);
			break;
		case WRITE_PAGEID_FORJAVA://
			printk(KERN_INFO "WRITE_PAGEID_FORJAVA\n");
			if(down_interruptible(&dev->java_mutex_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}  	
			dev->java_page_id = arg;
			up(&dev->c_read_sem);   
			//up(&dev->java_mutex_sem);   
			break;		
	
		//c��д�ӿ�
		case READ_PAGEID_FORC://
			printk(KERN_INFO "READ_PAGEID_FORC\n");
			if(down_interruptible(&dev->c_read_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}  
        	/*
			if(down_interruptible(&dev->java_mutex_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}  
        	*/	
        	if (copy_to_user((void *)arg,(void *)&(dev->java_page_id),sizeof(dev->java_page_id)))
        	{
        		printk(KERN_INFO "copy_to_user error\n");
				ret = -EFAULT;
			}
			else
			{
				printk(KERN_INFO "copy_to_user:%x to %lx value:%x\n",dev->java_page_id,arg,*(int*)arg);
			}
			up(&dev->java_mutex_sem);   
			break;
		case WRITE_PAGEID_FORC://
			printk(KERN_INFO "WRITE_PAGEID_FORC\n");
			if(down_interruptible(&dev->c_mutex_sem)) 
        	{
        		printk(KERN_INFO "get the signal return\n");
                return -ERESTARTSYS;
        	}  			
			dev->c_page_id = arg;
			up(&dev->java_read_sem);   
			//up(&dev->c_mutex_sem);   
			break;				
		default:
			return - EINVAL;//������֧�ֵ�����
	}
	
	return ret;
}

static struct file_operations flysemdriver_fops = {
	.owner		= THIS_MODULE,
	.open		= flysemdriver_open,
	.write		= flysemdriver_write,
	.read		= flysemdriver_read,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35) 
	.unlocked_ioctl = flysemdriver_ioctl,
#else
	.ioctl 		= flysemdriver_ioctl,
#endif
	.release 	= flysemdriver_release,
//	.close		= flykeydriver_close,	
};

static struct miscdevice misc= {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME,
	.fops	= &flysemdriver_fops,
};


static int __init  flysemdriver_init(void){
	
	int result;
    int ret = misc_register(&misc);
    if(ret<0 ){
    	return ret;
    }
	sem_devp = kmalloc(sizeof(struct sem_dev),GFP_KERNEL);
	
	if(!sem_devp)
	{
		printk(KERN_INFO "kmalloc error\n");
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(sem_devp , 0,sizeof(struct sem_dev));
	init_MUTEX(&sem_devp->dev_sem);    //��ʼ���ź���
	init_MUTEX(&sem_devp->user_sem);    //��ʼ���ź���
	init_MUTEX_LOCKED(&sem_devp->java_read_sem);
	init_MUTEX_LOCKED(&sem_devp->c_read_sem);
	init_MUTEX(&sem_devp->java_mutex_sem);   
	init_MUTEX(&sem_devp->c_mutex_sem);    
	
	printk(KERN_INFO "flysemdriver_success\n");
    return 0;
    
    fail_malloc:misc_deregister(&misc);
	return result;
}

static void __exit flysemdriver_exit(void){
	kfree(sem_devp);
	misc_deregister(&misc);
}

module_init(flysemdriver_init);
module_exit(flysemdriver_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("FlyAudio.Inc");
MODULE_DESCRIPTION("flyaudio sem driver");
