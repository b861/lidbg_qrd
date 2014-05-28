
#ifndef _MISC_DEV_SOC__
#define _MISC_DEV_SOC__

void lidbg_device_main(int argc, char **argv);
#define TEMP_LOG_PATH LIDBG_LOG_DIR"log_ct.txt"
#define CPU_TEMP_PATH "/sys/class/thermal/thermal_zone5/temp"
int thread_key(void *data);
int thread_button_init(void *data);
int thread_led(void *data);
int thread_thermal(void *data);
void led_resume(void);
void temp_init(void);
int button_suspend(void);
int button_resume(void);
int thread_sound_detect(void *data);


extern int fb_on;

#ifdef SOC_msm8x26

//led
#define LED_GPIO  (60)
#define LED_ON  do{SOC_IO_Output(0, LED_GPIO, 0); }while(0)
#define LED_OFF  do{SOC_IO_Output(0, LED_GPIO, 1); }while(0)

//ad
#define AD_KEY_PORT   (0xff)

//button
#define BUTTON_LEFT_1 (31)//k1
#define BUTTON_LEFT_2 (32)//k2
#define BUTTON_RIGHT_1 (33)//k3
#define BUTTON_RIGHT_2 (34)//k4


//USB
#define GPIO_USB_ID (99)  // error ,notice!

#define USB_ID_HIGH_DEV do{\
								SOC_IO_Config(GPIO_USB_ID,GPIO_CFG_INPUT,GPIO_CFG_PULL_UP,GPIO_CFG_16MA);\
								SOC_IO_Output(0,GPIO_USB_ID,1);\
							}while(0)
#define USB_ID_LOW_HOST do{  \
								SOC_IO_Config(GPIO_USB_ID,GPIO_CFG_OUTPUT,GPIO_CFG_NO_PULL,GPIO_CFG_16MA);\
								SOC_IO_Output(0, GPIO_USB_ID, 0);\
							}while(0)

// PANNE_PEN
#define LCD_ON  do{lidbg("LCD_ON\n");LPC_CMD_LCD_ON;}while(0)
#define LCD_OFF do{lidbg("LCD_OFF\n");LPC_CMD_LCD_OFF;}while(0)


#define 	PANEL_GPIO_RESET       (25)

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

#define T123_GPIO_RST          (28)
#define T123_RESET do{  \
		SOC_IO_Output(0, T123_GPIO_RST, 0);\
		msleep(300);\
		SOC_IO_Output(0, T123_GPIO_RST, 1);\
		msleep(20);\
	}while(0)

#endif


#endif
