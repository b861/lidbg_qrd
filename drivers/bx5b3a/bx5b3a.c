#include "lidbg.h"
#include <bx5b3a.h>

#define QL_DBG(f, x...) \
	printk(KERN_CRIT"[QLVX] %s: " f, __func__,## x)
#define QL_DBGL(lvl, f, x...) do {if (lvl) printk(KERN_CRIT"[QLVX] %s: " f, __func__,## x); }while(0)
#define QL_ERR(f, x...) \
	printk(KERN_CRIT"[QLVX] ERROR %s: " f, __func__,## x)



//#ifdef QL_VX_REG_I2C_DIRECT_ACCESS
#define CONTROL_BYTE_DA_WR     (0x0Au)
#define CONTROL_BYTE_DA_RD     (0x0Eu)

#define DA_QL_WRITE  {\
		CONTROL_BYTE_DA_WR, \
        0x00,  /* Address MS */\
        0x00,  /* Address LS */\
        0x00,  /* data LS */\
		0x00, \
	    0x00, \
        0x00,  /* data MS */\
    }

#define DA_QL_READ  {\
		CONTROL_BYTE_DA_RD, \
        0x00,  /* Address MS */\
        0x00,  /* Address LS */\
        0x00,  /* len MS */\
        0x00,  /* len LS */\
    }



struct i2c_client *i2c_quick_client = NULL;


LIDBG_DEFINE;

static struct delayed_work bx5b3a_work;
static struct workqueue_struct *bx5b3a_workqueue;

#if defined(CONFIG_FB)
struct notifier_block bx5b3a_fb_notif;
#elif defined(CONFIG_HAS_EARLYSUSPEND)

#endif


int ql_i2c_read(UINT32 addr, UINT32 *val, UINT32 data_size)
{
    UINT32 data;
    char buf[] = DA_QL_READ;
    char rx[10];
    int ret = -1;
    int write_size;

    buf[1] = (addr >> 8) & 0xff;
    buf[2] = addr & 0xff;
    buf[3] = (data_size >> 8) & 0xff;
    buf[4] = data_size & 0xff;

    write_size = 5;

    /* Read register */
    if ((ret = i2c_master_send( i2c_quick_client,
                                (char *)(&buf[0]),
                                write_size )) != write_size)
    {
        printk(KERN_ERR
               "%s: i2c_master_send failed (%d)!\n", __func__, ret);
        return -1;
    }
    //return number of bytes or error
    if ((ret = i2c_master_recv( i2c_quick_client,
                                (char *)(&rx[0]),
                                data_size )) != data_size)
    {
        printk(KERN_ERR
               "%s: i2c_master_recv failed (%d)!\n", __func__, ret);
        return -1;
    }

    data = rx[0];
    if (data_size > 1)
        data |= (rx[1] << 8);
    if (data_size > 2)
        data |= (rx[2] << 16) | (rx[3] << 24);

    *val = data;

    QL_DBG("r0x%x=0x%x\n", addr, data);

    return 0;

}

int ql_i2c_write(long addr, long val, int data_size)
{
    int ret = -1;
    int write_size;
    char buf[] = DA_QL_WRITE;

    QL_DBG("w0x%lx=0x%lx\n", addr, val);

    buf[1] = (uint8_t)(addr >> 8);  /* Address MS */
    buf[2] = (uint8_t)addr;  /* Address LS */

    buf[3] = val & 0xff;
    buf[4] = (data_size > 1) ? ((val >> 8) & 0xff) : 0;
    buf[5] = (data_size > 2) ? ((val >> 16) & 0xff) : 0;
    buf[6] = (data_size > 2) ? ((val >> 24) & 0xff) : 0;

    write_size = data_size + 3;
    if ((ret = i2c_master_send( i2c_quick_client,
                                (char *)(&buf[0]),
                                write_size )) != write_size)
    {
        printk(KERN_ERR
               "%s: i2c_master_send failed (%d)!\n", __func__, ret);
        return -1;
    }
    return 0;

}


static void ql_init_table(struct QL_VX_INIT_INFO *table, UINT16 count)
{
    UINT16 i;
    UINT16 addr;
    UINT32 data;
    UINT32 ret_value;

    for(i = 0; i < count; i++)
    {
        addr = table[i].address;
        data = table[i].data;
        ql_i2c_write(addr, data, 4);
        ret_value = 0;
        ql_i2c_read(addr, &ret_value, 4);

#if 0
        /* if you are using 32kHz sys_clk, and seeing i2c error after the first 0x154 write,
        	you may try enable this, and adjust the delay below, now it's 0.5 seconds */
        if ((addr == 0x154) && (data == 0x80000000))
            msleep(500);
#endif
    }

}


static int bx5b3a_devices_read_id(void)
{

    UINT32 ret_value;
    int ret;
    ret = ql_i2c_read(0x148, &ret_value, 4);

    if(ret_value == 0x3104) //read err
        return -1;
    return ret;
}

static void bx5b3a_enable(void)
{
    SOC_IO_Output(0, BX5B3A_GPIO_EN, 0);
    msleep(100);
    SOC_IO_Output(0, BX5B3A_GPIO_EN, 1);
    msleep(100);
}

static void T123_reset(void)
{
    T123_RESET;
}

static void panel_reset(void)
{
    LCD_RESET;
}

#if defined(CONFIG_FB)
void bx5b3a_suspend(void)
{}

void bx5b3a_resume(void)
{
    DUMP_FUN;
    queue_delayed_work(bx5b3a_workqueue, &bx5b3a_work, 0);
}

static int bx5b3a_fb_notifier_callback(struct notifier_block *self,
                                       unsigned long event, void *data)
{
    struct fb_event *evdata = data;
    int *blank;

    if (evdata && evdata->data && event == FB_EVENT_BLANK)
    {
        blank = evdata->data;

        if (*blank == FB_BLANK_UNBLANK)
            bx5b3a_resume();
        else if (*blank == FB_BLANK_POWERDOWN)
            bx5b3a_suspend();
    }

    return 0;
}
#elif defined(CONFIG_HAS_EARLYSUSPEND)

#endif

void bx5b3a_gpio_init(void)
{
    bx5b3a_enable();
    panel_reset();
    T123_reset();
}


static void bx5b3a_work_func(struct work_struct *work)
{
    int ret = 0;
    int i;
    DUMP_FUN;

    bx5b3a_gpio_init();

    msleep(100);

    for(i = 0; i < 5; ++i)
    {
        ret = bx5b3a_devices_read_id();
        if (!ret)
            break;
        else
        {
            lidbg(KERN_CRIT "bx5b3a:bx5b3a match ID falied,num:%d.\n", i + 1);
            msleep(100);
            continue;
        }
    }

    if(i == 3)
        return;
    else
        lidbg(KERN_CRIT "bx5b3a:bx5b3a match ID success!\n");

    ql_init_table(ql_initialization_setting, sizeof(ql_initialization_setting) / sizeof(struct QL_VX_INIT_INFO));
    //msleep(100);
    //ql_init_table(ql_hs_setting, sizeof(ql_hs_setting) / sizeof(struct QL_VX_INIT_INFO));
    //ql_init_table(ql_dpi_on_setting, sizeof(ql_dpi_on_setting) / sizeof(struct QL_VX_INIT_INFO));


}

int is_bx5b3a_exist(void)
{
    int exist, retry;
    for(retry = 0; retry < 5; retry++)
    {
        exist = bx5b3a_devices_read_id();
        if (exist != 0)
        {
            msleep(50);
            continue;
        }
        else
        {
            return 1;
        }
    }
    return -1;
}


static int bx5b3a_probe(struct platform_device *pdev)
{
    int ret = 0;
    lidbg("%s:enter\n", __func__);

    i2c_quick_client = i2c_get_client(BX5B3A_I2C_BUS);
    if(i2c_quick_client == NULL)
    {
        lidbgerr("bx5b3a.get i2c_client fail\n");
        return 0;
    }
    i2c_quick_client->addr = BX5B3A_I2C_ADDR;

#if 0
    if(is_bx5b3a_exist() < 0)
    {
        lidbg("bx5b3a.miss\n");
        return 0;
    }
#endif
    INIT_DELAYED_WORK(&bx5b3a_work, bx5b3a_work_func);
    bx5b3a_workqueue = create_workqueue("bx5b3a");

#ifdef PLATFORM_msm8974
    queue_delayed_work(bx5b3a_workqueue, &bx5b3a_work, 0);
#endif

#if defined(CONFIG_FB)
    bx5b3a_fb_notif.notifier_call = bx5b3a_fb_notifier_callback;
    ret = fb_register_client(&bx5b3a_fb_notif);
    if (ret)
        lidbg("Unable to register bx5b3a_fb_notif: %d\n", ret);
#elif defined(CONFIG_HAS_EARLYSUSPEND)

#endif

    return 0;

}

static int bx5b3a_remove(struct platform_device *pdev)
{
    //	cancel_work_sync(bx5b3a_workqueue);
    //	flush_workqueue(bx5b3a_workqueue);
    //	destroy_workqueue(bx5b3a_workqueue);
    return 0;
}

#ifdef CONFIG_PM
static int bx5b3a_ops_suspend(struct device *dev)
{
    DUMP_FUN;
    //bx5b3a_suspend();
    return 0;
}
static int bx5b3a_ops_resume(struct device *dev)
{
    DUMP_FUN;
    //bx5b3a_resume();
    return 0;
}
static struct dev_pm_ops bx5b3a_ops =
{
    .suspend	= bx5b3a_ops_suspend,
    .resume		= bx5b3a_ops_resume,
};
#endif

static struct platform_device bx5b3a_devices =
{
    .name			= "bx5b3a",
    .id 			= 0,
};

static struct platform_driver bx5b3a_driver =
{
    .probe = bx5b3a_probe,
    .remove = bx5b3a_remove,
    .driver = {
        .name = "bx5b3a",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &bx5b3a_ops,
#endif
    },
};

static int bx5b3a_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;
    platform_device_register(&bx5b3a_devices);
    platform_driver_register(&bx5b3a_driver);
    return 0;

}

static void __exit bx5b3a_exit(void) {}

module_init(bx5b3a_init);
module_exit(bx5b3a_exit);
MODULE_LICENSE("GPL");



