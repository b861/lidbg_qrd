#include "lidbg.h"


#define WL_COUNT_TYPE "count"
#define WL_UNCOUNT_TYPE "uncount"

LIST_HEAD(lidbg_wakelock_list);
static spinlock_t  new_item_lock;
static struct task_struct *wl_item_dbg_task;
static int g_wakelock_dbg = 0;
static int g_wakelock_dbg_item = 0;



struct wakelock_item *get_wakelock_item(struct list_head *client_list, const char *name)
{
    struct wakelock_item *pos;
    unsigned long flags;

    spin_lock_irqsave(&new_item_lock, flags);
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (!strcmp(pos->name, name))
        {
            spin_unlock_irqrestore(&new_item_lock, flags);
            return pos;
        }
    }
    spin_unlock_irqrestore(&new_item_lock, flags);
    return NULL;
}
bool new_wakelock_item(struct list_head *client_list, bool cnt_wakelock, const char *name)
{
    struct wakelock_item *add_new_item;
    unsigned long flags;

    add_new_item = kmalloc(sizeof(struct wakelock_item), GFP_ATOMIC);
    if(add_new_item == NULL)
    {
        lidbg("<err.register_wakelock:kzalloc.wakelock_item>\n");
        return false;
    }

    add_new_item->name = kmalloc(strlen(name) + 1, GFP_ATOMIC);
    if(add_new_item->name == NULL)
    {
        lidbg("<err.register_wakelock:kzalloc.name>\n");
        return false;
    }

    strcpy(add_new_item->name, name);
    add_new_item->cunt = 1;
    add_new_item->cunt_max = 1;
    add_new_item->is_count_wakelock = cnt_wakelock;

    spin_lock_irqsave(&new_item_lock, flags);
    list_add(&(add_new_item->tmp_list), client_list);
    spin_unlock_irqrestore(&new_item_lock, flags);
    lidbg("<NEW:[%s]>\n", add_new_item->name);
    return true;
}


bool register_wakelock(struct list_head *client_list, bool cnt_wakelock, const char *name)
{
    struct wakelock_item *wakelock_pos = get_wakelock_item(client_list, name);
    if(wakelock_pos)
    {
        wakelock_pos->cunt++;
        wakelock_pos->cunt_max++;
        if(g_wakelock_dbg)
            lidbg("<ADD:%d[%s],%d>\n", wakelock_pos->cunt, wakelock_pos->name, wakelock_pos->cunt_max);
        return true;
    }
    else
        return new_wakelock_item( client_list, cnt_wakelock, name);
}

bool unregister_wakelock(struct list_head *client_list, const char *name)
{
    struct wakelock_item *wakelock_pos = get_wakelock_item(client_list, name);

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
        lidbg("<LOST:[%s]>\n", name );
        if(g_wakelock_dbg)
        {
            if(!list_empty(client_list))
            {
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
    struct list_head *client_list ;

    return ;
    client_list = &lidbg_wakelock_list;
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

static int thread_wl_item_dbg(void *data)
{
    char cmd_tmp[128];
    int count = 0;
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    while(!kthread_should_stop())
    {
        sprintf(cmd_tmp, "c wakelock lock count f%d", ++count);
        fs_readwrite_file("/dev/mlidbg0", cmd_tmp, NULL, 0);
        if(!g_wakelock_dbg_item)
            ssleep(30);
        else
            msleep(g_wakelock_dbg_item);
    }
    return 1;
}
void start_wl_dbg_thread(char *dbg_ms)
{
    g_wakelock_dbg_item = simple_strtoul(dbg_ms, 0, 0);
    wl_item_dbg_task = kthread_run(thread_wl_item_dbg, NULL, "ftf_wl_dbg");
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
        is_count_wakelock = true;

    if (!strcmp(wakelock_action, "lock"))
        register_wakelock(&lidbg_wakelock_list, is_count_wakelock, wakelock_name);
    else if (!strcmp(wakelock_action, "unlock"))
        unregister_wakelock(&lidbg_wakelock_list, wakelock_name);
    else if (!strcmp(wakelock_action, "show"))
        lidbg_show_wakelock();
    else if (!strcmp(wakelock_action, "dbg"))
        start_wl_dbg_thread(wakelock_type);

}
void lidbg_wakelock_register(bool to_lock, const char *name)
{
    if(g_wakelock_dbg)
        lidbg("<lidbg_wakelock_register:[%d][%s]>\n", to_lock, name);

    if(to_lock)
    {
        register_wakelock(&lidbg_wakelock_list, false, name);
    }
    else
    {
        unregister_wakelock(&lidbg_wakelock_list, name);
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
    FS_REGISTER_INT(g_wakelock_dbg_item, "wakelock_dbg_item", 0, NULL);

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


