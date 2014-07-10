
#include "lidbg.h"

#define FLYPARAMETER_NODE "/dev/block/mmcblk0p25"
LIDBG_DEFINE;

recovery_meg_t *g_recovery_meg = NULL;
char *p_kmem = NULL;

bool flyparameter_info_get(void)
{
    if(p_kmem && fs_file_read(FLYPARAMETER_NODE, p_kmem, sizeof(recovery_meg_t)) >= 0)
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
    if( p_info && fs_file_write(FLYPARAMETER_NODE, false, (void *) p_info, sizeof(recovery_meg_t)) >= 0)
    {
        lidbg("flyparameter:save success\n");
        return true;
    }
    lidbg("flyparameter:save err\n");
    return false;
}
int thread_lidbg_flyparameter_init(void *data)
{
    p_kmem = kzalloc(sizeof(recovery_meg_t), GFP_KERNEL);
    if(!p_kmem)
        lidbgerr("kzalloc.p_kmem\n");

    if(!flyparameter_info_get())
        lidbgerr("flyparameter_info_get\n");
    return 0;

}

int lidbg_flyparameter_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;

    CREATE_KTHREAD(thread_lidbg_flyparameter_init, NULL);

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


