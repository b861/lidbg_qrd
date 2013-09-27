#ifndef _LIGDBG_CMN__
#define _LIGDBG_CMN__

int  lidbg_exe(char path[]);
int  lidbg_mount(char path[]);
int  lidbg_insmod(char path[]);
int  lidbg_chmod(char path[]);
int  lidbg_cp(char from[],char to[]);
int  lidbg_mv(char from[],char to[]);
int  lidbg_rm(char path[]);
int  lidbg_rmdir(char path[]);
int  lidbg_mkdir(char path[]);
int  lidbg_touch(char path[]);
int  lidbg_reboot(void);
int  lidbg_setprop(char key[],char value[]);

void mod_cmn_main(int argc, char **argv);
u32 lidbg_get_ns_count(void);
int  lidbg_launch_user( char bin_path[], char argv1[],char argv2[],char argv3[],char argv4[],char argv5[],char argv6[]);
int lidbg_readwrite_file(const char *filename, char *rbuf,const char *wbuf, size_t length);
int lidbg_task_kill_select(char *task_name);
int lidbg_get_current_time(char *time_string,struct rtc_time *ptm);

#ifdef BUILD_FOR_RECOVERY
#define MOUNT_PATH "/sbin/mount"
#define INSMOD_PATH "/sbin/insmod"
#define CHMOD_PATH  "/sbin/chmod"
#define MV_PATH  "/sbin/mv"
#define RM_PATH  "/sbin/rm"
#define RMDIR_PATH  "/sbin/rmdir"
#define MKDIR_PATH  "/sbin/mkdir"
#define TOUCH_PATH  "/sbin/touch"
#define REBOOT_PATH  "/sbin/reboot"
#define SETPROP_PATH "/sbin/setprop"
#else
#define MOUNT_PATH "/system/bin/mount"
#define INSMOD_PATH "/system/bin/insmod"
#define CHMOD_PATH "/system/bin/chmod"
#define MV_PATH "/system/bin/mv"
#define RM_PATH  "/system/bin/rm"
#define RMDIR_PATH  "/system/bin/rmdir"
#define MKDIR_PATH  "/system/bin/mkdir"
#define TOUCH_PATH  "/system/bin/touch"
#define REBOOT_PATH  "/system/bin/reboot"
#define SETPROP_PATH "/system/bin/setprop"
#endif

#endif

