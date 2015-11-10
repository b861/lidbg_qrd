#include <../drivers/flyaudio/lidbg_interface.h>
#include "lidbg_def.h"
#include "cmn_func.h"
#include <linux/msm_tsens.h>

#include "cmn_func.c"
#include "boot_freq_ctrl.c"
#include "lidbg_i2c.c"

LIDBG_THREAD_DEFINE;

int __init lidbg_kernel_init(void)
{
    DUMP_BUILD_TIME;
    printk(KERN_CRIT"===lidbg_kernel_init===\n");
    freq_ctrl_start();
    lidbg_i2c_start();
    LIDBG_GET_THREAD;
    return 0;
}


EXPORT_SYMBOL_GPL(plidbg_dev);

core_initcall(lidbg_kernel_init);
