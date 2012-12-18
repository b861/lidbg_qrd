#ifndef _LIGDBG_DEV__
#define _LIGDBG_DEV__

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

///////////
#define BEGIN_KMEM do{old_fs = get_fs();set_fs(get_ds());}while(0)
#define END_KMEM   do{set_fs(old_fs);}while(0)

//#define KERN_CRIT       "<2>"    /* critical conditions */
#define lidbg(msg...)  do { printk( KERN_CRIT "lidbg: " msg); }while(0)


////////////
#include "lidbg_io.h"
#include "lidbg_key.h"
#include "lidbg_touch.h"
#include "lidbg_cmn.h"
#include "lidbg_ad.h"
#include "lidbg_def.h"
#include "lidbg_mem.h"
#include "lidbg_i2c.h"
#include "lidbg_serial.h"
#include "lidbg_spi.h"
#include "lidbg_init.h"
#include "lidbg_display.h"
#include "lidbg_servicer.h"

//#include "mlidbg_legacy_i2c.h"

////////////
#include "lidbg_soc.h"
#include "fly_soc.h"

#include "devices.h"


struct lidbg_fn_t {
//io
	void (*SOC_IO_Output) (u32 group, u32 index, bool status);
	bool (*SOC_IO_Input) (u32 group, u32 index, u32 pull);
	
//i2c
	int (*SOC_I2C_Send) (int bus_id, char chip_addr, char *buf, unsigned int size);
	int (*SOC_I2C_Rec)(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);

};
#define LIDBG_SIZE	MEM_SIZE_4_KB	/*全局内存最大1K字节*/

/*lidbg设备结构体*/
struct lidbg_dev
{
    struct cdev cdev; /*cdev结构体*/
    unsigned char mem[LIDBG_SIZE]; /*全局内存*/
    struct lidbg_fn_t soc_func_tbl;
};

#endif

