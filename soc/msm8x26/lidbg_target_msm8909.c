//dts

struct hw_version_specific g_hw_version_specific[] = 
{
	{//msm8909 v1
		.gpio_lcd_reset = -1,
		.gpio_t123_reset = -1,
		.gpio_dsi83_en = 8,

		.gpio_usb_id = 27,
		.gpio_usb_power = -1,
		.gpio_usb_switch = -1,
	
		.gpio_int_gps = 96,
	
		.gpio_int_button_left1 = 25,
		.gpio_int_button_left2 = 26,
		.gpio_int_button_right1 = 23,
		.gpio_int_button_right2 = 24,
	
		.gpio_led1 = 0,
		.gpio_led2 = 1,
	
		.gpio_int_mcu_i2c_request = 94,
		.gpio_mcu_wp = 33,
		.gpio_mcu_app = 34,
	
		.gpio_ts_int = 13,
		.gpio_ts_rst = 12,

		.gpio_dvd_tx = 20,
		.gpio_dvd_rx = 21,

		.gpio_bt_tx = 4,
		.gpio_bt_rx = 5,
	
		.i2c_bus_dsi83 = 1,
		.i2c_bus_ts = 5,
		.i2c_bus_gps = 5,
		.i2c_bus_tef6638 = 5,
		.i2c_bus_lpc = 4,

		.fly_parameter_node = "/dev/block/platform/msm_sdcc.1/by-name/flyparameter",
	},

};

