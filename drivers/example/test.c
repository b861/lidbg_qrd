
#include "lidbg.h"


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
                buff[0] = 0x12;
                buff[1] = 0x34;

                SOC_I2C_Send(1, 0x88 >> 1, buff, 2);
                SOC_IO_Output(0, 27, 1);
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

    CREATE_KTHREAD(thread_test, NULL);

    return 0;

}
void lidbg_test_deinit(void)
{


}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_test_init);
module_exit(lidbg_test_deinit);

