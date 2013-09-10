#ifndef __LIDBG_TOUCH_EVENT_
#define __LIDBG_TOUCH_EVENT_

struct tspara
{
    int x;
    int y;
    bool press;
} ;
struct dev_password
{
    struct list_head tmp_list;
    char *password;
    void (*cb_password)(char * password );
};
extern struct tspara g_curr_tspara;
extern bool te_regist_password(char *password, void (*cb_password)(char *password ));

#endif
