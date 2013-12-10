
#include "lidbg.h"
#include "lidbg_fs.h"

//zone below [fs.timeworker.tools]
#define TW_WARN(fmt, args...) printk("[futengfei.tw]warn.%s: " fmt,__func__,##args)
#define TW_ERR(fmt, args...) printk("[futengfei.tw]err.%s: " fmt,__func__,##args)
#define TW_SUC(fmt, args...) printk("[futengfei.tw]suceed.%s: " fmt,__func__,##args)

enum
{
    DEBUG_BASE = 1U << 0,
    DEBUG_MODE_TEST = 1U << 1,
};
static int time_worker_mask = DEBUG_BASE;

struct time_worker_item
{
    struct rb_node item_node;
    bool is_schelded;
    u64 timeout;
    void (*callback)(void);
};

volatile static struct time_worker_item *gv_tmp_item = NULL;
volatile static bool is_clear_tree_running = false;

static struct rb_root time_worker_root_tree = RB_ROOT;
static struct task_struct *timeworker_main_task;
static struct completion timeworker_wait;
static struct timer_list tw_main_timer;
static atomic_t tw_client_thread_cunt = ATOMIC_INIT(0);
static spinlock_t  new_item_lock;
//zone end


//zone below [fs.timeworker.driver]
void time_worker_clear_tree(struct rb_root *root);
void timer_modify(struct timer_list *timer, bool again, struct time_worker_item *item, unsigned long expires);

bool time_worker_new(struct rb_root *root, u64 timeout, void (*callback)(void))
{
    struct time_worker_item *add_new_item, *pos;
    struct rb_node **tmp_node, *parent = NULL;
    bool is_smaller = false;

    unsigned long flags;

    add_new_item = kmalloc(sizeof(struct time_worker_item), GFP_ATOMIC);
    if(add_new_item == NULL)
    {
        TW_ERR("<kmalloc>\n");
        return false;
    }

    spin_lock_irqsave(&new_item_lock, flags);
    add_new_item->callback = callback;
    add_new_item->timeout = timeout;
    add_new_item->is_schelded = false;
    tmp_node = &(root->rb_node);

    while (*tmp_node)
    {
        pos = rb_entry( *tmp_node, struct time_worker_item, item_node);

        parent = *tmp_node;
        if (add_new_item->timeout < pos->timeout)
        {
            tmp_node = &(*tmp_node)->rb_left;
            is_smaller = true;
        }
        else if (add_new_item->timeout > pos->timeout)
        {
            tmp_node = &(*tmp_node)->rb_right;
            is_smaller = false;
        }
        else
        {
            spin_unlock_irqrestore(&new_item_lock, flags);
            TW_ERR("<equal:%pF>\n", pos->callback);
            kfree(add_new_item);
            return false;
        }
    }
    rb_link_node(&add_new_item->item_node, parent, tmp_node);
    rb_insert_color(&add_new_item->item_node, root);
    spin_unlock_irqrestore(&new_item_lock, flags);

    TW_SUC("<new:[%d,%lld,%pF]>\n", is_smaller, add_new_item->timeout, add_new_item->callback);
    if (time_worker_mask & DEBUG_BASE)
        TW_WARN("=%d=============1===============:[%lld]\n", is_clear_tree_running, add_new_item->timeout);

    if(is_smaller)
    {
        del_timer(&tw_main_timer);
        time_worker_clear_tree(&time_worker_root_tree);
    }
    if (!is_clear_tree_running)
        time_worker_clear_tree(&time_worker_root_tree);
    return true;
}

struct time_worker_item *time_worker_search(struct rb_root *root, u64 timeout)
{
    struct time_worker_item *pos;
    struct rb_node *node;
    unsigned long flags;

    if (RB_EMPTY_ROOT(root))
    {
        TW_ERR("<nobody register>\n");
        return NULL;
    }

    spin_lock_irqsave(&new_item_lock, flags);
    node = root->rb_node;
    while (node)
    {
        pos = rb_entry(node, struct time_worker_item, item_node);

        if (timeout < pos->timeout)
            node = node->rb_left;
        else if (timeout > pos->timeout)
            node = node->rb_right;
        else
        {
            spin_unlock_irqrestore(&new_item_lock, flags);
            if (time_worker_mask & DEBUG_BASE)
                TW_SUC("<find:%lld>\n", timeout);
            return pos;
        }
    }
    spin_unlock_irqrestore(&new_item_lock, flags);
    if (time_worker_mask &DEBUG_BASE)
        TW_ERR("<not find:%lld>\n", timeout);
    return NULL;
}

bool time_worker_register(struct rb_root *root, int timeout, void (*callback)(void))
{
    u64 req_jiffies = 0;
    req_jiffies = get_jiffies_64() ;
    if (time_worker_mask & DEBUG_BASE)
        TW_WARN("[%ld,%lld,%lld]\n", msecs_to_jiffies(timeout), req_jiffies, req_jiffies + msecs_to_jiffies(timeout));
    req_jiffies += msecs_to_jiffies(timeout);

    if (time_worker_mask & DEBUG_MODE_TEST)
        req_jiffies = timeout;

    if(!RB_EMPTY_ROOT(root))
    {
        while(time_worker_search(root, req_jiffies))
            req_jiffies++;
    }
    return time_worker_new(root, req_jiffies, callback);
}

bool time_worker_del(struct rb_root *root, struct time_worker_item *item, int timeout)
{
    struct time_worker_item *pos;
    unsigned long flags;
    if (RB_EMPTY_ROOT(root))
    {
        TW_ERR("<nobody register>\n");
        return false;
    }

    if(item)
        pos = item;
    else
        pos = time_worker_search(root, timeout) ;

    spin_lock_irqsave(&new_item_lock, flags);
    if (pos)
    {
        rb_erase(&pos->item_node, root);
        RB_CLEAR_NODE(&pos->item_node);
        if (time_worker_mask & DEBUG_BASE)
            TW_SUC("<DEL:%lld>\n", pos->timeout);
        kfree(pos);
    }
    else
        TW_ERR("<pos=null>\n");
    spin_unlock_irqrestore(&new_item_lock, flags);
    return true;
}

void time_worker_show(struct rb_root *root)
{
    struct rb_node *node;
    struct time_worker_item *pos;
    int index = 0;
    unsigned long flags;
    if (!RB_EMPTY_ROOT(root))
    {
        spin_lock_irqsave(&new_item_lock, flags);
        for (node = rb_first(root); node; node = rb_next(node))
        {
            pos = rb_entry(node, struct time_worker_item, item_node);
            TW_WARN("index%d:[%lld]\n", index++, pos->timeout);
        }
        spin_unlock_irqrestore(&new_item_lock, flags);
    }
    else
        TW_ERR("<nobody register>\n");
}

struct time_worker_item *time_worker_get_valid_item(struct rb_root *root)
{
    struct rb_node *node;
    struct time_worker_item *pos;
    unsigned long flags;
    if (!RB_EMPTY_ROOT(root))
    {
        spin_lock_irqsave(&new_item_lock, flags);
        for (node = rb_first(root); node; node = rb_next(node))
        {
            pos = rb_entry(node, struct time_worker_item, item_node);
            if(!pos->is_schelded)
            {
                spin_unlock_irqrestore(&new_item_lock, flags);
                return pos;
            }
        }
        spin_unlock_irqrestore(&new_item_lock, flags);
    }
    else
        TW_ERR("<nobody register>\n");
    return NULL;
}


void time_worker_clear_tree(struct rb_root *root)
{
    struct time_worker_item *pos;

    if (time_worker_mask & DEBUG_BASE)
        TW_ERR("<==IN==>\n");

    if (!RB_EMPTY_ROOT(root))
    {
        pos = time_worker_get_valid_item(root);
        if(pos)
        {
            is_clear_tree_running = true;
            if (time_worker_mask & DEBUG_MODE_TEST)
                timer_modify(&tw_main_timer, true, pos, jiffies + pos->timeout * HZ);
            else
                timer_modify(&tw_main_timer, true, pos, pos->timeout);
        }
        else
        {
            is_clear_tree_running = false;
            if (time_worker_mask & DEBUG_BASE)
                TW_ERR("<time_worker_get_valid_item=null>\n");
        }
    }
    else
    {
        is_clear_tree_running = false;
        TW_ERR("<nobody register>\n");
    }

}

int client_thread_commen_func(void *data)
{
    struct time_worker_item *item = data;
    item->callback();
    atomic_dec(&tw_client_thread_cunt);
    time_worker_del(&time_worker_root_tree, item, 0);
    return 0;
}
void client_thread_creat(struct time_worker_item *item)
{
    unsigned long flags;
    spin_lock_irqsave(&new_item_lock, flags);
    gv_tmp_item = NULL;
    spin_unlock_irqrestore(&new_item_lock, flags);

    atomic_inc(&tw_client_thread_cunt);
    kthread_run(client_thread_commen_func, item, "ftf_tw_client%d", atomic_read(&tw_client_thread_cunt));
}
void timer_commen_func(unsigned long data)
{
    struct time_worker_item *pos = (struct time_worker_item *)data;
    pos->is_schelded = true;
    if (time_worker_mask & DEBUG_BASE)
        TW_WARN("==============2===============:[%lld]\n",  pos->timeout);

    if(pos->callback)
    {
        unsigned long flags;
        spin_lock_irqsave(&new_item_lock, flags);
        gv_tmp_item = pos;
        spin_unlock_irqrestore(&new_item_lock, flags);

        complete(&timeworker_wait);
        time_worker_clear_tree(&time_worker_root_tree);
    }
    else
        TW_ERR("<item->callback()>\n");
}
void timer_modify(struct timer_list *timer, bool again, struct time_worker_item *item, unsigned long expires)
{
    if(again)
        del_timer(&tw_main_timer);
    tw_main_timer.function = timer_commen_func;
    tw_main_timer.data = (unsigned long) item;
    tw_main_timer.expires = expires;
    add_timer(&tw_main_timer);
}

//zone end


//zone below [fs.timeworker.interface]
bool tw_time_worker_register(int timeout, void (*callback)(void))
{
    return time_worker_register(&time_worker_root_tree, timeout, callback);
}
//zone end

void fs_timeworker_test(void)
{
    TW_WARN("<callback_test>\n");

}
void lidbg_fs_timework_test(int cmd, int cmd_para)
{

    TW_WARN("<=====[%d,%d]=====>\n", cmd, cmd_para);
    switch (cmd)
    {
    case 1:
        time_worker_show(&time_worker_root_tree);
        break;
    case 2:
        tw_time_worker_register(cmd_para, fs_timeworker_test);
        break;
    case 3:
        time_worker_del(&time_worker_root_tree, NULL, cmd_para);
        break;
    case 4:
        time_worker_clear_tree(&time_worker_root_tree);
        break;
    default:
        TW_WARN("<cmd:%d>\n", cmd);
        break;
    }

}
static int thread_timeworker_main_func(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    while(!kthread_should_stop())
    {
        if( !wait_for_completion_interruptible(&timeworker_wait))
        {
            client_thread_creat(gv_tmp_item);
        }
    }
    return 1;
}
void lidbg_fs_timeworker_init(void)
{
    spin_lock_init(&new_item_lock);
    init_completion(&timeworker_wait);
    init_timer(&tw_main_timer);
    timeworker_main_task = kthread_run(thread_timeworker_main_func, NULL, "ftf_tw_task");
}

EXPORT_SYMBOL(tw_time_worker_register);

