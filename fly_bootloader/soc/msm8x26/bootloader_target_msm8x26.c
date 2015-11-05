#include "fly_target.h"

struct bootloader_hw_config g_hw_info[] =
{
	//msm8226
	{
		.display_info = {
			.dsi83_slave_add = 0x2d,
			.dsi83_en_pin = 62,
			.dsi83_sda = 10,
			.dsi83_scl = 11,
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
			.ctp_int = 69,
			.ctp_rst = 24,
			.ctp_sda = 18,
			.ctp_scl = 19,
		},

		.lpc_info = {
			.lpc_slave_add = 0x50,
			.lpc_sda = 6,
			.lpc_scl = 7,
		},
		
		.dbg_uart_port = "console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 ",
	},
	//msm8974
	{
		.display_info = {
			.dsi83_slave_add = 0x2d,
			.dsi83_en_pin = 58,
			.dsi83_sda = 29,
			.dsi83_scl = 30,
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
			.ctp_int = 14,
			.ctp_rst = 12,
			.ctp_sda = 6,
			.ctp_scl = 7,
		},

		.lpc_info = {
			.lpc_slave_add = 0x50,
			.lpc_sda = 2,
			.lpc_scl = 3,
		},

		.dbg_uart_port = "console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 ",
	},
	//msm8909
	{
		.display_info = {
			.dsi83_slave_add = 0x2d,
			.dsi83_en_pin = 8,
			.dsi83_sda = 6,
			.dsi83_scl = 7,
		},

               .adc_info[0] = {
                       .ad_ch = -1,
                       .ad_ctrl_ch = -1,
                       .ad_vol = 3200,
               },

               .adc_info[ADC_KEY_CHNL - 1] = {
                       .ad_ch = -1,
                       .ad_ctrl_ch = -1,
                       .ad_vol = 0,
               },

		.ctp_info = {
			.ctp_int = 13,
			.ctp_rst = 12,
			.ctp_sda = 14,
			.ctp_scl = 15,
		},

		.lpc_info = {
			.lpc_slave_add = 0x50,
			.lpc_sda = 29,
			.lpc_scl = 30,
		},
		
		.dbg_uart_port = "console=ttyHSL1,115200,n8 androidboot.console=ttyHSL1 ",
	}
};
