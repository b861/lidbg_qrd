
#include "lidbg.h"
extern struct backlight_device *bd;
extern U32 cedric_backlight_set_intensity(struct backlight_device *bd);

unsigned int soc_pwm_set(int pwm_id, int duty_ns, int period_ns)
{
    return 1;
}

void soc_bl_init(void)
{

}

unsigned int   soc_bl_set(u32 bl_level)
{
	int ret = 0;
	
	(*bd).props.brightness = bl_level;
	ret = cedric_backlight_set_intensity(bd);
	if(ret) {
		printk("[BL]: Set backlight failed !\n");
		return 0;
	}
	else
		return 1;
}

EXPORT_SYMBOL(soc_bl_set);
EXPORT_SYMBOL(soc_pwm_set);

