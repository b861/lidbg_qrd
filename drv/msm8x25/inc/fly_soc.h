
#ifndef _FLY_SOC__
#define _FLY_SOC__


#ifdef SOC_COMPILE
#include "lidbg.h"
#else
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/syscalls.h>
#include <asm/system.h>
#include <linux/time.h>
#include <linux/pwm.h>

#include <linux/stat.h>


#include <linux/i2c.h>
#include <linux/spi/spi.h>

//#include <linux/smp_lock.h>


#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>

#include <linux/types.h>

#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()¡¢kthread_run()
#include <linux/input.h>


#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>

#endif


//IO

/*
/ GPIO TLMM: Pullup/Pulldown /
enum {
	GPIO_CFG_NO_PULL,
	GPIO_CFG_PULL_DOWN,
	GPIO_CFG_KEEPER,
	GPIO_CFG_PULL_UP,
};

/GPIO TLMM: Drive Strength /
enum {
	GPIO_CFG_2MA,
	GPIO_CFG_4MA,
	GPIO_CFG_6MA,
	GPIO_CFG_8MA,
	GPIO_CFG_10MA,
	GPIO_CFG_12MA,
	GPIO_CFG_14MA,
	GPIO_CFG_16MA,
};

*/

void  SOC_IO_Output(u32 group, u32 index, bool status);
bool  SOC_IO_Input(u32 group, u32 index, u32 pull);
void SOC_IO_Output_Ext(u32 group, u32 index, bool status, u32 pull, u32 drive_strength);
bool SOC_IO_Config(u32 index, bool direction, u32 pull, u32 drive_strength);


//IO_IRQ

#ifndef SOC_COMPILE
typedef irqreturn_t (*pinterrupt_isr)(int irq, void *dev_id);
#endif

//interrupt_type

/*

#define IRQF_TRIGGER_RISING	0x00000001
#define IRQF_TRIGGER_FALLING	0x00000002
#define IRQF_TRIGGER_HIGH	0x00000004
#define IRQF_TRIGGER_LOW	0x00000008

*/

bool SOC_IO_ISR_Add(u32 irq, u32 interrupt_type, pinterrupt_isr func, void *dev);//set port as input first
bool SOC_IO_ISR_Enable(u32 irq);
bool SOC_IO_ISR_Disable(u32 irq);
bool SOC_IO_ISR_Del (u32 irq);


//AD
//return 0 when err
// 0-AIN2
// 1-AIN3
// 2-AIN4
// 3-REM1
// 4-REM2
//#define ADC_MAX_CH (8)
bool SOC_ADC_Get (u32 channel , u32 *value);


//KEY
#if 0 //linux/input.h
//key_value

KEY_MENU,   KEY_HOME,  KEY_BACK,
        KEY_DOWN,   KEY_UP,  KEY_RIGHT, KEY_LEFT,
        KEY_VOLUMEDOWN, KEY_VOLUMEUP, KEY_PAUSE, KEY_MUTE,
        KEY_POWER, KEY_SLEEP, KEY_WAKEUP

#endif
#ifndef SOC_COMPILE
        //type
#define KEY_RELEASED    (0)
#define KEY_PRESSED      (1)
#define KEY_PRESSED_RELEASED   ( 2)
#endif
        void SOC_Key_Report(u32 key_value, u32 type);



//IIC
//7bit i2c sub_addr
//bus_id : 0/1
//return how many bytes read/write
//when err , <0
int SOC_I2C_Send(int bus_id, char chip_addr, char *buf, unsigned int size);
int SOC_I2C_Rec(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
int SOC_I2C_Rec_Simple(int bus_id, char chip_addr, char *buf, unsigned int size);//without sub_addr
int SOC_I2C_Rec_2B_SubAddr(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
int SOC_I2C_Rec_3B_SubAddr(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);

int SOC_I2C_Rec_SAF7741(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
int SOC_I2C_Send_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
int SOC_I2C_Rec_TEF7000(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);


//BL
//level : 0~255   0-dim, 255-bright
int SOC_BL_Set( u32 level);


//PWR
void SOC_PWR_ShutDown(void);//power-down



#ifndef SOC_COMPILE

//LOG
#define SERVICER_DONOTHING  (0)
#define LOG_DMESG  (1)
#define LOG_LOGCAT (2)
#define LOG_ALL    (3)
#define LOG_CONT    (4)


#endif

void SOC_Log_Dump(int cmd );
void SOC_Capts_Insmod(int cmd);


#endif
