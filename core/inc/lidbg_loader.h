#ifndef _LIGDBG_LOADER__
#define _LIGDBG_LOADER__

extern int load_modules_count;
#define  RECOVERY_MODE_DIR "/sbin/chmod"
#define LIDBG_MODULE_LOG do{DUMP_BUILD_TIME;load_modules_count++;}while(0)

#endif

