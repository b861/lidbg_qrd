#include "lidbg.h"


#define WL_COUNT_TYPE "count"
#define WL_UNCOUNT_TYPE "uncount"

LIST_HEAD(lidbg_wakelock_list);
static spinlock_t  new_item_lock;
static int g_wakelock_dbg = 0;



struct wakelock_item *get_wakelock_item(const char *name)
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
bool new_wakelock_item(bool cnt_wakelock,const char *name)
{
    struct wakelock_item *add_new_item;
    struct list_head *client_list = &lidbg_wakelock_list;
    unsigned long flags;

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
    add_new_item->is_count_wakelock = cnt_wakelock;

    spin_lock_irqsave(&new_item_lock, flags);
    list_add(&(add_new_item->tmp_list), client_list);
    spin_unlock_irqrestore(&new_item_lock, flags);
	//if(g_wakelock_dbg)
	lidbg("<NEW:[%s]>\n", add_new_item->name);
    return true;
}


bool register_wakelock(bool cnt_wakelock, const char *name)
{
    struct wakelock_item *wakelock_pos = get_wakelock_item(name);
    if(wakelock_pos)
    {
        wakelock_pos->cunt++;
        wakelock_pos->cunt_max++;
        if(g_wakelock_dbg)
            lidbg("<ADD:%d[%s],%d>\n", wakelock_pos->cunt, wakelock_pos->name, wakelock_pos->cunt_max);
        return true;
    }
    else
        return new_wakelock_item(cnt_wakelock, name);
}

bool unregister_wakelock(const char *name)
{
    struct wakelock_item *wakelock_pos = get_wakelock_item(name);
	
    if(wakelock_pos)
    {

        if(wakelock_pos->is_count_wakelock)
        {
            if(wakelock_pos->cunt > 0)
                wakelock_pos->cunt--;
        }
        else
            wakelock_pos->cunt = 0;

        if(g_wakelock_dbg)
            lidbg("<DEL:%d[%s],%d>\n", wakelock_pos->cunt, wakelock_pos->name,  wakelock_pos->cunt_max);
        return true;
    }
    else
    {
        //can't find the wakelock? show the wakelock list.
		if(g_wakelock_dbg)
		{
			struct list_head *client_list = &lidbg_wakelock_list;
			if(!list_empty(client_list))
			{
					lidbg("<===============WAKELOCK_LIST=================%s>\n",name);
					lidbg_show_wakelock();
			}
		}
        return false;
    }
}

void lidbg_show_wakelock(void)
{
    int index = 0;
    struct wakelock_item *pos;
    struct list_head *client_list = &lidbg_wakelock_list;

    if(list_empty(client_list))
        lidbg("<err.lidbg_show_wakelock:nobody_register>\n");
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->name)
        {
            index++;
            lidbg("<%d.INFO%d:[%s],%d,%d>\n", pos->cunt, index, pos->name, pos->is_count_wakelock, pos->cunt_max);
        }
    }
}


void lidbg_wakelock_stat(int argc, char **argv)
{

    char *wakelock_action = NULL;
    char *wakelock_type = NULL;
    char *wakelock_name = NULL;
	bool is_count_wakelock = false;
    if(argc < 3)
    {
        printk("[futengfei]err.lidbg_wakelock_stat:echo \"c wakelock lock count name\" > /dev/mlidbg0\n");
        return;
    }

    wakelock_action = argv[0];
    wakelock_type = argv[1];
    wakelock_name = argv[2];
	
	if(!strcmp(wakelock_type, WL_COUNT_TYPE))
		is_count_wakelock=true;

    if (!strcmp(wakelock_action, "lock"))
        register_wakelock(is_count_wakelock, wakelock_name);
    else if (!strcmp(wakelock_action, "unlock"))
        unregister_wakelock(wakelock_name);
    else if (!strcmp(wakelock_action, "show"))
        lidbg_show_wakelock();

}
void lidbg_wakelock_register(bool to_lock, const char*name)
{
	if(g_wakelock_dbg)
		lidbg("<lidbg_wakelock_register:[%d][%s]>\n",to_lock, name);

	if(to_lock)
	{
		register_wakelock(false, name);
	}
	else
	{
		unregister_wakelock(name);
	}
}

void cb_kv_show_list(char *key, char *value)
{
    lidbg_show_wakelock();
}
static int __init lidbg_wakelock_stat_init(void)
{
    DUMP_BUILD_TIME;

    spin_lock_init(&new_item_lock);
    FS_REGISTER_INT(g_wakelock_dbg, "wakelock_dbg", 0, cb_kv_show_list);

    return 0;
}

static void __exit lidbg_wakelock_stat_exit(void)
{
}

module_init(lidbg_wakelock_stat_init);
module_exit(lidbg_wakelock_stat_exit);

MODULE_DESCRIPTION("wakelock.stat");
MODULE_LICENSE("GPL");


EXPORT_SYMBOL(lidbg_wakelock_list);
EXPORT_SYMBOL(lidbg_wakelock_stat);
EXPORT_SYMBOL(lidbg_show_wakelock);
EXPORT_SYMBOL(lidbg_wakelock_register);


