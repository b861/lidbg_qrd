/* Copyright (c) 2012, swlee
 *
 */



#include "lidbg.h"
#define DEVICE_NAME "mlidbg_cmn"
struct lidbg_dev *lidbg_devp; /*�豸�ṹ��ָ��*/



u32 GetNsCount(void)
{
    struct timespec t_now;
    getnstimeofday(&t_now);
    return t_now.tv_sec * 1000 + t_now.tv_nsec / 1000000;

}


ssize_t  cmn_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{


    return size;
}

ssize_t  cmn_write(struct file *filp, const char __user *buffer, size_t size, loff_t *offset)
{

    return size;
}


int cmn_open(struct inode *inode, struct file *filp)
{

    return 0;
}

#if 0
static int cmn_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{

    return 1;
}
#endif

void mod_cmn_main(int argc, char **argv)
{


    return;
}

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
#if 0
    .ioctl	=	cmn_ioctl,
#endif
    .open   = cmn_open,
    .read   =   cmn_read,
    .write  =  cmn_write,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};

void soc_func_tbl_default()
{
	lidbgerr("soc_func_tbl_default:this func not ready!\n");

}

static int __init cmn_init(void)
{
    int ret;

    //����Ĵ�������Զ������豸�ڵ㣬���Ǹýڵ���/devĿ¼�£�������/dev/miscĿ¼��
    //��ʵmisc_register���������豸��10����register_chrdev()�� misc�豸��ʵҲ����������ַ��豸��
    //ע����������ʱ����misc_register����ע�ᣬ�˺����л��Զ������豸�ڵ㣬���豸�ļ�������mknodָ����豸�ļ�����Ϊmisc_register()�����class_device_create()����device_create()��

    ret = misc_register(&misc);
    dbg (DEVICE_NAME"cmn dev_init\n");
    DUMP_BUILD_TIME;
#if 0
    {
        int tmp1, tmp2, tmp3;
        tmp1 = GetNsCount();
        msleep(5);
        tmp2 = GetNsCount();
        tmp3 = tmp2 - tmp1;
        dbg ("tmp3=%x \n", tmp3);

    }
#endif
    /* ��̬�����豸�ṹ����ڴ�*/
    lidbg_devp = kmalloc(sizeof(struct lidbg_dev), GFP_KERNEL);

    if (!lidbg_devp)    /*����ʧ��*/
    {
        //result =  - ENOMEM;
        //goto fail_malloc;
        lidbg ("kmalloc fail\n");
        return -1;
    }
    memset(lidbg_devp, 0, sizeof(struct lidbg_dev));
    //memset(&lidbg_devp->soc_func_tbl, soc_func_tbl_default, sizeof(struct lidbg_fn_t));
    {
    	int i;
		for(i=0;i<sizeof(lidbg_devp->soc_func_tbl)/4;i++)
		{
			((int*)&(lidbg_devp->soc_func_tbl))[i]=soc_func_tbl_default;

		}
		//fot test
		//(lidbg_devp->soc_func_tbl.pfnSOC_Write_Servicer)(i);


	}

    return ret;
}

static void __exit cmn_exit(void)
{
    misc_deregister(&misc);
    dbg (DEVICE_NAME"cmn  dev_exit\n");
}

module_init(cmn_init);
module_exit(cmn_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");


EXPORT_SYMBOL(mod_cmn_main);
EXPORT_SYMBOL(GetNsCount);

EXPORT_SYMBOL(lidbg_devp);


