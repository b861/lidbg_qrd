#ifndef _LIGDBG_FUNCDEF__
#define _LIGDBG_FUNCDEF__


#define SOC_IO_Output (lidbg_dev_pt->soc_func_tbl.SOC_IO_Output)
#define SOC_IO_Input  (lidbg_dev_pt->soc_func_tbl.SOC_IO_Input)
#define SOC_IO_Output_Ext (lidbg_dev_pt->soc_func_tbl.SOC_IO_Output_Ext)
#define SOC_IO_Config  (lidbg_dev_pt->soc_func_tbl.SOC_IO_Config)
//i2c
#define SOC_I2C_Send  (lidbg_dev_pt->soc_func_tbl.SOC_I2C_Send)
#define SOC_I2C_Rec   (lidbg_dev_pt->soc_func_tbl.SOC_I2C_Rec)
#define SOC_I2C_Rec_Simple   (lidbg_dev_pt->soc_func_tbl.SOC_I2C_Rec_Simple)
//io-irq
#define SOC_IO_ISR_Add  (lidbg_dev_pt->soc_func_tbl.SOC_IO_ISR_Add)
#define SOC_IO_ISR_Enable   (lidbg_dev_pt->soc_func_tbl.SOC_IO_ISR_Enable)
#define SOC_IO_ISR_Disable  (lidbg_dev_pt->soc_func_tbl.SOC_IO_ISR_Disable)
#define SOC_IO_ISR_Del  (lidbg_dev_pt->soc_func_tbl.SOC_IO_ISR_Del)
//ad
#define SOC_ADC_Get  (lidbg_dev_pt->soc_func_tbl.SOC_ADC_Get)
//key
#define SOC_Key_Report  (lidbg_dev_pt->soc_func_tbl.SOC_Key_Report)
//bl
#define SOC_BL_Set  (lidbg_dev_pt->soc_func_tbl.SOC_BL_Set)
//pwr
#define SOC_PWR_ShutDown  (lidbg_dev_pt->soc_func_tbl.SOC_PWR_ShutDown)
//
#define SOC_Log_Dump  (lidbg_dev_pt->soc_func_tbl.SOC_Log_Dump)


#endif

