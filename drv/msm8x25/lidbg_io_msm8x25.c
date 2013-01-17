


#include "lidbg.h"


u8 soc_io_config_log[IO_LOG_NUM];

//unsigned long flags;
//spinlock_t gpio_log_lock; /* global dsp lock */


void soc_io_init(void)
{
    //spin_lock_init(&gpio_log_lock);
    memset(soc_io_config_log, 0, IO_LOG_NUM);

}



/*

int soc_gpio_log(struct io_config *pio_config)
{
    int i;
    bool ret = 0;

	int rc;

	lidbg("soc_gpio_log+!\n");

	   rc = gpio_tlmm_config(GPIO_CFG(pio_config->index, 0,
			   pio_config->direction, pio_config->pull,
			   pio_config->drive_strength), pio_config->disable);
	   if (rc)
	   	{
		   lidbg("%s: gpio_tlmm_config for %d failed\n",
			   __func__, pio_config->index);
	   		ret = 0;
	   		return ret;
	   	}


       soc_io_request_log[pio_config->] = 1;

       ret = 1;

    lidbg("soc_gpio_log-!\n");

    return ret;
}

int soc_gpio_find_log( u32 index) // 1--log
{

    return soc_io_request_log[index];

}
*/

//static int soc_irq_set_type(unsigned int irq, unsigned int type)
//{

//    return 0;
//}


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
    //soc_irq_enable(pio_int_config->ext_int_num);

    return 1;
}


/*
int soc_io_write(struct io_config *pio_config)
{


#if 0
    lidbg("\nsoc_io_write:");
    lidbg("group %d;", pio_config->group);
    lidbg("index %d;", pio_config->index);
    lidbg("status %d;\n\n", pio_config->status);
#endif

soc_io_output(pio_config->index, pio_config->status);

return 1;





}


int soc_io_read(struct io_config *pio_config)
{


		return soc_io_input(pio_config->index);

}

*/
int soc_io_config(u32 index, bool direction, u32 pull, u32 drive_strength, bool force_reconfig)
{
    int rc;

    //avoid bp change io config,just for debug
    //#define CONFIG_IO_EVERY_TIMES
    //#ifdef CONFIG_IO_EVERY_TIMES

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
    //#endif



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
            //goto free_gpio;
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


}



int soc_io_output(u32 index, bool status)
{

    //gpio_set_value_cansleep(index, status);
    gpio_set_value(index, status);

    return 1;

}

bool soc_io_input( u32 index)
{

    return gpio_get_value(index);

}

#ifndef _LIGDBG_SHARE__

EXPORT_SYMBOL(soc_io_output);
EXPORT_SYMBOL(soc_io_input);

EXPORT_SYMBOL(soc_io_irq);
EXPORT_SYMBOL(soc_irq_enable);
EXPORT_SYMBOL(soc_irq_disable);
EXPORT_SYMBOL(soc_io_config);
#endif



