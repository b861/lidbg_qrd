#ifndef _LIGDBG_MSM8x25__
#define _LIGDBG_MSM8x25__


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
//#include <mach/pmic.h>
#include <mach/socinfo.h>
#include <mach/vreg.h>
#include <mach/rpc_pmapp.h>
//#include <mach/msm_battery.h>
#include <mach/rpc_server_handset.h>
#include <mach/socinfo.h>
#include <mach/msm_smsm.h>

#include <mach/msm_rpcrouter.h>

//i2c-gpio
#define MSM_I2C_GPIO_SDA2 107
#define MSM_I2C_GPIO_SCL2 32

#if 0
enum
{
    GPIO_CFG_INPUT,
    GPIO_CFG_OUTPUT,
};

/* GPIO TLMM: Pullup/Pulldown */
enum
{
    GPIO_CFG_NO_PULL,
    GPIO_CFG_PULL_DOWN,
    GPIO_CFG_KEEPER,
    GPIO_CFG_PULL_UP,
};

/* GPIO TLMM: Drive Strength */
enum
{
    GPIO_CFG_2MA,
    GPIO_CFG_4MA,
    GPIO_CFG_6MA,
    GPIO_CFG_8MA,
    GPIO_CFG_10MA,
    GPIO_CFG_12MA,
    GPIO_CFG_14MA,
    GPIO_CFG_16MA,
};

enum
{
    GPIO_CFG_ENABLE,
    GPIO_CFG_DISABLE,
};

#define GPIO_CFG(gpio, func, dir, pull, drvstr) \
	((((gpio) & 0x3FF) << 4)        |	\
	((func) & 0xf)                  |	\
	(((dir) & 0x1) << 14)           |	\
	(((pull) & 0x3) << 15)          |	\
	(((drvstr) & 0xF) << 17))


#endif



struct io_config
{
    __u32 index;
    __u32 status;
    __u32 pull;
    bool direction;
    __u32 drive_strength;
    bool disable;

} ;


//typedef irqreturn_t (*pinterrupt_isr)(int irq, void *dev_id);

struct io_int_config
{
    __u32 ext_int_num;
    unsigned long irqflags;
    pinterrupt_isr pisr;
    void *dev;
} ;

//#define MAKE_GPIO_LOG(group,index)   ((group<<5)|(index))
#define IO_LOG_NUM  (132)
#define AD_LOG_NUM  (16)
#define TTY_DEV "msm-uart"


#define LIDBG_GPIO_PULLUP  GPIO_CFG_PULL_UP



//#define LIDBG_GPIO_PULLDOWN  GPIO_PULLDOWN

///////////////////////////////////////

void  soc_io_init(void);

void  soc_ad_init(void);
u32  soc_ad_read(u32 ch);
u32  soc_bl_set(u32 bl_level);
u32  soc_pwm_set(int pwm_id, int duty_ns, int period_ns);
void soc_bl_init(void);

int soc_io_irq(struct io_int_config *pio_int_config);
void soc_irq_disable(unsigned int irq);
void soc_irq_enable(unsigned int irq);

int soc_io_output(u32 index, bool status);
bool soc_io_input(u32 index);
int soc_io_config(u32 index, bool direction, u32 pull, u32 drive_strength, bool force_reconfig);
int soc_serial_set_tty(int port, int baud, char parity, int bits);
void soc_i2c_gpio_config(struct platform_device *pdev);



void lidbg_soc_main(int argc, char **argv);

///////////////////////////////////////
#define ADC_MAX_CH (8)


struct fly_smem
{
    int reserved1;
    u32 ch[ADC_MAX_CH];
    int reserved2;
    int bl_value;
};

extern struct fly_smem *p_fly_smem ;



#endif

