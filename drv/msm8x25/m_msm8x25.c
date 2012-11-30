/*======================================================================
    A rk2818 driver as an example of char device drivers

    The initial developer of the original code is Baohua Song
    <author@linuxdriver.cn>. All Rights Reserved.
======================================================================*/

#include "lidbg.h"


struct fly_smem *p_fly_smem = NULL;




int msm8x25_init(void)
{
    lidbg("msm8x25_init\n");

    lidbg( "SMEM_FLY_READ_ADC = %d\n", SMEM_ID_VENDOR0);
    p_fly_smem = (struct fly_smem *)smem_alloc(SMEM_ID_VENDOR0, sizeof(struct fly_smem));

    if (p_fly_smem == NULL)
    {
        printk( "smem_alloc fail!\n");
        return 0;
    }



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


MODULE_AUTHOR("Lsw");
//MODULE_LICENSE("Dual BSD/GPL");
MODULE_LICENSE("GPL");

module_init(msm8x25_init);
module_exit(msm8x25_exit);
