/* Copyright (c) 2012, swlee
 *
 */

//#define SOC_COMPILE

#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"

#include "lidbg_enter.h"
#if 0
LIDBG_THREAD_DEFINE;
#else
LIDBG_DEFINE;
#endif
#endif

#include "devices.h"


#define LIDBG_GPIO_PULLUP  GPIO_CFG_PULL_UP

static struct task_struct *led_task;
static struct task_struct *key_task;
static struct task_struct *pwr_task;
static struct task_struct *dev_init_task;
static struct task_struct *resume_task;
static struct task_struct *usb_rst_task;

int thread_dev_init(void *data);
int thread_led(void *data);
int thread_key(void *data);
int thread_pwr(void *data);
int thread_usb(void *data);
static int thread_resume(void *data);

void fly_devices_init(void);

int platform_id;

struct platform_devices_resource devices_resource;


#ifdef CONFIG_HAS_EARLYSUSPEND
static void devices_early_suspend(struct early_suspend *handler);
static void devices_late_resume(struct early_suspend *handler);
struct early_suspend early_suspend;

#endif

bool suspend_test = 0;

int i2c_devices_probe(int i2c_bus, unsigned char *i2c_devices_list)
{
    int rc, i, j = 0;
    u8 *i2c_devices;
    u8 tmp[8];

#define I2C_PROBE_INTERVAL_TIME (10)

    for(i = 1; i<(0xff >> 1); i++)
    {
        rc = SOC_I2C_Rec(i2c_bus, i, 0, &tmp, 1 );

        if (rc >= 0)
        {
            i2c_devices_list[j] = i;
            j++;
        }
        else
        {
            msleep(I2C_PROBE_INTERVAL_TIME);
        }
    }
    printk("\n");
    lidbg("find <%d> i2c_devices:", j);
    for(i = 0; i < j; i++)
    {
        printk("0x%x, ", i2c_devices_list[i]);

    }
    printk("\n");
    printk("\n");
    return j;
}


void get_platform(void)
{
#if 0
    u8 i2c_devices_found[32], i;
    memset(i2c_devices_found, 0, 32);
    i2c_devices_probe(1, i2c_devices_found);
    for(i = 0; i < 32; i++)
    {
        if(i2c_devices_found[i] == 0xe)
        {
            platform_id = PLATFORM_SKU7;
        }
        else
        {
            platform_id = PLATFORM_FLY;
        }
    }
#else
    PLATFORM_GET;
#endif

    lidbg("platform_id = %d\n", platform_id);

}

void pwr_key_scan(void)
{
#if 1
    bool val;
	if(suspend_test == 0)
		return;

    val = SOC_IO_Input(PWR_SLEEP_PORT, PWR_SLEEP_INDEX, LIDBG_GPIO_PULLUP);

    if(val == 0)
    {

		while(SOC_IO_Input(PWR_SLEEP_PORT, PWR_SLEEP_INDEX, LIDBG_GPIO_PULLUP)==0)
		{
			WHILE_ENTER;
			msleep(100);

		}
		
        lidbg("pwr_key_scan goto sleep !\n");

#ifdef DEBUG_UMOUNT_USB
		SOC_Write_Servicer(UMOUNT_USB);
		msleep(1000);
#endif
		
		SOC_PWR_ShutDown();


    }
#endif
}



int thread_pwr(void *data)
{
	//msleep(PWR_SLEEP_TEST_TIME_DELAY);
	//LPCControlSupendTestStart();
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            pwr_key_scan();
			
			if(suspend_test == 1)
            	msleep(PWR_SLEEP_PORT_POLLING_TIME);
			else
				msleep(1000);
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}




struct ad_key_remap
{
    u32 ch;
    u32 ad_value;
    u32 key;
};


static struct ad_key_remap ad_key[] =
{
    //left
    {0, 530 , KEY_RESERVED/*KEY_MEDIA*/},
    {2, 1548, KEY_NEXT},
    {2, 2040, KEY_PREVIOUS},
    {2, 1058, KEY_HOME},

    //right
    {1, 1034, KEY_VOLUMEUP},
    {1, 1530, KEY_VOLUMEDOWN},
    {2, 2558, KEY_MENU},
    {1, 2024, KEY_BACK},

    //left button
    //{2, 3055, KEY_POWER},
    //right button
   // {1, 3055, KEY_MUTE},

};
#define AD_OFFSET  (100)
#define AD_VAL_MAX  (3300)

int find_ad_key(u32 ch)
{
    int val = 0;
    int i;

    SOC_ADC_Get(ch, &val);
    if(val > AD_VAL_MAX)
        return 0xff;

    for(i = 0; i < SIZE_OF_ARRAY(ad_key); i++)
    {
        if(ch == ad_key[i].ch)
            if((val > ad_key[i].ad_value - AD_OFFSET) && (val < ad_key[i].ad_value + AD_OFFSET))
            {
                //SOC_Key_Report(ad_key[i].key, KEY_PRESSED_RELEASED);
                lidbg("find_ad_key:ch%d=%d,key_id=%d,sendkey=%d\n", ch, val, i,ad_key[i].key);
                return i;
            }
    }

	return 0xff;


}

void key_scan(void)
{
#if 0
    {
        for(i = 0; i < ADC_MAX_CH; i++)
        {
            SOC_ADC_Get(i, &val);
            lidbg("SOC_ADC_Get:ch%d=%d\n", i, val);
        }
        printk("\n");

    }
#else
	static int old_key = 0xff;
	int key = 0xff;
    key=find_ad_key(0);if(key != 0xff) goto find_key;
    key=find_ad_key(1);if(key != 0xff) goto find_key;
    key=find_ad_key(2);if(key != 0xff) goto find_key;

find_key:

	if((old_key != 0xff)&&(key == 0xff))
	{
		SOC_Key_Report(ad_key[old_key].key, KEY_PRESSED_RELEASED);

	if(old_key == 0)
	{
		static bool usb_id_host = 1;
		if(usb_id_host)
		{
			USB_HUB_DISABLE;
			USB_ID_HIGH_DEV;
			usb_id_host = 0;
			lidbg("usb_id change to dev\n");
		}
		else
		{
			USB_HUB_ENABLE;
			USB_ID_LOW_HOST;
			usb_id_host = 1;
			lidbg("usb_id change to host\n");
	
		}
	}

#if 0

	if(old_key == 0)
	{
		SOC_Log_Dump(LOG_DMESG);

	}
	else if(old_key == 1)
	{
		SOC_Log_Dump(LOG_LOGCAT);

	}
	else if(old_key == 2)
	{
		SOC_Log_Dump(LOG_ALL);

	}
	else if(old_key == 4)
	{
		static bool pwr_en=1;
		if(pwr_en)
		{
			lidbg("LPCControlPWRDisenable\n");
			LPCControlPWRDisenable();
			pwr_en=0;
			
		}
		else
		{
			lidbg("LPCControlPWREnable\n");
			LPCControlPWREnable();
			pwr_en=1;
		}

	}

#endif




	}



	old_key = key;


#endif


    //
#ifdef DEBUG_GPIO_RETURN_KEY
    if(SOC_IO_Input(0, GPIO_SCAN_KEY_RETURN, GPIO_CFG_PULL_DOWN) == 1)
    {
        //SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);
        //	 lidbg("KEY_BACK+\n");
        SOC_PWR_ShutDown();

    }
#endif

}




void led_on(void)
{

#if 1
    static int led_status = 0;

    if(led_status == 0)
    {
        LED_OFF;
        led_status = 1;
    }
    else
    {
        LED_ON;
        led_status = 0;
    }

#endif
}


int thread_dev_init(void *data)
{

    fly_devices_init();

	while(1)//for polling test
	{
		msleep(10*1000);

	}
    return 0;
}


int thread_led(void *data)
{

    //linux驱动开发之内核线程
    //http://hi.baidu.com/zzcqh/blog/item/36b2b2eabebcf9ded539c9ec.html
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1) //条件为真
        {
            led_on();
            msleep(1000);
        }
        else //条件为假
        {
            //让出CPU运行其他线程，并在指定的时间内重新被调度
            schedule_timeout(HZ);
        }
    }
    return 0;
}


int thread_key(void *data)
{

    //linux驱动开发之内核线程
    //http://hi.baidu.com/zzcqh/blog/item/36b2b2eabebcf9ded539c9ec.html
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1) //条件为真
        {
            key_scan();
            msleep(AD_KEY_READ_POLLING_TIME);
            // msleep(10);
        }
        else //条件为假
        {
            //让出CPU运行其他线程，并在指定的时间内重新被调度
            schedule_timeout(HZ);
        }
    }
    return 0;
}


#ifdef DEBUG_USB_RST
int thread_usb(void *data)
{
	int usb_rst_enable=0;
    
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1) //条件为真
        {
		usb_rst_enable=u2k_read();
	if(usb_rst_enable==USB_RST_ACK)
		{
			
			USB_SWITCH_DISCONNECT;
			msleep(7000);
			USB_SWITCH_CONNECT;
			USB_HUB_RST;
			msleep(3000);
			printk("  ........................rstUSBover...................................over. \n");
		}
            msleep(USB_REC_POLLING_TIME);
        }
        else 
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}
#endif

struct platform_device soc_devices =
{
    .name			= "soc_devices",
    .id 			= 0,
};


static int soc_dev_probe(struct platform_device *pdev)
{

    int err;

    lidbg("soc_dev_probe\n");
	
    PWR_EN_ON;

    get_platform();

    if(platform_id ==  PLATFORM_FLY)
    {
        devices_resource.led_gpio = GPIO_LED_FLY;

    }
    else if(platform_id ==  PLATFORM_SKU7)
    {
        devices_resource.led_gpio = GPIO_LED_SKU7;

    }


#ifdef DEBUG_LED
    led_task = kthread_create(thread_led, NULL, "led_task");
    if(IS_ERR(led_task))
    {
        lidbg("Unable to start kernel thread.\n");
    }else wake_up_process(led_task);
#endif


    if(platform_id ==  PLATFORM_FLY)
    {
        dev_init_task = kthread_create(thread_dev_init, NULL, "dev_init_task");
        if(IS_ERR(dev_init_task))
        {
            lidbg("Unable to start kernel thread.\n");

        }else wake_up_process(dev_init_task);


#ifdef DEBUG_AD_KEY
        key_task = kthread_create(thread_key, NULL, "key_task");
        if(IS_ERR(key_task))
        {
            lidbg("Unable to start kernel thread.\n");
        }else wake_up_process(key_task);
#endif

#ifdef DEBUG_POWER_KEY
        pwr_task = kthread_create(thread_pwr, NULL, "pwr_task");
        if(IS_ERR(pwr_task))
        {
            lidbg("Unable to start kernel thread.\n");

        }else  wake_up_process(pwr_task);
#endif

#ifdef DEBUG_USB_RST
        usb_rst_task = kthread_create(thread_usb, NULL, "usb_rst_task");
        if(IS_ERR(usb_rst_task))
        {
            lidbg("Unable to start kernel thread.\n");

        }else wake_up_process(usb_rst_task);
#endif


    }


#ifdef CONFIG_HAS_EARLYSUSPEND
{
		
		//\u5728suspend\u7684\u65f6\u5019\u5148\u6267\u884c\u4f18\u5148\u7b49\u7ea7\u4f4e\u7684handler\uff0c\u5728resume\u7684\u65f6\u5019\u5219\u5148\u6267\u884c\u7b49\u7ea7\u9ad8\u7684handler
		early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;//EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
		early_suspend.suspend = devices_early_suspend;
		early_suspend.resume = devices_late_resume;
		register_early_suspend(&early_suspend);
		
}
#endif

	return 0;

}
static int soc_dev_remove(struct platform_device *pdev)
{
    lidbg("soc_dev_remove\n");

    if(led_task)
    {
        kthread_stop(led_task);
        led_task = NULL;
    }
#ifdef DEBUG_AD_KEY
    if(key_task)
    {
        kthread_stop(key_task);
        key_task = NULL;
    }
#endif
#ifdef DEBUG_POWER_KEY
    if(pwr_task)
    {
        kthread_stop(pwr_task);
        pwr_task = NULL;
    }
#endif
#ifdef DEBUG_USB_RST
    if(usb_rst_task)
    {
        kthread_stop(usb_rst_task);
        usb_rst_task = NULL;
    }
#endif

    return 0;

}

#ifdef CONFIG_PM


#ifdef CONFIG_HAS_EARLYSUSPEND
static void devices_early_suspend(struct early_suspend *handler)
{

	DUMP_FUN_ENTER;
    if(platform_id ==  PLATFORM_FLY)
    {
		LCD_OFF;
		
#ifdef DEBUG_UMOUNT_USB
		SOC_Write_Servicer(UMOUNT_USB);
#endif
		USB_HUB_DISABLE;
		USB_SWITCH_DISCONNECT;
		USB_ID_HIGH_DEV;

		
   	}
	DUMP_FUN_LEAVE;

}

static void devices_late_resume(struct early_suspend *handler)
{
	int err;

	DUMP_FUN_ENTER;
    if(platform_id ==  PLATFORM_FLY)
    {

		lidbg("create thread_resume!\n");
		
		BL_SET(BL_MAX / 2);

		
		resume_task = kthread_create(thread_resume, NULL, "dev_resume_task");
		if(IS_ERR(resume_task))
		{
			lidbg("Unable to start kernel thread.\n");
			err = PTR_ERR(resume_task);
		}else wake_up_process(resume_task);

		
    }
	DUMP_FUN_LEAVE;
}
#endif





static int  soc_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
    lidbg("soc_dev_suspend\n");


    if(platform_id ==  PLATFORM_FLY)
    {	
        lidbg("turn lcd off!\n");

#ifdef DEBUG_BUTTON
        SOC_IO_ISR_Disable(BUTTON_LEFT_1);
        SOC_IO_ISR_Disable(BUTTON_LEFT_2);
        SOC_IO_ISR_Disable(BUTTON_RIGHT_1);
        SOC_IO_ISR_Disable(BUTTON_RIGHT_2);
#endif
        

    }


    return 0;

}


static int thread_resume(void *data)
{

	DUMP_FUN_ENTER;
	msleep(3000);

	
	lidbg("usb enable\n");
	USB_ID_LOW_HOST;
	msleep(2000);
	USB_SWITCH_CONNECT;
	USB_HUB_RST;
	DUMP_FUN_LEAVE;
	return 0;


}


static int soc_dev_resume(struct platform_device *pdev)
{

    lidbg("soc_dev_resume\n");
		

    if(platform_id ==  PLATFORM_FLY)
    {
    //disable usb first
    	USB_HUB_DISABLE;
		USB_ID_HIGH_DEV;
		USB_SWITCH_DISCONNECT;
		
    
        PWR_EN_ON;

#ifdef DEBUG_BUTTON
        SOC_IO_ISR_Enable(BUTTON_LEFT_1);
        SOC_IO_ISR_Enable(BUTTON_LEFT_2);
        SOC_IO_ISR_Enable(BUTTON_RIGHT_1);
        SOC_IO_ISR_Enable(BUTTON_RIGHT_2);
#endif


        lidbg("turn lcd on!\n");
        LCD_ON;

    }


    return 0;

}
#endif

static struct platform_driver soc_devices_driver =
{
    .probe = soc_dev_probe,
    .remove = soc_dev_remove,
    .suspend =  soc_dev_suspend,
    .resume =  soc_dev_resume,
    .driver = {
        .name = "soc_devices",
        .owner = THIS_MODULE,

    },


};


struct work_struct work_left_button1;
static void work_left_button1_fn(struct work_struct *work)
{

	
#ifdef DEBUG_POWER_KEY
	static int count = 0;
	count++;
	lidbg("work_left_button1_fn %d\n",count);
	if(count%40 == 0)
	{
		if(suspend_test)
		{
			printk("\n\n");
			lidbg("LPCControlSupendTestStop\n");
			printk("\n\n");
			LCD_OFF;
			msleep(500);
			LCD_ON;
			suspend_test = 0;
			LPCControlSupendTestStop();
			
		}
		else
		{
			printk("\n\n");
			lidbg("LPCControlSupendTestStart\n");
			printk("\n\n");
			LCD_OFF;
			msleep(500);
			LCD_ON;
			suspend_test = 1;
			LPCControlSupendTestStart();
			//SOC_Log_Dump(LOG_CONT);
		}

	}
#endif
}

struct work_struct work_right_button1;
static void work_right_button1_fn(struct work_struct *work)
{

#if 1
		static int count = 0;
		count++;
		lidbg("work_left_button1_fn %d\n",count);
		if(count%40 == 0)
		{

			static bool usb_test=0;
			if(usb_test)
			{
				lidbg("LPCControlUSBTestStop\n");
				LCD_OFF;
				msleep(500);
				LCD_ON;
				//LPCControlSupendTestStop();
				usb_test=0;
				
			}
			else
			{
				lidbg("LPCControlUSBTestStart\n");
				LCD_OFF;
				msleep(500);
				LCD_ON;
				//LPCControlSupendTestStart();
				usb_test=1;
			}	
		}
#endif



}







irqreturn_t irq_left_button1(int irq, void *dev_id)
{
#if 0
    static int bl = 0;
    bl = bl + 10;
    SOC_BL_Set(bl & BL_MAX);
    lidbg("irq_left_button1: %d, bl=%d\n", irq, bl & BL_MAX);
#else

   lidbg("irq_left_button1: %d\n", irq);
   schedule_work(&work_left_button1);

#endif
    return IRQ_HANDLED;

}


irqreturn_t irq_left_button2(int irq, void *dev_id)
{


	lidbg("irq_left_button2: %d\n", irq);


    return IRQ_HANDLED;

}


irqreturn_t irq_right_button1(int irq, void *dev_id)
{

    lidbg("irq_right_button1: %d\n", irq);
    //led_on();
	schedule_work(&work_right_button1);

    return IRQ_HANDLED;

}

irqreturn_t irq_right_button2(int irq, void *dev_id)
{

    lidbg("irq_right_button2: %d\n", irq);


    return IRQ_HANDLED;

}

void fly_devices_init(void)
{
    lidbg("fly_devices_init\n");


    if(platform_id ==  PLATFORM_FLY)
    {
    
		PWR_EN_ON;
		USB_ID_LOW_HOST;
		USB_SWITCH_CONNECT;
		USB_HUB_RST;


        lidbg("turn lcd on!\n");
        LCD_ON;
        BL_SET(BL_MAX / 2);

#ifdef DEBUG_BUTTON

		INIT_WORK(&work_left_button1, work_left_button1_fn);
		INIT_WORK(&work_right_button1, work_right_button1_fn);

        SOC_IO_Input(BUTTON_LEFT_1, BUTTON_LEFT_1, GPIO_CFG_PULL_UP);
        SOC_IO_Input(BUTTON_LEFT_2, BUTTON_LEFT_2, GPIO_CFG_PULL_UP);
        SOC_IO_Input(BUTTON_RIGHT_1, BUTTON_RIGHT_1, GPIO_CFG_PULL_UP);
        SOC_IO_Input(BUTTON_RIGHT_2, BUTTON_RIGHT_2, GPIO_CFG_PULL_UP);

        SOC_IO_ISR_Add(BUTTON_LEFT_1, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_left_button1, NULL);
        SOC_IO_ISR_Add(BUTTON_LEFT_2, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_left_button2, NULL);
        SOC_IO_ISR_Add(BUTTON_RIGHT_1, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_right_button1, NULL);
        SOC_IO_ISR_Add(BUTTON_RIGHT_2, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, irq_right_button2, NULL);



#endif


    }
}


int dev_init(void)
{

    DUMP_BUILD_TIME;

#ifdef FLY_DEBUG
    lidbg("debug version\n");
#else
    lidbg("release version\n");
#endif

#ifndef SOC_COMPILE
#if 0
	LIDBG_GET_THREAD;
#else
	LIDBG_GET;
#endif
#endif
#if 0
    PWR_EN_ON;

    get_platform();

    if(platform_id ==  PLATFORM_FLY)
    {
        devices_resource.led_gpio = GPIO_LED_FLY;

    }
    else if(platform_id ==  PLATFORM_SKU7)
    {
        devices_resource.led_gpio = GPIO_LED_SKU7;

    }
#endif

    platform_device_register(&soc_devices);
    platform_driver_register(&soc_devices_driver);

    return 0;
}

void dev_exit(void)
{
    lidbg("dev_exit\n");

}



void lidbg_device_main(int argc, char **argv)
{

    lidbg("lidbg_device_main\n");

    if(argc < 1)
    {
        lidbg("Usage:\n");
        lidbg("camera set_id camid\n");
        lidbg("bl value\n");
        lidbg("pwroff\n");
        return;

    }

    if(!strcmp(argv[0], "camera"))
    {
        if(!strcmp(argv[1], "set_id"))
        {
            //u32 cam_id =  simple_strtoul(argv[2], 0, 0);
            //SOC_Camera_Set(cam_id);
        }
    }


    if(!strcmp(argv[0], "bl"))
    {
        u32 bl;
        bl = simple_strtoul(argv[1], 0, 0);
        SOC_BL_Set(bl);
    }

    if(!strcmp(argv[0], "pwroff"))
    {
        msleep(3000);
        lidbg("Ready to entry sleep ...");
        SOC_PWR_ShutDown();
    }


    if(!strcmp(argv[0], "ad"))
    {
        u32 ch, val;
        ch = simple_strtoul(argv[1], 0, 0);
        SOC_ADC_Get(ch, &val);
        lidbg("ch%d = %x\n", ch, val);

    }


}



EXPORT_SYMBOL(lidbg_device_main);


MODULE_AUTHOR("lsw, <sw.lee.g2@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);

