#ifndef _LIGDBG_FUNCDEF__
#define _LIGDBG_FUNCDEF__

extern struct lidbg_dev *plidbg_dev;

static inline int check_pt(void)
{
    while (plidbg_dev==NULL)
    {
        printk("lidbg:check_pt-plidbg_dev==NULL\n");
        msleep(200);
    }
    return 0;
}

#define SOC_IO_Output (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Output))
#define SOC_IO_Input  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Input))
#define SOC_IO_Output_Ext (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Output_Ext))
#define SOC_IO_Config  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_Config))
//i2c
#define SOC_I2C_Send  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send))
#define SOC_I2C_Rec   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec))
#define SOC_I2C_Rec_Simple   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_Simple))

#define SOC_I2C_Rec_SAF7741  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_SAF7741))
#define SOC_I2C_Send_TEF7000   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send_TEF7000))
#define SOC_I2C_Rec_TEF7000   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_TEF7000))



//io-irq
#define SOC_IO_ISR_Add  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Add))
#define SOC_IO_ISR_Enable   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Enable))
#define SOC_IO_ISR_Disable  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Disable))
#define SOC_IO_ISR_Del  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Del))
//ad
#define SOC_ADC_Get  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_ADC_Get))
//key
#define SOC_Key_Report  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Key_Report))
//bl
#define SOC_BL_Set  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_BL_Set))
//pwr
#define SOC_PWR_ShutDown  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown))
//for kernel build
#define SOC_PWR_GetStatus  check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_PWR_GetStatus)
#define SOC_PWR_SetStatus  check_pt()?:(plidbg_dev->soc_func_tbl.pfnSOC_PWR_SetStatus)

//
#define SOC_Write_Servicer  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Write_Servicer))

//video
#define lidbg_video_main  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnlidbg_video_main))
#define video_io_i2c_init  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnvideo_io_i2c_init))
#define flyVideoChannelInitall  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnflyVideoInitall))
#define flyVideoSignalPinTest  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnflyVideoTestSignalPin))
#define flyVideoImageQualityConfig  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnflyVideoImageQualityConfig))
#define video_init_config check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnvideo_init_config)

//display/touch

#define SOC_Display_Get_Res  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Display_Get_Res))

//video 
#define camera_open_video_signal_test check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfncamera_open_video_signal_test)
#define camera_open_video_color check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfncamera_open_video_color)
//lpc
#define  SOC_LPC_Send  (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_LPC_Send))
#define global_video_format_flag  (plidbg_dev->soc_func_tbl.pfnglobal_video_format_flag)
#define global_video_channel_flag  (plidbg_dev->soc_func_tbl.pfnglobal_video_channel_flag)

//dev
#define SOC_Dev_Suspend_Prepare (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Dev_Suspend_Prepare))

//wakeup
#define SOC_PWR_Ignore_Wakelock    (plidbg_dev->soc_func_tbl.pfnSOC_PWR_Ignore_Wakelock)

//mic
#define SOC_Mic_Enable   (check_pt()?NULL:(plidbg_dev->soc_func_tbl.pfnSOC_Mic_Enable))

//
#define ptasklist_lock   (plidbg_dev->soc_pvar_tbl.pvar_tasklist_lock)


#endif

