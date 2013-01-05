#ifndef __TW9912_H__
#define __TW9912_H__
#define DEBUG_TW9912
//#define DEBUG_PLOG_TW9912
#define I2C_US_IO_1
#define Progressive_Output_Format 0x12
#define Interlaced_Output_Format 0x02 
#define TW9912_RESET 43
#define Progressive_Output_CLKx2 0x49  
#define Interlaced_Output_CLKx1  0x05
#if 1
#define tw9912_dbg(msg...)  do { printk( KERN_CRIT "TW9912: " msg); }while(0)
#else
#define tw9912_dbg(msg...)  do {}while(0)
#endif   
#if 1
#define tw9912_RESX_UP do{i2c_io_config(TW9912_RESET,GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA,0);gpio_set_value(TW9912_RESET, 1);}while(0)//34
#define tw9912_RESX_DOWN do{i2c_io_config(TW9912_RESET,GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA,0);gpio_set_value(TW9912_RESET, 0);}while(0)//9912 reset output, active low 
#else
#define tw9912_RESX_UP do{}while(0)
#define tw9912_RESX_DOWN do{}while(0)
#endif
/*
typedef enum 
{
	YIN0=0,//not use
	YIN1,//not use
	YIN2,// now is use Y of SEPARATION / DVD_CVBS
	YIN3,//now is use AUX/BACK_CVBS
	SEPARATION,//Progressive
	NOTONE,
}Vedio_Channel;
typedef enum 
{
	NTSC_I=1,
	PAL_I,
	NTSC_P,
	PAL_P,
	STOP_VIDEO,
	COLORBAR,
	OTHER,
}Vedio_Format;
*/
typedef enum 
{
	source_60Hz =0,//NTSC
	source_50Hz,//PAL
	source_other,
}Source_in;
typedef struct 
{
	Vedio_Channel Channel;
	Vedio_Format Format;
	Source_in vedio_source;
}TW9912_Signal;
/**************************/
typedef struct 
{
	unsigned char index;	//register index
	unsigned char valu;		//resister valu
}TW9912_reg;

typedef struct
{
	TW9912_reg chip_status1;//index 0x01
	TW9912_reg chip_status2;//0x31
	TW9912_reg standard_selet;//0x1c
	TW9912_reg component_video_format;//0x1e
	TW9912_reg macrovision_detection;//0x30
	TW9912_reg input_detection;//0xc1
}TW9912_input_info;
/**************************/
typedef enum 
{
	TW9912_initall_not=0,
	TW9912_initall_yes,
}TW9912_initall_flag;
typedef struct 
{
	TW9912_initall_flag flag;	
	Vedio_Channel Channel;
	Vedio_Format format;		
}TW9912_initall_status;
typedef struct 
{
	Vedio_Channel Channel;
	Vedio_Format format;	
}Last_config_t;
/**************************/
int Tw9912_init_PALi(void);
int Tw9912_init(Vedio_Format config_pramat,Vedio_Channel Channel);

int Tw9912_appoint_pin_testing_video_signal(Vedio_Channel Channel);
Vedio_Format testing_video_signal(Vedio_Channel Channel);
int Tw9912_init_NTSCp(void);
#endif
