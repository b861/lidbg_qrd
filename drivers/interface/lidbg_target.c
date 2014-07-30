
#ifdef PLATFORM_msm8226
#include "lidbg_target_msm8226.c"
#elif defined(PLATFORM_msm8974)
#include "lidbg_target_msm8974.c"
#endif


#ifdef SOC_mt3360
#include "lidbg_target_mt3360.c"
#endif


EXPORT_SYMBOL(g_hw_version_specific);


