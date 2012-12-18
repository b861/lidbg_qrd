//obj-m       += test.o

#if 0
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


#define lidbg(msg...)  do { printk( "lidbg: " msg); }while(0)

#else

#include "lidbg.h"


#endif
struct lidbg_dev *lidbg_devp=NULL; 



#if 1
struct msm_camera_i2c_reg_conf
{
    uint16_t reg_addr;
    u8 reg_data;

};

static struct msm_camera_i2c_reg_conf tmp0[] =
{
    {0x4202, 0xf},
    {0x4800, 0x25},
    {0x0103, 0x01},
};


static struct msm_camera_i2c_reg_conf ov5647_recommend_settings[] =
{
    {0x3035, 0x11},
    {0x303c, 0x11},
    {0x370c, 0x03},
    {0x5000, 0x06},
    {0x5003, 0x08},
    {0x5a00, 0x08},
    {0x3000, 0xff},
    {0x3001, 0xff},
    {0x3002, 0xff},
    {0x301d, 0xf0},
    {0x3a18, 0x00},
    {0x3a19, 0xf8},
    {0x3c01, 0x80},
    {0x3b07, 0x0c},
    {0x3708, 0x64},
    {0x3630, 0x2e},
    {0x3632, 0xe2},
    {0x3633, 0x23},
    {0x3634, 0x44},
    {0x3620, 0x64},
    {0x3621, 0xe0},
    {0x3600, 0x37},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x3731, 0x02},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3f05, 0x02},
    {0x3f06, 0x10},
    {0x3f01, 0x0a},
    {0x3a08, 0x01},
    {0x3a0f, 0x58},
    {0x3a10, 0x50},
    {0x3a1b, 0x58},
    {0x3a1e, 0x50},
    {0x3a11, 0x60},
    {0x3a1f, 0x28},
    {0x4001, 0x02},
    {0x4000, 0x09},
    {0x3000, 0x00},
    {0x3001, 0x00},
    {0x3002, 0x00},
    {0x3017, 0xe0},
    {0x301c, 0xfc},
    {0x3636, 0x06},
    {0x3016, 0x08},
    {0x3827, 0xec},
    {0x3018, 0x44},
    {0x3035, 0x21},
    {0x3106, 0xf5},
    {0x3034, 0x18},
    {0x301c, 0xf8},
    /*lens setting*/
    {0x5000, 0x86},
    {0x5800, 0x11},
    {0x5801, 0x0c},
    {0x5802, 0x0a},
    {0x5803, 0x0b},
    {0x5804, 0x0d},
    {0x5805, 0x13},
    {0x5806, 0x09},
    {0x5807, 0x05},
    {0x5808, 0x03},
    {0x5809, 0x03},
    {0x580a, 0x06},
    {0x580b, 0x08},
    {0x580c, 0x05},
    {0x580d, 0x01},
    {0x580e, 0x00},
    {0x580f, 0x00},
    {0x5810, 0x02},
    {0x5811, 0x06},
    {0x5812, 0x05},
    {0x5813, 0x01},
    {0x5814, 0x00},
    {0x5815, 0x00},
    {0x5816, 0x02},
    {0x5817, 0x06},
    {0x5818, 0x09},
    {0x5819, 0x05},
    {0x581a, 0x04},
    {0x581b, 0x04},
    {0x581c, 0x06},
    {0x581d, 0x09},
    {0x581e, 0x11},
    {0x581f, 0x0c},
    {0x5820, 0x0b},
    {0x5821, 0x0b},
    {0x5822, 0x0d},
    {0x5823, 0x13},
    {0x5824, 0x22},
    {0x5825, 0x26},
    {0x5826, 0x26},
    {0x5827, 0x24},
    {0x5828, 0x24},
    {0x5829, 0x24},
    {0x582a, 0x22},
    {0x582b, 0x20},
    {0x582c, 0x22},
    {0x582d, 0x26},
    {0x582e, 0x22},
    {0x582f, 0x22},
    {0x5830, 0x42},
    {0x5831, 0x22},
    {0x5832, 0x02},
    {0x5833, 0x24},
    {0x5834, 0x22},
    {0x5835, 0x22},
    {0x5836, 0x22},
    {0x5837, 0x26},
    {0x5838, 0x42},
    {0x5839, 0x26},
    {0x583a, 0x06},
    {0x583b, 0x26},
    {0x583c, 0x24},
    {0x583d, 0xce},
    /* manual AWB,manual AE,close Lenc,open WBC*/
    {0x3503, 0x03}, /*manual AE*/
    {0x3501, 0x10},
    {0x3502, 0x80},
    {0x350a, 0x00},
    {0x350b, 0x7f},
    {0x5001, 0x01}, /*manual AWB*/
    {0x5180, 0x08},
    {0x5186, 0x04},
    {0x5187, 0x00},
    {0x5188, 0x04},
    {0x5189, 0x00},
    {0x518a, 0x04},
    {0x518b, 0x00},
    {0x5000, 0x06}, /*No lenc,WBC on*/
    {0x4005, 0x18},
    {0x4051, 0x8f},
};

static struct msm_camera_i2c_reg_conf tmp1[] =
{
    {0x4202, 0xf},
    {0x4800, 0x25},
};


static struct msm_camera_i2c_reg_conf ov5647_prev_settings[] =
{
    /*1280*960 Reference Setting 24M MCLK 2lane 280Mbps/lane 30fps
    for back to preview*/
    {0x3035, 0x21},
    {0x3036, 0x37},
    {0x3821, 0x07},
    {0x3820, 0x41},
    {0x3612, 0x09},
    {0x3618, 0x00},
    {0x380c, 0x07},
    {0x380d, 0x68},
    {0x380e, 0x03},
    {0x380f, 0xd8},
    {0x3814, 0x31},
    {0x3815, 0x31},
    {0x3709, 0x52},
    {0x3808, 0x05},
    {0x3809, 0x00},
    {0x380a, 0x03},
    {0x380b, 0xc0},
    {0x3800, 0x00},
    {0x3801, 0x18},
    {0x3802, 0x00},
    {0x3803, 0x0e},
    {0x3804, 0x0a},
    {0x3805, 0x27},
    {0x3806, 0x07},
    {0x3807, 0x95},
    {0x4004, 0x02},
};

static struct msm_camera_i2c_reg_conf tmp2[] =
{
    {0x0100, 0x1},
    {0x4800, 0x4},
    {0x4202, 0x0},
    {0x3208, 0x0},
    {0x380e, 0x3},
    {0x380f, 0xd8},
    {0x3500, 0x0},
    {0x3501, 0x3d},
    {0x3502, 0x40},
    {0x350a, 0x0},
    {0x350b, 0x70},
    {0x3208, 0x10},
    {0x3208, 0x0a},
    {0x3208, 0x00},
    {0x380e, 0x0e},
    {0x380f, 0xc8},
    {0x3500, 0x0},
    {0x3501, 0xec},
    {0x3502, 0x40},
    {0x350a, 0x00},
    {0x350b, 0x80},
    {0x3208, 0x10},
    {0x3208, 0xa0},



};


int mipi_init(void)
{
#if 1
    int i = 0;
    u8 data = 0;
    u8 buf[3];



    lidbg("Mipi init2+\n");

#if 0
    SOC_IO_Output(0, 93, 1);
    SOC_IO_Output(0, 30, 1);

    msleep(100);

    SOC_IO_Output(0, 23, 0);
    msleep(100);
    SOC_IO_Output(0, 23, 1);
    msleep(200);
#endif

    i = 0;
    while(i < SIZE_OF_ARRAY(tmp0))
    {

        buf[0] = ( tmp0[i].reg_addr) >> 8;
        buf[1] = ( tmp0[i].reg_addr);
        buf[2] = ( tmp0[i].reg_data);
        SOC_I2C_Send(0, 0x36, buf , 3);
        SOC_I2C_Rec_2B_SubAddr(0, 0x36, tmp0[i].reg_addr, &data , 1);
        lidbg("addr %x, %x\n", tmp0[i].reg_addr, data);
        i++;
    }

    i = 0;
    while(i < SIZE_OF_ARRAY(ov5647_recommend_settings))
    {

        buf[0] = ( ov5647_recommend_settings[i].reg_addr) >> 8;
        buf[1] = ( ov5647_recommend_settings[i].reg_addr);
        buf[2] = ( ov5647_recommend_settings[i].reg_data);
        SOC_I2C_Send(0, 0x36, buf , 3);
        SOC_I2C_Rec_2B_SubAddr(0, 0x36, ov5647_recommend_settings[i].reg_addr, &data , 1);
        lidbg("addr %x, %x\n", ov5647_recommend_settings[i].reg_addr, data);
        i++;
    }


    i = 0;
    while(i < SIZE_OF_ARRAY(tmp1))
    {

        buf[0] = ( tmp1[i].reg_addr) >> 8;
        buf[1] = ( tmp1[i].reg_addr);
        buf[2] = ( tmp1[i].reg_data);
        SOC_I2C_Send(0, 0x36, buf , 3);
        SOC_I2C_Rec_2B_SubAddr(0, 0x36, tmp1[i].reg_addr, &data , 1);
        lidbg("addr %x, %x\n", tmp1[i].reg_addr, data);
        i++;
    }

    i = 0;
    while(i < SIZE_OF_ARRAY(ov5647_prev_settings))
    {

        buf[0] = ( ov5647_prev_settings[i].reg_addr) >> 8;
        buf[1] = ( ov5647_prev_settings[i].reg_addr);
        buf[2] = ( ov5647_prev_settings[i].reg_data);
        SOC_I2C_Send(0, 0x36, buf , 3);
        SOC_I2C_Rec_2B_SubAddr(0, 0x36, ov5647_prev_settings[i].reg_addr, &data , 1);
        lidbg("addr %x, %x\n", ov5647_prev_settings[i].reg_addr, data);
        i++;
    }

    i = 0;
    while(i < SIZE_OF_ARRAY(tmp2))
    {

        buf[0] = ( tmp2[i].reg_addr) >> 8;
        buf[1] = ( tmp2[i].reg_addr);
        buf[2] = ( tmp2[i].reg_data);
        SOC_I2C_Send(0, 0x36, buf , 3);
        SOC_I2C_Rec_2B_SubAddr(0, 0x36, tmp2[i].reg_addr, &data , 1);
        lidbg("addr %x, %x\n", tmp2[i].reg_addr, data);
        i++;
    }



    lidbg("mipi init2-\n");

    return 0;
#endif
}
#endif






























#if 0
irqreturn_t io_test_irq(int irq, void *dev_id)
{

    lidbg("io_test_irq: %d \n", irq);
    return 0;

}

int lidbg_test_init(void)
{
    int error;
    error = SOC_IO_Input(4, 24, GPIO_PULL_DISABLE);
    SOC_IO_Output(4, 24, 1);
    SOC_IO_Output(4, 24, 0);
    {
        struct io_int_config   *pio_int_config;
        int i;
        pio_int_config = kmalloc(sizeof(struct io_int_config), GFP_KERNEL);

        pio_int_config->ext_int_sel = 42;
        pio_int_config->interrupt_type = IRQ_TYPE_EDGE_RISING;
        pio_int_config->pisr = io_test_irq;
        pio_int_config->dev = (void *)pio_int_config;

        for(i = 0; i < 8; i++)
        {
            lidbg("i: %d \n", i);

            pio_int_config->ext_int_num = i;
            soc_io_irq(pio_int_config);
        }

    }

    return 0;

}
#endif


static struct task_struct *key_task;



void key_scan(void)
{

    // int val,i=500;
#if 1

    //SOC_ADC_Get(8, &val);
    //SOC_ADC_Get(9, &val);
    //SOC_ADC_Get(14, &val);
    //lidbg(".\n");
    while(1);
    udelay(5);

#endif

}


int thread_key_xxx(void *data)
{

	
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            //key_scan();
			
again:	LIDBG_GET(lidbg_devp);
		if(lidbg_devp == NULL )
			{
			printk("[futengfei]   lidbg_devp == NULL\n");
			goto again;
		}
			
            while(1)
            {
			msleep(3000);
			lidbg_devp->soc_func_tbl.SOC_IO_Output(0, 33, 1);
			msleep(3000);
			lidbg_devp->soc_func_tbl.SOC_IO_Output(0, 33, 0);

            }
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


irqreturn_t io_test_irq(int irq, void *dev_id)
{

    lidbg("io_test_irq: %d \n", irq);
    return IRQ_HANDLED;

}


int lidbg_test_init(void)
{
    int err, times = 0;
    //int i;
    //u8 data;
    //mipi_init();

    //SOC_IO_Input(76, 76, 3);
    //SOC_IO_ISR_Add(76, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, io_test_irq, NULL);
    //return 0;
    //msleep(5000);

    lidbg("lidbg_test_init.\n");
    //SOC_Log_Dump(LOG_DVD_RESET);
    // while(1)
    //	udelay(5);

    key_task = kthread_create(thread_key_xxx, NULL, "key_task");
    if(IS_ERR(key_task))
    {
        lidbg("Unable to start kernel thread.\n");
        err = PTR_ERR(key_task);
        key_task = NULL;

    }
    wake_up_process(key_task);
    return 0;

}
void lidbg_test_deinit(void)
{


}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_test_init);
module_exit(lidbg_test_deinit);

EXPORT_SYMBOL(thread_key_xxx);


