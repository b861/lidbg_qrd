#include "lidbg.h"

#define MONKEY_PATH "/flysystem/lib/out/lidbg_monkey.conf"
LIST_HEAD(monkey_config_list);

int g_monkey_dbg = 0;
static bool is_monkey_inited = false;

struct monkey_dev
{
    struct completion monkey_wait;
    bool monkey_enable;
    bool random_on_en;
    bool random_off_en;
    int gpio;
    int on_ms;
    int off_ms;
};
struct monkey_dev *g_monkey_dev;

void monkey_work_func()
{
    u32 ret;
    u32 delay = g_monkey_dev->on_ms;
    //on
    if(g_monkey_dev->random_on_en)
    {
        get_random_bytes(&ret, sizeof(ret));
        delay = ret % g_monkey_dev->on_ms;
    }
    if(g_monkey_dbg)
        LIDBG_WARN("==on==%d=====\n", delay);
    SOC_IO_Output(0, g_monkey_dev->gpio, 1);
    msleep(delay);
    //off
    delay = g_monkey_dev->off_ms;
    if(g_monkey_dev->random_off_en)
    {
        get_random_bytes(&ret, sizeof(ret));
        delay = ret % g_monkey_dev->off_ms;
    }
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

    g_monkey_dev->gpio = gpio;
    g_monkey_dev->random_on_en = on_en;
    g_monkey_dev->random_off_en = off_en;
    g_monkey_dev->on_ms = on_ms;
    g_monkey_dev->off_ms = off_ms;

    if(!enable)
        g_monkey_dev->monkey_enable = false;
    else
    {
        g_monkey_dev->monkey_enable = true;
        complete(&g_monkey_dev->monkey_wait);
    }
}
int monkey_init(void *data)
{
    FS_REGISTER_INT(g_monkey_dbg, "monkey_dbg", 0, NULL);

    g_monkey_dev = kzalloc(sizeof(struct monkey_dev), GFP_ATOMIC);
    if(g_monkey_dev == NULL)
        LIDBG_ERR("<kzalloc.g_monkey_dev>\n");

    init_completion(&g_monkey_dev->monkey_wait);
    g_monkey_dev->monkey_enable = false;;

    if(fs_fill_list(MONKEY_PATH, FS_CMD_FILE_CONFIGMODE, &monkey_config_list) < 0)
        LIDBG_ERR("fs_fill_list:%s\n", MONKEY_PATH);
    else
    {
        fs_get_intvalue(&monkey_config_list, "random_on_en", &g_monkey_dev->random_on_en, NULL);
        fs_get_intvalue(&monkey_config_list, "random_off_en", &g_monkey_dev->random_off_en, NULL);
        fs_get_intvalue(&monkey_config_list, "gpio", &g_monkey_dev->gpio, NULL);
        fs_get_intvalue(&monkey_config_list, "on_ms", &g_monkey_dev->on_ms, NULL);
        fs_get_intvalue(&monkey_config_list, "off_ms", &g_monkey_dev->off_ms, NULL);
        LIDBG_WARN("[%d,%d,%d,%d,%d]\n\n", g_monkey_dev->gpio, g_monkey_dev->random_on_en, g_monkey_dev->random_off_en, g_monkey_dev->on_ms, g_monkey_dev->off_ms);
        CREATE_KTHREAD(monkey_work, NULL);
    }

    return 0;
}


static int __init lidbg_monkey_init(void)
{
    DUMP_FUN;
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

