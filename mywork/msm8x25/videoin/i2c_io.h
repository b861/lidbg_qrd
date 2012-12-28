#ifndef __i2c_io_H__
#define __i2c_io_H__

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

#include "lidbg_enter.h"
//#define GPIO_I2C_SCL   43 //tw9912 reset
//#define GPIO_I2C_SDA   15//tp0708
#define GPIO_I2C_SCL   32 
#define GPIO_I2C_SDA   107

typedef enum 
{
	NACK=0,
	ACK,
}i2c_ack;

typedef enum 
{
	hkj_WRITE=0,
	hkj_READ,
}i2c_WR_flag;
void i2c_io_config_init(void);
int i2c_io_config(unsigned int index, unsigned int direction,unsigned int pull,unsigned int drive_strength,unsigned int flag);
i2c_ack i2c_read_byte(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
i2c_ack i2c_write_byte(int bus_id, char chip_addr, char *buf, unsigned int size);
i2c_ack i2c_read_2byte(int bus_id, char chip_addr, unsigned int sub_addr, char *buf, unsigned int size);
 i2c_ack i2c_write_2byte(int bus_id, char chip_addr, char *buf, unsigned int size);

#endif  /* __mm_i2c_H__ */
