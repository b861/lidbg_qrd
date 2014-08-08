
#ifndef _MISC_DEV_SOC__
#define _MISC_DEV_SOC__

void lidbg_device_main(int argc, char **argv);
int thread_key(void *data);
int thread_button_init(void *data);
int thread_led(void *data);
int thread_thermal(void *data);
void led_resume(void);
void temp_init(void);
int button_suspend(void);
int button_resume(void);
int thread_sound_detect(void *data);
int SOC_Get_System_Sound_Status_func(void *para, int length);
int  iGPS_sound_status(void);
void set_system_performance(bool enable);


// PANNE_PEN
#define LCD_ON  do{lidbg("LCD_ON\n");LPC_CMD_LCD_ON;}while(0)
#define LCD_OFF do{lidbg("LCD_OFF\n");LPC_CMD_LCD_OFF;}while(0)

#endif
