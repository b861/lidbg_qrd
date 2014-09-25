
#include "lidbg.h"
#include "pwm.h"

#define PWM_DBG
#define BACKIGHT_PWM_ID 1
static struct pwm_device *pwm = NULL;
static pwm_config_code pwmConfig = {0};

unsigned int soc_pwm_set(int pwm_id, int duty_ns, int period_ns)
{
    return 1;
}

void soc_bl_init(void)
{
	int ret =0;
/*
* PWM Frequency: f = BCLK/((PWMP+1)*(PWMRSN+1))
* Duty Ratoin: High/(High+Low) = ((PWMH+1)/(PWMRSN+1))*100%
*/
	pwmConfig.clk_id = CLKSRC_27M;
	pwmConfig.pin_id = 1;

	pwmConfig.pwm_en = 1;
	pwmConfig.pwm_mode = NORMAL_TRI_MODE;
	pwmConfig.pwm_high = 599;
	pwmConfig.pwm_rsn = 999;
	pwmConfig.pwm_prescale =8;

	pwm = pwm_request(BACKIGHT_PWM_ID, "backlight");
	if(!pwm)
		lidbg("Request pwm failed !\n");

	memcpy((void *)&(pwm->pwm_cfg_code), (void *)&pwmConfig, sizeof(pwm_config_code));

	ret = pwm_config(pwm, 0, 0);//duty_ns, period_ns are not used
	if(!ret)
		lidbg("Config pwm failed !\n");
//	pwm_enable(pwm);
}

/* bl_level: 1 ~ 100 */
unsigned int   soc_bl_set(u32 bl_level)
{
	bool ret = 0;
	lidbg("<****** soc_bl_set bl_level = %d ******>\n", bl_level);

	ret = pwm_set_intensity(pwm, bl_level);
	if(!ret)
		lidbg("Set bl level %d failed !\n", bl_level);

	ret = pwm_mode_switch(pwm, pwm->pwm_cfg_code.pwm_mode);
	if (!ret)
	 printk("Set mode failed !\n");

#ifdef PWM_DBG
	pwm_dump(BACKIGHT_PWM_ID);
#endif
	return 0;
}

EXPORT_SYMBOL(soc_bl_set);
EXPORT_SYMBOL(soc_pwm_set);

