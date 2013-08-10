
#include "lidbg_def.h"
#include "lidbg_enter.h"

static struct task_struct *test_task;

LIDBG_DEFINE;

void test_func1(int test_enable)
{
    	char buff[2];
	if(!test_enable)
		return ;
	buff[0] = 0x12;
	buff[1] = 0x34;
	SOC_I2C_Send(1, 0x88 >> 1, buff, 2);
	SOC_IO_Output(0, 27, 1);
	lidbg(".");
	msleep(10);
}

#define LIDBG_KILL_LIST_PATH "/flysystem/lib/out/fastboot_not_kill_list.conf"
LIST_HEAD(kill_list_test);
void test_fileserver_stability(int test_enable)
{
    char *delay;
    printk("[futengfei].%s========%s\n", (test_enable ? "on" : "off"), __func__);
    if(!test_enable)
        return ;
    //test_fileserver_stability
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_SHOW, NULL, NULL,NULL);
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_IS_STRINFILE, "cn.flyaudio.navigation", NULL,NULL);
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_IS_STRINFILE, "cn.flyaudio.navigationfutengfei", NULL,NULL);
    fileserver_deal_cmd(&lidbg_config_list, FS_CMD_LIST_SHOW, NULL, NULL,NULL);
    fileserver_deal_cmd(&kill_list_test, FS_CMD_LIST_SHOW, NULL, NULL,NULL);
    fileserver_deal_cmd(&lidbg_config_list, FS_CMD_LIST_SHOW, NULL, NULL,NULL);
    fileserver_deal_cmd(&lidbg_config_list, FS_CMD_LIST_GETVALUE, NULL, "futengfei",&delay);
    printk("[futengfei]get key value:[%d]\n",simple_strtoul(delay, 0, 0) );
    fileserver_deal_cmd(&lidbg_config_list, FS_CMD_LIST_GETVALUE, NULL, "mayanping",&delay);
    printk("[futengfei]get key value:[%d]\n",simple_strtoul(delay, 0, 0) );
    //append a string to a file to save some state;
    fileserver_main(NULL, FS_CMD_FILE_APPENDMODE, "\n###save some state###\n ts=gt801\n", NULL);
    ssleep(3);

}
int thread_test(void *data)
{
    //init it
    fileserver_main(LIDBG_KILL_LIST_PATH, FS_CMD_FILE_LISTMODE, NULL, &kill_list_test);	
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {

            while(1)
            {
				test_func1(0);
				test_fileserver_stability(1);

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

