
#include "lidbg_def.h"

#include "lidbg_enter.h"

LIDBG_DEFINE;



static struct task_struct *key_task;


int thread_key_xxx(void *data)
{

	
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
			
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



int lidbg_test_init(void)
{
    int err, times = 0;
    lidbg("lidbg_test_init.\n");
	LIDBG_GET;


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



