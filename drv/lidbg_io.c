
//http://blog.chinaunix.net/uid-20543672-id-3076669.html

#include "lidbg.h"
#define DEVICE_NAME "mlidbg_io"
#ifdef _LIGDBG_SHARE__
LIDBG_SHARE_DEFINE;
void *global_lidbg_devp;

#endif

#define IO_IOCTL_READ  		(0)
#define IO_IOCTL_WRITE  	(1)

irqreturn_t io_test_irq(int irq, void *dev_id)
{

    lidbg("io_test_irq: %d \n", irq);
    return IRQ_HANDLED;

}

void mod_io_main(int argc, char **argv)
{

#ifdef SOC_RK2818

    if(argc < 4)
    {
        lidbg("Usage:\n");
        lidbg("w group index mux_index status\n");
        lidbg("r group index mux_index \n");
        return;
    }
#endif

#ifdef SOC_TCC8803


    if(argc < 3)
    {
        lidbg("Usage:\n");
        lidbg("w group index status\n");
        lidbg("r group index\n");
        lidbg("i\n");
        return;
    }
#endif

#ifdef SOC_MSM8x25

    if(argc < 2)
    {
        lidbg("Usage:\n");
        lidbg("w index status\n");
        lidbg("r index \n");
        lidbg("i\n");
        return;
    }

#endif

    if(!strcmp(argv[0], "r"))
    {
#ifdef SOC_TCC8803
        struct io_config io_cfg;
        __u32 group;
        __u32 index;
        __u32 status;
        group = simple_strtoul(argv[1], 0, 0);
        index = simple_strtoul(argv[2], 0, 0);

        io_cfg.group = group;
        io_cfg.index = index;

        status = soc_io_read(&io_cfg);
#endif


#ifdef SOC_MSM8x25
        u32 index;
        u32 pull;
        u32 drive_strength;
        bool status;
        index = simple_strtoul(argv[1], 0, 0);

        share_soc_io_config( index,  GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA, 1);
        status = share_soc_io_input(index);

#endif
        lidbg("read: %x\n", status);
        *(int *)(((struct lidbg_dev *)global_lidbg_devp)->mem) = (int)status;


    }
    else if(!strcmp(argv[0], "w"))
    {

#ifdef SOC_TCC8803
        struct io_config io_cfg;
        __u32 group;
        __u32 index;
        __u32 status;
        //lidbg("w group index mux_index status\n");
        group = simple_strtoul(argv[1], 0, 0);
        index = simple_strtoul(argv[2], 0, 0);
        status = simple_strtoul(argv[3], 0, 0);

        io_cfg.group = group;
        io_cfg.index = index;
        io_cfg.status = status;

        soc_io_write(&io_cfg);
#endif

#ifdef SOC_MSM8x25

        u32 index;
        u32 pull;
        u32 drive_strength;
        bool status;

        if(argc < 3)
        {
            lidbg("Usage:\n");
            lidbg("w index status\n");
            lidbg("r index \n");
            lidbg("i\n");
            return;
        }

        index = simple_strtoul(argv[1], 0, 0);
        status = simple_strtoul(argv[2], 0, 0);

        share_soc_io_config( index,  GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA, 1);
        share_soc_io_output(index, status);

#endif
    }
    else if(!strcmp(argv[0], "i"))
    {

        struct io_int_config   *pio_int_config;
        pio_int_config = kmalloc(sizeof(struct io_int_config), GFP_KERNEL);

        pio_int_config->ext_int_num = simple_strtoul(argv[1], 0, 0);
        lidbg("ext_int_num: %d\n", pio_int_config->ext_int_num);
        pio_int_config->ext_int_num = MSM_GPIO_TO_INT(pio_int_config->ext_int_num);
        lidbg("ext_int_num: %d\n", pio_int_config->ext_int_num);

        pio_int_config->irqflags =  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT;

        pio_int_config->pisr = io_test_irq;
        pio_int_config->dev = (void *)pio_int_config;
        share_soc_io_irq(pio_int_config);

    }

}


int io_open(struct inode *inode, struct file *filp)
{

    lidbg("io_open!\n");
    return 0;
}

ssize_t  io_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{

    return size;
}


ssize_t  io_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{

    return size;
}


static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   = io_open,
    .read   =   io_read,
    .write  =  io_write,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};

static void share_set_func_tbl(void)
{
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnmod_io_main = mod_io_main;
}
static int __init io_init(void)
{
    int ret = 0;
    lidbg (DEVICE_NAME"io_dev_init\n");
    DUMP_BUILD_TIME;


#ifdef _LIGDBG_SHARE__
    LIDBG_SHARE_GET;
    share_set_func_tbl();
    global_lidbg_devp = plidbg_share->lidbg_devp;
#endif
    //下面的代码可以自动生成设备节点，但是该节点在/dev目录下，而不在/dev/misc目录下
    //其实misc_register就是用主设备号10调用register_chrdev()的 misc设备其实也就是特殊的字符设备。
    //注册驱动程序时采用misc_register函数注册，此函数中会自动创建设备节点，即设备文件。无需mknod指令创建设备文件。因为misc_register()会调用class_device_create()或者device_create()。
#if 1
    ret = misc_register(&misc);
#endif
    return ret;
}

static void __exit io_exit(void)
{
#if 1
    misc_deregister(&misc);
#endif
    lidbg (DEVICE_NAME"dev_exit\n");
}

module_init(io_init);
module_exit(io_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");

#ifndef _LIGDBG_SHARE__

EXPORT_SYMBOL(mod_io_main);
#endif

