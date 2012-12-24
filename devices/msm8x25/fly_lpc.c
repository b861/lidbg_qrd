#include "lidbg.h"
#include "fly_lpc.h"

//#define LPC_DEBUG_LOG


#ifndef USU_EXTERNEL_SUSPEND_PENDING
static int suspend_pending = 0;
#endif


struct fly_hardware_info GlobalHardwareInfo;
struct fly_hardware_info *pGlobalHardwareInfo;

UINT32 iDriverResumeTime = 0;
BOOL bResumeError = FALSE;
u32 lpc_send_rec_count = 0;

static struct task_struct *lpc_task;
int thread_lpc(void *data);

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
    u32 i;
	static u32 acc_off_count = 0;
	if (iDriverResumeTime)
	{
		if (GetTickCount() - iDriverResumeTime >= 6*1000)
		{
			//bResumeError = TRUE;
			printk("JQilin Resume Error...\n");
		}
	}
	iDriverResumeTime = 0;
#ifdef LPC_DEBUG_LOG
    lidbg("From LPC:");//mode ,command,para
    for(i = 0; i < length; i++)
    {
        printk("%x ", p[i]);

    }
    printk("\n");
#endif

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
					lidbg("AccOff Count:%d\n",acc_off_count);

				}
				
	            if((suspend_pending == PM_STATUS_ON)&&(late_resume_ok == 1))
	            {
					msleep(100);
					
	                lidbg("Ready ACC OFF!\n");
	                msleep(100);
	                LPCControlToSleep();
	                SOC_PWR_ShutDown();
	            }
				else
				{
					lidbg("suspend_pending...\n");
					if(late_resume_ok == 0)
						lidbg("late_resume_ok == 0...\n");

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

    while (!SOC_IO_Input(MCU_IIC_REQ_G, MCU_IIC_REQ_I, 0))
    {
        actualReadFromMCU(buff, iReadLen);
        iReadLen = 16;
    }
	//SOC_IO_ISR_Enable(MCU_IIC_REQ_ISR);
}

int thread_lpc(void *data)
{


	BYTE buff[] = {0x00,0x96,0x00,0x00,0x00,0x00};
	BYTE iRandom = 0;
	buff[5] = iRandom;
	buff[4] = iRandom;
	buff[3] = iRandom;
	buff[2] = iRandom;
	static u32 re_sleep_count = 0;
	msleep(10*1000);
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
  			if((1 == late_resume_ok)&&(suspend_pending == PM_STATUS_ON))
			{
#ifdef LPC_DEBUG_LOG
				lidbg("lpc_send_rec_count=%d\n",re_sleep_count);
#endif	
				if(lpc_send_rec_count >= 3)
				{
					re_sleep_count++;
					lidbg("\n\n\nerrlsw:lpc_send_rec_count > 3 ,do SOC_PWR_ShutDown again! %d\n\n\n",re_sleep_count);
					SOC_PWR_ShutDown();
					lpc_send_rec_count=0;

				}

				LPCCombinDataStream(buff, SIZE_OF_ARRAY(buff));
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
	int err;
    pGlobalHardwareInfo = &GlobalHardwareInfo;
    INIT_WORK(&pGlobalHardwareInfo->FlyIICInfo.iic_work, workFlyMCUIIC);

	//let i2c_c high
    while (SOC_IO_Input(0, MCU_IIC_REQ_I, GPIO_CFG_PULL_UP)==0)
    {
    	u8 buff[32];
        actualReadFromMCU(buff, 32);
       
    }

    //SOC_IO_Input(0, MCU_IIC_REQ_I, GPIO_CFG_PULL_UP);
    SOC_IO_ISR_Add(MCU_IIC_REQ_ISR, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, MCUIIC_isr, pGlobalHardwareInfo);

#ifdef FLY_DEBUG
	lpc_task = kthread_create(thread_lpc, NULL, "lpc_task");
	if(IS_ERR(lpc_task))
	{
		lidbg("Unable to start kernel thread.\n");
		err = PTR_ERR(lpc_task);
		lpc_task = NULL;

	}
	wake_up_process(lpc_task);
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

	lpc_send_rec_count=0;

	iDriverResumeTime = GetTickCount();

		//clear lpc i2c buffer
    while (!SOC_IO_Input(MCU_IIC_REQ_G, MCU_IIC_REQ_I, 0))
    {
        actualReadFromMCU(buff, iReadLen);
        iReadLen = 16;
    }
}


