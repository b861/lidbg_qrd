/* Copyright (c) 2012, swlee
 *
 */

// http://blog.csdn.net/luoshengyang/article/details/6568411
#include "lidbg.h"


#define MEM_CLEAR 0x1  /*清0全局内存*/
#define GET_GLOBAL 0x2

#define LIDBG_MAJOR 167    /*预设的lidbg的主设备号*/
#define LIDBG_MINOR 0    /*预设的lidbg的主设备号*/

//#define DUMMY_NAME "dummy_dev"

struct cdev *my_cdev;
struct class *my_class;


static lidbg_major = LIDBG_MAJOR;


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
DECLARE_MUTEX(lidbg_lock);
#else
DEFINE_SEMAPHORE(lidbg_lock);
#endif



static struct task_struct *msg_task;
static int thread_msg(void *data);

#define TOTAL_LOGS 100
#define LOG_BYTES  64

typedef struct
{
int w_pos;
int r_pos;
char log[TOTAL_LOGS][LOG_BYTES];
}lidbg_msg;

lidbg_msg *plidbg_msg=NULL;


int thread_msg(void *data)
{
	int start_index=0;
	int end_index=0;
	return 0;
	plidbg_msg = (struct lidbg_msg *)kmalloc(sizeof( lidbg_msg), GFP_KERNEL);

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
			start_index =plidbg_msg->w_pos;
			end_index =plidbg_msg->r_pos;
	
			if(start_index != end_index)
			{
				printk("%s\n",plidbg_msg->log[end_index]);
				plidbg_msg->r_pos = (end_index + 1)  % TOTAL_LOGS;
			}
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}



/*文件打开函数*/
int lidbg_open(struct inode *inode, struct file *filp)
{
    DUMP_FUN;
    /*将设备结构体指针赋值给文件私有数据指针*/
    filp->private_data = (struct lidbg_dev *)global_lidbg_devp;
    if (filp->f_flags & O_NONBLOCK)    // \u963b\u585e
    {
        if (down_trylock(&lidbg_lock))  // \u5982\u679c\u83b7\u53d6\u4e0d\u5230\u4fe1\u53f7\u91cf\uff0c\u5219\u7acb\u5373\u8fd4\u56de\u3002

            return -EBUSY;
    }                                  
    else                               // \u975e\u963b\u585e
    {
        /* \u83b7\u53d6\u4fe1\u53f7\u91cf */
        down(&lidbg_lock);    // \u7b2c\u4e8c\u6b21\u83b7\u53d6\u4fe1\u53f7\u91cf\u65f6\u4f1a\u8fdb\u5165\u4f11\u7720\uff0c\u7b2c\u4e00\u4e2a\u5e94\u7528\u7a0b\u5e8f\u91ca\u653e\u540e\u624d\u4f1a\u88ab\u5524\u9192\u3002
    }
    return 0;
}



/*文件释放函数*/
int lidbg_release(struct inode *inode, struct file *filp)
{
    DUMP_FUN;
    up(&lidbg_lock); 
    return 0;
}


#if 0
/* ioctl设备控制函数 */
static int lidbg_ioctl(struct inode *inodep, struct file *filp, unsigned
                       int cmd, unsigned long arg)
{
    struct lidbg_dev *dev = filp->private_data;/*获得设备结构体指针*/

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


/*读函数*/
static ssize_t lidbg_read(struct file *filp, char __user *buf, size_t size,
                          loff_t *ppos)
{
#if 0
    unsigned long p =  *ppos;
    unsigned int count = size;
    int ret = 0;
    struct lidbg_dev *dev = filp->private_data; /*获得设备结构体指针*/


    lidbg("pos=%x\n", *ppos);
    lidbg("size=%d\n", size);


    /*分析和获取有效的写长度*/
    if (p >= LIDBG_SIZE)
        return count ?  - ENXIO : 0;
    if (count > LIDBG_SIZE - p)
        count = LIDBG_SIZE - p;

    /*内核空间->用户空间*/
    if (copy_to_user(buf, (void *)(dev->mem + p), count))
    {
        ret =  - EFAULT;
    }
    else
    {
        *ppos += count;
        ret = count;

        lidbg(KERN_INFO "read %d bytes(s) from %d\n", count, p);
    }
    return ret;
#else

    unsigned int count = size;
    int ret = 0,read_value=0;
    struct lidbg_dev *dev = filp->private_data; /*获得设备结构体指针*/

    /*内核空间->用户空间*/
	memcpy(&read_value, (void *)(dev->mem), 4);
	printk("lidbg_read:read_value=%x,read_count=%d\n",(u32)read_value,count);
    if (copy_to_user(buf, (void *)(dev->mem), count))
    {
        ret =  - EFAULT;
    }
    else
    {
        ret = count;
    }
	
    return count;

#endif

}



/*写函数 主要操作在这里 入口*/
static ssize_t lidbg_write(struct file *filp, const char __user *buf,
                           size_t size, loff_t *ppos)
{
    //unsigned long p =  *ppos;
    int argc = 0;
    //int ret = size;
    int i = 0;
    char *pt;
    char *argv[32] = {NULL};
    struct lidbg_dev *dev = filp->private_data; /*获得设备结构体指针*/
#if 0
    /*分析和获取有效的写长度*/
    if (p >= LIDBG_SIZE)
        return count ?  - ENXIO : 0;
    if (count > LIDBG_SIZE - p)
        count = LIDBG_SIZE - p;

    /*用户空间->内核空间*/
    if (copy_from_user(dev->mem + p, buf, count))
        ret =  - EFAULT;
    else
    {
        *ppos += count;
        ret = count;

        lidbg(KERN_INFO "written %d bytes(s) from %d\n", count, p);
    }
#endif
#if 0
    lidbg("size=%d\n", size);
    i = 0;
    while(i < size)
    {
        lidbg("%x", buf[i]);

        lidbg("-");
        i++;
    }
#endif

    memset(dev->mem, '\0', LIDBG_SIZE);

    /*用户空间->内核空间*/
    copy_from_user(dev->mem, buf, size);

    // 获取命令 存入argv数组
    pt = dev->mem;
    argv[0] = pt;

    while(*pt != '\0')
    {
        while(*pt != ' ')
        {
            pt++;
            if((*pt == '\0') || (*pt == 0xa)) //结尾是0xa
                break;

        }
        *pt = '\0';
        pt++;
        argc++;
        argv[argc] = pt;
    }

    argv[argc] = NULL;

    //lidbg("argc=%d\n", argc);

    i = 0;
    lidbg("cmd:");
    while(i < argc)
    {
        printk("%s ", argv[i]);
        i++;
    }
    printk("\n");



    // 解析命令

    // 调用其他驱动
    if(!strcmp(argv[0], "d"))
    {

        struct file *file = NULL;
        mm_segment_t old_fs;
        ssize_t result;
        ssize_t ret;
        char buf[512];
        memset(buf, 0x56, 512);
        //lidbg("d\n");

        file = filp_open(argv[1],  O_RDWR, 0);
        if(IS_ERR(file))
        {
            lidbg("open device io error");
        }

        //old_fs = get_fs();
        //set_fs(get_ds());
        BEGIN_KMEM;

        if (!strcmp(argv[2], "write"))
        {
            ret = file->f_op->write(file, &argv[3], argc - 3, &file->f_pos); //未测试
        }
        else if(!strcmp(argv[2], "read"))
        {
            result = file->f_op->read(file, &argv[3], argc - 3, &file->f_pos);
        }
        else if(!strcmp(argv[2], "ioctl"))
        {
#if 0
            result = file->f_op->ioctl(file->f_dentry->d_inode, file,
                                       /*strtoul*/simple_strtoul(argv[3], 0, 0), /*strtoul用户态*/simple_strtoul(argv[4], 0, 0));//cmd, arg
#endif
        }

        //set_fs(old_fs);
        END_KMEM;
        filp_close(file, 0);

    }
    // 调用其他模块的函数
    else if (!strcmp(argv[0], "c"))
    {
        int new_argc;
        char **new_argv;
        new_argc = argc - 2;
        new_argv = argv + 2;

        if(!strcmp(argv[1], "lidbg_get"))
        {
        	lidbg("lidbg_devp addr = %x\n",(u32)(struct lidbg_dev *)global_lidbg_devp);
			*(u32 *)(((struct lidbg_dev *)global_lidbg_devp)->mem) = (u32)(struct lidbg_dev *)global_lidbg_devp;
		   
        }


        if(!strcmp(argv[1], "mem"))
        {
            lidbg_mem_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "i2c"))
        {
            mod_i2c_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "io"))
        {
            mod_io_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "ad"))
        {
            mod_ad_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "spi"))
        {
            //mod_spi_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "display"))
        {
            lidbg_display_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "key"))
        {
            lidbg_key_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "touch"))
        {
            lidbg_touch_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "soc"))
        {
            lidbg_soc_main(new_argc, new_argv);
        }
        else if(!strcmp(argv[1], "serial"))
        {
            // lidbg_serial_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "device"))
        {
          //  lidbg_device_main(new_argc, new_argv);
        }

        else if(!strcmp(argv[1], "servicer"))
        {
            lidbg_servicer_main(new_argc, new_argv);
        }
#if 1
        else if(!strcmp(argv[1], "video"))
        {
      	  if(((struct lidbg_dev *)global_lidbg_devp)!=NULL)
         	 ((struct lidbg_dev *)global_lidbg_devp)->soc_func_tbl.pfnlidbg_video_main(new_argc, new_argv);
        }
#endif

	else if (!strcmp(argv[0], "msg"))
	{
		memcpy(plidbg_msg->w_pos, dev->mem, size);
		

	}

    }

    return size;//若不为size则重复执行
}

/* seek文件定位函数 */
static loff_t lidbg_llseek(struct file *filp, loff_t offset, int orig)
{
    loff_t ret = 0;
    switch (orig)
    {
    case 0:   /*相对文件开始位置偏移*/
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
    case 1:   /*相对文件当前位置偏移*/
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

/*文件操作结构体*/
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

/*初始化并注册cdev*/
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

/*设备驱动模块加载函数*/
int lidbg_init(void)
{
    int result,err;
    dev_t devno = MKDEV(lidbg_major, 0);
    lidbg("lidbg_init\n");
    //dump_build_time();
    DUMP_BUILD_TIME;
    /* 申请设备号*/
    if (lidbg_major)
        result = register_chrdev_region(devno, 1, "lidbg");
    else  /* 动态申请设备号 */
    {
        result = alloc_chrdev_region(&devno, 0, 1, "lidbg");
        lidbg_major = MAJOR(devno);
    }
    if (result < 0)
        return result;
#if 0
    /* 动态申请设备结构体的内存*/
    lidbg_devp = kmalloc(sizeof(struct lidbg_dev), GFP_KERNEL);

    if (!lidbg_devp)    /*申请失败*/
    {
        result =  - ENOMEM;
        goto fail_malloc;
    }
    memset(lidbg_devp, 0, sizeof(struct lidbg_dev));
#endif
    lidbg_setup_cdev((struct lidbg_dev *)global_lidbg_devp, 0);


    lidbg("creating mlidbg class.\n");


    //自动在/dev下创建my_device设备文件
    //在驱动初始化的代码里调用class_create为该设备创建一个class，
    /* creating your own class */
    my_class = class_create(THIS_MODULE, "mlidbg_class");
    if(IS_ERR(my_class))
    {
        lidbg("Err: failed in creating mlidbg class.\n");
        return -1;
    }

    //再为每个设备调用 class_device_create创建对应的设备
    /* register your own device in sysfs, and this will cause udevd to create corresponding device node */
    // class_device_create(my_class, NULL, MKDEV(LIDBG_MAJOR, LIDBG_MINOR), NULL, "mlidbg" "%d", LIDBG_MINOR );
    device_create(my_class, NULL, MKDEV(LIDBG_MAJOR, LIDBG_MINOR), NULL, "mlidbg" "%d", LIDBG_MINOR );

    /*\u521b\u5efa/proc/hello\u6587\u4ef6*/
    lidbg_create_proc();

	//set_func_tbl();

    lidbg_soc_init();

    lidbg_board_init();


	msg_task = kthread_create(thread_msg, NULL, "msg_task");
	if(IS_ERR(msg_task))
	{
		lidbg("Unable to start kernel thread.\n");

	}else wake_up_process(msg_task);

    return 0;

fail_malloc:
    unregister_chrdev_region(devno, 1);
    return result;
}

/*模块卸载函数*/
void lidbg_exit(void)
{
    lidbg("mlidbg_exit\n");

    cdev_del(&((struct lidbg_dev *)global_lidbg_devp)->cdev);   /*注销cdev*/
    kfree(((struct lidbg_dev *)global_lidbg_devp));     /*释放设备结构体内存*/


    // class_device_destroy(my_class, MKDEV(LIDBG_MAJOR, LIDBG_MINOR));
    device_destroy(my_class, MKDEV(LIDBG_MAJOR, LIDBG_MINOR));

    class_destroy(my_class);

    /*\u5220\u9664/proc/hello\u6587\u4ef6*/
    lidbg_remove_proc();

    lidbg_board_deinit();
    lidbg_soc_deinit();

    // lidbg_key_deinit();
    unregister_chrdev_region(MKDEV(lidbg_major, 0), 1); /*释放设备号*/
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
        //entry->owner = THIS_MODULE;
        entry->read_proc = lidbg_proc_read;
        entry->write_proc = lidbg_proc_write;
    }
}

void lidbg_remove_proc(void)
{
    remove_proc_entry(LIDBG_DEVICE_PROC_NAME, NULL);
}







MODULE_AUTHOR("Lsw");
//MODULE_LICENSE("Dual BSD/GPL");
MODULE_LICENSE("GPL");

module_param(lidbg_major, int, S_IRUGO);

module_init(lidbg_init);
module_exit(lidbg_exit);
