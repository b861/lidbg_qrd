#include "fly_target.h"

struct bootloader_hw_config g_hw_info[] =
{
	//msm8226
	{
		.display_info = {
			.dsi83_slave_add = 0x2d,
			.dsi83_en_pin = 62,
			.i2c_blsp_id = 1,
			.i2c_qup_id = 2,
		},

		.adc_info[0] = {
			.ad_ch = 35,
			.ad_ctrl_ch = 3,
			.ad_vol = 2900000,
		},

		.adc_info[ADC_KEY_CHNL - 1] = {
			.ad_ch = 37,
			.ad_ctrl_ch = 5,
			.ad_vol = 2900000,
		},

		.ctp_info = {
			//gt911
			.chip_data[0] = {
				.name = "gt911",
				.ctp_slave_add = 0x14,
				.point_data_add = 0x814e,
			},

			//gt811
			.chip_data[CTP_USED_VAR - 1] = {
				.name = "gt811",
				.ctp_slave_add = 0x5d,
				.point_data_add = 0x0721,
			},

			.ctp_int = 69,
			.ctp_rst = 24,
			.ctp_sda = 18,
			.ctp_scl = 19,
		},

		.lpc_info = {
			.lpc_slave_add = 0xa0,
			.lpc_sda = 6,
			.lpc_scl = 7,
		},
	},
	//msm8974
	{
		.display_info = {
			.dsi83_slave_add = 0x2d,
			.dsi83_en_pin = 58,
			.i2c_blsp_id = 1,
			.i2c_qup_id = 5,
		},

		.adc_info[0] = {
			.ad_ch = 38,
			.ad_ctrl_ch = 6,
			.ad_vol = 3200000,
		},

		.adc_info[ADC_KEY_CHNL - 1] = {
			.ad_ch = 39,
			.ad_ctrl_ch = 7,
			.ad_vol = 3200000,
		},

		.ctp_info = {
			.chip_data[CTP_USED_VAR - 1] = {
				.name = "gt911",
				.ctp_slave_add = 0x5d,
				.point_data_add = 0x814e,
			},

			.ctp_int = 14,
			.ctp_rst = 12,
			.ctp_sda = 6,
			.ctp_scl = 7,
		},

		.lpc_info = {
			.lpc_slave_add = 0xa0,
			.lpc_sda = 2,
			.lpc_scl = 3,
		},
	}
};
