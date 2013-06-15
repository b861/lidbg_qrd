
#include "lidbg_def.h"
#include "lidbg_enter.h"

static struct task_struct *test_task;

LIDBG_DEFINE;

int thread_test(void *data)
{
    char buff[2];
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {

            while(1)
            {
            	buff[0]=0x12;
            	buff[1]=0x34;

		SOC_I2C_Send(1,0x88>>1, buff, 2);
		SOC_IO_Output(0,27,1);		
		lidbg(".");
            	msleep(10);
				

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
    DUMP_BUILD_TIME;
    LIDBG_GET;


    test_task = kthread_create(thread_test, NULL, "test_task");
    if(IS_ERR(test_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(test_task);
    return 0;

}
void lidbg_test_deinit(void)
{


}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_test_init);
module_exit(lidbg_test_deinit);

