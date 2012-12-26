
#ifndef _LIGDBG_ENTER__
#define _LIGDBG_ENTER__

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
#include <linux/hrtimer.h>

#include <linux/stat.h>


#include <linux/i2c.h>
#include <linux/spi/spi.h>


#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif


#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>

#include <linux/types.h>

#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()、kthread_run()
#include <linux/input.h>


#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/fb.h>


#include "mach/hardware.h"
#include "mach/irqs.h"



#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/i2c/pca953x.h>
#include <linux/slab.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>

//msm8x25
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio_event.h>
#include <linux/usb/android.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/i2c.h>
#include <linux/android_pmem.h>
#include <linux/bootmem.h>
#include <linux/mfd/marimba.h>
#include <linux/power_supply.h>
#include <linux/regulator/consumer.h>
#include <linux/memblock.h>
#include <asm/mach/mmc.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <mach/board.h>
#include <mach/msm_iomap.h>
#include <mach/msm_hsusb.h>
#include <mach/rpc_hsusb.h>
#include <mach/rpc_pmapp.h>
#include <mach/usbdiag.h>
#include <mach/msm_memtypes.h>
#include <mach/msm_serial_hs.h>
#include <mach/pmic.h>
#include <mach/socinfo.h>
#include <mach/vreg.h>
#include <mach/rpc_pmapp.h>
//#include <mach/msm_battery.h>
#include <mach/rpc_server_handset.h>
#include <mach/socinfo.h>
#include <mach/msm_smsm.h>

#include <mach/msm_rpcrouter.h>


#endif

#ifndef SOC_COMPILE

typedef irqreturn_t (*pinterrupt_isr)(int irq, void *dev_id);

#define lidbg(msg...)  do { printk( KERN_CRIT "lidbg: " msg); }while(0)

#define KEY_RELEASED    (0)
#define KEY_PRESSED      (1)
#define KEY_PRESSED_RELEASED   ( 2)



#define SERVICER_DONOTHING  (0)
#define LOG_DMESG  (1)
#define LOG_LOGCAT (2)
#define LOG_ALL (3)
#define LOG_CONT    (4)
#define WAKEUP_KERNEL (10)
#define SUSPEND_KERNEL (11)

#define USB_RST_ACK (88)

#define LOG_DVD_RESET (64)
#define LOG_CAP_TS_GT811 (65)
#define LOG_CAP_TS_FT5X06 (66)
#define LOG_CAP_TS_FT5X06_SKU7 (67)
#define LOG_CAP_TS_RMI (68)
#define LOG_CAP_TS_GT801 (69)
#define CMD_FAST_POWER_OFF (70)
#define UMOUNT_USB (80)



#endif


typedef enum
{
  PM_STATUS_EARLY_SUSPEND_PENDING,
  PM_STATUS_SUSPEND_PENDING,
  PM_STATUS_RESUME_OK,
  PM_STATUS_LATE_RESUME_OK,
}FAST_PWROFF_STATUS;


#define BEGIN_KMEM do{old_fs = get_fs();set_fs(get_ds());}while(0)
#define END_KMEM   do{set_fs(old_fs);}while(0)


struct lidbg_fn_t {
//io
	void (*pfnSOC_IO_Output) (u32 group, u32 index, bool status);
	bool (*pfnSOC_IO_Input) (u32 group, u32 index, u32 pull);
	void (*pfnSOC_IO_Output_Ext)(u32 group, u32 index, bool status, u32 pull, u32 drive_strength);
	bool (*pfnSOC_IO_Config)(u32 index, bool direction, u32 pull, u32 drive_strength);
	
//i2c
	int (*pfnSOC_I2C_Send) (int bus_id, char chip_addr, char *buf, unsigned int size);
	int (*pfnSOC_I2C_Rec)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
	int (*pfnSOC_I2C_Rec_Simple)(int bus_id, char chip_addr, char *buf, unsigned int size);
	
//io-irq
	bool (*pfnSOC_IO_ISR_Add)(u32 irq, u32 interrupt_type, pinterrupt_isr func, void *dev);
	bool (*pfnSOC_IO_ISR_Enable)(u32 irq);
	bool (*pfnSOC_IO_ISR_Disable)(u32 irq);
	bool (*pfnSOC_IO_ISR_Del )(u32 irq);

//ad
	bool (*pfnSOC_ADC_Get)(u32 channel , u32 *value);


//key
	 void (*pfnSOC_Key_Report)(u32 key_value, u32 type);
//bl
	int (*pfnSOC_BL_Set)( u32 level);

//pwr
	void (*pfnSOC_PWR_ShutDown)(void);
	int (*pfnSOC_PWR_GetStatus)(void);

//
	void (*pfnSOC_Write_Servicer)(int cmd );



};

#define LIDBG_SIZE	0x00001000 //MEM_SIZE_4_KB	/*全局内存最大1K字节*/

/*lidbg设备结构体*/
struct lidbg_dev
{
    struct cdev cdev; /*cdev结构体*/
    unsigned char mem[LIDBG_SIZE]; /*全局内存*/
	unsigned char lidbg_smem[LIDBG_SIZE/4]; // 1k
    struct lidbg_fn_t soc_func_tbl;
};

#if 1

#define LIDBG_DEFINE  struct lidbg_dev *plidbg_dev = NULL

#define LIDBG_GET  \
 	do{\
	 mm_segment_t old_fs;\
	 struct file *fd = NULL;\
	 while(1){\
	 	printk("lidbg:try open mlidbg0!\n");\
	 	fd = filp_open("/dev/mlidbg0", O_RDWR, 0);\
	    if((fd == 0xffffffff) || (fd == 0))msleep(50);\
	    else break;\
	 }\
	 BEGIN_KMEM;\
	 fd->f_op->write(fd, "c lidbg_get", sizeof("c lidbg_get"), &fd->f_pos);\
	 fd->f_op->read(fd, &plidbg_dev, 4 ,&fd->f_pos);\
	 END_KMEM;\
	filp_close(fd,0);\
	if(plidbg_dev == NULL)\
	{\
		printk("LIDBG_GET fail!\n");\
	}\
}while(0)


#endif

#ifndef SOC_COMPILE
#include "lidbg_func_def.h"
#endif

#endif

