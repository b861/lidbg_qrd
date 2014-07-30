//dts


#ifdef PLATFORM_msm8226
struct hw_version_specific g_hw_version_specific[] = 
{

{//v1
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
		{101,150, 600000, "600000"},
		{151,500, 300000, "300000"},
		{0,0, 0, "0"},//end flag
	},
	
	.cpu_freq_list = "300000,384000,600000,787200,998400,1094400,1190400,1305600,1344000,1401600",
	.cpu_freq_temp_node = "/sys/class/thermal/thermal_zone5/temp",
	.cpu_freq_recovery_limit = "600000",

	.fly_parameter_node = "/dev/block/mmcblk0p25",
	.system_switch_en = 1,

	},
{//v2
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
		{101,150, 600000, "600000"},
		{151,500, 300000, "300000"},
		{0,0, 0, "0"},//end flag
	},
	
	.cpu_freq_list = "300000,384000,600000,787200,998400,1094400,1190400,1305600,1344000,1401600",
	.cpu_freq_temp_node = "/sys/class/thermal/thermal_zone5/temp",
	.cpu_freq_recovery_limit = "600000",

	.fly_parameter_node = "/dev/block/mmcblk0p25",
	
	.system_switch_en = 1,
	},

};

#elif defined(PLATFORM_msm8974)
struct hw_version_specific g_hw_version_specific[] = 
{

{//v1
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
			.ch = 38,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2500, KEY_HOME},
						}
		},
		
		{    
		    .ch = 39,
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
{//v2
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
			.ch = 38,
			.offset = 100,
			.max = 3300,
			.key_item = {
						{2500, KEY_HOME},
						}
		},
		
		{    
		    .ch = 39,
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

#endif
#ifdef SOC_mt3360

struct hw_version_specific g_hw_version_specific[] =
{

{//v1
	.gpio_lcd_reset = -1,
	.gpio_t123_reset = -1,
	.gpio_dsi83_en = -1,

	.gpio_usb_id = -1,
	.gpio_usb_power = -1,
	.gpio_usb_switch = -1,

	.gpio_int_gps = -1,

	.gpio_int_button_left1 = -1,
	.gpio_int_button_left2 = -1,
	.gpio_int_button_right1 = -1,
	.gpio_int_button_right2 = -1,

	.gpio_led1 = -1,
	.gpio_led2 = -1,

	.gpio_int_mcu_i2c_request = -1,
	.gpio_mcu_wp = -1,
	.gpio_mcu_app = -1,

	.gpio_ts_int= -1,
	.gpio_ts_rst= -1,


	.i2c_bus_dsi83 = -1,
	.i2c_bus_bx5b3a= -1,
	.i2c_bus_ts = -1,
	.i2c_bus_gps = -1,
	.i2c_bus_saf7741 = -1,
	.i2c_bus_lpc = -1,


	.ap_key_left = -1,
	.ap_key_right = -1,

	.cpu_freq_thermal = 
	{
		{0,0, 0, "0"},//end flag
	},

	},
{//v2
	.gpio_lcd_reset = -1,
	.gpio_t123_reset = -1,
	.gpio_dsi83_en = -1,

	.gpio_usb_id = -1,
	.gpio_usb_power = -1,
	.gpio_usb_switch = -1,

	.gpio_int_gps = -1,

	.gpio_int_button_left1 = -1,
	.gpio_int_button_left2 = -1,
	.gpio_int_button_right1 = -1,
	.gpio_int_button_right2 = -1,

	.gpio_led1 = -1,
	.gpio_led2 = -1,

	.gpio_int_mcu_i2c_request = -1,
	.gpio_mcu_wp = -1,
	.gpio_mcu_app = -1,

	.gpio_ts_int= -1,
	.gpio_ts_rst= -1,


	.i2c_bus_dsi83 = -1,
	.i2c_bus_bx5b3a= -1,
	.i2c_bus_ts = -1,
	.i2c_bus_gps = -1,
	.i2c_bus_saf7741 = -1,
	.i2c_bus_lpc = -1,


	.ap_key_left = -1,
	.ap_key_right = -1,

	
	.cpu_freq_thermal = 
	{
		{0,0, 0, "0"},//end flag
	},
	},

};
#endif
EXPORT_SYMBOL(g_hw_version_specific);


