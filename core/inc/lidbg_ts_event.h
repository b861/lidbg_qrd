#ifndef __LIDBG_TOUCH_EVENT_
#define __LIDBG_TOUCH_EVENT_

struct tspara
{
    int x;
    int y;
    bool press;
} ;

extern struct tspara g_curr_tspara;
extern bool te_regist_password(char *password, void (*cb_password)(char *password ));
extern bool te_is_ts_touched(void);


#define TE_WARN(fmt, args...) pr_info("[futengfei.te]warn.%s: " fmt,__func__,##args)
#define TE_ERR(fmt, args...) pr_info("[futengfei.te]err.%s: " fmt,__func__,##args)
#define TE_SUC(fmt, args...) pr_info("[futengfei.te]suceed.%s: " fmt,__func__,##args)

#endif
