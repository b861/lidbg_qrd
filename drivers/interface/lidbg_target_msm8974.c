//dts

struct hw_version_specific g_hw_version_specific[] = 
{

{//msm8974 v1
	.gpio_lcd_reset = 25,
	.gpio_t123_reset = 28,
	.gpio_dsi83_en = 58,

	.gpio_usb_id = -1,
	.gpio_usb_power = 109,
	.gpio_usb_switch = -1,
	
	.gpio_int_gps = 50,

	.gpio_int_button_left1 = 54,
	.gpio_int_button_left2 = 53,
	.gpio_int_button_right1 = 56,
	.gpio_int_button_right2 = 55,

	.gpio_led1 = 60,
	.gpio_led2 = -1,

	.gpio_int_mcu_i2c_request = 108,
	.gpio_mcu_wp = 35,
	.gpio_mcu_app = 36,

	.gpio_ts_int= 14,
	.gpio_ts_rst= 13,

	
	.i2c_bus_dsi83 = 3,
	.i2c_bus_bx5b3a= 3,
	.i2c_bus_ts = 2,
	.i2c_bus_gps = 2,
	.i2c_bus_saf7741 = 2,
	.i2c_bus_lpc = 0,

	.ad_key = 
	{
		{   
			.ch = 39,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2500, KEY_HOME},
						}
		},
		
		{    
		    .ch = 38,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2000, KEY_MENU},
						{2500, KEY_BACK},
						}
		},
	},


	.cpu_freq_thermal = 
	{
		{0,0, 0, "0"},//end flag
	},
	
	.cpu_freq_list = "300000,422400,652800,729600,883200,960000,1036800,1190400,1267200,1497600,1574400,1728000,1958400,2265600",
	.cpu_freq_temp_node = "/sys/class/thermal/thermal_zone5/temp",
	.cpu_freq_recovery_limit = "600000",

	.fly_parameter_node = "/dev/block/mmcblk0p25",
	},
{//msm8974 v2
	.gpio_lcd_reset = 80,
	.gpio_t123_reset = -1,
	.gpio_dsi83_en = 58,

	.gpio_usb_id = -1,
	.gpio_usb_power = 78,
	.gpio_usb_switch = -1,
	
	.gpio_int_gps = 50,
	
	.gpio_int_button_left1 = 54,
	.gpio_int_button_left2 = 53,
	.gpio_int_button_right1 = 56,
	.gpio_int_button_right2 = 55,
	
	.gpio_led1 = 75,
	.gpio_led2 = 81,
	
	.gpio_int_mcu_i2c_request = 76,
	.gpio_mcu_wp = 79,
	.gpio_mcu_app = 68,
	
	.gpio_ts_int= 14,
	.gpio_ts_rst= 12,
	
	
	.i2c_bus_dsi83 = 3,
	.i2c_bus_bx5b3a= 3,
	.i2c_bus_ts = 2,
	.i2c_bus_gps = 2,
	.i2c_bus_saf7741 = 2,
	.i2c_bus_lpc = 0,


	.ad_key = 
	{
		{   
			.ch = 39,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2500, KEY_HOME},
						}
		},
		
		{    
		    .ch = 38,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2000, KEY_MENU},
						{2500, KEY_BACK},
						}
		},
	},

	
	.cpu_freq_thermal = 
	{
		{0,0, 0, "0"},//end flag
	},
	
	.cpu_freq_list = "300000,422400,652800,729600,883200,960000,1036800,1190400,1267200,1497600,1574400,1728000,1958400,2265600",
	.cpu_freq_temp_node = "/sys/class/thermal/thermal_zone5/temp",
	.cpu_freq_recovery_limit = "600000",

	.fly_parameter_node = "/dev/block/mmcblk0p25",
	},

};

