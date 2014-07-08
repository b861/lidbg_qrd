#ifndef  __IO_DEFINE_
#define __IO_DEFINE_

#define GPIO_NOTHING LED_GPIO

#ifdef SOC_msm8x25
//lpc
#define  LPC_I2_ID  	  (0)
#define  MCU_WP_GPIO 	  (29)
#define  MCU_IIC_REQ_GPIO (30)
#define  MCU_WP_GPIO_SET  do{SOC_IO_Output(0, MCU_WP_GPIO, 1); }while(0)

//gps
#define GPS_I2C_BUS 	  (1)

//touch
#define TS_I2C_BUS 		(1)
#define GTP_RST_PORT    (27)
#define GTP_INT_PORT    (48)

#define FLY_GPS_SO  "gps.msm8625.so"

#endif


struct hw_version_specific
{
	int gpio_lcd_reset;
	int gpio_t123_reset;

	int gpio_usb_id;
	int gpio_usb_power;
	int gpio_usb_switch;
};


#ifdef SOC_msm8x26

//lpc
#define  LPC_I2_ID        (0)
#define  MCU_IIC_REQ_GPIO (108)
#define  MCU_WP_GPIO      (35)
#define  GPIO_APP_STATUS  (36)


#define  MCU_WP_GPIO_SET  do{SOC_IO_Output(0, MCU_WP_GPIO, 0); }while(0)

//dsi83
#define 	DSI83_I2C_BUS  		   (2)
#define 	DSI83_GPIO_EN          (62)

//gps
#define GPS_I2C_BUS (5)
#define GPS_INT	    (50)

//led
#define LED_GPIO  (60)
#define LED_ON  do{SOC_IO_Output(0, LED_GPIO, 0); }while(0)
#define LED_OFF  do{SOC_IO_Output(0, LED_GPIO, 1); }while(0)

//button
#define BUTTON_LEFT_1 (31)//k1
#define BUTTON_LEFT_2 (32)//k2
#define BUTTON_RIGHT_1 (33)//k3
#define BUTTON_RIGHT_2 (34)//k4

//lcd

#define LCD_RESET do{\
			if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_lcd_reset == -1 )\
				break ;\
			SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_lcd_reset, 0);\
			msleep(20);\
			SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_lcd_reset, 1);\
}while(0)\



//t123
#define T123_RESET do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_t123_reset == -1 )\
			break ;\
		SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_t123_reset, 0);\
		msleep(300);\
		SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_t123_reset, 1);\
		msleep(20);\
	}while(0)


//usb
#define USB_SWITCH_CONNECT  do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_switch == -1 )\
			break ;\
			SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_switch, 0);\
	}while(0)

		
#define USB_SWITCH_DISCONNECT do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_switch == -1 )\
			break ;\
			SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_switch, 1);\
	}while(0)


#define USB_POWER_ENABLE do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_power == -1 )\
			break ;\
			if(g_var.hw_info.hw_version == 1)\
				SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_power, 0);\
			else\
				SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_power, 1);\
	}while(0)


#define USB_POWER_DISABLE do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_power == -1 )\
			break ;\
			if(g_var.hw_info.hw_version == 1)\
				SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_power, 1);\
			else\
				SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_power, 0);\
	}while(0)



#define USB_ID_LOW_HOST do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_id == -1 )\
			break ;\
			SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_id, 0);\
	}while(0)

#define USB_ID_HIGH_DEV do{\
		if(g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_id == -1 )\
			break ;\
			SOC_IO_Output(0, g_hw_version_specific[g_var.hw_info.hw_version - 1].gpio_usb_id, 1);\
	}while(0)


#define USB_WORK_ENABLE do{\
				lidbg("USB_WORK_ENABLE\n");\
				USB_SWITCH_CONNECT;\
    			USB_POWER_ENABLE;\
    			USB_ID_LOW_HOST;\
			}while(0)
#define USB_WORK_DISENABLE  do{\
			lidbg("USB_WORK_DISENABLE\n");\
			USB_SWITCH_DISCONNECT;\
			USB_POWER_DISABLE;\
			USB_ID_HIGH_DEV;\
			}while(0)
//ad
#define AD_KEY_PORT_L   (35)
#define AD_KEY_PORT_R   (37)

#ifdef PLATFORM_msm8974
#define TS_I2C_BUS              (2)
#define GTP_RST_PORT    (13)  
#define GTP_INT_PORT    (14)
#endif

#ifdef PLATFORM_msm8226
#define TS_I2C_BUS              (5)
#define GTP_RST_PORT    (24)  
#define GTP_INT_PORT    (69)
#endif
/*
//touch
#define TS_I2C_BUS 		(5)
#define GTP_RST_PORT    (24)  
#define GTP_INT_PORT    (69)
*/

//
#define  SAF7741_I2C_BUS  (5)

/*
//TBD
struct thermal_ctrl thermal_ctrl[] =
{
	{0,1401600,"1401600"},
	{90,1094400,"1094400"},
	{100,600000,"600000"},
	{110,300000,"300000"}
}
*/
//temp
#define THRESHOLDS_STEP0    (0)
#define FREQ_STEP0     		(1401600)
#define FREQ_STEP0_STRING   ("1401600")

#define THRESHOLDS_STEP1    (80)
#define FREQ_STEP1   	 	(1094400)
#define FREQ_STEP1_STRING   ("1094400")

#define THRESHOLDS_STEP2    (90)
#define FREQ_STEP2   	 	(787200)
#define FREQ_STEP2_STRING   ("787200")

#define THRESHOLDS_STEP3    (100)
#define FREQ_STEP3   	 	(600000)
#define FREQ_STEP3_STRING   ("600000")


#define FREQ_RECOVERY_STRING   ("600000")


#define CPU_TEMP_PATH 		"/sys/class/thermal/thermal_zone5/temp"
#define TEMP_FREQ_TEST_STR	 "300000,384000,600000,787200,998400,1094400,1190400,1305600,1344000,1401600"


#ifdef PLATFORM_msm8226
#define FLY_GPS_SO  "gps.msm8226.so"
#elif defined(PLATFORM_msm8974)
#define FLY_GPS_SO  "gps.msm8974.so"
#endif

#endif

extern struct hw_version_specific g_hw_version_specific[];

#endif
