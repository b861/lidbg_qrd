/*======================================================================
======================================================================*/

#include "lidbg.h"

struct fly_smem *p_fly_smem = NULL;


int soc_temp_get(void)
{
	return SMEM_TEMP;
}


int msm8x25_init(void)
{
	DUMP_BUILD_TIME;//LIDBG_MODULE_LOG;

    lidbg( "smem_alloc id = %d\n", SMEM_ID_VENDOR0);
    p_fly_smem = (struct fly_smem *)smem_alloc(SMEM_ID_VENDOR0, sizeof(struct fly_smem));

    if (p_fly_smem == NULL)
    {
        lidbg( "smem_alloc fail,kmalloc mem!\n");
        p_fly_smem = (struct fly_smem *)kmalloc(sizeof(struct fly_smem), GFP_KERNEL);
		if(p_fly_smem == NULL)
	    {
	        LIDBG_ERR("<err.register_wakelock:kzalloc.name>\n");
	    }
        memset(p_fly_smem, 0, sizeof(struct fly_smem));
    }
	
    soc_bl_init();
    soc_io_init();
    soc_ad_init();
	
    return 0;
}

/*Ä£¿éÐ¶ÔØº¯Êý*/
void msm8x25_exit(void)
{
    lidbg("msm8x25_exit\n");

}

void lidbg_soc_main(int argc, char **argv)
{
    lidbg("lidbg_soc_main\n");

    if(argc < 3)
    {
        lidbg("Usage:\n");
        return;
    }
}


EXPORT_SYMBOL(lidbg_soc_main);
EXPORT_SYMBOL(p_fly_smem);
EXPORT_SYMBOL(soc_temp_get);

MODULE_AUTHOR("Lsw");
MODULE_LICENSE("GPL");

module_init(msm8x25_init);
module_exit(msm8x25_exit);
