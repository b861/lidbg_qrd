#include "fly_target.h"

struct bootloader_hw_config g_hw_info[] =
{
	{
		.ctp_info = {
			.chip_data[CTP_USED_VAR - 1] = {
				.name = "gt911",
				.ctp_slave_add = 0x5d,
				.point_data_add = 0x814e,
			},

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
