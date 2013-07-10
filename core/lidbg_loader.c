#include "lidbg.h"

struct task_struct *loader_task;

char *insmod_list[] =
{
    "lidbg_share.ko",
    "lidbg_msg.ko",
	"lidbg_common.ko",
	"lidbg_servicer.ko",
	"lidbg_touch.ko",
	"lidbg_key.ko",
	"lidbg_i2c.ko",
	"lidbg_soc_msm8x25.ko",
	"lidbg_io.ko",
	"lidbg_ad.ko",
	"lidbg_main.ko",
	"lidbg_fly_soc.ko",
	NULL,

};

char *insmod_path[] =
{
    "/system/lib/modules/out/",
    "/flysystem/lib/out/",
    NULL,
};

void launch_user( char bin_path[], char argv1[],char argv2[])
{
    char *argv[] = { bin_path, argv1, argv2, NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin", NULL };
    int ret;
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_EXEC);

  if (ret < 0)
        lidbg("lunch fail!\n");
    else
        lidbg("lunch success!\n");

}



int thread_loader(void *data)
{
	int i,j;
	char path[100];
	DUMP_FUN_ENTER;

	//return 0;
	//msleep(100);
	for(i=0;insmod_path[i]!=NULL;i++)	
	{
		for(j=0;insmod_list[j]!=NULL;j++)
		{
			sprintf(path, "%s%s", insmod_path[i],insmod_list[j]);
			lidbg("load %s\n",path);
			launch_user("/system/bin/insmod", path ,NULL);
			msleep(100);
		}
	}
	
	msleep(1000);	
	launch_user("/system/bin/chmod", "0777", "/dev/lidbg_share");
	launch_user("/system/bin/chmod", "0777", "/dev/mlidbg0");

	DUMP_FUN_LEAVE;
	return 0;

}


int __init loader_init(void)
{
    int ret;
    DUMP_BUILD_TIME;
    loader_task = kthread_create(thread_loader, NULL, "lidbg_loader");
    if(IS_ERR(loader_task))
    {
        lidbg("Unable to start thread.\n");

    }
    else wake_up_process(loader_task);

    return 0;
}

void __exit loader_exit(void)
{

}

module_init(loader_init);
module_exit(loader_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");
