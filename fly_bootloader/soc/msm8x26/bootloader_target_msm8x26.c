#include "fly_target.h"

struct bootloader_hw_config g_hw_info[] =
{
	{
		.ctp_info = {
			.ctp_slave_add = 0x5d,
			.point_data_add = 0x804e,

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
