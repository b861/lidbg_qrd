#ifndef __BOOTLOADER_TARGET_H__
#define __BOOTLOADER_TARGET_H__

#include "fly_private.h"

struct display_config
{
	int dsi83_slave_add;
	int dsi83_en_pin;

	int i2c_blsp_id;
	int i2c_qup_id;
};

struct adc_config
{
	int ad_ch;
	int ad_ctrl_ch;
	int ad_vol;
};

struct ctp_config
{
	int ctp_slave_add;
	int point_data_add;

	int ctp_int;
	int ctp_rst;
	int ctp_sda;
	int ctp_scl;
};

struct lpc_config
{
	int lpc_slave_add;
	int lpc_sda;
	int lpc_scl;
};

struct bootloader_hw_config
{
	struct display_config display_info;
	struct adc_config adc_info[ADC_KEY_CHNL];
	struct ctp_config ctp_info;
	struct lpc_config lpc_info;
};

extern struct bootloader_hw_config g_hw_info[];
#define g_bootloader_hw g_hw_info[BOARD_VERSION]
#endif
