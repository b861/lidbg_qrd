#ifndef _LIGDBG_FUNCDEF__
#define _LIGDBG_FUNCDEF__

extern struct lidbg_dev *plidbg_dev;

static inline int check_pt(void)
{
	while (plidbg_dev==NULL)
	{
		printk("lidbg:check_pt-plidbg_dev==NULL\n");
		msleep(100);
	}
 	return 0;
}

#define SOC_IO_Output (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Output))
#define SOC_IO_Input  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Input))
#define SOC_IO_Output_Ext (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Output_Ext))
#define SOC_IO_Config  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Config))
//i2c
#define SOC_I2C_Send  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send))
#define SOC_I2C_Rec   (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec))
#define SOC_I2C_Rec_Simple   (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_Simple))
//io-irq
#define SOC_IO_ISR_Add  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Add))
#define SOC_IO_ISR_Enable   (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Enable))
#define SOC_IO_ISR_Disable  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Disable))
#define SOC_IO_ISR_Del  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Del))
//ad
#define SOC_ADC_Get  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_ADC_Get))
//key
#define SOC_Key_Report  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_Key_Report))
//bl
#define SOC_BL_Set  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_BL_Set))
//pwr
#define SOC_PWR_ShutDown  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown))
//for kernel build
#define SOC_PWR_GetStatus  check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_PWR_GetStatus)
#define SOC_PWR_SetStatus  check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_PWR_SetStatus)

//
#define SOC_Write_Servicer  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_Write_Servicer))

//video
#define lidbg_video_main  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnlidbg_video_main))
#define video_io_i2c_init  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnvideo_io_i2c_init))
#define flyVedioInitall  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnflyVideoInitall))
#define flyVedioTestSignalPin  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnflyVideoTestSignalPin))
#define flyVedioImageQualityConfig  (check_pt()?:(plidbg_dev->soc_func_tbl.pfnflyVideoImageQualityConfig))
#define video_init_config (check_pt()?:(plidbg_dev->soc_func_tbl.pfnvideo_init_config))
#endif

