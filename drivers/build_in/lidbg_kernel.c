#include <../drivers/flyaudio/lidbg_interface.h>
#include "lidbg_def.h"
#include "cmn_func.h"

#include "cmn_func.c"
#include "boot_freq_ctrl.c"

LIDBG_THREAD_DEFINE;

int __init lidbg_kernel_init(void)
{
	DUMP_BUILD_TIME;
	freq_ctrl_start();
	LIDBG_GET_THREAD;
	return 0;
}


EXPORT_SYMBOL_GPL(plidbg_dev);

core_initcall(lidbg_kernel_init);
