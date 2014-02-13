#include "lidbg.h"

LIDBG_DEFINE;

int g_monkey_dbg = 0;

struct monkey_dev
{
    struct completion monkey_wait;
    int monkey_enable;
    int random_on_en;
    int random_off_en;
    int gpio;
    int on_ms;
    int off_ms;
};
struct monkey_dev *g_monkey_dev;

void monkey_work_func(void)
{
    u32 delay = g_monkey_dev->on_ms;
    //on
    if(g_monkey_dev->random_on_en)
        delay = lidbg_get_random_number(g_monkey_dev->on_ms);
    if(g_monkey_dbg)
        LIDBG_WARN("==on==%d=====\n", delay);
    SOC_IO_Output(0, g_monkey_dev->gpio, 1);
    msleep(delay);
    //off
    delay = g_monkey_dev->off_ms;
    if(g_monkey_dev->random_off_en)
        delay = lidbg_get_random_number(g_monkey_dev->off_ms);
    if(g_monkey_dbg)
        LIDBG_WARN("==off==%d=====\n", delay);
    SOC_IO_Output(0, g_monkey_dev->gpio, 0);
    msleep(delay);

}
int monkey_work(void *data)
{
    while(1)
    {
        if(g_monkey_dev->monkey_enable)
            monkey_work_func();
        else
            wait_for_completion_interruptible(&g_monkey_dev->monkey_wait);
    }
    return 0;
}

void monkey_enable(int enable, int gpio, int on_en, int off_en, int on_ms, int off_ms)
{
    LIDBG_WARN("<%d,%d,%d,%d,%d,%d>\n", enable, gpio, on_en, off_en, on_ms, off_ms);
    if(enable == 2)
    {
        g_monkey_dev->gpio = gpio;
        g_monkey_dev->random_on_en = on_en;
        g_monkey_dev->random_off_en = off_en;
        g_monkey_dev->on_ms = on_ms;
        g_monkey_dev->off_ms = off_ms;
    }
    if(!enable)
        g_monkey_dev->monkey_enable = false;
    else
    {
        g_monkey_dev->monkey_enable = true;
        complete(&g_monkey_dev->monkey_wait);
    }
}
void cb_kv_monkey_enable(char *key, char *value)
{
    monkey_enable(g_monkey_dev->monkey_enable, 123, 0, 0, 1000, 1000);
}
int monkey_init(void *data)
{

    g_monkey_dev = kzalloc(sizeof(struct monkey_dev), GFP_ATOMIC);
    if(g_monkey_dev == NULL)
        LIDBG_ERR("<kzalloc.g_monkey_dev>\n");

    init_completion(&g_monkey_dev->monkey_wait);

    FS_REGISTER_INT(g_monkey_dbg, "monkey_dbg", 0, NULL);
    FS_REGISTER_INT(g_monkey_dev->monkey_enable, "monkey_work_en", 0, cb_kv_monkey_enable);
    FS_REGISTER_INT(g_monkey_dev->random_on_en, "random_on_en", 0, NULL);
    FS_REGISTER_INT(g_monkey_dev->random_off_en, "random_off_en", 0, NULL);
    FS_REGISTER_INT(g_monkey_dev->gpio, "gpio", 0 , NULL);
    FS_REGISTER_INT(g_monkey_dev->on_ms, "on_ms", 0, NULL);
    FS_REGISTER_INT(g_monkey_dev->off_ms, "off_ms", 0, NULL);
    LIDBG_WARN("[%d,%d,%d,%d,%d]\n\n", g_monkey_dev->gpio, g_monkey_dev->random_on_en, g_monkey_dev->random_off_en, g_monkey_dev->on_ms, g_monkey_dev->off_ms);
    CREATE_KTHREAD(monkey_work, NULL);

    return 0;
}


static int __init lidbg_monkey_init(void)
{
    DUMP_FUN;
    LIDBG_GET;
    CREATE_KTHREAD(monkey_init, NULL);
    return 0;
}

static void __exit lidbg_monkey_exit(void)
{
}

module_init(lidbg_monkey_init);
module_exit(lidbg_monkey_exit);

MODULE_LICENSE("GPL");


EXPORT_SYMBOL(monkey_enable);

