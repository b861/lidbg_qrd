#ifndef _LIGDBG_LOADER__
#define _LIGDBG_LOADER__

extern int load_modules_count;
void  lidbg_insmod(char path[]);

#define  RECOVERY_MODE_DIR "/sbin/recovery"
#ifdef SOC_mt3360
#define  FLY_MODE_FILE "/data4write/flysystem/lib/out/lidbg_loader.ko"
#else
#define  FLY_MODE_FILE "/flysystem/lib/out/lidbg_loader.ko"
#endif
#define LIDBG_MODULE_LOG do{DUMP_BUILD_TIME;load_modules_count++;}while(0)

#endif

