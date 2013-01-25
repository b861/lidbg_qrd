/* Copyright (c) 2012, swlee
 *
 */



//#define SOC_COMPILE


#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"

#include "lidbg_enter.h"

LIDBG_DEFINE;
#endif


#include "lidbg_lpc.h"


//#define LPC_DEBUG_LOG



#ifdef CONFIG_HAS_EARLYSUSPEND
static void lpc_early_suspend(struct early_suspend *handler);
static void lpc_late_resume(struct early_suspend *handler);
struct early_suspend early_suspend;

#endif


struct fly_hardware_info GlobalHardwareInfo;
struct fly_hardware_info *pGlobalHardwareInfo;

UINT32 iDriverResumeTime = 0;
BOOL bResumeError = FALSE;
u32 lpc_send_rec_count = 0;

u32 resume_count = 0;


static struct task_struct *lpc_task;
int thread_lpc(void *data);
bool lpc_work_en = 0;

UINT32 GetTickCount(void)

{

    struct timespec t_now;

    do_posix_clock_monotonic_gettime(&t_now);

    monotonic_to_bootbased(&t_now);

    return t_now.tv_sec * 1000 + t_now.tv_nsec / 1000000;

}

void LPCCombinDataStream(BYTE *p, UINT len)
{
    UINT i = 0;
    BYTE checksum = 0;
    BYTE bufData[16];
    BYTE *buf;
    bool bMalloc = FALSE;
    if (3 + len + 1 > 16)
    {
        buf = (BYTE *)kmalloc(sizeof(BYTE) * (4 + len), GFP_KERNEL);
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
        buf[3+i] = p[i];
        checksum += p[i];
    }

    buf[3+i] = checksum;
#ifdef LPC_DEBUG_LOG
    lidbg("ToMCU:%x %x %x\n", p[0], p[1], p[2]);
#endif
    SOC_I2C_Send(I2_0_ID, MCU_ADDR_W >> 1, buf, 3 + i + 1);


    if (bMalloc)
    {
        kfree(buf);
        buf = NULL;
    }
}

//EXPORT_SYMBOL(LPCCombinDataStream);


void LPCBackLightOn(void)
{
    BYTE buff[] = {0x00, 0x94, 0x01, 0x99};

    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));

}

void LPCBackLightOff(void)
{
    BYTE buff[] = {0x00, 0x94, 0x00, 0x98};

    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));

}

void LPCNoReset(void)
{
    BYTE buff[] = {0x00, 0x03, 0x05};
    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));

}




void LPCPowerOnOK(void)
{
    BYTE buff[] = {0x00, 0x00, 0x00};

    DBG0("Control To MCU Power On######################");
    LPCCombinDataStream(buff, 3);
}

void LPCControlHowLongToPowerOn(ULONG iTime)
{
    BYTE buff[] = {0x00, 0x98, 0x00, 0x00, 0x00, 0x00};

    buff[5] = (BYTE)(iTime);
    iTime = iTime >> 8;
    buff[4] = (BYTE)(iTime);
    iTime = iTime >> 8;
    buff[3] = (BYTE)(iTime);
    iTime = iTime >> 8;
    buff[2] = (BYTE)(iTime);

    LPCCombinDataStream(buff, 6);
}

void LPCControlToSleep(void)
{
    BYTE buff[] = {0x00, 0x01, 0x00};

    LPCCombinDataStream(buff, 3);
}
void LPCControlReset(void)
{
    BYTE buff[] = {0x00, 0x03, 0x04};

    LPCCombinDataStream(buff, 3);
}

void LPCControlSupendTestStart(void)
{
    BYTE buff[] = {0x00, 0x04, 0x00};
    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));


}

void LPCControlSupendTestStop(void)
{
    BYTE buff[] = {0x00, 0x04, 0x01};
    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));


}

void LPCControlPWREnable(void)
{
    BYTE buff[] = {0x00, 0x05, 0x01};
    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));


}

void LPCControlPWRDisenable(void)
{
    BYTE buff[] = {0x00, 0x05, 0x00};
    LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));


}


void controlToMCUPing(BOOL bWork)
{
    BYTE buff[] = {0x00, 0x02, 0x00};

    if (bWork)
    {
        buff[2] = 0x00;
    }
    else
    {
        buff[2] = 0x01;
    }

    LPCCombinDataStream(buff, 3);
}

extern bool late_resume_ok;

static void LPCdealReadFromMCUAll(BYTE *p, UINT length)
{
#if 1
    static u32 acc_off_count = 0;
    if (iDriverResumeTime)
    {
        if (GetTickCount() - iDriverResumeTime >= 6 * 1000)
        {
            //bResumeError = TRUE;
            printk("JQilin Resume Error...\n");
        }
    }
    iDriverResumeTime = 0;
#ifdef LPC_DEBUG_LOG
{
	u32 i;
    lidbg("From LPC:");//mode ,command,para
    for(i = 0; i < length; i++)
    {
        printk("%x ", p[i]);

    }
    printk("\n");
}
#endif
	lpc_send_rec_count = 0;

    switch (p[0])
    {
    case LPC_SYSTEM_TYPE:
        switch (p[1])
        {
        case 0x01://ACC OFF/ON
            //case 0x09:
            switch (p[2])
            {
            case 0x00://ACC OFF
                if (bResumeError)
                {
                    lidbg("JQilin Resume Error And Block...\n");
                    return;
                }
                else
                {
                    acc_off_count++;
                    lidbg("AccOff Count:%d\n", acc_off_count);

                }

                if(SOC_PWR_GetStatus() == PM_STATUS_LATE_RESUME_OK)
                {
                    msleep(100);

                    lidbg("Ready ACC OFF!\n");
                    msleep(100);
					lidbg("a\n");
                    LPCControlToSleep();
					lidbg("b\n");
                    SOC_PWR_ShutDown();
					lidbg("c\n");
                }
                else
                {
                    lidbg("PM_STATUS_LATE_RESUME_OK not ready...\n");

                }
                break;
            }
        }
    case 0x96:
        switch (p[2])
        {
        case 0x7f:
#ifdef LPC_DEBUG_LOG
            lidbg("LPC ping return!\n");
#endif
		if(lpc_send_rec_count > 0)
            lpc_send_rec_count--;
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
                    DBG0("\nRead From MCU CRC Error");
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

    SOC_I2C_Rec_Simple(I2_0_ID, MCU_ADDR_R >> 1, p, length);
    if (readFromMCUProcessor(p, length))
    {

#ifdef LPC_DEBUG_LOG
        DBG0("More ");
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

    //lidbg(".\n");
    schedule_work(&pGlobalHardwareInfo->FlyIICInfo.iic_work);
    return IRQ_HANDLED;
}

static void workFlyMCUIIC(struct work_struct *work)
{
    BYTE buff[16];
    BYTE iReadLen = 12;


    //DBG("ThreadFlyMCUIIC running\n");
    //SOC_IO_ISR_Disable(MCU_IIC_REQ_ISR);

    while (SOC_IO_Input(MCU_IIC_REQ_G, MCU_IIC_REQ_I, 0) == 0)
    {
        //WHILE_ENTER;
        actualReadFromMCU(buff, iReadLen);
        iReadLen = 16;
    }
    //SOC_IO_ISR_Enable(MCU_IIC_REQ_ISR);
}

int thread_lpc(void *data)
{


    BYTE buff[] = {0x00, 0x96, 0x00, 0x00, 0x00, 0x00};
    BYTE iRandom = 0;
	
    static u32 re_sleep_count = 0;
    buff[5] = iRandom;
    buff[4] = iRandom;
    buff[3] = iRandom;
    buff[2] = iRandom;
    msleep(10 * 1000);
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            if((SOC_PWR_GetStatus() == PM_STATUS_LATE_RESUME_OK) && (lpc_work_en == 1) && (resume_count > 0))
            {
#ifdef LPC_DEBUG_LOG
                lidbg("lpc_send_rec_count=%d\n", lpc_send_rec_count);
#endif
                if(lpc_send_rec_count >= 5)
                {
                    re_sleep_count++;
                    lidbg("\n\n\nerrlsw:lpc_send_rec_count > 5 ,do SOC_PWR_ShutDown again! %d\n\n\n", re_sleep_count);
					
#ifdef AUTO_SLEEP_WHEN_WAKEUP_NOT_BY_LPC
					if(resume_count > 0)
                    	SOC_PWR_ShutDown();
#endif
                    lpc_send_rec_count = 0;

                }
                //lidbg("thread_lpc:LPCCombinDataStream\n");

                LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));

#if 0
				BYTE buff[32];
				
				struct timex  txc;
				struct rtc_time tm;
				do_gettimeofday(&(txc.time));
				rtc_time_to_tm(txc.time.tv_sec,&tm);
				//printk(\u201cUTC time :%d-%d-%d %d:%d:%d /n\u201d,tm.tm_year+1900,tm.tm_mon, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);

				
                LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));
#endif
	
                lpc_send_rec_count ++;
            }
            msleep(2000);
        }
        else
        {
            schedule_timeout(HZ);
        }
    }

    return 0;

}



void mcuFirstInit(void)
{
    pGlobalHardwareInfo = &GlobalHardwareInfo;
    INIT_WORK(&pGlobalHardwareInfo->FlyIICInfo.iic_work, workFlyMCUIIC);

    //let i2c_c high
    while (SOC_IO_Input(0, MCU_IIC_REQ_I, GPIO_CFG_PULL_UP) == 0)
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
    }

    //SOC_IO_Input(0, MCU_IIC_REQ_I, GPIO_CFG_PULL_UP);
    SOC_IO_ISR_Add(MCU_IIC_REQ_ISR, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, MCUIIC_isr, pGlobalHardwareInfo);

#ifdef FLY_DEBUG
    lpc_task = kthread_create(thread_lpc, NULL, "lpc_task");
    if(IS_ERR(lpc_task))
    {
        lidbg("Unable to start kernel thread.\n");
    }
    else wake_up_process(lpc_task);
#endif


}



void LPCSuspend(void)
{

    SOC_IO_ISR_Disable(MCU_IIC_REQ_ISR);
}

void LPCResume(void)
{
    BYTE buff[16];
    BYTE iReadLen = 12;

    SOC_IO_ISR_Enable(MCU_IIC_REQ_ISR);

    lpc_send_rec_count = 0;

    iDriverResumeTime = GetTickCount();

    //clear lpc i2c buffer
    while (SOC_IO_Input(MCU_IIC_REQ_G, MCU_IIC_REQ_I, 0) == 0)
    {
        WHILE_ENTER;
        actualReadFromMCU(buff, iReadLen);
        iReadLen = 16;
    }
}




static int  lpc_probe(struct platform_device *pdev)
{

    DUMP_FUN;
	
    lidbg("lpc communication+\n");
    mcuFirstInit();
    LPCPowerOnOK();
    LPCNoReset();
    LPCBackLightOn();
    lidbg("lpc communication-\n");
    lpc_work_en = 1;

#ifdef CONFIG_HAS_EARLYSUSPEND
    early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    early_suspend.suspend = lpc_early_suspend;
    early_suspend.resume = lpc_late_resume;
    register_early_suspend(&early_suspend);
#endif

    return 0;
}



static int  lpc_remove(struct platform_device *pdev)
{


    return 0;
}


#ifdef CONFIG_HAS_EARLYSUSPEND
static void lpc_early_suspend(struct early_suspend *handler)
{
    DUMP_FUN;
    lpc_work_en = 0;
    LPCSuspend();


}
static void lpc_late_resume(struct early_suspend *handler)
{
    DUMP_FUN_ENTER;
    LPCResume();
    LPCPowerOnOK();
    LPCNoReset();
    LPCBackLightOn();
    lpc_work_en = 1;
    DUMP_FUN_LEAVE;

}

#endif




#ifdef CONFIG_PM
static int lpc_suspend(struct device *dev)
{
    DUMP_FUN;

    TELL_LPC_PWR_OFF;

    return 0;
}

static int lpc_resume(struct device *dev)
{
    DUMP_FUN;

    TELL_LPC_PWR_ON;
    //msleep(200);
	resume_count++;

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
    //lpc
    ((struct lidbg_dev *)plidbg_dev)->soc_func_tbl.pfnSOC_LPC_Send = LPCCombinDataStream;

}

static int __init lpc_init(void)
{
    DUMP_BUILD_TIME;

	
#ifndef FLY_DEBUG
	lidbg("lpc_init do nothing");
#else
#ifndef SOC_COMPILE
    LIDBG_GET;
    set_func_tbl();
#endif
    platform_device_register(&lidbg_lpc);
    platform_driver_register(&lpc_driver);
#endif

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


