#include "lidbg.h"

LIDBG_DEFINE;

#define RUN_ACCBOOT
#define DEVICE_NAME "lidbg_acc"
#define HAL_SO "/flysystem/lib/hw/flyfa.default.so"


void lidbg_accoff_main(void);
void lidbg_accon_main(void);
void lidbg_suspendon_main(void);
void lidbg_suspendoff_main(void);

int acc_on =0;
static DECLARE_COMPLETION(acc_ready);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
DECLARE_MUTEX(lidbg_acc_sem);
#else
DEFINE_SEMAPHORE(lidbg_acc_sem);
#endif

static struct task_struct *acc_task;
static struct task_struct *resume_task;
static int thread_acc(void *data);
static int thread_acc_resume(void *data);


#ifdef CONFIG_HAS_EARLYSUSPEND
static void acc_early_suspend(struct early_suspend *handler);
static void acc_late_resume(struct early_suspend *handler);
struct early_suspend early_suspend;
#endif

typedef struct
{
	unsigned int  acc_flag;
} lidbg_acc;

lidbg_acc *plidbg_acc = NULL;


void wakelock_stat(int lock,const char* name)
{
	lidbg_wakelock_register(lock,name);
}


static void set_func_tbl(void)
{
   // plidbg_dev->soc_func_tbl.pfnSOC_Get_WakeLock = fastboot_get_wake_locks;
    plidbg_dev->soc_func_tbl.pfnSOC_WakeLock_Stat = wakelock_stat;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void acc_early_suspend(struct early_suspend *handler)
{
	//USB_WORK_DISENABLE;
	if(!fs_is_file_exist(HAL_SO))
	{
		//USB_WORK_DISENABLE;
	}

	DUMP_FUN_ENTER;
}

static void acc_late_resume(struct early_suspend *handler)
{

    DUMP_FUN_ENTER;

        lidbg("create thread_acc_resume!\n");
        resume_task = kthread_create(thread_acc_resume, NULL, "acc_resume_task");
        if(IS_ERR(resume_task))
        {
            lidbg("Unable to start kernel  resume_task thread.\n");
            PTR_ERR(resume_task);
        }
        else wake_up_process(resume_task);

    DUMP_FUN_LEAVE;
}
#endif



static int thread_acc_resume(void *data)
{
    DUMP_FUN_ENTER;

   	 //msleep( );
    	if(!fs_is_file_exist(HAL_SO))
	{
		//USB_WORK_ENABLE;
	}


    DUMP_FUN_LEAVE;
    return 0;

}


void acc_pwroff(void)
{
	DUMP_FUN_ENTER;

	//USB_WORK_DISENABLE;
	
#ifdef RUN_ACCBOOT


switch(acc_on)
{
	case 1:
		lidbg("send CMD_FAST_POWER_OFF  to lidbg_server\n");
		SOC_Write_Servicer(CMD_FAST_POWER_OFF);
		break;
	case 2:
		lidbg("send CMD_ACC_ON  to lidbg_server\n");
		SOC_Write_Servicer(CMD_ACC_ON);
		break;
	case 3:
		lidbg("send CMD_ACC_OFF  to lidbg_server\n");
		SOC_Write_Servicer(CMD_ACC_OFF);
		break;
	default:
		lidbg("send ACC_STATE %d  to lidbg_server\n",acc_on);
		break;
}
#endif
}


int thread_acc(void *data)
{
    	lidbg("thread_acc.\n");

    plidbg_acc = ( lidbg_acc *)kmalloc(sizeof(lidbg_acc), GFP_KERNEL);
    memset(plidbg_acc, 0, sizeof(*plidbg_acc));

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            	wait_for_completion(&acc_ready);
		//USB_WORK_DISENABLE;
		msleep(200);
		acc_pwroff();
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


ssize_t  acc_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
    return size;
}

ssize_t  acc_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{

	lidbg("acc_write.\n");
	acc_on=0;
	/*down(&lidbg_acc_sem);

	memset(&(plidbg_acc->acc_flag), 0, sizeof(plidbg_acc->acc_flag));
	if(copy_from_user(&(plidbg_acc->acc_flag), buffer, size))
	{
		lidbg("copy_from_user ERR\n");
	}
	lidbg("acc write %s\n", plidbg_acc->acc_flag)

	up(&lidbg_acc_sem);

	complete(&acc_ready);*/

	{

		char data_rec[20];
		if (copy_from_user( data_rec, buf, count))
		{
		printk("copy_from_user ERR\n");
		}
		data_rec[count] =  '\0';

		printk("acc_nod_write:==%d====[%s]\n", count, data_rec);

		// processing data
		if(!(strnicmp(data_rec, "screen_on", count - 1)))
		{
			printk("******into screen_on********\n");
			lidbg_accon_main();
		}
		else if(!(strnicmp(data_rec, "screen_off", count - 1)))
		{
			printk("******into screen_off********\n");
			lidbg_accoff_main();
		}
		else if(!(strnicmp(data_rec, "suspend_on", count - 1)))
		{
			printk("******into suspend_on********\n");
			lidbg_suspendon_main();
		}
		else if(!(strnicmp(data_rec, "suspend_off", count - 1)))
		{
			printk("******into suspend_off********\n");
			lidbg_suspendoff_main();
		}
		else if(!(strnicmp(data_rec, "power", count - 1)))
		{
			printk("******goto fastboot********\n");
			acc_on=1;
			complete(&acc_ready);
		}
		else if(!(strnicmp(data_rec, "acc_on", count - 1)))
		{
			printk("******goto acc_on********\n");
			acc_on=2;
			complete(&acc_ready);
		}
		else if(!(strnicmp(data_rec, "acc_off", count - 1)))
		{
			printk("******goto acc_off********\n");
			acc_on=3;
			complete(&acc_ready);
		}
		else if(!(strnicmp(data_rec, "backairfan_on", count - 1)))
		{
			printk("******goto backairfan_on********\n");
			AIRFAN_BACK_ON;
		}
		else if(!(strnicmp(data_rec, "backairfan_off", count - 1)))
		{
			printk("******goto backairfan_off********\n");
			AIRFAN_BACK_OFF;
		}
	}
	return count;
}


int acc_open(struct inode *inode, struct file *filp)
{
    //down(&lidbg_msg_sem);

    return 0;
}

int acc_release(struct inode *inode, struct file *filp)
{
    //up(&lidbg_msg_sem);
    return 0;
}

void cb_password_poweroff(char *password )
{
    acc_pwroff();
}
void cb_password_disconnect_usb(char *password )
{
    lidbg_accoff_main();
}
void cb_password_connect_usb(char *password )
{
    lidbg_accon_main();
}
static int  acc_probe(struct platform_device *pdev)
{

	if(!fs_is_file_exist(HAL_SO))
	{
		FORCE_LOGIC_ACC;
	}
	te_regist_password("001200", cb_password_poweroff);
	te_regist_password("001201", cb_password_disconnect_usb);
	te_regist_password("001202", cb_password_connect_usb);
	return 0;
}

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   =   acc_open,
    .read   =   acc_read,
    .write  =   acc_write,
    .release =  acc_release,
};

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};


static int  acc_remove(struct platform_device *pdev)
{
    return 0;
}

#ifdef CONFIG_PM
static int acc_resume(struct device *dev)
{
    DUMP_FUN_ENTER;
  //  lidbg_readwrite_file("/sys/power/state", NULL, "on", sizeof("on")-1);

    return 0;

}

static int acc_suspend(struct device *dev)
{
    DUMP_FUN_ENTER;
    //work_en = 0;
    return 0;

}


static struct dev_pm_ops acc_pm_ops =
{
    .suspend	= acc_suspend,
    .resume		= acc_resume,
};
#endif

static struct platform_driver acc_driver =
{
    .probe		= acc_probe,
    .remove     = acc_remove,
    .driver         = {
        .name = "lidbg_acc1",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &acc_pm_ops,
#endif
    },
};

static struct platform_device lidbg_acc_device =
{
    .name               = "lidbg_acc1",
    .id                 = -1,
};


static int __init acc_init(void)
{
	int ret;
	LIDBG_GET;
    	set_func_tbl();

    platform_device_register(&lidbg_acc_device);

    platform_driver_register(&acc_driver);


	INIT_COMPLETION(acc_ready);
	ret = misc_register(&misc);

#ifdef CONFIG_HAS_EARLYSUSPEND
	{
		early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;//EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
		early_suspend.suspend = acc_early_suspend;
		early_suspend.resume = acc_late_resume;
		register_early_suspend(&early_suspend);
	}
#endif

	acc_task = kthread_create(thread_acc, NULL, "acc_task");
	if(IS_ERR(acc_task))
	{
		 lidbg("Unable to start kernel thread.\n");
	}
	else wake_up_process(acc_task);

	lidbg_chmod("/dev/lidbg_acc");
	
	lidbg (DEVICE_NAME"acc  dev_init\n");


	return ret;
}

static void __exit acc_exit(void)
{
	misc_deregister(&misc);
    	lidbg (DEVICE_NAME"acc  dev_exit\n");
}

//zone end
void lidbg_accoff_main(void)
{
	printk("\n************into lidbg_accoff_main******\n");
	//LCD_OFF;
	    u8 buff[] = {0x02, 0x0d, 0x0};//LPCControlPWREnable
            SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));
			
	if(SOC_Hal_Acc_Callback)
		SOC_Hal_Acc_Callback(0);
}

void lidbg_accon_main(void)
{
	//LCD_ON;
	printk("\n************into lidbg_accon_main******\n");
	u8 buff[] = {0x02, 0x0d, 0x1};//LPCControlPWREnable
        SOC_LPC_Send(buff, SIZE_OF_ARRAY(buff));
	if(SOC_Hal_Acc_Callback)
		SOC_Hal_Acc_Callback(1);

}

void lidbg_suspendon_main(void)
{
	printk("\n************into lidbg_suspendon_main******\n");
	if(SOC_Hal_Acc_Callback)
		SOC_Hal_Acc_Callback(2);
	//msleep(4000);
	USB_WORK_ENABLE;
}

void lidbg_suspendoff_main(void)
{
	printk("\n************into lidbg_suspendoff_main******\n");
	USB_WORK_DISENABLE;
	if(SOC_Hal_Acc_Callback)
		SOC_Hal_Acc_Callback(3);	
}

module_init(acc_init);
module_exit(acc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("mstar lidbg_acc driver");

EXPORT_SYMBOL(lidbg_accoff_main);
