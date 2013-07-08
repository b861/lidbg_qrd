#include "lidbg.h"

struct task_struct *loader_task;


void launch_user( char bin_path[], char argv1[],char argv2[])
{
    char *argv[] = { bin_path, argv1, argv2, NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin", NULL };
    int ret;
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_EXEC);

  if (ret < 0)
        lidbg("lunch fail!\n");
    else
        lidbg("lunch  success!\n");

}



int thread_loader(void *data)
{
	DUMP_FUN_ENTER;
	
	launch_user("/system/bin/lidbg_servicer", NULL,NULL);
	launch_user("/flysystem/bin/lidbg_servicer", NULL,NULL);
	return 1;
	//msleep(100);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_share.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_msg.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_common.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_servicer.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_touch.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_key.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_i2c.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_soc_msm8x25.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_io.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_ad.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_main.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_fly_soc.ko",NULL);



	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_share.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_msg.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_common.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_servicer.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_touch.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_key.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_i2c.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_soc_msm8x25.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_io.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_ad.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_main.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_fly_soc.ko",NULL);



	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_fastboot.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_lpc.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_soc_devices.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_videoin.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_to_bpmsg.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_gps.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_ts_to_recov.ko",NULL);
	launch_user("/system/bin/insmod", "/system/lib/modules/out/lidbg_ts_probe.ko",NULL);

	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_fastboot.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_lpc.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_soc_devices.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_videoin.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_to_bpmsg.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_gps.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_ts_to_recov.ko",NULL);
	launch_user("/system/bin/insmod", "/flysystem/lib/out/lidbg_ts_probe.ko",NULL);



	msleep(1000);	
	launch_user("/system/bin/chmod", "0777", "/dev/lidbg_share");


	DUMP_FUN_LEAVE;



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
