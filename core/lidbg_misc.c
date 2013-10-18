#include "lidbg.h"

static struct task_struct *udisk_update_task;
static struct completion udisk_update_wait;


void cb_password_chmod(char *password )
{
    lidbg_chmod("/system/bin/mount");
    lidbg_mount("/system");

    lidbg_chmod("/data");
}
void cb_password_upload(char *password )
{
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_upload_machine_log();
}
void cb_password_call_apk(char *password )
{
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_call_apk();
}
void cb_password_remove_apk(char *password )
{
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_remove_apk();
}
void cb_password_clean_all(char *password )
{
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_clean_all();
}
void cb_password_update(char *password )
{
    if(fs_is_file_exist("/mnt/usbdisk/out/release"))
    {
        int ret = -1;
        TE_WARN("<===============UPDATE_INFO =================>\n" );
        ret = fs_update("/mnt/usbdisk/out/release", "/mnt/usbdisk/out", "/flysystem/lib/out");
        fs_file_log("<called:%s.%d>\n", __func__ , ret); //tmp,del later

        if( ret >= 0)
            lidbg_reboot();
    }
    else
    {
        TE_ERR("<up>\n" );
    }
}

void cb_password_kmsg(char *password )
{
    if(lidbg_exe("/flysystem/lib/out/lidbg_kmsg") < 0)
        TE_ERR("Exe lidbg_kmsg failed !\n");
}
static int thread_udisk_update(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    while(!kthread_should_stop())
    {
        if(!wait_for_completion_interruptible(&udisk_update_wait))
        {
            ssleep(4);
            cb_password_update("000000");
        }
    }
    return 1;
}
static int usb_nc_update(struct notifier_block *nb, unsigned long action, void *data)
{
    switch (action)
    {
    case USB_DEVICE_ADD:
        complete(&udisk_update_wait);
        break;
    case USB_DEVICE_REMOVE:
        break;
    }
    return NOTIFY_OK;
}

static struct notifier_block usb_nb_update =
{
    .notifier_call = usb_nc_update,
};

static int __init lidbg_misc_init(void)
{
    TE_WARN("<==IN==>\n");

    init_completion(&udisk_update_wait);
    usb_register_notify(&usb_nb_update);

    te_regist_password("001100", cb_password_remove_apk);
    te_regist_password("001101", cb_password_upload);
    te_regist_password("001102", cb_password_call_apk);
    te_regist_password("001110", cb_password_clean_all);
    te_regist_password("001111", cb_password_chmod);
    te_regist_password("001112", cb_password_update);
    te_regist_password("010000", cb_password_kmsg);

    udisk_update_task = kthread_run(thread_udisk_update, NULL, "ftf_te_update");

    TE_WARN("<==OUT==>\n\n");
    return 0;
}

static void __exit lidbg_misc_exit(void)
{
}

module_init(lidbg_misc_init);
module_exit(lidbg_misc_exit);

MODULE_AUTHOR("futengfei");
MODULE_DESCRIPTION("misc zone");
MODULE_LICENSE("GPL");

