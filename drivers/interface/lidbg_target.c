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
	.i2c_bus_ts = 5,
	.i2c_bus_gps = 5,
	.i2c_bus_saf7741 = 5,
	.i2c_bus_lpc = 0,

	
	.ap_key_left = 35,
	.ap_key_right = 37,
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
	.i2c_bus_ts = 5,
	.i2c_bus_gps = 5,
	.i2c_bus_saf7741 = 5,
	.i2c_bus_lpc = 0,


	.ap_key_left = 35,
	.ap_key_right = 37,
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

	.gpio_int_button_left1 = 31,
	.gpio_int_button_left2 = 32,
	.gpio_int_button_right1 = 33,
	.gpio_int_button_right2 = 34,

	.gpio_led1 = 60,
	.gpio_led2 = -1,

	.gpio_int_mcu_i2c_request = 108,
	.gpio_mcu_wp = 35,
	.gpio_mcu_app = 36,

	.gpio_ts_int= 14,
	.gpio_ts_rst= 13,

	
	.i2c_bus_dsi83 = 3,
	.i2c_bus_ts = 2,
	.i2c_bus_gps = 2,
	.i2c_bus_saf7741 = 2,
	.i2c_bus_lpc = 0,

	
	.ap_key_left = 35,
	.ap_key_right = 37,

	},
{//v2
	.gpio_lcd_reset = 25,
	.gpio_t123_reset = 28,
	.gpio_dsi83_en = 58,
	
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
	
	.gpio_ts_int= 14,
	.gpio_ts_rst= 13,
	
	
	.i2c_bus_dsi83 = 3,
	.i2c_bus_ts = 2,
	.i2c_bus_gps = 2,
	.i2c_bus_saf7741 = 2,
	.i2c_bus_lpc = 0,
	
	
	.ap_key_left = 35,
	.ap_key_right = 37,
	},

};

#endif
EXPORT_SYMBOL(g_hw_version_specific);


