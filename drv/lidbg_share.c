/* Copyright (c) 2012, swlee
 *
 */

#include "lidbg.h"
#define DEVICE_NAME "lidbg_share"
struct lidbg_share *plidbg_share = NULL;

//struct lidbg_dev *lidbg_devp; /*设备结构体指针*/
void *global_lidbg_devp;

ssize_t  share_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
	int ret;
	while(plidbg_share == NULL)
		lidbg("plidbg_share == NULL\n");
	
    printk("share_read:read_value=%x,read_count=%d\n", (u32)plidbg_share, 4);
    if (copy_to_user(buffer, (void *)&plidbg_share, 4))
    {
        ret =  - EFAULT;
    }
    else
    {
        ret = size;
    }


    return size;
}

ssize_t  share_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{

    return size;
}


int share_open(struct inode *inode, struct file *filp)
{
	DUMP_FUN;

    return 0;
}

int share_release(struct inode *inode, struct file *filp)
{
	DUMP_FUN;
    return 0;
}


void mod_share_main(int argc, char **argv)
{

    return;
}

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
#if 0
    .ioctl	=	share_ioctl,
#endif
    .open   = share_open,
    .read   =   share_read,
    .write  =  share_write,
    .release = share_release,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};

void share_func_tbl_default()
{
    lidbgerr("share_func_tbl_default:this func not ready!\n");

}
void soc_func_tbl_default()
{
    lidbgerr("soc_func_tbl_default:this func not ready!\n");

}

static int __init share_init(void)
{
    int ret;

    ret = misc_register(&misc);
    lidbg (DEVICE_NAME"share dev_init\n");
    DUMP_BUILD_TIME;
    plidbg_share = kmalloc(sizeof(struct lidbg_share), GFP_KERNEL);

    if (!plidbg_share) 
    {

        lidbg ("kmalloc fail\n");
        return -1;
    }
    memset(plidbg_share, 0, sizeof(struct lidbg_share));
    {
        int i;
        for(i = 0; i < sizeof(plidbg_share->share_func_tbl) / 4; i++)
        {
            ((int *)&(plidbg_share->share_func_tbl))[i] = share_func_tbl_default;

        }

    }
/////

    /* 动态申请设备结构体的内存*/
    plidbg_share->lidbg_devp = kmalloc(sizeof(struct lidbg_dev), GFP_KERNEL);
    global_lidbg_devp = plidbg_share->lidbg_devp;
    if (!plidbg_share->lidbg_devp)    /*申请失败*/
    {
        //result =  - ENOMEM;
        //goto fail_malloc;
        lidbg ("kmalloc fail\n");
        return -1;
    }
    memset(plidbg_share->lidbg_devp, 0, sizeof(struct lidbg_dev));
    //memset(&lidbg_devp->soc_func_tbl, soc_func_tbl_default, sizeof(struct lidbg_fn_t));
    {
        int i;
        for(i = 0; i < sizeof(plidbg_share->lidbg_devp->soc_func_tbl) / 4; i++)
        {
            ((int *)&(plidbg_share->lidbg_devp->soc_func_tbl))[i] = soc_func_tbl_default;

        }
        //fot test
        //(lidbg_devp->soc_func_tbl.pfnSOC_Write_Servicer)(i);
    }


    memset(&(plidbg_share->lidbg_devp->soc_pvar_tbl), 0, sizeof(struct lidbg_pvar_t));
	
    return ret;
}

static void __exit share_exit(void)
{
    misc_deregister(&misc);
    lidbg (DEVICE_NAME"share  dev_exit\n");
}

module_init(share_init);
module_exit(share_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");

#ifndef _LIGDBG_SHARE__

EXPORT_SYMBOL(global_lidbg_devp);
#endif

