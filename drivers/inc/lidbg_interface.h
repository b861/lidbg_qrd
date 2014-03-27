
#ifndef _FLY_INTERFACE__
#define _FLY_INTERFACE__

#ifdef BUILD_DRIVERS
#include <msm8226_devices.h>
#include <lidbg_bpmsg.h>
#include <lidbg_ts_probe.h>
#include <lidbg_monkey.h>
#include <lidbg_lpc.h>
#else
#include <linux/miscdevice.h>
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
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/syscalls.h>
#include <asm/system.h>
#include <linux/fb.h>
#include <linux/stat.h>
#include <linux/proc_fs.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/semaphore.h>
#include <linux/kfifo.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/input.h>
#include <linux/wakelock.h>
#include <linux/vmalloc.h>
#endif





#if (defined(BUILD_SOC) || defined(BUILD_CORE) || defined(BUILD_DRIVERS))
#define NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE (110)
#define NOTIFIER_MINOR_ACC_ON (0)
#define NOTIFIER_MINOR_ACC_OFF (1)

#else
#define NOTIFIER_VALUE(major,minor)  (((major)&0xffff)<<16 | ((minor)&0xffff))

#define BEGIN_KMEM do{old_fs = get_fs();set_fs(get_ds());}while(0)
#define END_KMEM   do{set_fs(old_fs);}while(0)

#define KEY_RELEASED    (0)
#define KEY_PRESSED      (1)
#define KEY_PRESSED_RELEASED   ( 2)

typedef irqreturn_t (*pinterrupt_isr)(int irq, void *dev_id);

#define ADC_MAX_CH (8)
struct fly_smem
{
    unsigned char reserved[4];
    unsigned int ch[ADC_MAX_CH];
    int reserved2;
    int bl_value;
};


#endif

typedef enum
{
    FLY_ACC_ON,
    FLY_ACC_OFF,
    FLY_READY_TO_SUSPEND,
    FLY_SUSPEND,
} FLY_SYSTEM_STATUS;


struct lidbg_fn_t
{
    //io
    /*
     GPIO TLMM: Pullup/Pulldown
    enum {
    	GPIO_CFG_NO_PULL,
    	GPIO_CFG_PULL_DOWN,
    	GPIO_CFG_KEEPER,
    	GPIO_CFG_PULL_UP,
    };

    GPIO TLMM: Drive Strength
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
    void (*pfnSOC_IO_Output) (unsigned int group, unsigned int index, bool status);
    bool (*pfnSOC_IO_Input) (unsigned int group, unsigned int index, unsigned int pull);
    void (*pfnSOC_IO_Output_Ext)(unsigned int group, unsigned int index, bool status, unsigned int pull, unsigned int drive_strength);
    bool (*pfnSOC_IO_Config)(unsigned int index, bool direction, unsigned int pull, unsigned int drive_strength);

    //i2c
    /*
    7bit i2c sub_addr
    bus_id : 0/1
    return how many bytes read/write
    when err , <0
    */
    int (*pfnSOC_I2C_Send) (int bus_id, char chip_addr, char *buf, unsigned int size);
    int (*pfnSOC_I2C_Rec)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfnSOC_I2C_Rec_Simple)(int bus_id, char chip_addr, char *buf, unsigned int size);

    int (*pfnSOC_I2C_Rec_SAF7741)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfnSOC_I2C_Send_TEF7000)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfnSOC_I2C_Rec_TEF7000)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
    int (*pfnSOC_I2C_Rec_2B_SubAddr)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);


    //io-irq
    //interrupt_type

    /*

    #define IRQF_TRIGGER_RISING	0x00000001
    #define IRQF_TRIGGER_FALLING	0x00000002
    #define IRQF_TRIGGER_HIGH	0x00000004
    #define IRQF_TRIGGER_LOW	0x00000008

    */
    bool (*pfnSOC_IO_ISR_Add)(unsigned int irq, unsigned int interrupt_type, pinterrupt_isr func, void *dev);
    bool (*pfnSOC_IO_ISR_Enable)(unsigned int irq);
    bool (*pfnSOC_IO_ISR_Disable)(unsigned int irq);
    bool (*pfnSOC_IO_ISR_Del )(unsigned int irq);

    //ad
    /*
     return 0 when err
    // 0-AIN2
    // 1-AIN3
    // 2-AIN4
    // 3-REM1
    // 4-REM2
    */
    bool (*pfnSOC_ADC_Get)(unsigned int channel , unsigned int *value);

    //key
    void (*pfnSOC_Key_Report)(unsigned int key_value, unsigned int type);
    //bl
    /*level : 0~255   0-dim, 255-bright*/
    int (*pfnSOC_BL_Set)( unsigned int level);

    //display/touch
    int (*pfnSOC_Display_Get_Res)(unsigned int *screen_x, unsigned int *screen_y);

    //lpc
    void (*pfnSOC_LPC_Send)(unsigned char *p, unsigned int len);

	
    void (*pfnSOC_System_Status)(FLY_SYSTEM_STATUS status);
    struct fly_smem *(*pfnSOC_Get_Share_Mem)(void);
};

struct lidbg_pvar_t
{
    //all pointer
    int temp;
    FLY_SYSTEM_STATUS system_status;
    int machine_id;
    int cpu_freq;
    bool is_fly;
    unsigned int flag_for_15s_off;
    bool is_usb11;
    bool fake_suspend;
    bool acc_flag;

};

struct lidbg_interface
{
    union
    {
        struct lidbg_fn_t soc_func_tbl;
        unsigned char reserve[256];
    };
    union
    {
        struct lidbg_pvar_t soc_pvar_tbl;
        unsigned char reserve1[128];
    };
};



#define LIDBG_DEV_CHECK_READY  (plidbg_dev != NULL)

#define LIDBG_DEFINE  struct lidbg_interface *plidbg_dev = NULL

#define g_var  plidbg_dev->soc_pvar_tbl

#define LIDBG_GET  \
 	do{\
	 mm_segment_t old_fs;\
	 struct file *fd = NULL;\
	 printk("lidbg:call LIDBG_GET by %s\n",__FUNCTION__);\
	 while(1){\
	 	printk("lidbg: %s:%s try open lidbg_interface!\n",__FILE__,__FUNCTION__);\
	 	fd = filp_open("/dev/lidbg_interface", O_RDWR, 0);\
	 	printk("lidbg:get fd=%x\n",(int)fd);\
	    if((fd == NULL)||((int)fd == 0xfffffffe)){printk("lidbg:get fd fail!\n");msleep(500);}\
	    else break;\
	 }\
	 BEGIN_KMEM;\
	 fd->f_op->read(fd, (void*)&plidbg_dev, 4 ,&fd->f_pos);\
	 END_KMEM;\
	filp_close(fd,0);\
	if(plidbg_dev == NULL)\
	{\
		printk("LIDBG_GET fail!\n");\
	}\
}while(0)


#define LIDBG_THREAD_DEFINE   \
    struct lidbg_interface *plidbg_dev = NULL;\
	static struct task_struct *getlidbg_task;\
	static int thread_getlidbg(void *data);\
	int thread_getlidbg(void *data)\
	{\
		LIDBG_GET;\
		return 0;\
	}

#define LIDBG_GET_THREAD  do{\
	getlidbg_task = kthread_create(thread_getlidbg, NULL, "getlidbg_task");\
	if(IS_ERR(getlidbg_task))\
	{\
		printk("Unable to start kernel thread.\n");\
	}else wake_up_process(getlidbg_task);\
}while(0)


extern struct lidbg_interface *plidbg_dev;
	
static inline int check_pt(void)
{
	while (plidbg_dev == NULL)
	{
		printk("lidbg:check if plidbg_dev==NULL\n");
		printk("%s,line %d\n", __FILE__, __LINE__);
		dump_stack();//provide some information
		msleep(200);
	}
	return 0;
}
	
//io
#define SOC_IO_Output (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Output))
#define SOC_IO_Input  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Input))
#define SOC_IO_Output_Ext (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Output_Ext))
#define SOC_IO_Config  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Config))
//i2c
#define SOC_I2C_Send  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send))
#define SOC_I2C_Rec   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec))
#define SOC_I2C_Rec_Simple   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_Simple))
	
#define SOC_I2C_Rec_SAF7741  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_SAF7741))
#define SOC_I2C_Send_TEF7000   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send_TEF7000))
#define SOC_I2C_Rec_TEF7000   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_TEF7000))
#define SOC_I2C_Rec_2B_SubAddr   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_2B_SubAddr))
//io-irq
#define SOC_IO_ISR_Add  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Add))
#define SOC_IO_ISR_Enable   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Enable))
#define SOC_IO_ISR_Disable  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Disable))
#define SOC_IO_ISR_Del  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Del))
//ad
#define SOC_ADC_Get  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_ADC_Get))
//key
#define SOC_Key_Report  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Key_Report))
//bl
#define SOC_BL_Set  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_BL_Set))
	
//display/touch
#define SOC_Display_Get_Res  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Display_Get_Res))
	
//lpc
#define  SOC_LPC_Send  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_LPC_Send))

#define FLAG_FOR_15S_OFF   (plidbg_dev->soc_pvar_tbl.flag_for_15s_off)	
	
#define SOC_Get_Share_Mem (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Get_Share_Mem))
#define SOC_System_Status (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_System_Status))

#endif
