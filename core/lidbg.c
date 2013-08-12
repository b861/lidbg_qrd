/* Copyright (c) 2012, swlee
 *
 */
// http://blog.csdn.net/luoshengyang/article/details/6568411
#include "lidbg.h"

struct lidbg_dev *global_lidbg_devp = NULL;

#define MEM_CLEAR 0x1  /*��0ȫ���ڴ�*/
#define GET_GLOBAL 0x2

#define LIDBG_MAJOR 167    /*Ԥ���lidbg�����豸��*/
#define LIDBG_MINOR 0    /*Ԥ���lidbg�����豸��*/

struct cdev *my_cdev;
struct class *my_class;

static lidbg_major = LIDBG_MAJOR;


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
DECLARE_MUTEX(lidbg_lock);
#else
DEFINE_SEMAPHORE(lidbg_lock);
#endif


/*�ļ��򿪺���*/
int lidbg_open(struct inode *inode, struct file *filp)
{
    DUMP_FUN;
    /*���豸�ṹ��ָ�븳ֵ���ļ�˽������ָ��*/
    filp->private_data = (struct lidbg_dev *)global_lidbg_devp;
    if (filp->f_flags & O_NONBLOCK)
    {
        if (down_trylock(&lidbg_lock))

            return -EBUSY;
    }
    else
    {
        down(&lidbg_lock);
    }
    return 0;
}


/*�ļ��ͷź���*/
int lidbg_release(struct inode *inode, struct file *filp)
{
    DUMP_FUN;
    up(&lidbg_lock);
    return 0;
}


#if 0
/* ioctl�豸���ƺ��� */
static int lidbg_ioctl(struct inode *inodep, struct file *filp, unsigned
                       int cmd, unsigned long arg)
{
    struct lidbg_dev *dev = filp->private_data;/*����豸�ṹ��ָ��*/

    switch (cmd)
    {
    case MEM_CLEAR:
        memset(dev->mem, 0, LIDBG_SIZE);
        lidbg(KERN_INFO "lidbg is set to zero\n");
        break;
    case GET_GLOBAL:
        break;
    default:
        return  - EINVAL;
    }
    return 0;
}
#endif


/*������*/
static ssize_t lidbg_read(struct file *filp, char __user *buf, size_t size,
                          loff_t *ppos)
{
    unsigned int count = size;
    int ret = 0, read_value = 0;
    struct lidbg_dev *dev = filp->private_data; /*����豸�ṹ��ָ��*/

    /*�ں˿ռ�->�û��ռ�*/
    memcpy(&read_value, (void *)(dev->mem), 4);
    printk("lidbg_read:read_value=%x,read_count=%d\n", (u32)read_value, count);
    if (copy_to_user(buf, (void *)(dev->mem), count))
    {
        ret =  - EFAULT;
    }
    else
    {
        ret = count;
    }

    return count;
}

#include "lidbg_cmd.c"

/*д���� ��Ҫ���������� ���*/
static ssize_t lidbg_write(struct file *filp, const char __user *buf,
                           size_t size, loff_t *ppos)
{
    struct lidbg_dev *dev = filp->private_data; /*����豸�ṹ��ָ��*/
    memset(dev->mem, '\0', LIDBG_SIZE);

    /*�û��ռ�->�ں˿ռ�*/
    copy_from_user(dev->mem, buf, size);

	parse_cmd(dev->mem);
    return size;//����Ϊsize���ظ�ִ��
}

/* seek�ļ���λ���� */
static loff_t lidbg_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret = 0;
    switch (orig)
    {
    case 0:   /*����ļ���ʼλ��ƫ��*/
        if (offset < 0)
        {
            ret =  - EINVAL;
            break;
        }
        if ((unsigned int)offset > LIDBG_SIZE)
        {
            ret =  - EINVAL;
            break;
        }
        filp->f_pos = (unsigned int)offset;
        ret = filp->f_pos;
        break;
    case 1:   /*����ļ���ǰλ��ƫ��*/
        if ((filp->f_pos + offset) > LIDBG_SIZE)
        {
            ret =  - EINVAL;
            break;
        }
        if ((filp->f_pos + offset) < 0)
        {
            ret =  - EINVAL;
            break;
        }
        filp->f_pos += offset;
        ret = filp->f_pos;
        break;
    default:
        ret =  - EINVAL;
        break;
    }
    return ret;
}

/*�ļ������ṹ��*/
static const struct file_operations lidbg_fops =
{
    .owner = THIS_MODULE,
    .llseek = lidbg_llseek,
    .read = lidbg_read,
    .write = lidbg_write,
#if 0
    .ioctl = lidbg_ioctl,
#endif
    .open = lidbg_open,
    .release = lidbg_release,
};

/*��ʼ����ע��cdev*/
static void lidbg_setup_cdev(struct lidbg_dev *dev, int index)
{
    int err, devno = MKDEV(lidbg_major, index);

    cdev_init(&dev->cdev, &lidbg_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &lidbg_fops;
    err = cdev_add(&dev->cdev, devno, 1);
    if (err)
        lidbg(KERN_NOTICE "Error %d adding LED%d", err, index);
}


void lidbg_create_proc(void);
void lidbg_remove_proc(void);

void soc_func_tbl_default()
{
    lidbgerr("soc_func_tbl_default:this func not ready!\n");

}


/*�豸����ģ����غ���*/
int lidbg_init(void)
{
    int result, err;
    dev_t devno = MKDEV(lidbg_major, 0);
    lidbg("lidbg_init\n");
    DUMP_BUILD_TIME;

    /* �����豸��*/
    if (lidbg_major)
        result = register_chrdev_region(devno, 1, "lidbg");
    else  /* ��̬�����豸�� */
    {
        result = alloc_chrdev_region(&devno, 0, 1, "lidbg");
        lidbg_major = MAJOR(devno);
    }
    if (result < 0)
        return result;
#if 1
    /* ��̬�����豸�ṹ����ڴ�*/
    global_lidbg_devp = kmalloc(sizeof(struct lidbg_dev), GFP_KERNEL);

    if (!global_lidbg_devp)    /*����ʧ��*/
    {
        result =  - ENOMEM;
        goto fail_malloc;
    }
    memset(global_lidbg_devp, 0, sizeof(struct lidbg_dev));

	{
		int i;
		for(i = 0; i < sizeof(global_lidbg_devp->soc_func_tbl) / 4; i++)
		{
			((int *)&(global_lidbg_devp->soc_func_tbl))[i] = soc_func_tbl_default;

		}
	}

	memset(&(global_lidbg_devp->soc_pvar_tbl), NULL, sizeof(struct lidbg_pvar_t));



#endif
    lidbg_setup_cdev((struct lidbg_dev *)global_lidbg_devp, 0);


    lidbg("creating mlidbg class.\n");

    //�Զ���/dev�´���my_device�豸�ļ�
    //��������ʼ���Ĵ��������class_createΪ���豸����һ��class��
    /* creating your own class */
    my_class = class_create(THIS_MODULE, "mlidbg");
    if(IS_ERR(my_class))
    {
        lidbg("Err: failed in creating mlidbg class.\n");
        return -1;
    }

    //��Ϊÿ���豸���� class_device_create������Ӧ���豸
    /* register your own device in sysfs, and this will cause udevd to create corresponding device node */
    // class_device_create(my_class, NULL, MKDEV(LIDBG_MAJOR, LIDBG_MINOR), NULL, "mlidbg" "%d", LIDBG_MINOR );
    device_create(my_class, NULL, MKDEV(LIDBG_MAJOR, LIDBG_MINOR), NULL, "mlidbg" "%d", LIDBG_MINOR );

    lidbg_create_proc();

    return 0;

fail_malloc:
    unregister_chrdev_region(devno, 1);
    return result;
}

/*ģ��ж�غ���*/
void lidbg_exit(void)
{
    lidbg("mlidbg_exit\n");

    cdev_del(&((struct lidbg_dev *)global_lidbg_devp)->cdev);   /*ע��cdev*/
    kfree(((struct lidbg_dev *)global_lidbg_devp));     /*�ͷ��豸�ṹ���ڴ�*/

    // class_device_destroy(my_class, MKDEV(LIDBG_MAJOR, LIDBG_MINOR));
    device_destroy(my_class, MKDEV(LIDBG_MAJOR, LIDBG_MINOR));

    class_destroy(my_class);

    lidbg_remove_proc();

    unregister_chrdev_region(MKDEV(lidbg_major, 0), 1); /*�ͷ��豸��*/
}


#define LIDBG_DEVICE_PROC_NAME  "lidbg"
ssize_t lidbg_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len;

    char *hello_str = "Hello, world!\n";
    lidbg("lidbg_proc_read\n");
    len = strlen(hello_str);
    strcpy(page, hello_str);
    /*     * Signal EOF.     */
    *eof = 1;
    return len;

}

ssize_t lidbg_proc_write(struct file *filp, const char __user *buff, unsigned long len, void *data)
{
    lidbg("lidbg_proc_write\n");

    filp->private_data = (struct lidbg_dev *)global_lidbg_devp;
    lidbg_write(filp, buff, len, 0);
    return len;
}

void lidbg_create_proc(void)
{
    struct proc_dir_entry *entry;

    entry = create_proc_entry(LIDBG_DEVICE_PROC_NAME, 0, NULL);
    if(entry)
    {
        entry->read_proc = lidbg_proc_read;
        entry->write_proc = lidbg_proc_write;
    }
}

void lidbg_remove_proc(void)
{
    remove_proc_entry(LIDBG_DEVICE_PROC_NAME, NULL);
}

MODULE_AUTHOR("Lsw");
MODULE_LICENSE("GPL");


module_param(lidbg_major, int, S_IRUGO);

module_init(lidbg_init);
module_exit(lidbg_exit);
