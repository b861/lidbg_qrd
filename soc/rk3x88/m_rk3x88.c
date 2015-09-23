/*======================================================================
======================================================================*/

#include "lidbg.h"

struct fly_smem *p_fly_smem = NULL;

#define grf_readl(offset)		readl_relaxed(RK30_GRF_BASE + offset)
#define grf_writel(v, offset)	do { writel_relaxed(v, RK30_GRF_BASE + offset); dsb(); } while (0)
#define REG_NUM 106
static int reg_bak[REG_NUM];
static int reg_change[REG_NUM] = {0};

void grf_backup(void)
{
	DUMP_FUN;
	int i,j,reg_tmp[REG_NUM];

	for(i = 0; i < REG_NUM; i++ )
		reg_bak[i] = grf_readl(4*i);

	//soc_io_output(0, WIFI_PWR, 0);

	for(i = 0; i < 40; i++ )
	{
		if(4*i == 0x74)
			grf_writel(0xFFF00000, 4*i);
		else
			grf_writel(0xFFFF0000, 4*i);
	}
	for(i = 0; i < 15; i++ )
		grf_writel(0xFFFF0000, 4*i + 0x164);

	for(i = 0,j = 0; i < REG_NUM; i++ )
	{
		reg_tmp[i] = grf_readl(4*i);
		if(reg_tmp[i] != reg_bak[i])
		{
			lidbg("reg value diff offset[0x%x] -> REG_BAK:[0x%x] REG_TMP:[0x%x] \n", 4*i, reg_bak[i], reg_tmp[i]);
			reg_change[j++] = i;
		}
	}
	return;
}

void grf_restore(void)
{
	DUMP_FUN;
	int i,j,reg_tmp[REG_NUM];

	for(j = 0; j < REG_NUM; j++)
	{
		if(reg_change[j] == 0) break;
		i = reg_change[j];
		grf_writel(( reg_bak[i] | 0xFFFF0000 ), 4*i);
	}

	for(i = 0; i < REG_NUM; i++ )
	{
		reg_tmp[i] = grf_readl(4*i);
		if(reg_tmp[i] != reg_bak[i])
			lidbg("reg value diff offset[0x%x] -> REG_BAK:[0x%x] REG_TMP:[0x%x] \n", 4*i, reg_bak[i], reg_tmp[i]);
	}

}

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



int lidbg_write_file(const char *filename, const char *wbuf, size_t length)
{
    int ret = 0;
    struct file *filp = (struct file *) - ENOENT;
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);

    filp = filp_open(filename, O_RDWR | O_CREAT, 0666);
    if (IS_ERR(filp) || !filp->f_op)
    {
    
		lidbg("kernel_write_file:filp_open Error\n");
        ret = -ENOENT;
        return ret;
    }

    ret = filp->f_op->write(filp, wbuf, length, &filp->f_pos);
    if (ret < 0)
    {
        lidbg("kernel_write_file:write Error\n");
        return ret;
    }

    if (!IS_ERR(filp))
        filp_close(filp, NULL);
    set_fs(oldfs);
    return ret;
}

int thread_get_mac_addr(void *data)
{
	 return 0;
}



int rk3x88_init(void)
{
	DUMP_BUILD_TIME;

    CREATE_KTHREAD(thread_get_mac_addr, NULL);

    platform_device_register(&lidbg_soc);
    platform_driver_register(&lidbg_soc_driver);
    return 0;
}

/*Ä£¿éÐ¶ÔØº¯Êý*/
void rk3x88_exit(void)
{

}

EXPORT_SYMBOL(grf_backup);
EXPORT_SYMBOL(grf_restore);
EXPORT_SYMBOL(lidbg_soc_main);
EXPORT_SYMBOL(p_fly_smem);
EXPORT_SYMBOL(soc_temp_get);

MODULE_AUTHOR("Lsw");
MODULE_LICENSE("GPL");

module_init(rk3x88_init);
module_exit(rk3x88_exit);
