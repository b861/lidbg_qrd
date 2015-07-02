//like dts

struct hw_version_specific g_hw_version_specific[] = 
{//px3 v1 
{
	
	.gpio_led1 = -1,

	.gpio_ts_int= -1,
	.gpio_ts_rst= -1,

	.i2c_bus_ts = 0,
	.fly_parameter_node = "/dev/block/flyparameter",
	.lpc_disable = 1,

	},
};

