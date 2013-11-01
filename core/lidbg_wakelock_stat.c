#include "lidbg.h"

LIST_HEAD(lidbg_wakelock_list);

struct wakelock_item *get_wakelock_item(char *name)
{
    struct wakelock_item *pos;
    struct list_head *client_list = &lidbg_wakelock_list;
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (!strcmp(pos->name, name))
        {
            return pos;
        }
    }
    return NULL;
}
bool new_wakelock_item(char *name)
{
    struct wakelock_item *add_new_item;
    struct list_head *client_list = &lidbg_wakelock_list;

    add_new_item = kzalloc(sizeof(struct wakelock_item), GFP_KERNEL);
    add_new_item->name = (char *)kzalloc(strlen(name), GFP_KERNEL);
    if(add_new_item->name == NULL)
    {
        lidbg("<err.register_wakelock:kzalloc>\n");
        return false;
    }

    memset(add_new_item->name, '\0', strlen(name));
    memcpy(add_new_item->name, name, strlen(name));
    add_new_item->cunt++;
    add_new_item->cunt_max++;
    list_add(&(add_new_item->tmp_list), client_list);

    lidbg("<NEW:[%s]>\n", add_new_item->name);
    return true;
}


bool register_wakelock(char *name)
{
    struct wakelock_item *wakelock_pos = get_wakelock_item(name);
    if(wakelock_pos)
    {
        wakelock_pos->cunt++;
        wakelock_pos->cunt_max++;
        lidbg("<ADD:%d[%s],%d>\n", wakelock_pos->cunt, wakelock_pos->name, wakelock_pos->cunt_max);
        return true;
    }
    else
        return new_wakelock_item(name);
}

bool unregister_wakelock(char *name)
{
    struct wakelock_item *wakelock_pos = get_wakelock_item(name);
    if(wakelock_pos)
    {
        if(wakelock_pos->cunt > 0)
            wakelock_pos->cunt--;
        lidbg("<RDC:%d[%s],%d>\n", wakelock_pos->cunt, wakelock_pos->name,  wakelock_pos->cunt_max);
        return true;
    }
    else
    {
        //can't find the wakelock? show the wakelock list.
        lidbg("<===============WAKELOCK_LIST=================>\n");
        lidbg_show_wakelock();
        return false;
    }
}

void lidbg_show_wakelock(void)
{
    int count = 0;
    struct wakelock_item *pos;
    struct list_head *client_list = &lidbg_wakelock_list;

    if(list_empty(client_list))
        lidbg("<err.lidbg_show_wakelock:nobody_register>\n");
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->name)
        {
            count++;
            lidbg("<%d.INFO%d:[%s],%d>\n", pos->cunt, count, pos->name, pos->cunt_max);
        }
    }
}


void lidbg_wakelock_stat(int argc, char **argv)
{

    char *wakelock_name = NULL;
    char *wakelock_type = NULL;
    if(argc < 2)
    {
        printk("[futengfei]err.lidbg_wakelock_stat:echo \"c wakelock lock name\" > /dev/mlidbg0\n");
        return;
    }

    wakelock_type = argv[0];
    wakelock_name = argv[1];

    if (!strcmp(wakelock_type, "lock"))
        register_wakelock(wakelock_name);
    else if (!strcmp(wakelock_type, "unlock"))
        unregister_wakelock(wakelock_name);
    else if (!strcmp(wakelock_type, "show"))
        lidbg_show_wakelock();

}


EXPORT_SYMBOL(lidbg_wakelock_stat);
EXPORT_SYMBOL(lidbg_show_wakelock);


