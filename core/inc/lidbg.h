#ifndef _LIGDBG_DEV__
#define _LIGDBG_DEV__

#include <linux/random.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/fb.h>
#include <linux/vmalloc.h>
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
#include <linux/kprobes.h>
#include <asm/traps.h>


#include <linux/i2c.h>
#include <linux/spi/spi.h>

#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/semaphore.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif


#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>

#include <linux/types.h>

#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()��kthread_run()
#include <linux/input.h>


#include <linux/proc_fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>

#include <linux/timer.h>
#include <linux/timex.h>

#include <linux/i2c-gpio.h>   //for i2c-gpio
#include <linux/i2c-algo-bit.h>	//add by huangzongqiang

////////////
//#define BUILD_FOR_RECOVERY

#include "lidbg_io.h"
#include "lidbg_key.h"
#include "lidbg_touch.h"
#include "lidbg_cmn.h"
#include "lidbg_fileserver.h"
#include "lidbg_ad.h"
#include "lidbg_def.h"
#include "lidbg_mem.h"
#include "lidbg_i2c.h"
#include "lidbg_uart.h"
#include "lidbg_spi.h"
#include "lidbg_display.h"
#include "lidbg_servicer.h"

////////////
#include "lidbg_soc.h"

#ifdef BUILD_DRIVERS
#include "lidbg_enter.h"
#endif

#endif

