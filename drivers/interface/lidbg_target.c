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

	
	.ap_key_left = 35,
	.ap_key_right = 37,
	
	.thermal_ctrl = 
	{
		{1,  80,  1401600,"1401600"},
		{81, 90,  1094400,"1094400"},
		{91, 100, 787200, "787200"},
		{101,150, 600000, "600000"},
		{151,500, 300000, "300000"},
		{0,0, 0, "0"},//end flag
	},

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


	.ap_key_left = 35,
	.ap_key_right = 37,

	.thermal_ctrl = 
	{
		{1,  80,  1401600,"1401600"},
		{81, 90,  1094400,"1094400"},
		{91, 100, 787200, "787200"},
		{101,150, 600000, "600000"},
		{151,500, 300000, "300000"},
		{0,0, 0, "0"},//end flag
	},
	
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

	
	.ap_key_left = 38,
	.ap_key_right = 39,

	.thermal_ctrl = 
	{
		{0,0, 0, "0"},//end flag
	},

	},
{//v2
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
	.gpio_ts_rst= 12,
	
	
	.i2c_bus_dsi83 = 3,
	.i2c_bus_bx5b3a= 3,
	.i2c_bus_ts = 2,
	.i2c_bus_gps = 2,
	.i2c_bus_saf7741 = 2,
	.i2c_bus_lpc = 0,
	
	
	.ap_key_left = 38,
	.ap_key_right = 39,

	
	.thermal_ctrl = 
	{
		{0,0, 0, "0"},//end flag
	},
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

	.thermal_ctrl = 
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

	
	.thermal_ctrl = 
	{
		{0,0, 0, "0"},//end flag
	},
	},

};
#endif
EXPORT_SYMBOL(g_hw_version_specific);


