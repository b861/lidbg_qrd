//like dts

struct hw_version_specific g_hw_version_specific[] = 
{//px3 v1 
{
	
	.gpio_led1 = -1,
	.gpio_ts_int= RK30_PIN0_PA7,
	.gpio_ts_rst= RK30_PIN0_PA6,
	.i2c_bus_ts = 2,
	.fly_parameter_node = "/dev/block/platform/emmc/by-name/flyparameter",
	.lpc_disable = 1,
	},
};

