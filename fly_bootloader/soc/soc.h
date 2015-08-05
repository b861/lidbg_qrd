#ifndef _FLYBOOTLOADER_SOC_
#define _FLYBOOTLOADER_SOC_

#include "fly_platform.h"

/*gpio operatios*/
void gpio_set_direction(int pin, int dir);
void gpio_set_val(int pin, int val);
int gpio_get_val(int pin);

/*adc get value*/
int adc_get(void);

/*get fb base addr*/
void *fb_base_get(void);

/*ptn operations*/
int ptn_read(char *ptn_name,unsigned  long len,unsigned char *buf);

/*display init*/
void flyaudio_display_init(void);

/*ctp reset*/
unsigned char ctp_read(char *buf,  unsigned char *sub_addr,unsigned int size);
unsigned char ctp_write( char *buf, unsigned int size);
int ctp_reset(void);

#endif
