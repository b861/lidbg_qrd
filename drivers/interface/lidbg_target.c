#ifdef SOC_msm8x26

struct hw_version_specific g_hw_version_specific[] = 
{

{//v1
	.gpio_lcd_reset = 25,
	.gpio_t123_reset = 28,
	
	.gpio_usb_id = -1,
	.gpio_usb_power = 109,
	.gpio_usb_switch = -1,
	},
{//v2
	.gpio_lcd_reset = -1,
	.gpio_t123_reset = -1,
	
	.gpio_usb_id = 23,
	.gpio_usb_power = 28,
	.gpio_usb_switch = 109,
	},

};
#endif
EXPORT_SYMBOL(g_hw_version_specific);


