/*======================================================================
======================================================================*/

#include "lidbg.h"

struct fly_smem *p_fly_smem = NULL;


int soc_temp_get(void)
{
	static long temp=-1;
	return (int)temp;
}

void lidbg_soc_main(int argc, char **argv)
{

    if(argc < 1)
    {
        lidbg("Usage:\n");
        lidbg("bl value\n");
        return;
    }

    if(!strcmp(argv[0], "bl"))
    {
        u32 bl;
        bl = simple_strtoul(argv[1], 0, 0);
        soc_bl_set(bl);
    }

    if(!strcmp(argv[0], "ad"))
    {
        u32 ch;
        ch = simple_strtoul(argv[1], 0, 0);
        lidbg("ch%d = %d\n", ch, soc_ad_read(ch));
    }
}


static int  lidbg_soc_probe(struct platform_device *pdev)
{
    DUMP_FUN;
    soc_io_init();
    return 0;
}
#ifdef CONFIG_PM

static int soc_suspend(struct device *dev)
{
    DUMP_FUN;
//	soc_io_suspend();
    return 0;
}
static int soc_resume(struct device *dev)
{
    DUMP_FUN;
//	soc_io_resume();
    return 0;
}


static struct dev_pm_ops lidbg_soc_ops =
{
    .suspend	= soc_suspend,
    .resume		= soc_resume,
};
#endif

static struct platform_device lidbg_soc =
{
    .name               = "lidbg_soc",
    .id                 = -1,
};

static struct platform_driver lidbg_soc_driver =
{
    .probe		= lidbg_soc_probe,
    .driver         = {
        .name = "lidbg_soc",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &lidbg_soc_ops,
#endif
    },
};



int tx_init(void)
{
    DUMP_BUILD_TIME;

    platform_device_register(&lidbg_soc);
    platform_driver_register(&lidbg_soc_driver);
    return 0;
}

/*ģ��ж�غ���*/
void tx_exit(void)
{

}


EXPORT_SYMBOL(lidbg_soc_main);
EXPORT_SYMBOL(p_fly_smem);
EXPORT_SYMBOL(soc_temp_get);

MODULE_AUTHOR("Lsw");
MODULE_LICENSE("GPL");

module_init(tx_init);
module_exit(tx_exit);
