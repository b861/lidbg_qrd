/*======================================================================
======================================================================*/

#include "lidbg.h"

#ifdef _LIGDBG_SHARE__
LIDBG_SHARE_DEFINE;
#endif

struct fly_smem *p_fly_smem = NULL;

#define DEBUG_USE_SMEM

static void share_set_func_tbl(void)
{
    //io
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_io_output = soc_io_output;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_io_input = soc_io_input;

    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_io_irq = soc_io_irq;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_irq_enable = soc_irq_enable;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_irq_disable = soc_irq_disable;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_io_config = soc_io_config;

    //ad
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_ad_read = soc_ad_read;

    //pwm
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_pwm_set = soc_pwm_set;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnsoc_bl_set = soc_bl_set;

    //soc init
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_soc_init = lidbg_soc_init;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_soc_deinit = lidbg_soc_deinit;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_board_init = lidbg_board_init;
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_board_deinit = lidbg_board_deinit;

    //soc main
    ((struct lidbg_share *)plidbg_share)->share_func_tbl.pfnlidbg_soc_main = lidbg_soc_main;
}


int msm8x25_init(void)
{
    DUMP_FUN;

#ifdef _LIGDBG_SHARE__
    LIDBG_SHARE_GET;
    share_set_func_tbl();
#endif

#ifdef DEBUG_USE_SMEM
    lidbg( "SMEM_FLY_READ_ADC = %d\n", SMEM_ID_VENDOR0);
    p_fly_smem = (struct fly_smem *)smem_alloc(SMEM_ID_VENDOR0, sizeof(struct fly_smem));

    if (p_fly_smem == NULL)
    {
        printk( "smem_alloc fail!\n");
        return 0;
    }
#else
    p_fly_smem = (struct fly_smem *)kmalloc(sizeof(struct fly_smem), GFP_KERNEL);
    memset(p_fly_smem, 0, sizeof(struct fly_smem));

#endif

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

#ifndef _LIGDBG_SHARE__

EXPORT_SYMBOL(lidbg_soc_main);
#endif

MODULE_AUTHOR("Lsw");
MODULE_LICENSE("GPL");

module_init(msm8x25_init);
module_exit(msm8x25_exit);
