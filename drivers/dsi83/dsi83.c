
#include "lidbg.h"
#include <dsi83.h>

LIDBG_DEFINE;

static struct delayed_work dsi83_work;
static struct workqueue_struct *dsi83_workqueue;

#if defined(CONFIG_FB)
	struct notifier_block dsi83_fb_notif;
#elif defined(CONFIG_HAS_EARLYSUSPEND)

#endif

static int SN65_register_read(unsigned char sub_addr,char *buf)
{
	int ret;
	ret = SOC_I2C_Rec(DSI83_I2C_BUS,DSI83_I2C_ADDR,sub_addr,buf,1);
	return ret;
}

static int SN65_register_write( char *buf)
{
	int ret;
	ret = SOC_I2C_Send(DSI83_I2C_BUS,DSI83_I2C_ADDR,buf,2);
	return ret;
}

static int SN65_Sequence_seq4(void)
{
	int ret = 0,i;
	char buf2[2];
	char *buf_piont = NULL;
	buf_piont = dsi83_conf;
	buf2[0]=0x00;
	buf2[1]=0x00;
	printk(KERN_CRIT "dsi83:Sequence 4\n");
	for(i=0;buf_piont[i] !=0xff ;i+=2)
	{
		ret = SN65_register_write(&buf_piont[i]);
		buf2[0] = buf_piont[i];
		ret = SN65_register_read(buf2[0],&buf2[1]);
		lidbg_dsi83("register 0x%x=0x%x\n", buf_piont[i], buf_piont[i+1]);

		if(buf2[1] != buf_piont[i+1])
		{
			printk(KERN_CRIT "Warning regitster(0x%.2x),write(0x%.2x) and read back(0x%.2x) Unequal\n",\
			buf_piont[i],buf_piont[i+1],buf2[1]);
		}
	}
	return ret;
}

static int SN65_Sequence_seq6(void)
{
	int ret;
	char buf2[2];
	buf2[0]=0x0d;
	buf2[1]=0x01;
	printk(KERN_CRIT "dsi83:Sequence 6\n");
	ret = SN65_register_write(buf2);
	return ret;
}
static int SN65_Sequence_seq7(void)
{
	int ret;
	char buf2[2];
	printk(KERN_CRIT "dsi83:Sequence 7\n");

	buf2[0]=0x0a;
	buf2[1]=0x00;
	ret = SN65_register_read(buf2[0],&buf2[1]);
	lidbg_dsi83("read(0x0a) = 0x%.2x\n",buf2[1]);

	{
		unsigned char k,i=0;
		k = buf2[1]&0x80;
		while(!k)
		{
			ret = SN65_register_read(buf2[0],&buf2[1]);
			k = buf2[1]&0x80;
			printk(KERN_CRIT "dsi83:Wait for %d,r = 0x%.2x\n",i,buf2[1]);
			i++;
			if(i>100)
			{
				printk(KERN_CRIT "dsi83:Warning wait time out .. break\n");
				break;
			}
			msleep(20);
		}
	}
return ret;
}

static int SN65_Sequence_seq8(void)  /*seq 8 the bit must be set after the CSR`s are updated*/
{
	int ret;
	char buf2[2];
	
#ifdef DSI83_DEBUG
	dsi83_dump_reg();
#endif
	printk(KERN_CRIT "dsi83:Sequence 8\n");
	buf2[0]=0x09;
	buf2[1]=0x01;
	ret = SN65_register_write(buf2);
	lidbg_dsi83("write(0x09) = 0x%.2x\n",buf2[1]);
	
#ifdef DSI83_DEBUG
	mdelay(100);
	dsi83_dump_reg();
#endif	
	return ret;
}


#ifdef DSI83_DEBUG
static void dsi83_dump_reg(void)
{
	int i;
	unsigned char reg;
	
	char buf1[2];
	buf1[0]=0xe5;
	buf1[1]=0xff;

	for (i = 0; i < 0x3d; i++) {
		SN65_register_read(i, &reg);
		printk(KERN_CRIT "dsi83:Read reg-0x%x=0x%x\n", i, reg);
	}

/*
	int ret;
	SN65_register_read(0xe1, &reg);
	printk(KERN_CRIT "[LSH]:reg-0xE1=0x%x.\n",reg);
	SN65_register_read(0xe5, &reg);
	printk(KERN_CRIT "[LSH]:reg-0xE5=0x%x.\n",reg);
	printk(KERN_CRIT "*****************************\n");

	for(i = 0; i < 20; ++i)
	{
		
		ret = SN65_register_write(buf1);
		if(!ret)
			printk(KERN_CRIT "[LSH]:write reg 0xe5 error.\n");
		msleep(50);
		
		SN65_register_read(0xe1, &reg);
		printk(KERN_CRIT "[LSH]:reg-0xE1=0x%x.\n",reg);
		SN65_register_read(0xe5, &reg);
		printk(KERN_CRIT "[LSH]:reg-0xE5=0x%x.\n",reg);

		printk(KERN_CRIT "------------------------\n");
		msleep(10);
	}
*/

}
#endif


static int SN65_devices_read_id(void)
{
	// addr:0x08 - 0x00
	uint8_t id[9] = {0x01, 0x20, 0x20, 0x20, 0x44, 0x53, 0x49, 0x38, 0x35};
	uint8_t chip_id[9];
	int i, j;

	for (i = 0x8, j = 0; i >= 0; i--, j++) 
	{
		if(SN65_register_read(i,&chip_id[j]) < 0)
			return -1;
		else
			lidbg_dsi83("%s():reg 0x%x = 0x%x", __func__, i, chip_id[j]);
	}

	return memcmp(id, chip_id, 9);

}

static void dsi83_enable(void)
{
	SOC_IO_Output(0, DSI83_GPIO_EN, 1);
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
void dsi83_suspend(void)
{}

void dsi83_resume(void)
{
	queue_delayed_work(dsi83_workqueue, &dsi83_work, DSI83_DELAY_TIME);
}

static int dsi83_fb_notifier_callback(struct notifier_block *self,
				 unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int *blank;
	
	if (evdata && evdata->data && event == FB_EVENT_BLANK) 
	{
		blank = evdata->data;
		
		if (*blank == FB_BLANK_UNBLANK)
			dsi83_resume();
		else if (*blank == FB_BLANK_POWERDOWN)
			dsi83_suspend();
	}

	return 0;
}
#elif defined(CONFIG_HAS_EARLYSUSPEND)

#endif

void dsi83_gpio_init()
{
	dsi83_enable();
	panel_reset();
	T123_reset();
}


static void dsi83_work_func(struct work_struct *work)
{
    int ret = 0;
	int i;
	DUMP_FUN;

	dsi83_gpio_init();
	
	for(i = 0; i < 3; ++i)
	{
		ret = SN65_devices_read_id();
		if (!ret)
			break;
		else
		{
			printk(KERN_CRIT "dsi83:DSI83 match ID falied,num:%d.\n", i+1);
			continue;
		}
	}

	if(i == 3)
		return;
	else
		printk(KERN_CRIT "dsi83:DSI83 match ID success!\n");

	SN65_Sequence_seq4();
	
	ret = SN65_Sequence_seq6();
	if(ret < 0)
		printk(KERN_CRIT "dsi83:SN65_Sequence_seq6(),err,ret = %d.\n", ret);
	
	SN65_Sequence_seq7();
	
	ret = SN65_Sequence_seq8();
	if(ret < 0)
		printk(KERN_CRIT "dsi83:SN65_Sequence_seq8(),err,ret = %d.\n", ret);

}



static int dsi83_probe(struct platform_device *pdev)
{
	int ret = 0;
	lidbg_dsi83("%s:enter\n", __func__);

	INIT_DELAYED_WORK(&dsi83_work, dsi83_work_func);
	dsi83_workqueue = create_workqueue("dsi83");
	queue_delayed_work(dsi83_workqueue, &dsi83_work, DSI83_DELAY_TIME);

#if defined(CONFIG_FB)
		dsi83_fb_notif.notifier_call = dsi83_fb_notifier_callback;
		ret = fb_register_client(&dsi83_fb_notif);
		if (ret)
				lidbg("Unable to register dsi83_fb_notif: %d\n",ret);
#elif defined(CONFIG_HAS_EARLYSUSPEND)

#endif

	return 0;

}

static int dsi83_remove(struct platform_device *pdev)
{
	cancel_work_sync(dsi83_workqueue);
	flush_workqueue(dsi83_workqueue);
	destroy_workqueue(dsi83_workqueue);
    return 0;
}


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
	DUMP_BUILD_TIME;
	LIDBG_GET;
    platform_device_register(&dsi83_devices);
    platform_driver_register(&dsi83_driver);
    return 0;

}

static void __exit dsi83_exit(void){}


module_init(dsi83_init);
module_exit(dsi83_exit);
MODULE_LICENSE("GPL");

