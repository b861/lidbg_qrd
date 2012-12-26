#ifndef _LIGDBG_FUNCDEF__
#define _LIGDBG_FUNCDEF__


#define SOC_IO_Output (plidbg_dev->soc_func_tbl.pfnSOC_IO_Output)
#define SOC_IO_Input  (plidbg_dev->soc_func_tbl.pfnSOC_IO_Input)
#define SOC_IO_Output_Ext (plidbg_dev->soc_func_tbl.pfnSOC_IO_Output_Ext)
#define SOC_IO_Config  (plidbg_dev->soc_func_tbl.pfnSOC_IO_Config)
//i2c
#define SOC_I2C_Send  (plidbg_dev->soc_func_tbl.pfnSOC_I2C_Send)
#define SOC_I2C_Rec   (plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec)
#define SOC_I2C_Rec_Simple   (plidbg_dev->soc_func_tbl.pfnSOC_I2C_Rec_Simple)
//io-irq
#define SOC_IO_ISR_Add  (plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Add)
#define SOC_IO_ISR_Enable   (plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Enable)
#define SOC_IO_ISR_Disable  (plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Disable)
#define SOC_IO_ISR_Del  (plidbg_dev->soc_func_tbl.pfnSOC_IO_ISR_Del)
//ad
#define SOC_ADC_Get  (plidbg_dev->soc_func_tbl.pfnSOC_ADC_Get)
//key
#define SOC_Key_Report  (plidbg_dev->soc_func_tbl.pfnSOC_Key_Report)
//bl
#define SOC_BL_Set  (plidbg_dev->soc_func_tbl.pfnSOC_BL_Set)
//pwr
#define SOC_PWR_ShutDown  (plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown)
#define SOC_PWR_GetStatus  (plidbg_dev->soc_func_tbl.pfnSOC_PWR_GetStatus)

//
#define SOC_Write_Servicer  (plidbg_dev->soc_func_tbl.pfnSOC_Write_Servicer)


#endif

