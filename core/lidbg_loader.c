
#define NOT_USE_MEM_LOG

#include "lidbg.h"

int load_modules_count = 0;

char *insmod_list[] =
{
    SOC_KO,
    "lidbg_mem_log.ko",
    "lidbg_common.ko",
    "lidbg_fileserver.ko",
    "lidbg_trace_msg.ko",
    "lidbg_wakelock_stat.ko",
    "lidbg_msg.ko",
    "lidbg_servicer.ko",
    "lidbg_touch.ko",
    "lidbg_key.ko",
    "lidbg_i2c.ko",
    "lidbg_io.ko",
    "lidbg_ad.ko",
    "lidbg_uart.ko",
    "lidbg_main.ko",
    "lidbg_misc.ko",
    HAL_KO,
    NULL,
};

char *insmod_path[] =
{
    "/sdcard/out/",
    "/system/lib/modules/out/",
    "/flysystem/lib/out/",
    NULL,
};
bool is_file_exist(char *file)
{
    struct file *filep;
    filep = filp_open(file, O_RDONLY , 0);
    if(IS_ERR(filep))
        return false;
    else
    {
        filp_close(filep, 0);
        return true;
    }
}
void launch_user( char bin_path[], char argv1[], char argv2[])
{
    char *argv[] = { bin_path, argv1, argv2, NULL };
    static char *envp[] = { "HOME=/", "TERM=linux", "PATH=/system/bin:/sbin", NULL };
    int ret;
    ret = call_usermodehelper(bin_path, argv, envp, UMH_WAIT_PROC);
}

int thread_check_restart(void *data)
{
    DUMP_FUN_ENTER;
    msleep(5000);
    lidbg("load_modules_count=%d\n", load_modules_count);
    if(load_modules_count == 0)
    {
        lidbg("load_modules_count err,call kernel_restart!\n");
        kernel_restart(NULL);
    }
    DUMP_FUN_LEAVE;
    return 0;
}

int thread_loader(void *data)
{
    int i, j;
    char path[100];
    DUMP_FUN_ENTER;
    CREATE_KTHREAD(thread_check_restart, NULL);

    for(i = 0; insmod_path[i] != NULL; i++)
    {
        for(j = 0; insmod_list[j] != NULL; j++)
        {
            sprintf(path, "%s%s", insmod_path[i], insmod_list[j]);
            //lidbg("load %s\n",path);
            	if( is_file_exist(RECOVERY_MODE_DIR))
            	launch_user("/sbin/insmod", path , NULL);
				else
				launch_user("/system/bin/insmod", path , NULL);
        }
    }

    //launch_user("/system/bin/chmod", "0777", "/dev/mlidbg0");
    DUMP_FUN_LEAVE;
    return 0;

}

int __init loader_init(void)
{
    DUMP_BUILD_TIME;
    CREATE_KTHREAD(thread_loader, NULL);
    return 0;
}

void __exit loader_exit(void)
{}

module_init(loader_init);
module_exit(loader_exit);

EXPORT_SYMBOL(load_modules_count);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.");
