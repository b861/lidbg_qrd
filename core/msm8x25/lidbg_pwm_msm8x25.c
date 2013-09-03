
#include "lidbg.h"


unsigned int soc_pwm_set(int pwm_id, int duty_ns, int period_ns)
{
    return 1;
}

void soc_bl_init(void)
{
#if (defined(BOARD_V1) || defined(BOARD_V2))

#else
	pmapp_disp_backlight_init();
#endif

}

unsigned int   soc_bl_set(u32 bl_level)
{
#if (defined(BOARD_V1) || defined(BOARD_V2))
	if (p_fly_smem == NULL)
	{
		printk( "p_fly_smem == NULL\n");
		return 0;
	}
#if 1
	p_fly_smem->bl_value = (int)bl_level;
#else
	p_fly_smem->ap2bp[0] = (int)bl_level;
#endif

#else
	int err;
	err = pmapp_disp_backlight_set_brightness(bl_level);
	if(err)
	{
	     printk("Backlight set brightness failed !\n");
	     return 0;
	}

#endif
    return 1;
}

EXPORT_SYMBOL(soc_bl_set);
EXPORT_SYMBOL(soc_pwm_set);

