
#include "lidbg.h"


void mod_ad_main(int argc, char **argv)
{

    u32 val, ch, interval;

    if(argc < 2)
    {
        lidbg("Usage:\n");
        lidbg("ch interval\n");
        return;
    }
    ch = simple_strtoul(argv[0], 0, 0);
    interval = simple_strtoul(argv[1], 0, 0);

    while(1)
    {
        if(interval == 0) break;
        val = soc_ad_read(ch);
        lidbg("soc_ad_read: %d\n", val);
        msleep(interval);
    }
}

static int __init ad_init(void)
{
	LIDBG_MODULE_LOG;
    return 0;
}

static void __exit ad_exit(void)
{
}

module_init(ad_init);
module_exit(ad_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");


EXPORT_SYMBOL(mod_ad_main);


