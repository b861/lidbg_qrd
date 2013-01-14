
#include "lidbg_def.h"

#include "lidbg_enter.h"
#include "devices.h"






#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/memory.h>
#include <linux/memory_hotplug.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/oom.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/sysfs.h>

static struct task_struct *key_task;

LIDBG_DEFINE;

static int soc_task_kill(char *task_name)
{
	struct task_struct *p;
	struct task_struct *selected = NULL;
	DUMP_FUN_ENTER;

	//read_lock(&tasklist_lock);
	for_each_process(p) {
		struct mm_struct *mm;
		struct signal_struct *sig;

		task_lock(p);
		mm = p->mm;
		sig = p->signal;
		task_unlock(p);
		
		selected = p;
		printk( "process %d (%s)\n",
			     p->pid, p->comm);

		if(!strcmp(p->comm, task_name))
		{
			lidbg("find %s to kill\n",task_name);
			
			if (selected) {
				force_sig(SIGKILL, selected);
			break;
		}

	}
	//read_unlock(&tasklist_lock);
		}
	DUMP_FUN_LEAVE;
	return 1;
}







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
            
                soc_task_kill("mediaserver");
                USB_WORK_ENABLE;
                msleep(30000);

				
				SOC_Write_Servicer(UMOUNT_USB);
				msleep(5000);
                USB_WORK_DISENABLE;
				msleep(30000);

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
    lidbg("lidbg_test_init.\n");
    LIDBG_GET;


    key_task = kthread_create(thread_key_xxx, NULL, "key_task");
    if(IS_ERR(key_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(key_task);
    return 0;

}
void lidbg_test_deinit(void)
{


}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_test_init);
module_exit(lidbg_test_deinit);



