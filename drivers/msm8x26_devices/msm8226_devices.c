
#include "lidbg.h"
LIDBG_DEFINE;
int led_en ;
int ad_en;
int button_en;
int thread_dev_init(void *data);
int thread_led(void *data);
int thread_key(void *data);
void fly_devices_init(void);
struct ad_key_remap
{
    u32 ch;
    u32 ad_value;
    u32 key;
};
static struct ad_key_remap ad_key[] =
{
    //  CHANNAL_NO 11 13 15 16
    //steering wheel   rem2_in  mpp2
    {11, 2517, KEY_HOME},
    {11, 2517, KEY_HOME},
    {11, 2517, KEY_HOME},
    {11, 2517, KEY_HOME},
    //feel  key  x_ain_3   mpp4
    {13, 1372, KEY_BACK},
    {13, 1372, KEY_BACK},
    {13, 1372, KEY_BACK},
    {13, 1372, KEY_BACK},
    //feel  key  x_ain_4  mpp6
    {15, 1372, KEY_BACK},
    {15, 1372, KEY_BACK},
    {15, 1372, KEY_BACK},
    {15, 1372, KEY_BACK},
     //feel  key  rem1_in  mpp7
    {16, 1108, KEY_MENU},
    {16, 1372, KEY_BACK},
    {16, 1108, KEY_MENU},
    {16, 1372, KEY_BACK},
};
#define AD_OFFSET  (100)
#define AD_VAL_MAX  (3300)
void led_on(void)
{
     LED_ON;
	 msleep(1000);
	 LED_OFF;
	 msleep(1000);
}


int thread_led(void *data)
{
    while(1)
    {
          if(led_en)
                led_on();
    }
    return 0;
}

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
                lidbg("find_ad_key:ch%d=%d,key_id=%d,sendkey=%d\n", ch, val, i, ad_key[i].key);
                return i;
            }
    }

    return 0xff;

}

void key_scan(void)
{
    static int old_key = 0xff;
    int key = 0xff;
    key = find_ad_key(4);
    if(key != 0xff) goto find_key;
    key = find_ad_key(2);
    if(key != 0xff) goto find_key;

find_key:

    //if((old_key != 0xff) && (key == 0xff))
    {
        SOC_Key_Report(ad_key[old_key].key, KEY_PRESSED_RELEASED);
    }
    old_key = key;
}

int thread_dev_init(void *data)
{

    fly_devices_init();
    return 0;
}
int thread_key(void *data)
{
    while(1)
    {
        key_scan();
        msleep(100);
    }
    return 0;
}
static int soc_dev_probe(struct platform_device *pdev)
{
    lidbg("=====soc_dev_probe====\n");
    CREATE_KTHREAD(thread_dev_init, NULL);
	
    FS_REGISTER_INT(led_en, "led_en", 1, NULL);
    if(led_en)
        CREATE_KTHREAD(thread_led, NULL);
      
    FS_REGISTER_INT(ad_en, "ad_en", 0, NULL);
    if(ad_en)
	   CREATE_KTHREAD(thread_key, NULL);

    return 0;

}
static int soc_dev_remove(struct platform_device *pdev)
{
    lidbg("soc_dev_remove\n");
    return 0;

}
static int  soc_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
    lidbg("soc_dev_suspend\n");
    SOC_IO_ISR_Disable(BUTTON_LEFT_1);
    SOC_IO_ISR_Disable(BUTTON_LEFT_2);
    SOC_IO_ISR_Disable(BUTTON_RIGHT_1);
    SOC_IO_ISR_Disable(BUTTON_RIGHT_2);
    return 0;

}
static int soc_dev_resume(struct platform_device *pdev)
{
    lidbg("soc_dev_resume\n");

    SOC_IO_ISR_Enable(BUTTON_LEFT_1);
    SOC_IO_ISR_Enable(BUTTON_LEFT_2);
    SOC_IO_ISR_Enable(BUTTON_RIGHT_1);
    SOC_IO_ISR_Enable(BUTTON_RIGHT_2);   
	SOC_IO_Config(LED_GPIO, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_8MA);
	return 0;
}
struct work_struct work_left_button1;
static void work_left_button1_fn(struct work_struct *work)
{
    SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
}

struct work_struct work_right_button1;
static void work_right_button1_fn(struct work_struct *work)
{
    SOC_Key_Report(KEY_MENU, KEY_PRESSED_RELEASED);

}
irqreturn_t irq_left_button1(int irq, void *dev_id)
{
    lidbg("irq_left_button1: %d\n", irq);
    schedule_work(&work_left_button1);
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
    FS_REGISTER_INT(button_en, "button_en", 0, NULL);
	if(button_en)
	{
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
	}
}
struct platform_device soc_devices =
{
    .name			= "msm8x26_soc_devices",
    .id 			= 0,
};
static struct platform_driver soc_devices_driver =
{
    .probe = soc_dev_probe,
    .remove = soc_dev_remove,
    .suspend = soc_dev_suspend,
    .resume = soc_dev_resume,
    .driver = {
        .name = "msm8x26_soc_devices",
        .owner = THIS_MODULE,
    },
};
int dev_init(void)
{
	lidbg("=======msm826_dev_init========\n");
	LIDBG_GET;
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
        return;
    }
}
EXPORT_SYMBOL(lidbg_device_main);
MODULE_AUTHOR("lsw, <sw.lee.g2@gmail.com>");
MODULE_DESCRIPTION("Devices Driver");
MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);

