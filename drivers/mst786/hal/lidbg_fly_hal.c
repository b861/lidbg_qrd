/* Copyright (c) 2012, swlee
 *
 */
#define LIDBG_FLY_HAL

#include "lidbg.h"

LIDBG_DEFINE;

struct task_struct *soc_task;
FLY_SYSTEM_STATUS g_system_status = FLY_ACC_ON;


char *insmod_list[] =
{
    "lidbg_fastboot.ko",
//    "lidbg_lpc.ko",
//	"lidbg_devices.ko",
//	"lidbg_bpmsg.ko",
//	"lidbg_gps.ko",
//	"lidbg_videoin.ko",
//	"lidbg_ts_probe.ko",
	NULL,
};

char *insmod_path[] =
{
    "/system/lib/modules/out/",
    "/flysystem/lib/out/",
    NULL,
};

void hal_func_tbl_default(void)
{
    lidbgerr("hal_func_tbl_default:this func not ready!\n");
	//print who call this
	dump_stack();

}
int soc_thread(void *data)
{
	int i,j;
	char path[100];
	for(i=0;insmod_path[i]!=NULL;i++)	
	{
		for(j=0;insmod_list[j]!=NULL;j++)
		{
			sprintf(path, "%s%s", insmod_path[i],insmod_list[j]);
			//lidbg("load %s\n",path);
			lidbg_insmod( path );
			msleep(100);
		}
	}

#if (defined(BOARD_V1) || defined(BOARD_V2))

#else
		msleep(1000);
		if(lidbg_exe("/system/bin/lidbg_servicer",NULL,NULL,NULL,NULL,NULL,NULL)<0)
			lidbg_exe("/flysystem/bin/lidbg_servicer",NULL,NULL,NULL,NULL,NULL,NULL);
#endif
	return 0;
}



bool SOC_IO_ISR_Add(u32 irq, u32  interrupt_type, pinterrupt_isr func, void *dev)
{
    bool ret = 0;
    struct io_int_config io_int_config1;

    io_int_config1.ext_int_num = GPIO_TO_INT(irq);
    io_int_config1.irqflags = interrupt_type;
    io_int_config1.pisr = func;
    io_int_config1.dev = dev;

    lidbg("ext_int_num:%d \n", irq);

    ret =  soc_io_irq(&io_int_config1);
    return ret;
}

bool SOC_IO_ISR_Enable(u32 irq)
{
    soc_irq_enable(GPIO_TO_INT(irq));
    return 1;
}


bool SOC_IO_ISR_Disable(u32 irq)
{
    soc_irq_disable(GPIO_TO_INT(irq));
    return 1;
}
bool SOC_IO_ISR_Del (u32 irq)
{
    free_irq(GPIO_TO_INT(irq), NULL);
    return 1;
}


bool SOC_IO_Config(u32 index, bool direction, u32 pull, u32 drive_strength)
{
    return soc_io_config( index,  direction, pull, drive_strength, 1);
}

void SOC_IO_Output_Ext(u32 group, u32 index, bool status, u32 pull, u32 drive_strength)
{
    soc_io_config( index,  GPIO_CFG_OUTPUT, pull, drive_strength, 1);
    soc_io_output(group, index,  status);
}

void SOC_IO_Output(u32 group, u32 index, bool status)
{
    soc_io_config( index,  GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA, 0);
    soc_io_output(group, index,  status);
}

bool SOC_IO_Input(u32 group, u32 index, u32 pull)
{
    soc_io_config( index,  GPIO_CFG_INPUT, pull/*GPIO_CFG_NO_PULL*/, GPIO_CFG_16MA, 0);
    return soc_io_input(index);
}

bool SOC_ADC_Get (u32 channel , u32 *value)
{
    *value = 0xffffffff;

    *value = soc_ad_read(channel);

    if(*value == 0xffffffff)
        return 0;
    return 1;
}

void SOC_Key_Report(u32 key_value, u32 type)
{
    lidbg_key_report(key_value, type);
}

// 7bit i2c sub_addr
int SOC_I2C_Send(int bus_id, char chip_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_send( bus_id,  chip_addr,  0,  buf,  size);
}
int SOC_I2C_Rec(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_recv( bus_id,  chip_addr,  sub_addr, buf,  size);
}

int SOC_I2C_Rec_2B_SubAddr(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_recv_sub_addr_2bytes( bus_id,  chip_addr,  sub_addr, buf,  size);
}

int SOC_I2C_Rec_3B_SubAddr(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_recv_sub_addr_3bytes( bus_id,  chip_addr,  sub_addr, buf,  size);
}

int SOC_I2C_Rec_Simple(int bus_id, char chip_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_recv_no_sub_addr( bus_id,  chip_addr,  0, buf,  size);
}

int i2c_api_do_recv_SAF7741(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
int i2c_api_do_send_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
int i2c_api_do_recv_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);

int SOC_I2C_Rec_SAF7741(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_recv_SAF7741( bus_id,  chip_addr, sub_addr, buf,  size);
}

int SOC_I2C_Send_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_send_TEF7000( bus_id,  chip_addr, sub_addr, buf,  size);
}

int SOC_I2C_Rec_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
{
    return  i2c_api_do_recv_TEF7000( bus_id,  chip_addr, sub_addr, buf,  size);
}

void SOC_PWM_Set(int pwm_id, int duty_ns, int period_ns)
{
    soc_pwm_set(pwm_id, duty_ns, period_ns);
}

//0~255
int SOC_BL_Set( u32 bl_level)
{
    soc_bl_set(bl_level);
    return 1;

}

void SOC_Write_Servicer(int cmd)
{
    k2u_write(cmd);
}


int SOC_Display_Get_Res(u32 *screen_x, u32 *screen_y)
{
    return soc_get_screen_res(screen_x, screen_y);
}

void SOC_Mic_Enable( bool enable)
{}
	//I2c_Rate	 i2c_api_set_rate(int  bus_id, int rate)
	//int (*pfnSOC_I2C_Set_Rate)(int  bus_id, int rate);
int SOC_I2C_Set_Rate(int  bus_id, int rate)
{
	return		 i2c_api_set_rate(bus_id, rate);
}

void SOC_IO_Uart_Cfg(u32 baud)
{
	soc_io_uart_cfg(baud);
}

void SOC_IO_Uart_Send( u32 baud,const char *fmt, ... )
{
	va_list args;
	int n;
	char printbuffer[256];

	va_start ( args, fmt );
    n = vsprintf ( printbuffer, (const char *)fmt, args );
    va_end ( args );
	soc_io_uart_send(baud,(unsigned char *)printbuffer);

}


struct fly_smem* SOC_Get_Share_Mem(void)
{
	return p_fly_smem;
}

void SOC_System_Status(FLY_SYSTEM_STATUS status)
{
	g_system_status = status;
}


static void set_func_tbl(void)
{
    //io
    plidbg_dev->soc_func_tbl.pfnSOC_IO_Output = SOC_IO_Output;
    plidbg_dev->soc_func_tbl.pfnSOC_IO_Input = SOC_IO_Input;
    plidbg_dev->soc_func_tbl.pfnSOC_IO_Output_Ext = SOC_IO_Output_Ext;
    plidbg_dev->soc_func_tbl.pfnSOC_IO_Config = SOC_IO_Config;
  
    //i2c
    plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send = SOC_I2C_Send;
    plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec = SOC_I2C_Rec;
    plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_Simple = SOC_I2C_Rec_Simple;
	//add by huangzongqiang	SOC_I2C_Rec_2B_SubAddr(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size)
	plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_2B_SubAddr=SOC_I2C_Rec_2B_SubAddr;
	//I2c_Rate	 i2c_api_set_rate(int  bus_id, int rate)
	//int (*pfnSOC_I2C_Set_Rate)(int  bus_id, int rate);
	plidbg_dev->soc_func_tbl.pfnSOC_I2C_Set_Rate=SOC_I2C_Set_Rate;
    plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_SAF7741 = SOC_I2C_Rec_SAF7741;
    plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send_TEF7000 = SOC_I2C_Send_TEF7000;
    plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_TEF7000 = SOC_I2C_Rec_TEF7000;

    //io-irq
    plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Add = SOC_IO_ISR_Add;
    plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Enable = SOC_IO_ISR_Enable;
    plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Disable = SOC_IO_ISR_Disable;
    plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Del = SOC_IO_ISR_Del;

    //ad
    plidbg_dev->soc_func_tbl.pfnSOC_ADC_Get = SOC_ADC_Get;

    //key
    plidbg_dev->soc_func_tbl.pfnSOC_Key_Report = SOC_Key_Report;

    //bl
    plidbg_dev->soc_func_tbl.pfnSOC_BL_Set = SOC_BL_Set;

    //
    plidbg_dev->soc_func_tbl.pfnSOC_Write_Servicer = SOC_Write_Servicer;
    //video

    //display/touch
    plidbg_dev->soc_func_tbl.pfnSOC_Display_Get_Res = SOC_Display_Get_Res;

    //mic
    plidbg_dev->soc_func_tbl.pfnSOC_Mic_Enable = SOC_Mic_Enable;

   // uart
    plidbg_dev->soc_func_tbl.pfnSOC_IO_Uart_Send = SOC_IO_Uart_Send;

   plidbg_dev->soc_func_tbl.pfnSOC_Get_Share_Mem = SOC_Get_Share_Mem;
   plidbg_dev->soc_func_tbl.pfnSOC_System_Status = SOC_System_Status;

   
}

int hal_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int hal_release(struct inode *inode, struct file *filp)
{
    return 0;
}


ssize_t hal_read(struct file *filp, char __user *buf, size_t size,
                          loff_t *ppos)
{
    unsigned int count = 4;
    int ret = 0;
	u32 read_value = 0;
	read_value = (u32)plidbg_dev;

    printk("hal_read:read_value=%x,read_count=%d\n", (u32)read_value, count);
    if (copy_to_user(buf, &read_value, count))
    {
        ret =  - EFAULT;
    }
    else
    {
        ret = count;
    }

    return count;
}



#define DEVICE_NAME "lidbg_hal"

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   =   hal_open,
    .read   =   hal_read,
    .release =  hal_release,
};


static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,

};


int fly_hal_init(void)
{
	int ret;
	DUMP_BUILD_TIME;
	ret = misc_register(&misc);

	plidbg_dev = kmalloc(sizeof(struct lidbg_hal), GFP_KERNEL);
	{
		int i;
		for(i = 0; i < sizeof(plidbg_dev->soc_func_tbl) / 4; i++)
		{
			((int *)&(plidbg_dev->soc_func_tbl))[i] = hal_func_tbl_default;

		}
	}
	memset(&(plidbg_dev->soc_pvar_tbl), (int)NULL, sizeof(struct lidbg_pvar_t));

	set_func_tbl();
	
    soc_task = kthread_create(soc_thread, NULL, "lidbg_soc_thread");
    if(IS_ERR(soc_task))
    {
        lidbg("Unable to start thread.\n");

    }
    else wake_up_process(soc_task);
    return 0;
}

void fly_hal_deinit(void)
{
}

module_init(fly_hal_init);
module_exit(fly_hal_deinit);

EXPORT_SYMBOL(g_system_status);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

