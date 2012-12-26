/*======================================================================*/

#include "lidbg.h"


static struct task_struct *pwr_up_task;


int thread_pwr_up(void *data)
{
    lidbg("thread_pwr_up!\n");



    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {

            msleep(1000);
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}

void lidbg_soc_init(void)
{
    lidbg("lidbg_soc_init!\n");


}

void lidbg_soc_deinit(void)
{
    lidbg("lidbg_soc_deinit!\n");


}

void lidbg_board_init(void)
{
    int err;
    lidbg("lidbg_board_init!\n");



#ifdef DEBUG_POWER_UP
    pwr_up_task = kthread_create(thread_pwr_up, NULL, "pwr_up_task");
    if(IS_ERR(pwr_up_task))
    {
        lidbg("Unable to start kernel thread.\n");
    }else  wake_up_process(pwr_up_task);
#endif




}



void lidbg_board_deinit(void)
{
    lidbg("lidbg_board_deinit!\n");


#ifdef DEBUG_POWER_KEY_UP
    if(pwr_up_task)
    {
        kthread_stop(pwr_up_task);
        pwr_up_task = NULL;
    }
#endif
}


EXPORT_SYMBOL(lidbg_soc_init);
EXPORT_SYMBOL(lidbg_soc_deinit);
EXPORT_SYMBOL(lidbg_board_init);
EXPORT_SYMBOL(lidbg_board_deinit);

