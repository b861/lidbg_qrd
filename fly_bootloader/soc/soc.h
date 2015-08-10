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

/*platform logo addr get*/
int logo_addr_get(unsigned char **partition_add, int partition_add_len,
					unsigned char **rgb565_add, int rgb565_add_len,
					unsigned char **rgb888_add, int rgb888_add_len, int *need_alloc_flag);

/*ptn operations*/
int ptn_read(char *ptn_name,unsigned  long len,unsigned char *buf);

/*display init*/
void flyaudio_display_init(void);

/* flyrecovery */
int boot_flyrecovery_from_mmc(void);

/* dbg msg enable or disable*/
char *dbg_msg_en(const char *system_cmd, int dbg_msg_en);

#endif
