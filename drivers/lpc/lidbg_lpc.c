/* Copyright (c) 2012, swlee
 *
 */
#include "lidbg.h"

LIDBG_DEFINE;
static bool lpc_work_en = true;

#define DATA_BUFF_LENGTH_FROM_MCU   (128)

#define BYTE u8
#define UINT u32
#define UINT32 u32
#define BOOL bool
#define ULONG u32

#define FALSE 0
#define TRUE 1

#define LPC_SYSTEM_TYPE 0x00

typedef struct _FLY_IIC_INFO
{
    struct work_struct iic_work;
} FLY_IIC_INFO, *P_FLY_IIC_INFO;



struct fly_hardware_info
{

    FLY_IIC_INFO FlyIICInfo;

    BYTE buffFromMCU[DATA_BUFF_LENGTH_FROM_MCU];
    BYTE buffFromMCUProcessorStatus;
    UINT buffFromMCUFrameLength;
    UINT buffFromMCUFrameLengthMax;
    BYTE buffFromMCUCRC;
    BYTE buffFromMCUBak[DATA_BUFF_LENGTH_FROM_MCU];

};

#define  MCU_ADDR_W  0xA0
#define  MCU_ADDR_R  0xA1

int lpc_ping_test = 0;
int lpc_ctrl_by_app = 0;

#define LPC_DEBUG_LOG

#ifdef CONFIG_HAS_EARLYSUSPEND
static void lpc_early_suspend(struct early_suspend *handler);
static void lpc_late_resume(struct early_suspend *handler);
struct early_suspend early_suspend;
#endif

struct fly_hardware_info GlobalHardwareInfo;
struct fly_hardware_info *pGlobalHardwareInfo;
static DECLARE_COMPLETION(lpc_read_wait); 

int thread_lpc(void *data)
{
	LPC_PRINT(true,-1,"thread_lpc start");

    while(1)
    {
        if((lpc_work_en)&&(lpc_ctrl_by_app == 0))
            LPC_CMD_NO_RESET;
        msleep(5000);
    }
    return 0;
}

u32 ping_data = 0;
bool ping_repay = 0;
#define LPC_LOG_PATH LIDBG_LOG_DIR"lpc_log.txt"

int thread_lpc_ping_test(void *data)
{
 	u32 ping_wait_cnt = 0;
	
 	u32 err_cnt = 0;
	msleep(1000*20);
    while(1)
    {
		LPC_CMD_PING_TEST(ping_data & 0xff);

		while(ping_repay == 0)
		{
			msleep(50);
			ping_wait_cnt ++;
			if(ping_wait_cnt > 20*5) 
			{
				lidbg_fs_log(LPC_LOG_PATH,"lpc_ping_test err %d\n",err_cnt);
				err_cnt++;
				break;
			}
			
		}
		ping_data++;
		ping_wait_cnt = 0;
		ping_repay = 0;
		//msleep(1000);

    }
    return 0;
}


void LPCCombinDataStream(BYTE *p, UINT len)
{
    UINT i = 0;
    int ret ;
    BYTE checksum = 0;
    BYTE bufData[16];
    BYTE *buf;
    bool bMalloc = FALSE;

    if(!lpc_work_en)
        return;

    if (3 + len + 1 > 16)
    {
        buf = (BYTE *)kmalloc(sizeof(BYTE) * (4 + len), GFP_KERNEL);
        if (buf == NULL)
        {
            LIDBG_ERR("kmalloc.\n");
        }
        bMalloc = TRUE;
    }
    else
    {
        buf = bufData;
    }

    buf[0] = 0xFF;
    buf[1] = 0x55;
    buf[2] = len + 1;
    checksum = buf[2];
    for (i = 0; i < len; i++)
    {
        buf[3 + i] = p[i];
        checksum += p[i];
    }

    buf[3 + i] = checksum;

#ifdef SEND_DATA_WITH_UART
    ret=SOC_Uart_Send(buf);
#else
    ret=SOC_I2C_Send(LPC_I2_ID, MCU_ADDR_W >> 1, buf, 3 + i + 1);
#endif

#ifdef LPC_DEBUG_LOG
    lidbg("ToMCU.%d:%x %x %x\n", ret, p[0], p[1], p[2]);
#endif

    if (bMalloc)
    {
        kfree(buf);
        buf = NULL;
    }
}


static void LPCdealReadFromMCUAll(BYTE *p, UINT length)
{
#if 1
#ifdef LPC_DEBUG_LOG
    {
        u32 i;
        lidbg("From LPC:");//mode ,command,para
        for(i = 0; i < length; i++)
        {
            printk("%x ", p[i]);

        }
        lidbg("\n");
    }
#endif

    switch (p[0])
    {
    case LPC_SYSTEM_TYPE:
		if((p[1] ==0x44) && (p[2] == (ping_data & 0xff))
						&&	 (p[3] == (ping_data & 0xff))
						&&	(p[4] == (ping_data & 0xff))
						&&	(p[5] == (ping_data & 0xff))
						&&	(p[6] == (ping_data & 0xff))
		)
			ping_repay = 1;
        break;
    case 0x96:
        switch (p[2])
        {
        case 0x7f:
#ifdef LPC_DEBUG_LOG
            lidbg("LPC ping return!\n");
#endif
            break;
        }
    default:
        break;
    }
#endif
}

static BOOL readFromMCUProcessor(BYTE *p, UINT length)
{
    UINT i;

    for (i = 0; i < length; i++)
    {
        switch (pGlobalHardwareInfo->buffFromMCUProcessorStatus)
        {
        case 0:
            if (0xFF == p[i])
            {
                pGlobalHardwareInfo->buffFromMCUProcessorStatus = 1;
            }
            break;
        case 1:
            if (0xFF == p[i])
            {
                pGlobalHardwareInfo->buffFromMCUProcessorStatus = 1;
            }
            else if (0x55 == p[i])
            {
                pGlobalHardwareInfo->buffFromMCUProcessorStatus = 2;
            }
            else
            {
                pGlobalHardwareInfo->buffFromMCUProcessorStatus = 0;
            }
            break;
        case 2:
            pGlobalHardwareInfo->buffFromMCUProcessorStatus = 3;
            pGlobalHardwareInfo->buffFromMCUFrameLength = 0;
            pGlobalHardwareInfo->buffFromMCUFrameLengthMax = p[i];
            pGlobalHardwareInfo->buffFromMCUCRC = p[i];
            break;
        case 3:
            if (pGlobalHardwareInfo->buffFromMCUFrameLength < (pGlobalHardwareInfo->buffFromMCUFrameLengthMax - 1))
            {
                pGlobalHardwareInfo->buffFromMCU[pGlobalHardwareInfo->buffFromMCUFrameLength] = p[i];
                pGlobalHardwareInfo->buffFromMCUCRC += p[i];
                pGlobalHardwareInfo->buffFromMCUFrameLength++;
            }
            else
            {
                pGlobalHardwareInfo->buffFromMCUProcessorStatus = 0;
                if (pGlobalHardwareInfo->buffFromMCUCRC == p[i])
                {
                    LPCdealReadFromMCUAll(pGlobalHardwareInfo->buffFromMCU, pGlobalHardwareInfo->buffFromMCUFrameLengthMax - 1);
                }
                else
                {
                    lidbg("\nRead From MCU CRC Error");
                }
            }
            break;
        default:
            pGlobalHardwareInfo->buffFromMCUProcessorStatus = 0;
            break;
        }
    }

    if (pGlobalHardwareInfo->buffFromMCUProcessorStatus > 1)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL actualReadFromMCU(BYTE *p, UINT length)
{

    if(!lpc_work_en)
        return FALSE;

    SOC_I2C_Rec_Simple(LPC_I2_ID, MCU_ADDR_R >> 1, p, length);
    if (readFromMCUProcessor(p, length))
    {

#ifdef LPC_DEBUG_LOG
        lidbg("More ");
#endif
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//MCUµÄIIC¶Á´¦Àí
irqreturn_t MCUIIC_isr(int irq, void *dev_id)
{
	if(g_var.recovery_mode)
	{
		if(lpc_ctrl_by_app)
			complete(&lpc_read_wait);
	}
	else	
    	schedule_work(&pGlobalHardwareInfo->FlyIICInfo.iic_work);
    return IRQ_HANDLED;
}

static void workFlyMCUIIC(struct work_struct *work)
{
    BYTE buff[16];
    BYTE iReadLen = 12;

    while (SOC_IO_Input(MCU_IIC_REQ_GPIO, MCU_IIC_REQ_GPIO, 0) == 0)
    {
        actualReadFromMCU(buff, iReadLen);
        iReadLen = 16;
    }
}


void mcuFirstInit(void)
{
    pGlobalHardwareInfo = &GlobalHardwareInfo;
    INIT_WORK(&pGlobalHardwareInfo->FlyIICInfo.iic_work, workFlyMCUIIC);
    
    //let i2c_c high
    while (SOC_IO_Input(0, MCU_IIC_REQ_GPIO, GPIO_CFG_PULL_UP) == 0)
    {
        u8 buff[32];
        static int count = 0;
        count++;
        WHILE_ENTER;
        actualReadFromMCU(buff, 32);
        if(count > 100)
        {
            lidbg("exit mcuFirstInit!\n");
            break;
        }
        msleep(100);
    }
    SOC_IO_ISR_Add(MCU_IIC_REQ_GPIO, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, MCUIIC_isr, pGlobalHardwareInfo);

    CREATE_KTHREAD(thread_lpc, NULL);

	
    FS_REGISTER_INT(lpc_ping_test, "lpc_ping_test", 0, NULL);
	
	if(lpc_ping_test)
    	CREATE_KTHREAD(thread_lpc_ping_test, NULL);

}


void LPCSuspend(void)
{

    SOC_IO_ISR_Disable(MCU_IIC_REQ_GPIO);
}

void LPCResume(void)
{
    BYTE buff[16];
    BYTE iReadLen = 12;

    SOC_IO_ISR_Enable(MCU_IIC_REQ_GPIO);

    //clear lpc i2c buffer
    while (SOC_IO_Input(MCU_IIC_REQ_GPIO, MCU_IIC_REQ_GPIO, 0) == 0)
    {
        WHILE_ENTER;
        actualReadFromMCU(buff, iReadLen);
        iReadLen = 16;
    }
}

void lpc_linux_sync(bool print,int mint,char *extra_info)
{
    static char buff[64] = {0x00, 0xfd};
    int mtime = 0;
    memset(&buff[2], '\0', sizeof(buff) - 2);
    mtime = ktime_to_ms(ktime_get_boottime());
    snprintf(&buff[2], sizeof(buff) - 3, "%s:%d %d.%d", extra_info,mint, mtime / 1000, mtime % 1000);

    SOC_LPC_Send(buff, strlen(buff + 2) + 2);
	if(print)
    	lidbg("[%s]\n", buff + 2);
}



 int lpc_open(struct inode *inode, struct file *filp)
{
	DUMP_FUN;
	return 0;
}

 int lpc_close(struct inode *inode, struct file *filp)
{
	DUMP_FUN;
	return 0;
}

ssize_t  lpc_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
	int read_cnt;
    char *mem = NULL;
	if(size <= 0)
		return 0;
	lpc_ctrl_by_app = 1;
	mem = kzalloc(size, GFP_KERNEL);
    if (!mem)
    {
        LIDBG_ERR("kzalloc \n");
        return false;
    }

	lidbg("lpc_read size = %d+\n", size);
	wait_for_completion(&lpc_read_wait);
	lidbg("lpc_read size = %d-\n", size);

    read_cnt = SOC_I2C_Rec_Simple(LPC_I2_ID, MCU_ADDR_R >> 1, mem, size);


	if (copy_to_user(buffer, mem, size))
	{
		lidbg("copy_to_user ERR\n");
	}
	kfree(mem);

	return read_cnt;

}
static ssize_t lpc_write(struct file *filp, const char __user *buf,
                         size_t size, loff_t *ppos)
{
	int write_cnt;
    char *mem = kzalloc(size, GFP_KERNEL);
    if (!mem)
    {
        LIDBG_ERR("kzalloc \n");
        return false;
    }
	lpc_ctrl_by_app = 1;

	lidbg("lpc_write size = %d\n", size);

	if(copy_from_user(mem, buf, size))
	{
		lidbg("copy_from_user ERR\n");
	}
	
    write_cnt = SOC_I2C_Send(LPC_I2_ID, MCU_ADDR_W >> 1, mem, size);
	kfree(mem);
	return write_cnt;
}



static struct file_operations lpc_fops =
{
    .owner = THIS_MODULE,
    .open = lpc_open,
    .read = lpc_read,
    .write = lpc_write,
    .release = lpc_close,
};


static int  lpc_probe(struct platform_device *pdev)
{
    DUMP_FUN;
    if(g_var.is_fly)
    {
        lidbg("lpc_init do nothing.disable\n");
        return 0;
    }

#ifdef SOC_mt3360
	return 0;
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
    early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    early_suspend.suspend = lpc_early_suspend;
    early_suspend.resume = lpc_late_resume;
    register_early_suspend(&early_suspend);
#endif

    mcuFirstInit();

	INIT_COMPLETION(lpc_read_wait);
	lidbg_new_cdev(&lpc_fops, "fly_lpc");

    return 0;
}


static int  lpc_remove(struct platform_device *pdev)
{
    return 0;
}


#ifdef CONFIG_HAS_EARLYSUSPEND
static void lpc_early_suspend(struct early_suspend *handler) {}
static void lpc_late_resume(struct early_suspend *handler) {}
#endif


#ifdef CONFIG_PM
static int lpc_suspend(struct device *dev)
{
    DUMP_FUN;
    lpc_work_en = false;
    return 0;
}

static int lpc_resume(struct device *dev)
{
    DUMP_FUN;
	lpc_work_en = true;
    return 0;
}

static struct dev_pm_ops lpc_pm_ops =
{
    .suspend	= lpc_suspend,
    .resume		= lpc_resume,
};
#endif


static struct platform_device lidbg_lpc =
{
    .name               = "lidbg_lpc",
    .id                 = -1,
};

static struct platform_driver lpc_driver =
{
    .probe		= lpc_probe,
    .remove     = lpc_remove,
    .driver         = {
        .name = "lidbg_lpc",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &lpc_pm_ops,
#endif
    },
};


static void set_func_tbl(void)
{
#ifdef SOC_msm8x25
    ((struct lidbg_hal *)plidbg_dev)->soc_func_tbl.pfnSOC_LPC_Send = LPCCombinDataStream;
#else
    ((struct lidbg_interface *)plidbg_dev)->soc_func_tbl.pfnSOC_LPC_Send = LPCCombinDataStream;
#endif

}

static int __init lpc_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;
    set_func_tbl();
    platform_device_register(&lidbg_lpc);
    platform_driver_register(&lpc_driver);
    return 0;
}

static void __exit lpc_exit(void)
{
    platform_driver_unregister(&lpc_driver);
}


module_init(lpc_init);
module_exit(lpc_exit);


MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("lidbg lpc driver");

EXPORT_SYMBOL(lpc_linux_sync);

