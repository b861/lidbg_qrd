#ifndef  __IO_DEFINE_
#define __IO_DEFINE_

#define GPIO_NOTHING LED_GPIO

#define g_hw g_hw_version_specific[g_var.hw_info.hw_version - 1]  
#define check_gpio(gpio) if(gpio == -1 ) break

struct thermal_ctrl
{
	int temp_low;
	int temp_high;
	u32 limit_freq;
	char* limit_freq_string;
};

struct hw_version_specific
{
//gpio
	int gpio_lcd_reset;
	int gpio_t123_reset;
	int gpio_dsi83_en;

	int gpio_usb_id;
	int gpio_usb_power;
	int gpio_usb_switch;
	
	int gpio_int_gps;

	int gpio_int_button_left1;
	int gpio_int_button_left2;
	int gpio_int_button_right1;
	int gpio_int_button_right2;
	
	int gpio_led1;
	int gpio_led2;

	int gpio_int_mcu_i2c_request;
	int gpio_mcu_wp;
	int gpio_mcu_app;
	
	int gpio_ts_int;
	int gpio_ts_rst;


//i2c
	int i2c_bus_dsi83;
	int i2c_bus_bx5b3a;
	int i2c_bus_ts;
	int i2c_bus_gps;
	int i2c_bus_saf7741;
	int i2c_bus_lpc;

//ad
	int ap_key_left;
	int ap_key_right;

//temp
	struct thermal_ctrl cpu_freq_thermal[10];
			//cat sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
	char * cpu_freq_list;
	char * cpu_freq_temp_node;
	char * cpu_freq_recovery_limit;

//parameter
	char * fly_parameter_node;

//system_switch
	int system_switch_en;

};
extern struct hw_version_specific g_hw_version_specific[];

#define FLYPARAMETER_NODE g_hw.fly_parameter_node
#define SYSTEM_SWITCH_EN  g_hw.system_switch_en

//touch
#define TS_I2C_BUS      (g_hw.i2c_bus_ts)
#define GTP_RST_PORT    (g_hw.gpio_ts_rst)  
#define GTP_INT_PORT    (g_hw.gpio_ts_int)

//7741
#define SAF7741_I2C_BUS  (g_hw.i2c_bus_saf7741)

//lpc
#define  LPC_I2_ID        (g_hw.i2c_bus_lpc)
#define  MCU_IIC_REQ_GPIO (g_hw.gpio_int_mcu_i2c_request)

//dsi83
#define  DSI83_I2C_BUS  		(g_hw.i2c_bus_dsi83)
#define  DSI83_GPIO_EN          (g_hw.gpio_dsi83_en)

#define  BX5B3A_I2C_BUS  		(g_hw.i2c_bus_bx5b3a)
#define  BX5B3A_GPIO_EN  		DSI83_GPIO_EN

//gps
#define GPS_I2C_BUS (g_hw.i2c_bus_gps)
#define GPS_INT	    (g_hw.gpio_int_gps)

//led
#define LED_GPIO (g_hw.gpio_led1)
#define LED_ON  do{check_gpio(g_hw.gpio_led1);SOC_IO_Output(0, g_hw.gpio_led1, 0); }while(0)
#define LED_OFF  do{check_gpio(g_hw.gpio_led1);SOC_IO_Output(0, g_hw.gpio_led1, 1); }while(0)

//button
#define BUTTON_LEFT_1 (g_hw.gpio_int_button_left1)//k1
#define BUTTON_LEFT_2 (g_hw.gpio_int_button_left2)//k2
#define BUTTON_RIGHT_1 (g_hw.gpio_int_button_right1)//k3
#define BUTTON_RIGHT_2 (g_hw.gpio_int_button_right2)//k4


#ifdef SOC_msm8x26

#define  MCU_WP_GPIO_ON  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Output(0, g_hw.gpio_mcu_wp, 0);}while(0)
#define  MCU_WP_GPIO_OFF  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Output(0, g_hw.gpio_mcu_wp, 1);}while(0)

#define  MCU_APP_GPIO_ON  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Output(0, g_hw.gpio_mcu_app, 0);}while(0)
#define  MCU_APP_GPIO_OFF  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Output(0, g_hw.gpio_mcu_app, 1);}while(0)


//lcd

#define LCD_RESET do{\
			check_gpio(g_hw.gpio_lcd_reset);\
			SOC_IO_Output(0, g_hw.gpio_lcd_reset, 0);\
			msleep(20);\
			SOC_IO_Output(0, g_hw.gpio_lcd_reset, 1);\
}while(0)\



//t123
#define T123_RESET do{\
		check_gpio(g_hw.gpio_t123_reset);\
		SOC_IO_Output(0, g_hw.gpio_t123_reset, 0);\
		msleep(300);\
		SOC_IO_Output(0, g_hw.gpio_t123_reset, 1);\
		msleep(20);\
	}while(0)


//usb
#define USB_SWITCH_CONNECT  do{\
			check_gpio(g_hw.gpio_usb_switch);\
			SOC_IO_Output(0, g_hw.gpio_usb_switch, 0);\
	}while(0)

		
#define USB_SWITCH_DISCONNECT do{\
			check_gpio(g_hw.gpio_usb_switch);\
			SOC_IO_Output(0, g_hw.gpio_usb_switch, 1);\
	}while(0)


#define USB_POWER_ENABLE do{\
		LPC_CMD_USB5V_ON;\
		if(g_hw.gpio_usb_power == -1 )\
			break ;\
			if(g_var.hw_info.hw_version == 1)\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 0);\
			else\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 1);\
	}while(0)


#define USB_POWER_DISABLE do{\
			LPC_CMD_USB5V_OFF;\
			check_gpio(g_hw.gpio_usb_power);\
			if(g_var.hw_info.hw_version == 1)\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 1);\
			else\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 0);\
	}while(0)



#define USB_ID_LOW_HOST do{\
			check_gpio(g_hw.gpio_usb_id);\
			SOC_IO_Output(0, g_hw.gpio_usb_id, 0);\
	}while(0)

#define USB_ID_HIGH_DEV do{\
			check_gpio(g_hw.gpio_usb_id);\
			SOC_IO_Output(0, g_hw.gpio_usb_id, 1);\
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
#ifdef PLATFORM_msm8226
#define AD_KEY_PORT_L   (35)//(g_hw.ap_key_left)
#define AD_KEY_PORT_R   (37)//(g_hw.ap_key_right)
#elif defined(PLATFORM_msm8974)
#define AD_KEY_PORT_L   (38)//(g_hw.ap_key_left)
#define AD_KEY_PORT_R   (39)//(g_hw.ap_key_right)
#endif


#ifdef PLATFORM_msm8226
#define FLY_GPS_SO  "gps.msm8226.so"
#elif defined(PLATFORM_msm8974)
#define FLY_GPS_SO  "gps.msm8974.so"
#endif

#endif


#ifdef SOC_mt3360

#define  MCU_WP_GPIO_ON  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Output(0, g_hw.gpio_mcu_wp, 0);}while(0)
#define  MCU_WP_GPIO_OFF  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Output(0, g_hw.gpio_mcu_wp, 1);}while(0)

#define  MCU_APP_GPIO_ON  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Output(0, g_hw.gpio_mcu_app, 0);}while(0)
#define  MCU_APP_GPIO_OFF  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Output(0, g_hw.gpio_mcu_app, 1);}while(0)


//lcd

#define LCD_RESET do{\
			check_gpio(g_hw.gpio_lcd_reset);\
			SOC_IO_Output(0, g_hw.gpio_lcd_reset, 0);\
			msleep(20);\
			SOC_IO_Output(0, g_hw.gpio_lcd_reset, 1);\
}while(0)\

//usb
#define USB_SWITCH_CONNECT  do{\
			check_gpio(g_hw.gpio_usb_switch);\
			SOC_IO_Output(0, g_hw.gpio_usb_switch, 0);\
	}while(0)

		
#define USB_SWITCH_DISCONNECT do{\
			check_gpio(g_hw.gpio_usb_switch);\
			SOC_IO_Output(0, g_hw.gpio_usb_switch, 1);\
	}while(0)


#define USB_POWER_ENABLE do{\
		LPC_CMD_USB5V_ON;
		if(g_hw.gpio_usb_power == -1 )\
			break ;\
			if(g_var.hw_info.hw_version == 1)\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 0);\
			else\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 1);\
	}while(0)


#define USB_POWER_DISABLE do{\
			LPC_CMD_USB5V_OFF;\
			check_gpio(g_hw.gpio_usb_power);\
			if(g_var.hw_info.hw_version == 1)\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 1);\
			else\
				SOC_IO_Output(0, g_hw.gpio_usb_power, 0);\
	}while(0)



#define USB_ID_LOW_HOST do{\
			check_gpio(g_hw.gpio_usb_id);\
			SOC_IO_Output(0, g_hw.gpio_usb_id, 0);\
	}while(0)

#define USB_ID_HIGH_DEV do{\
			check_gpio(g_hw.gpio_usb_id);\
			SOC_IO_Output(0, g_hw.gpio_usb_id, 1);\
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
#define AD_KEY_PORT_L   (35)//(g_hw.ap_key_left)
#define AD_KEY_PORT_R   (37)//(g_hw.ap_key_right)


#define FLY_GPS_SO  "gps.msm8226.so"

#endif

#endif
