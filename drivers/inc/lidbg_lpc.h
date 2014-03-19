#ifndef __FLY_LPC_
#define __FLY_LPC_

#define DATA_BUFF_LENGTH_FROM_MCU   (128)

#define BYTE u8
#define UINT u32
#define UINT32 u32
#define BOOL bool
#define ULONG u32

#define FALSE 0
#define TRUE 1


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

#define LPC_SYSTEM_TYPE 0x00
#define LPC_SOUND_TYPE  0X01
#define LPC_KEY_TYPE	0X02
#define LPC_CAN_TYPE	0X03


#define  I2_0_ID  0
#define  I2_1_ID  1

#define  MCU_IIC_REQ_G (30)
#define  MCU_IIC_REQ_I (30)
#define  MCU_IIC_REQ_ISR  (30)

#define  MCU_ADDR_W  0xA0
#define  MCU_ADDR_R  0xA1


void mcuFirstInit(void);
void LPCBackLightOn(void);
void LPCBackLightOff(void);
void LPCPowerOnOK(void);
void LPCControlToSleep(void);
void LPCNoReset(void);
void LPCSuspend(void);
void LPCResume(void);
void LPCControlSupendTestStart(void);
void LPCControlSupendTestStop(void);

void LPCControlPWREnable(void);
void LPCControlPWRDisenable(void);

#define AUTO_SLEEP_WHEN_WAKEUP_NOT_BY_LPC


#endif
