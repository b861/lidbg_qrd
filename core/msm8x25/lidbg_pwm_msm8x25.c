
#include "lidbg.h"


unsigned int   soc_pwm_set(int pwm_id, int duty_ns, int period_ns)
{
    return 1;
}

void soc_bl_init(void)
{
	pmapp_disp_backlight_init();
}

unsigned int   soc_bl_set(u32 bl_level)
{
    if(1)//flycar
    {
#ifdef BOARD_V3
	int err;

	err = pmapp_disp_backlight_set_brightness(bl_level);
	if(err)
	{
	     printk("Backlight set brightness failed !\n");
	     return 0;
	}
#else
        if (p_fly_smem == NULL)
        {
            printk( "p_fly_smem == NULL\n");
            return 0;
        }
        p_fly_smem->bl_value = (int)bl_level;
#endif
    }
    else//sku7
    {
        int step = 0, i = 0;
        unsigned long flags;
        static int prev_bl = 17;
        lidbg("SOC_BL_Set %d\n", bl_level);
        unsigned int  gpio_backlight_en = 89;
        //spin_lock_irqsave(&lcdc_ips3p2335_spin_lock, flags); //disable local irq and preemption
        /* real backlight level, 1 - max, 16 - min, 17 - off */
        bl_level = 17 - bl_level;

        if (bl_level > prev_bl)
        {
            step = bl_level - prev_bl;
            if (bl_level == 17)
                step--;
        }
        else if (bl_level < prev_bl)
        {
            step = bl_level + 16 - prev_bl;
        }
        else
        {
            lidbg("%s: no change\n", __func__);
            //spin_unlock_irqrestore(&lcdc_ips3p2335_spin_lock, flags);
            return 0;
        }

        if (bl_level == 17)
        {
            /* turn off backlight */
            gpio_set_value(gpio_backlight_en, 0);
        }
        else
        {
            if (prev_bl == 17)
            {
                /* turn on backlight */
                gpio_set_value(gpio_backlight_en, 1);
                udelay(30);
            }

            /* adjust backlight level */
            for (i = 0; i < step; i++)
            {
                gpio_set_value(gpio_backlight_en, 0);
                udelay(1);
                gpio_set_value(gpio_backlight_en, 1);
                udelay(1);
            }
        }
        prev_bl = bl_level;

    }
    return 1;
}

#ifndef _LIGDBG_SHARE__
EXPORT_SYMBOL(soc_bl_set);
EXPORT_SYMBOL(soc_pwm_set);
#endif

