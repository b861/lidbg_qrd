
#include "lidbg.h"
#define PATH_FLY_HW_INFO_CONFIG USB_MOUNT_POINT"/machine_info.conf"
#define PATH_FLY_HW_INFO_CONFIG2 USB_MOUNT_POINT"/conf/machine_info.conf"

LIDBG_DEFINE;

static fly_hw_data *g_fly_hw_data = NULL;
static struct completion usb_flyparameter_wait;
recovery_meg_t *g_recovery_meg = NULL;
char *p_kmem = NULL;

void fly_hw_info_show(char *when, fly_hw_data *p_info)
{
    lidbg("flyparameter:--------%s:g_fly_hw_data:flag=%x,hw=%d,ts=%d,%d,lcd=%d\n", when,
          p_info->flag,
          p_info->hw_info.hw_version,
          p_info->hw_info.ts_type,
          p_info->hw_info.ts_config,
          p_info->hw_info.lcd_type);
}

bool fly_hw_info_get(fly_hw_data *p_info)
{
    if(p_info && fs_file_read(FLYPARAMETER_NODE, (char *)p_info, MEM_SIZE_512_KB , sizeof(fly_hw_data)) >= 0)
    {
        fly_hw_info_show("fly_hw_info_get", p_info);
        return true;
    }
    return false;
}

bool fly_hw_info_save(fly_hw_data *p_info)
{
    if( p_info && fs_file_write(FLYPARAMETER_NODE, false, (void *) p_info, MEM_SIZE_512_KB , sizeof(fly_hw_data)) >= 0)
    {
        lidbg("fly_hw_data:save success\n");
        return true;
    }
    lidbg("fly_hw_data:save err\n");
    return false;
}


bool flyparameter_info_get(void)
{
    if(p_kmem && fs_file_read(FLYPARAMETER_NODE, p_kmem, 0, sizeof(recovery_meg_t)) >= 0)
    {
        g_recovery_meg = (recovery_meg_t *)p_kmem;
        lidbg("flyparameter1:%s,%s,%s\n", g_recovery_meg->recoveryLanguage.flags, g_recovery_meg->bootParam.bootParamsLen.flags, g_recovery_meg->bootParam.upName.flags);
        lidbg("flyparameter2:%d,%s,%x\n", g_recovery_meg->bootParam.upName.val, g_recovery_meg->bootParam.autoUp.flags, g_recovery_meg->bootParam.autoUp.val);
        return true;
    }
    return false;
}
bool flyparameter_info_save(recovery_meg_t *p_info)
{
    if( p_info && fs_file_write(FLYPARAMETER_NODE, false, (void *) p_info, 0, sizeof(recovery_meg_t)) >= 0)
    {
        lidbg("flyparameter:save success\n");
        return true;
    }
    lidbg("flyparameter:save err\n");
    return false;
}

static void read_fly_hw_config_file(fly_hw_data *p_info)
{
    LIST_HEAD(hw_config_list);
    fs_fill_list(PATH_FLY_HW_INFO_CONFIG, FS_CMD_FILE_CONFIGMODE, &hw_config_list);
    fs_get_intvalue(&hw_config_list, "hw_version", &(p_info->hw_info.hw_version), NULL);
    fs_get_intvalue(&hw_config_list, "ts_type", &(p_info->hw_info.ts_type), NULL);
    fs_get_intvalue(&hw_config_list, "ts_config", &(p_info->hw_info.ts_config), NULL);
    fs_get_intvalue(&hw_config_list, "lcd_type", &(p_info->hw_info.lcd_type), NULL);
    fly_hw_info_show("fs_fill_list", p_info);
}

//from cmd line
int update_hw_info = 0;
__setup("update_hw_info", update_hw_info);
void hw_info_check_and_save(fly_hw_data *p_info)
{
    if((g_var.recovery_mode && update_hw_info && fs_is_file_exist(PATH_FLY_HW_INFO_CONFIG))
            || fs_is_file_exist(PATH_FLY_HW_INFO_CONFIG2))
    {
        read_fly_hw_config_file(p_info);

        p_info->flag = 0x12345678;
        fly_hw_info_save(p_info);
    }
}

int lidbg_fly_hw_info_init(void)
{
    g_fly_hw_data = kzalloc(sizeof(fly_hw_data), GFP_KERNEL);
    if(!g_fly_hw_data)
        lidbgerr("kzalloc.g_fly_hw_data\n");

    if(fs_is_file_exist(PATH_MACHINE_INFO_FILE))
    {
        lidbgerr("return.%s,miss\n", PATH_MACHINE_INFO_FILE);
        return -1;
    }

    lidbg("update_hw_info = %d\n", update_hw_info);

    hw_info_check_and_save(g_fly_hw_data);

    if(!fly_hw_info_get(g_fly_hw_data))
        lidbgerr("fly_hw_info_get\n");

    if(g_fly_hw_data->flag == 0x12345678)
        g_var.hw_info = g_fly_hw_data->hw_info;

    fly_hw_info_show("result", g_fly_hw_data);
    return 0;
}

static int thread_usb_notify_flyparameter(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    while(!kthread_should_stop())
    {
        if(!wait_for_completion_interruptible(&usb_flyparameter_wait))
        {
            ssleep(20);
            lidbg("thread_usb_notify_flyparameter\n");
		   hw_info_check_and_save(g_fly_hw_data);
        }
    }
    return 1;
}
static int usb_notify_flyparameter_func(struct notifier_block *nb, unsigned long action, void *data)
{
    switch (action)
    {
    case USB_DEVICE_ADD:
        complete(&usb_flyparameter_wait);
        break;
    case USB_DEVICE_REMOVE:
        break;
    }
    return NOTIFY_OK;
}

static struct notifier_block usb_notify_flyparameter =
{
    .notifier_call = usb_notify_flyparameter_func,
};
int thread_lidbg_flyparameter_init(void *data)
{
    p_kmem = kzalloc(sizeof(recovery_meg_t), GFP_KERNEL);
    if(!p_kmem)
        lidbgerr("kzalloc.p_kmem\n");

    if(!flyparameter_info_get())
        lidbgerr("flyparameter_info_get\n");

    usb_register_notify(&usb_notify_flyparameter);

    return 0;

}

int lidbg_flyparameter_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;
    init_completion(&usb_flyparameter_wait);

    CREATE_KTHREAD(thread_lidbg_flyparameter_init, NULL);
    CREATE_KTHREAD(thread_usb_notify_flyparameter, NULL);
    lidbg_fly_hw_info_init();//block other ko before hw_info set

    return 0;

}
void lidbg_flyparameter_deinit(void)
{
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudio Inc.futengfei.2014.7.10");

module_init(lidbg_flyparameter_init);
module_exit(lidbg_flyparameter_deinit);


EXPORT_SYMBOL(g_recovery_meg);
EXPORT_SYMBOL(flyparameter_info_save);


