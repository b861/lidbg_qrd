#ifndef __FLYCOMMON_H__
#define __FLYCOMMON_H__

extern void flyaudio_display_init(void);
char *dbg_msg_set(const char *system_cmd);
void flyaboot_init(unsigned *boot_into_recovery,bool *boot_into_fastboot);

#endif
