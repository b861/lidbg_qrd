
#define FS_WARN(fmt, args...) pr_info("[futengfei.fs]warn.%s: " fmt,__func__,##args)
#define FS_ERR(fmt, args...) pr_info("[futengfei.fs]err.%s: " fmt,__func__,##args)
#define FS_SUC(fmt, args...) pr_info("[futengfei.fs]suceed.%s: " fmt,__func__,##args)

#define DEBUG_MEM_FILE "/data/fs_private.txt"
#define LIDBG_KMSG_FILE_PATH "/data/lidbg_kmsg.txt"
#define MACHINE_ID_FILE "/data/MIF.txt"
#define LIDBG_NODE "/dev/mlidbg0"
#define KMSG_NODE "/proc/kmsg"

#define driver_sd_path "/data/drivers.txt"
#define driver_fly_path "/flysystem/lib/out/drivers.conf"
#define driver_lidbg_path "/system/lib/modules/out/drivers.conf"
#define core_sd_path "/data/core.txt"
#define core_fly_path "/flysystem/lib/out/core.conf"
#define core_lidbg_path "/system/lib/modules/out/core.conf"
#define cmd_sd_path "/data/cmd.txt"
#define cmd_fly_path "/flysystem/lib/out/cmd.conf"
#define cmd_lidbg_path "/system/lib/modules/out/cmd.conf"
#define state_sd_path "/data/state.txt"
#define state_mem_path "/dev/log/state.txt"
#define state_fly_path "/flysystem/lib/out/state.conf"
#define state_lidbg_path "/system/lib/modules/out/state.conf"

extern int g_mem_dbg;
extern int g_is_boot_completed;
extern int readwrite_file(const char *filename, char *wbuff, char *rbuff, int readlen);
extern int update_list(const char *filename, struct list_head *client_list);
extern int bfs_file_amend(char *file2amend, char *str_append);
extern void save_list_to_file(struct list_head *client_list, char *filename);
extern void set_machine_id(void);
extern void copy_all_conf_file(void);
extern void lidbg_fs_cmn_init(void);
extern void lidbg_fs_conf_init(void);
extern void lidbg_fs_keyvalue_init(void);
extern void lidbg_fs_log_init(void);
extern void lidbg_fs_update_init(void);
extern void chmod_for_apk(void);
extern void update_file_tm(void);
extern bool get_file_mftime(const char *filename, struct rtc_time *ptm);
extern bool get_file_tmstring(char *filename, char *tmstring);
extern bool is_file_tm_updated(const char *filename, struct rtc_time *pretm);
extern bool is_fly_system(void);


