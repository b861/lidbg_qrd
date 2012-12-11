#include "lidbg.h"
#include "fly_lpc.h"

struct fly_hardware_info GlobalHardwareInfo;
struct fly_hardware_info *pGlobalHardwareInfo;


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

    lidbg("ToMCU:%x %x %x\n", p[0], p[1], p[2]);

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

    DBG0("\nControl To MCU Power On######################");
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


static void LPCdealReadFromMCUAll(BYTE *p, UINT length)
{
#if 1
    u32 i;

    lidbg("From LPC:");//mode ,command,para
    for(i = 0; i < length; i++)
    {
        printk("%x ", p[i]);

    }
    printk("\n");


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
	            if(suspend_pending == PM_STATUS_ON)
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

				}
	                break;
	        }
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
        DBG0("More ");
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

void mcuFirstInit(void)
{
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

}



void LPCSuspend(void)
{

    SOC_IO_ISR_Disable(MCU_IIC_REQ_ISR);
}

void LPCResume(void)
{

    SOC_IO_ISR_Enable(MCU_IIC_REQ_ISR);
}


