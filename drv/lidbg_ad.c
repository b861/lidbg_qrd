


#include "lidbg.h"
#define DEVICE_NAME "mlidbg_ad"



ssize_t  ad_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{

#if 0
    //未测试

    struct ad_config ad_cfg;
    //char **kp_char;
    __u32 group;
    __u32 index;
    __u32 mux_index;
    __u32 status;
    int i;

    dbg ("ad_read!\n");


    //kp_char = kzalloc(size, GFP_KERNEL);
    //copy_from_user(*kp_char, buffer, size);

    i = 0;
    while(i < size)
    {
        lidbg("%s ", (char *)buffer[i]);
        i++;
    }
    lidbg("\n");


    group = simple_strtoul((char *)buffer[0], 0, 0);
    index = simple_strtoul((char *)buffer[1], 0, 0);
    mux_index = simple_strtoul((char *)buffer[2], 0, 0);
    status = simple_strtoul((char *)buffer[3], 0, 0);

    ad_cfg.group = group;
    ad_cfg.index = index;
    ad_cfg.mux_index = mux_index;
    ad_cfg.status = status;

    ad_cfg.directadn = GPAD_IN;
    ad_cfg.pull_en = 0;
    ad_cfg.interrupt_en = 0;
    ad_cfg.interrupt_type = 0;

    soc_ad_read(&ad_cfg);
#endif
    return size;
}

ssize_t  ad_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{
#if 0

    //未测试
    struct ad_config ad_cfg;
    //char **kp_char;
    __u32 group;
    __u32 index;
    __u32 mux_index;
    __u32 status;
    int i;

    dbg ("ad_write!\n");


    //kp_char = kzalloc(size, GFP_KERNEL);
    //copy_from_user(*kp_char, buffer, size);

    i = 0;
    while(i < size)
    {
        lidbg("%s ", (char *)buffer[i]);
        i++;
    }
    lidbg("\n");


    group = simple_strtoul((char *)buffer[0], 0, 0);
    index = simple_strtoul((char *)buffer[1], 0, 0);
    mux_index = simple_strtoul((char *)buffer[2], 0, 0);
    status = simple_strtoul((char *)buffer[3], 0, 0);

    ad_cfg.group = group;
    ad_cfg.index = index;
    ad_cfg.mux_index = mux_index;
    ad_cfg.status = status;

    ad_cfg.directadn = GPAD_OUT;
    ad_cfg.pull_en = 0;
    ad_cfg.interrupt_en = 0;
    ad_cfg.interrupt_type = 0;

    soc_ad_write(&ad_cfg);
#endif
    return size;
}


int ad_open(struct inode *inode, struct file *filp)
{

    lidbg("ad_open!\n");
    return 0;
}

#if 0
static int ad_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
#if 0
    dbg ("ad_adctl!\n");
    dbg (": %x %x\n", (u32)arg, cmd);
    /*
        switch(cmd)
        {
        case AD_ADCTL_READ:
        case AD_ADCTL_WRITE:
        {
            break;

        }
    */
    dbg (DEVICE_NAME": %d %d\n", (u32)arg, cmd);
    return 1;
default:
    return -EINVAL;
}
#endif
return 1;


}
#endif


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
        val = soc_ad_read(ch);
        lidbg("soc_ad_read: %d\n", val);
        msleep(interval);
    }
}


static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    //.ioctl	=	ad_ioctl,
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
static int __init ad_init(void)
{
    int ret;

    //下面的代码可以自动生成设备节点，但是该节点在/dev目录下，而不在/dev/misc目录下
    //其实misc_register就是用主设备号10调用register_chrdev()的 misc设备其实也就是特殊的字符设备。
    //注册驱动程序时采用misc_register函数注册，此函数中会自动创建设备节点，即设备文件。无需mknod指令创建设备文件。因为misc_register()会调用class_device_create()或者device_create()。

    ret = misc_register(&misc);
    dbg (DEVICE_NAME"dev_init\n");
    DUMP_BUILD_TIME;
    return ret;
}

static void __exit ad_exit(void)
{
    misc_deregister(&misc);
    dbg (DEVICE_NAME"dev_exit\n");
}

module_init(ad_init);
module_exit(ad_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");



EXPORT_SYMBOL(mod_ad_main);


