
#include "lidbg.h"

u8 soc_io_config_log[IO_LOG_NUM];

void soc_io_init(void)
{
    memset(soc_io_config_log, 0, IO_LOG_NUM);
}


void soc_irq_disable(unsigned int irq)
{
    disable_irq(irq);

}

void soc_irq_enable(unsigned int irq)
{
    enable_irq(irq);
}


#if 0
// for test
static irqreturn_t interrupt_isr(int irq, void *dev_id)
{

    soc_irq_enable(irq);
    lidbg("come into IRQ [%d]!\n", irq);

    return 1;
}
#endif


int soc_io_irq(struct io_int_config *pio_int_config)//need set to input first?
{


    if (request_irq(pio_int_config->ext_int_num, pio_int_config->pisr, pio_int_config->irqflags /*IRQF_ONESHOT |*//*IRQF_DISABLED*/, "lidbg_irq", pio_int_config->dev ))
    {
        lidbg("request_irq err!\n");
        return 0;
    }
    return 1;
}

int soc_io_config(u32 index, bool direction, u32 pull, u32 drive_strength, bool force_reconfig)
{

    if(direction == GPIO_CFG_OUTPUT)
        MDrv_GPIO_Pad_Oen(index);
    else if(direction == GPIO_CFG_INPUT)
        MDrv_GPIO_Pad_Odn(index);
    else
        printk("Gpio direction is not supported!\n");

    if(pull == GPIO_CFG_PULL_UP)
        MDrv_GPIO_Pull_High(index);
    else if(pull == GPIO_CFG_PULL_DOWN)
        MDrv_GPIO_Pull_Low(index);
    else
        printk("Gpio-%d no pull !\n", index);

#if 0
    int rc;

    if(force_reconfig)
    {
        rc = gpio_tlmm_config(GPIO_CFG(index, 0,
                                       direction, pull,
                                       drive_strength), GPIO_CFG_ENABLE);
        if (rc)
        {
            lidbg("%s: gpio_tlmm_config for %d failed\n",
                  __func__, index);
            return 0;
        }
    }

    if(soc_io_config_log[index] == 1)
    {
        return 1;
    }
    else
    {
        int err;

        if (!gpio_is_valid(index))
            return 0;


        lidbg("gpio_request:index %d\n" , index);

#ifndef CONFIG_IO_EVERY_TIMES
        if(!force_reconfig)
        {
            rc = gpio_tlmm_config(GPIO_CFG(index, 0,
                                           direction, pull,
                                           drive_strength), GPIO_CFG_ENABLE);
            if (rc)
            {
                lidbg("%s: gpio_tlmm_config for %d failed\n",
                      __func__, index);
                return 0;
            }
        }
#endif

        err = gpio_request(index, "lidbg_io");
        if (err)
        {
            lidbg("\n\nerr: gpio request failed!!!!!!\n\n\n");
        }

        if(direction == GPIO_CFG_INPUT)
            err = gpio_direction_input(index);
        else
            err = gpio_direction_output(index, 1);

        if (err)
        {
            lidbg("gpio_direction_set failed\n");
            goto free_gpio;
        }
        soc_io_config_log[index] = 1;

        return 1;

free_gpio:
        if (gpio_is_valid(index))
            gpio_free(index);
        return 0;
    }
#endif
    return 1;
}


int soc_io_output(u32 group, u32 index, bool status)
{

    MDrv_GPIO_Pad_Set(index);

    if(status == 1)
        MDrv_GPIO_Set_High(index);
    else
        MDrv_GPIO_Set_Low(index);

    return 1;
}

bool soc_io_input( u32 index)
{
    return MDrv_GPIO_Pad_Read(index);
}


EXPORT_SYMBOL(soc_io_output);
EXPORT_SYMBOL(soc_io_input);
EXPORT_SYMBOL(soc_io_irq);
EXPORT_SYMBOL(soc_irq_enable);
EXPORT_SYMBOL(soc_irq_disable);
EXPORT_SYMBOL(soc_io_config);
