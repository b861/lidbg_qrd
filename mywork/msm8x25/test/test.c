
#include "lidbg_enter.h"







static struct task_struct *key_task;



void key_scan(void)
{

    // int val,i=500;
#if 1

    //SOC_ADC_Get(8, &val);
    //SOC_ADC_Get(9, &val);
    //SOC_ADC_Get(14, &val);
    //lidbg(".\n");
    while(1);
    udelay(5);

#endif

}

LIDBG_DEFINE;

int thread_key_xxx(void *data)
{

	
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            //key_scan();
			
    	LIDBG_GET;

			
            while(1)
            {
			msleep(3000);
			SOC_IO_Output(0, 33, 1);
			msleep(3000);
			SOC_IO_Output(0, 33, 0);

            }
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


irqreturn_t io_test_irq(int irq, void *dev_id)
{

    lidbg("io_test_irq: %d \n", irq);
    return IRQ_HANDLED;

}


int lidbg_test_init(void)
{
    int err, times = 0;
    //int i;
    //u8 data;
    //mipi_init();

    //SOC_IO_Input(76, 76, 3);
    //SOC_IO_ISR_Add(76, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_ONESHOT, io_test_irq, NULL);
    //return 0;
    //msleep(5000);

    lidbg("lidbg_test_init.\n");
    //SOC_Log_Dump(LOG_DVD_RESET);
    // while(1)
    //	udelay(5);

    key_task = kthread_create(thread_key_xxx, NULL, "key_task");
    if(IS_ERR(key_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }else wake_up_process(key_task);
    return 0;

}
void lidbg_test_deinit(void)
{


}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_test_init);
module_exit(lidbg_test_deinit);



