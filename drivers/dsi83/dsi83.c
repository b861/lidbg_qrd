
#include "lidbg.h"

#define 	DSI83_GPIO_EN       (62)
#define 	PANEL_GPIO_RESET       (62)

#define 	DSI83_I2C_BUS  		(2)
#define 	DSI83_I2C_ADDR	      (0x2d)

//#define TEST_PATTERN

#define 	DSI83_TRACE_GPIO	      (0)


unsigned char dsi83_settings[] =
{
    0x09, 0x00,
    0x0A, 0x03,
    0x0B, 0x10,
    0x0D, 0x00,
    0x10, 0x26,
    0x11, 0x00,
    0x12, 0x19,
    0x13, 0x00,
    0x18, 0x78,
    0x19, 0x00,
    0x1A, 0x03,
    0x1B, 0x00,
    0x20, 0x00,
    0x21, 0x04,
    0x22, 0x00,
    0x23, 0x00,
    0x24, 0x00,
    0x25, 0x00,
    0x26, 0x00,
    0x27, 0x00,
    0x28, 0x21,
    0x29, 0x00,
    0x2A, 0x00,
    0x2B, 0x00,
    0x2C, 0x08,
    0x2D, 0x00,
    0x2E, 0x00,
    0x2F, 0x00,
    0x30, 0x03,
    0x31, 0x00,
    0x32, 0x00,
    0x33, 0x00,
    0x34, 0x2e,
    0x35, 0x00,
    0x36, 0x00,
    0x37, 0x00,
    0x38, 0x00,
    0x39, 0x00,
    0x3A, 0x00,
    0x3B, 0x00,
    0x3C, 0x00,
    0x3D, 0x00,
    0x3E, 0x00,
    0xff
};

static   struct i2c_adapter *adapter;

static void dsi83_trace_trigger(void)
{
#ifdef DSI83_TRACE_GPIO
    static unsigned char  flag = 0;
    static unsigned char  tmp;
    tmp = (flag ++ % 2);
    gpio_direction_output(DSI83_TRACE_GPIO, tmp );
    printk("dsi83_trace_trigger %d , %d\n", flag , tmp);
#endif
}

static int dsi83_enable(void)
{
    return gpio_direction_output(DSI83_GPIO_EN, 1);
}

static int dsi83_disable(void)
{
    return gpio_direction_output(DSI83_GPIO_EN, 0);
}

static int dsi83_i2c_read( uint8_t reg)
{
    int buf;
    int ret;
    uint8_t r;
    struct i2c_msg msg[2];
    r = reg;

    msg[0].flags	= 0;
    msg[0].addr	= DSI83_I2C_ADDR;
    msg[0].len	= 1;
    msg[0].buf	= &r;

    msg[1].flags	= I2C_M_RD;
    msg[1].addr	= DSI83_I2C_ADDR;
    msg[1].len	= 1;
    msg[1].buf	= (void *)&buf;

    ret = i2c_transfer(adapter, msg, 2);

    if (ret != 2)
        return -EIO;

    return buf;
}


static int dsi83_i2c_write( uint8_t reg, uint8_t d)
{
    int ret;
    struct i2c_msg msg[2];
    uint8_t r, c;
    r = reg;
    c = d;

    msg[0].flags	= 0;
    msg[0].addr	= DSI83_I2C_ADDR;
    msg[0].len	= 1;
    msg[0].buf	= &r;

    msg[1].flags	= 0;
    msg[1].addr	= DSI83_I2C_ADDR;
    msg[1].len	= 1;
    msg[1].buf	= &c;

    ret = i2c_transfer(adapter, msg, 2);

    if (ret != 2)
        return -EIO;

    return 0;
}


static int dsi83_match_id(void)
{
    /*Addresses 0x08 - 0x00 = {0x01, 0x20, 0x20, 0x20, 0x44, 0x53, 0x49, 0x38,0x35}*/
    uint8_t id[9] = {0x01, 0x20, 0x20, 0x20, 0x44, 0x53, 0x49, 0x38, 0x35};
    uint8_t chip_id[9];
    int i, j;

    for (i = 0x8, j = 0; i >= 0; i--, j++)
    {
        chip_id[j] = dsi83_i2c_read(i);
        printk( "%s:reg 0x%x = 0x%x", __func__, i, chip_id[j]);
    }

    return memcmp(id, chip_id, 9);
}

static void panel_reset(void)
{
    gpio_direction_output(PANEL_GPIO_RESET, 0);
    msleep(100);
    gpio_direction_output(PANEL_GPIO_RESET, 1);
}


int dsi83_io_config(u32 index)
{
    int err;
    struct gpiomux_setting io_setting_active =
    {
        .func = GPIOMUX_FUNC_GPIO,
        .drv = GPIO_CFG_16MA,
        .pull = GPIO_CFG_NO_PULL,
        .dir = GPIO_CFG_OUTPUT,
    };

    struct msm_gpiomux_config io_configs[] =
    {
        {
            .gpio = index,
            .settings = {
                [GPIOMUX_ACTIVE] = &io_setting_active,
            },
        },
    };
    msm_gpiomux_install(io_configs, ARRAY_SIZE(io_configs));

    err = gpio_request(index, "dsi83_io");
    if (err)
    {
        printk("err: gpio request failed!\n");
        return 0;
    }
    return 1;

}


static int dsi83_probe(struct platform_device *pdev)
{
    printk( "%s:enter\n", __func__);

    adapter = i2c_get_adapter(DSI83_I2C_BUS);

    dsi83_io_config(DSI83_GPIO_EN);
    dsi83_io_config(PANEL_GPIO_RESET);
#ifdef DSI83_TRACE_GPIO
    dsi83_io_config(DSI83_TRACE_GPIO);
    gpio_direction_output(DSI83_TRACE_GPIO, 1);// init high first
#endif

    dsi83_enable();
    panel_reset();

    if(dsi83_match_id() != 0)
        printk("%s:dsi83_match_id fail\n", __func__);

    return 0;
}

static int dsi83_remove(struct platform_device *pdev)
{
    gpio_free(PANEL_GPIO_RESET);
    gpio_free(DSI83_GPIO_EN);
    return 0;
}

void dsi83_dump_reg(void)
{
    int i;
    unsigned char read_val;

    for (i = 0; i < 0x3d; i++)
    {
        read_val = dsi83_i2c_read(i);
        printk("0x%x=0x%x\n", i, read_val);
    }
}


/*1.After power is applied and stable, all DSI Input lanes including DSI CLK(DA x P/N, DB x P/N) MUST be driven to
LP11 state.*/


/*2.Assert the EN pin*/
static void dsi83_seq2(void)
{
    dsi83_disable();
    msleep(20);
    dsi83_enable();
}

/*3.Wait for 1ms for the internal voltage regulator to stabilize*/
static void dsi83_seq3(void)
{
    msleep(20);
}

/*4.Initialize all CSR registers to their appropriate values based on the implementation (The SN65DSI83 is not
functional until the CSR registers are initialized)*/
int dsi83_seq4(void)
{
    int ret , i ;
    unsigned char read_val;
    char *buf_piont ;
    buf_piont = dsi83_settings;
    printk( "%s:enter", __func__);
    for(i = 0; buf_piont[i] != 0xff ; i += 2)
    {
        ret = dsi83_i2c_write(buf_piont[i], buf_piont[i + 1]);
        printk("register 0x%x=0x%x\n", buf_piont[i], buf_piont[i+1]);

#if 1
        read_val = dsi83_i2c_read( buf_piont[i]);
        if(read_val != buf_piont[i+1])
        {
            printk("dsi83: Warning regitster(0x%.2x),write(0x%.2x) and read back(0x%.2x) Unequal\n", \
                   buf_piont[i], buf_piont[i+1], read_val);
        }
#endif
    }
    return ret;
}

/*5.Start the DSI video stream*/

/*6.Set the PLL_EN bit(CSR 0x0D.0)*/
int dsi83_seq6(void)
{
    int ret;
    printk( "%s:enter", __func__);
    ret = dsi83_i2c_write( 0x0d, 0x01);
    return ret;
}

/*7.Wait for the PLL_LOCK bit to be set(CSR 0x0A.7)*/
int dsi83_seq7(void)
{
    unsigned char read_val, i;
    bool is_pll_lock ;

    printk( "%s:enter", __func__);

    is_pll_lock = false;
    i = 0;
    while(1)
    {
        read_val = dsi83_i2c_read( 0x0a);
        is_pll_lock = read_val & 0x80;
        if(is_pll_lock)break;
        printk("dsi83: wait for %d,r = 0x%.2x\n", i, read_val);
        i++;
        if(i > 10)
        {
            printk("dsi83:Warning wait time out .. break\n");
            break;
        }
        msleep(20);
    }
    return is_pll_lock;
}


/*8.Set the SOFT_RESET bit (CSR 0x09.0)*/
/*the bit must be set after the CSR`s are updated*/
int dsi83_seq8(void)
{
    int ret;
    printk( "%s:enter", __func__);

    dsi83_dump_reg();
    ret = dsi83_i2c_write(0x09, 0x01);
    return ret;
}


void dsi83_test_pattern(void)
{
    printk( "%s:enter", __func__);
    dsi83_seq4();
    dsi83_i2c_write(0x3c, 0xff);
}

static int thread_trace_err_status(void *data)
{
    unsigned char read_val;
    while(1)
    {
        read_val = dsi83_i2c_read( 0xe5);
        printk("dsi83: 0xe5 = 0x%.2x\n",  read_val);
	 dsi83_i2c_write( 0xe5 , read_val);
        msleep(1000);
    }
    return 0;
}


static struct task_struct *dsi83_task;
void dsi83_trace_err_status(void)
{
    dsi83_task = kthread_run(thread_trace_err_status, NULL, "dsi83_trace_err_status");
}


static void parse_cmd(char *cmd_buf)
{
    dsi83_trace_trigger();

    if(!strcmp(cmd_buf, "seq2"))
    {
        dsi83_seq2();
    }
    else if(!strcmp(cmd_buf, "seq3"))
    {
        dsi83_seq3();
    }
    else if(!strcmp(cmd_buf, "seq4"))
    {
#ifndef TEST_PATTERN
        dsi83_seq4();
#else
        dsi83_test_pattern();
#endif
    }
    else if(!strcmp(cmd_buf, "seq6"))
    {
        dsi83_seq6();
    }
    else if(!strcmp(cmd_buf, "seq7"))
    {
        dsi83_seq7();
    }
    else if(!strcmp(cmd_buf, "seq8"))
    {
        dsi83_seq8();
    }
    else if(!strcmp(cmd_buf, "dump"))
    {
        dsi83_dump_reg();
    }
    else if(!strcmp(cmd_buf, "trace_err_status"))
    {
        dsi83_trace_err_status();
    }
    else if(!strcmp(cmd_buf, "trigger"))
    {
        udelay(10);
    }

    dsi83_trace_trigger();
}

ssize_t dsi83_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    char cmd_buf[32];
    char *p = NULL;

    memset(cmd_buf, '\0', 32);

    if(copy_from_user(cmd_buf, buf, size))
    {
        printk("copy_from_user ERR\n");
    }
    if((p = memchr(cmd_buf, '\n', size)))
    {
        *p = '\0';
    }
    else
        cmd_buf[size] =  '\0';

    parse_cmd(cmd_buf);

    return size;

}

int dsi83_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int dsi83_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static struct file_operations dev_fops =
{
    .owner	=	THIS_MODULE,
    .open   =   dsi83_open,
    .write  =   dsi83_write,
    .release =  dsi83_release,
};

#define DEVICE_NAME "sn65dsi83"

static struct miscdevice misc =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,
};


static struct platform_device dsi83_devices =
{
    .name			= "dsi83",
    .id 			= 0,
};

static struct platform_driver dsi83_driver =
{
    .probe = dsi83_probe,
    .remove = dsi83_remove,
    .driver = {
        .name = "dsi83",
        .owner = THIS_MODULE,
    },
};

static int __devinit dsi83_init(void)
{
    printk( "%s:enter", __func__);
    platform_device_register(&dsi83_devices);
    platform_driver_register(&dsi83_driver);
    misc_register(&misc);
    return 0;
}

static void __exit dsi83_exit(void)
{
    misc_deregister(&misc);
}

module_init(dsi83_init);
module_exit(dsi83_exit);
MODULE_LICENSE("GPL");
