#include "fly_target.h"

struct bootloader_hw_config g_hw_info[] =
{
	{
		.adc_info[ADC_KEY_CHNL - 1] = {
			.ad_ch = -1,
			.ad_ctrl_ch = -1,
			.ad_vol = -1,
		},

		.ctp_info = {
			.ctp_slave_add = 0x5d,
			.point_data_add = 0x804e,

			.ctp_int = 7,
			.ctp_rst = 6,
			.ctp_sda = 28,
			.ctp_scl = 29,
		},

		.lpc_info = {
			.lpc_slave_add = 0xa0,
			.lpc_sda = 2,
			.lpc_scl = 3,
		},
	}
};
