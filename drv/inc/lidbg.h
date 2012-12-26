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
#include <linux/kthread.h> //kthread_create()¡¢kthread_run()
#include <linux/input.h>


#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>


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
//#include "fly_soc.h"
#include "lidbg_enter.h"

//#include "devices.h"



#endif

