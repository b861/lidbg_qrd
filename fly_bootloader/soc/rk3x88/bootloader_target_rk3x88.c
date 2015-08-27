#include "fly_target.h"

struct bootloader_hw_config g_hw_info[] =
{
	{
		.ctp_info = {
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
