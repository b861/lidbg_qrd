
#include "lidbg.h"
#define DEVICE_NAME "mlidbg_ad"

#ifdef _LIGDBG_SHARE__
LIDBG_SHARE_DEFINE;
#endif

ssize_t  ad_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
    return size;
}

ssize_t  ad_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{
    return size;
}


int ad_open(struct inode *inode, struct file *filp)
{
    lidbg("ad_open!\n");
    return 0;
}


void mod_ad_main(int argc, char **argv)
{

    __u32 val, ch, interval;

    if(argc < 2)
    {
        lidbg("Usage:\n");
        lidbg("ch interval\n");
        return;
    }
    ch = simple_strtoul(argv[0], 0, 0);
    interval = simple_strtoul(argv[1], 0, 0);

    while(1)
    {
        if(interval == 0) break;
        val = share_soc_ad_read(ch);
        lidbg("soc_ad_read: %d\n", val);
        msleep(interval);
    }
}


static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   = ad_open,
    .read   =   ad_read,
    .write  =  ad_write,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};


static void share_set_func_tbl(void)
{
    //ad
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnmod_ad_main = mod_ad_main;
}

static int __init ad_init(void)
{
    int ret = 0;
    DUMP_BUILD_TIME;

    //下面的代码可以自动生成设备节点，但是该节点在/dev目录下，而不在/dev/misc目录下
    //其实misc_register就是用主设备号10调用register_chrdev()的 misc设备其实也就是特殊的字符设备。
    //注册驱动程序时采用misc_register函数注册，此函数中会自动创建设备节点，即设备文件。无需mknod指令创建设备文件。因为misc_register()会调用class_device_create()或者device_create()。
#ifdef _LIGDBG_SHARE__
    LIDBG_SHARE_GET;
    share_set_func_tbl();
#endif

    lidbg (DEVICE_NAME"dev_init\n");
    return ret;
}

static void __exit ad_exit(void)
{
    misc_deregister(&misc);
    lidbg (DEVICE_NAME"dev_exit\n");
}

module_init(ad_init);
module_exit(ad_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");


#ifndef _LIGDBG_SHARE__
EXPORT_SYMBOL(mod_ad_main);
#endif

