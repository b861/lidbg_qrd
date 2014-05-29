#ifndef  __IO_DEFINE_
#define __IO_DEFINE_


#ifdef SOC_msm8x25
//lpc
#define  LPC_I2_ID  	  (0)
#define  MCU_WP_GPIO 	  (29)
#define  MCU_IIC_REQ_GPIO (30)
#define  MCU_WP_GPIO_SET  do{SOC_IO_Output(0, MCU_WP_GPIO, 1); }while(0)

//gps
#define GPS_I2C_BUS 	  (1)

//touch
#define TS_I2C_BUS (1)
#define RESET_GPIO (27)
#define INT_GPIO   (48)
#endif


#ifdef SOC_msm8x26
//lpc
#define  LPC_I2_ID        (0)
#define  MCU_WP_GPIO      (35)
#define  GPIO_APP_STATUS  (36)
#define  MCU_IIC_REQ_GPIO (108)

#define  MCU_WP_GPIO_SET  do{SOC_IO_Output(0, MCU_WP_GPIO, 0); }while(0)

//dsi83
#define 	DSI83_I2C_BUS  		   (2)
#define 	DSI83_GPIO_EN          (62)

//gps
#define GPS_I2C_BUS (5)

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
#define PANEL_GPIO_RESET (25)
#ifdef BOARD_V1
#define LCD_RESET do{  \
		SOC_IO_Output(0, PANEL_GPIO_RESET, 0);\
		msleep(10);\
		SOC_IO_Output(0, PANEL_GPIO_RESET, 1);\
		msleep(20);\
	}while(0)
#else
#define LCD_RESET
#endif

//t123
#define T123_GPIO_RST  (28)
#define T123_RESET do{  \
		SOC_IO_Output(0, T123_GPIO_RST, 0);\
		msleep(300);\
		SOC_IO_Output(0, T123_GPIO_RST, 1);\
		msleep(20);\
	}while(0)

//usb
#define GPIO_USB_ID 	(60)
#define GPIO_USB_EN 	(109)
#define USB_ID_HIGH_DEV do{\
								SOC_IO_Config(GPIO_USB_ID,GPIO_CFG_INPUT,GPIO_CFG_PULL_UP,GPIO_CFG_16MA);\
								SOC_IO_Output(0,GPIO_USB_ID,1);\
							}while(0)
#define USB_ID_LOW_HOST do{  \
								SOC_IO_Config(GPIO_USB_ID,GPIO_CFG_OUTPUT,GPIO_CFG_NO_PULL,GPIO_CFG_16MA);\
								SOC_IO_Output(0, GPIO_USB_ID, 0);\
							}while(0)

//ad
#define AD_KEY_PORT_L   (35)
#define AD_KEY_PORT_R   (37)


//touch
#define TS_I2C_BUS 		(5)
#define GTP_RST_PORT    (24)  
#define GTP_INT_PORT    (69)
#endif


#endif
