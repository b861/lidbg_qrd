//dts

struct hw_version_specific g_hw_version_specific[] = 
{

{//msm8226 v1
	.gpio_lcd_reset = 25,
	.gpio_t123_reset = 28,
	.gpio_dsi83_en = 62,
	
	.gpio_usb_id = -1,
	.gpio_usb_power = 109,
	.gpio_usb_switch = -1,
	
	.gpio_int_gps = 50,
	
	.gpio_int_button_left1 = 31,
	.gpio_int_button_left2 = 32,
	.gpio_int_button_right1 = 33,
	.gpio_int_button_right2 = 34,
	
	.gpio_led1 = 60,
	.gpio_led2 = -1,

	.gpio_int_mcu_i2c_request = 108,
	.gpio_mcu_wp = 35,
	.gpio_mcu_app = 36,
	
	.gpio_ts_int= 69,
	.gpio_ts_rst= 24,

	
	.i2c_bus_dsi83 = 2,
	.i2c_bus_bx5b3a= 2,
	.i2c_bus_ts = 5,
	.i2c_bus_gps = 5,
	.i2c_bus_saf7741 = 5,
	.i2c_bus_lpc = 0,

	.ad_key = 
	{
		{   
			.ch = 35,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2500, KEY_HOME},
						}
		},
		
		{    
		    .ch = 37,
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
		{1,  80,  1401600,"1401600"},
		{81, 90,  1094400,"1094400"},
		{91, 100, 787200, "787200"},
		{101,105, 600000, "600000"},
		{105,500, 300000, "300000"},
		{0,0, 0, "0"},//end flag
	},
	
	.cpu_freq_list = "300000,384000,600000,787200,998400,1094400,1190400,1305600,1344000,1401600",
	.cpu_freq_temp_node = "/sys/class/thermal/thermal_zone5/temp",
	.cpu_freq_recovery_limit = "600000",

	.fly_parameter_node = "/dev/block/mmcblk0p25",
	.system_switch_en = 1,

	},
{//msm8226 v2
	.gpio_lcd_reset = -1,
	.gpio_t123_reset = -1,
	.gpio_dsi83_en = 62,
	
	.gpio_usb_id = 23,
	.gpio_usb_power = 28,
	.gpio_usb_switch = 109,
	
	.gpio_int_gps = 50,
	
	.gpio_int_button_left1 = 31,
	.gpio_int_button_left2 = 32,
	.gpio_int_button_right1 = 33,
	.gpio_int_button_right2 = 34,

	.gpio_led1 = 60,
	.gpio_led2 = -1,

	.gpio_int_mcu_i2c_request = 108,
	.gpio_mcu_wp = 35,
	.gpio_mcu_app = 36,
	
	.gpio_ts_int= 69,
	.gpio_ts_rst= 24,

	
	.i2c_bus_dsi83 = 2,
	.i2c_bus_bx5b3a= 2,
	.i2c_bus_ts = 5,
	.i2c_bus_gps = 5,
	.i2c_bus_saf7741 = 5,
	.i2c_bus_lpc = 0,


	.ad_key = 
	{
		{   
			.ch = 35,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2500, KEY_HOME},
						}
		},
		
		{    
		    .ch = 37,
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
		{1,  80,  1401600,"1401600"},
		{81, 90,  1094400,"1094400"},
		{91, 100, 787200, "787200"},
		{101,105, 600000, "600000"},
		{105,500, 300000, "300000"},
		{0,0, 0, "0"},//end flag
	},
	
	.cpu_freq_list = "300000,384000,600000,787200,998400,1094400,1190400,1305600,1344000,1401600",
	.cpu_freq_temp_node = "/sys/class/thermal/thermal_zone5/temp",
	.cpu_freq_recovery_limit = "600000",

	.fly_parameter_node = "/dev/block/mmcblk0p25",
	
	.system_switch_en = 1,
	},

};

