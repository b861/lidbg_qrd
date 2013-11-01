#ifndef _LIGDBG_WAKELOCK_LOG__
#define _LIGDBG_WAKELOCK_LOG__

struct wakelock_item
{
    struct list_head tmp_list;
    char *name;
    int cunt;
    int cunt_max;
};

void lidbg_wakelock_stat(int argc, char **argv);
void lidbg_show_wakelock(void);

#endif
