//like dts

struct hw_version_specific g_hw_version_specific[] = 
{ 
	{	//px3 v1
	.gpio_led1 = RK30_PIN0_PB4,
	.gpio_ts_int= RK30_PIN0_PA7,
	.gpio_ts_rst= RK30_PIN0_PA6,
	.i2c_bus_ts = 2,
	.i2c_bus_tef6638 = -1,
	.fly_parameter_node = "/dev/block/platform/emmc/by-name/flyparameter",
	.lpc_disable = 1,
        .ad_val_mcu = 1,
	},

	{	//px3 v2
	.gpio_led1 = -1,
	.gpio_ts_int= RK30_PIN1_PB7,
	.gpio_ts_rst= RK30_PIN0_PB6,
	.i2c_bus_ts = 2,
	.i2c_bus_tef6638 = 3,
	.fly_parameter_node = "/dev/block/platform/emmc/by-name/flyparameter",
	},
};

