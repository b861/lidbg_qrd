#ifndef  __TARGET_MSM8x26__DEFINE_
#define __TARGET_MSM8x26__DEFINE_


#define  MCU_WP_GPIO_ON  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Output(0, g_hw.gpio_mcu_wp, 0);}while(0)
#define  MCU_WP_GPIO_OFF  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Output(0, g_hw.gpio_mcu_wp, 1);}while(0)
#define  MCU_SET_WP_GPIO_SUSPEND  do{check_gpio(g_hw.gpio_mcu_wp);SOC_IO_Suspend_Config(g_hw.gpio_mcu_wp,GPIOMUX_OUT_LOW,GPIO_CFG_NO_PULL,GPIOMUX_DRV_2MA);}while(0)

#define  MCU_APP_GPIO_ON  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Output(0, g_hw.gpio_mcu_app, 0);}while(0)
#define  MCU_APP_GPIO_OFF  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Output(0, g_hw.gpio_mcu_app, 1);}while(0)
#define  MCU_SET_APP_GPIO_SUSPEND  do{check_gpio(g_hw.gpio_mcu_app);SOC_IO_Suspend_Config(g_hw.gpio_mcu_app,GPIOMUX_OUT_LOW,GPIO_CFG_NO_PULL,GPIOMUX_DRV_2MA);}while(0)

//gps
#define MSM_GPS_POWER_ON do{\
	if(g_var.hw_info.hw_version < 3)break;\
	lidbg("MSM_GPS_POWER_ON\n"); \
	lidbg_shell_cmd("echo 1 > /sys/kernel/debug/regulator/8226_l18/enable"); \
}while(0)

#define MSM_GPS_POWER_OFF do{\
	if(g_var.hw_info.hw_version < 3)break;\
	lidbg("MSM_GPS_POWER_OFF"); \
	lidbg_shell_cmd("echo 0 > /sys/kernel/debug/regulator/8226_l18/enable"); \
}while(0)

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
			check_gpio(g_hw.gpio_usb_power);\
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
	
#define SET_USB_ID_SUSPEND do{\
				check_gpio(g_hw.gpio_usb_id);\
				SOC_IO_Suspend_Config(g_hw.gpio_usb_id,GPIOMUX_OUT_HIGH,GPIO_CFG_NO_PULL,GPIOMUX_DRV_2MA);\
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
			

#ifdef PLATFORM_msm8226
#define FLY_GPS_SO  "gps.msm8226.so"
#elif defined(PLATFORM_msm8974)
#define FLY_GPS_SO  "gps.msm8974.so"
#endif


#endif
