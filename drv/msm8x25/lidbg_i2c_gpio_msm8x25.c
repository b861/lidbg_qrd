
#include "lidbg.h"
void soc_i2c_gpio_config(struct platform_device *pdev)
 {
    struct i2c_gpio_platform_data *pdata;
	 pdata = pdev->dev.platform_data; 

	 printk("<++++++++ i2c-gpio config ++++++++>\n");

	 gpio_tlmm_config(GPIO_CFG(pdata->sda_pin, 0, (GPIO_CFG_OUTPUT | GPIO_CFG_INPUT), GPIO_CFG_PULL_UP, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	 gpio_tlmm_config(GPIO_CFG(pdata->scl_pin, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
	 
	 printk("<-------- i2c-gpio config -------->\n");
 }

#ifndef _LIGDBG_SHARE__
EXPORT_SYMBOL(soc_i2c_gpio_config);
#endif

