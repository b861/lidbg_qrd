#ifndef _LIGDBG_FILESERVER__
#define _LIGDBG_FILESERVER__

#define FS_WARN(fmt, args...) pr_info("[futengfei]warn.%s: " fmt,__func__,##args)
#define FS_ERR(fmt, args...) pr_info("[futengfei]err.%s: " fmt,__func__,##args)
#define FS_SUC(fmt, args...) pr_info("[futengfei]suceed.%s: " fmt,__func__,##args)
void lidbg_fileserver_main(int argc, char **argv);

#endif

