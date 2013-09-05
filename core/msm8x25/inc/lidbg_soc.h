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
#include <linux/regulator/consumer.h>
#include <linux/memblock.h>
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

#define SOC_KO  "lidbg_soc_msm8x25.ko"

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

int soc_io_output(u32 group,u32 index, bool status);
bool soc_io_input(u32 index);
int soc_io_config(u32 index, bool direction, u32 pull, u32 drive_strength, bool force_reconfig);

int soc_temp_get(void);
void lidbg_soc_main(int argc, char **argv);

///////////////////////////////////////
#define ADC_MAX_CH (8)

#if 1
struct fly_smem
{
    int reserved1;
    u32 ch[ADC_MAX_CH];
    int reserved2;
    int bl_value;
};
#define SMEM_AD  p_fly_smem->ch
#define SMEM_BL  p_fly_smem->bl_value
#define SMEM_TEMP  p_fly_smem->ch[6]

#else
struct fly_smem
{
    u32 bp2ap[16];
    u32 ap2bp[8];
};
#define SMEM_AD  p_fly_smem->bp2ap
#define SMEM_BL  p_fly_smem->ap2bp[0]
#define SMEM_TEMP  p_fly_smem->bp2ap[6]

#endif


extern struct fly_smem *p_fly_smem ;

#define IO_CONFIG_OUTPUT(group,index) do{  soc_io_config( index,  GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_16MA, 1);}while(0)
#define IO_CONFIG_INPUT(group,index) do{  soc_io_config( index,  GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA, 1);}while(0)

#define GPIO_TO_INT MSM_GPIO_TO_INT


 
//i2c-gpio
#define LIDBG_I2C_GPIO
	
#if	0
#define LIDBG_I2C_GPIO_SDA (107)
#define LIDBG_I2C_GPIO_SCL (32)
#else
#define LIDBG_I2C_GPIO_SDA (109)
#define LIDBG_I2C_GPIO_SCL (35)
#endif

#define LIDBG_I2C_BUS_ID (3)
#define LIDBG_I2C_DEFAULT_DELAY (1)




#define I2C_GPIO_CONFIG do{	 \
	 gpio_tlmm_config(GPIO_CFG(LIDBG_I2C_GPIO_SDA, 0, (GPIO_CFG_OUTPUT | GPIO_CFG_INPUT), GPIO_CFG_PULL_UP, GPIO_CFG_16MA), GPIO_CFG_ENABLE);\
	 gpio_tlmm_config(GPIO_CFG(LIDBG_I2C_GPIO_SCL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA), GPIO_CFG_ENABLE);\
}while(0)

//io_uart
#define TX_GPIO (123)  //27
#define TX_H  do{soc_io_output(0,TX_GPIO, 1);}while(0)
#define TX_L  do{soc_io_output(0,TX_GPIO, 0);}while(0)
#define TX_CFG  do{soc_io_config(TX_GPIO, GPIO_CFG_OUTPUT,GPIO_CFG_NO_PULL,GPIO_CFG_16MA,1);}while(0)

// 1.2Gh
#define IO_UART_DELAY_1200_115200 (14)
#define IO_UART_DELAY_1200_4800 (418)

//245M
#define IO_UART_DELAY_245_115200 (4)

// pm2.c low freq
#define IO_UART_DELAY_PM2_4800 (165)

#endif

