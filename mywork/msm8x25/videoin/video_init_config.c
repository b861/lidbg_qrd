#include "video_init_config.h"
static int flag_io_config=0;
static Vedio_Channel info_Vedio_Channel = NOTONE;
static Vedio_Channel info_com_top_Channel = YIN2;
extern TW9912_Signal signal_is_how[5];
u8 Image_Config[4][11]={
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0x90,		0Xa0,	0xa5,		0xb5,		0xc5,		0x03,		0xfa,		0x0,		0x10,		0x20,		0x30,	}, //BRIGHTNESS
					{	0x20,		0x28,		0x38,		0x48,		0x58,		0x48,		0x78,		0x88	,	0x98,		0x9F,		0xB3,},//CONTRAST
					{	0x0,		0x15,		0x30,		0x45,		0x65,		0x80,		0x95,		0xa5	,	0xd5,		0xe0,		0xff,},//HUE
					{	0x0,		0x15,		0x30,		0x45,		0x65,		0x80,		0x95,		0xa5	,	0xd5,		0xe0,		0xff,},//SHARPNESS
					   };
static u8 Tw9912_image_global[4][2]={
									{0x10,0x03},//BRIGHTNESS
									{0x11,0x48},//CONTRAST
																	
									{0x13,0x80},//HUE
									{0x14,0x80},//SHARPNESS
								};
u8 Image_Config_AUX_BACK[4][11]={
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0xe0,		0Xf3, 	0xf7,		0xfb,		0x00,		0x08,		0x0a,		0x0e,		0x014,	0x16,		0x18,	}, //BRIGHTNESS
					{	0x57,		0x61,		0x67,		0x6d,		0x73,		0x58,		0x7d,		0x7f	,	0x81,		0x83,		0x85,},//CONTRAST
					{	0x37,		0x47,		0x57,		0x67,		0x87,		0x97,		0xa7,		0xa5	,	0xd5,		0xe0,		0xff,},//HUE
					{	0x37,		0x47,		0x57,		0x67,		0x87,		0x97,		0xa7,		0xa5	,	0xd5,		0xe0,		0xff,},//SHARPNESS
					   };
static u8 Tw9912_image_global_AUX_BACK[4][2]={
									{0x10,0x08},//BRIGHTNESS
									{0x11,0x58},//CONTRAST
																	
									{0x13,0x97},//HUE
									{0x14,0x97},//SHARPNESS
								};
//spinlock_t spin_chipe_config_lock;
struct mutex lock_chipe_config;
struct semaphore sem;
void video_io_i2c_init_in(void)
{
	if (!flag_io_config)
	{	
		//spin_lock_init(&spin_chipe_config_lock);
		mutex_init(&lock_chipe_config);
		sema_init(&sem, 0);
		i2c_io_config_init();
		flag_io_config=1;
	}
}
int static VideoImage(void)
{int ret;
 int i=0;
 printk("\ntw9912:@@@@@VideoImage()\n");
 	for(i=0;i<4;i++)
 		{
 		if(info_com_top_Channel = YIN3)
		ret = write_tw9912(&Tw9912_image_global_AUX_BACK[i]);
		else
		ret = write_tw9912(&Tw9912_image_global[i]);
		}
return ret;
}
int flyVideoImageQualityConfig_in(Vedio_Effect cmd ,u8 valu)
{
int ret;
u8 Tw9912_image[2]={0,0,};//default input pin selet YIN0ss
	printk("\ntw9912:@@@@@flyVideoImageQualityConfig_in(cmd =%d,valu=%d)\n",cmd,valu);
	//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
	
	if(valu >10)
		{
			printk("\ntw9912:flyVideoImageQualityConfig_in() input valu is bad 10 errror\n\n");
			mutex_unlock(&lock_chipe_config);
			return -1;
		}
	if(info_com_top_Channel = YIN3)
	{
		switch (cmd)
			{
				case CONTRAST ://ok
					Tw9912_image[0]=0x10;
					Tw9912_image[1]=Image_Config[0][valu];
					
					Tw9912_image_global_AUX_BACK[0][1]=Image_Config_AUX_BACK[0][valu];//remember
					ret = write_tw9912(&Tw9912_image);
					break;
				case BRIGHTNESS ://ok
					Tw9912_image[0]=0x11;
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config_AUX_BACK[1][valu];
						Tw9912_image_global_AUX_BACK[1][1]=Image_Config_AUX_BACK[1][valu];
					}
					ret = write_tw9912(&Tw9912_image);
					break;
				case HUE ://bad
					Tw9912_image[0]=0x14;
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config_AUX_BACK[2][valu];
						Tw9912_image_global_AUX_BACK[3][1]=Image_Config_AUX_BACK[2][valu];
					}
					ret = write_tw9912(&Tw9912_image);
					break;
				case SHARPNESS ://bad
				case CHROMA_U :
				case CHROMA_V :
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config_AUX_BACK[3][valu];
						Tw9912_image_global_AUX_BACK[2][1]=Image_Config_AUX_BACK[3][valu];
					}
					Tw9912_image[0]=0x13;
					ret = write_tw9912(&Tw9912_image);

					Tw9912_image[0]=0x14;
					ret = write_tw9912(&Tw9912_image);
					break;
			}
	}
	else
	{
			switch (cmd)
			{
				case CONTRAST ://ok
					Tw9912_image[0]=0x10;
					Tw9912_image[1]=Image_Config_AUX_BACK[0][valu];
					
					Tw9912_image_global_AUX_BACK[0][1]=Image_Config_AUX_BACK[0][valu];//remember
					ret = write_tw9912(&Tw9912_image);
					break;
				case BRIGHTNESS ://ok
					Tw9912_image[0]=0x11;
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config_AUX_BACK[1][valu];
						Tw9912_image_global_AUX_BACK[1][1]=Image_Config_AUX_BACK[1][valu];
					}
					ret = write_tw9912(&Tw9912_image);
					break;
				case HUE ://bad
					Tw9912_image[0]=0x14;
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config_AUX_BACK[2][valu];
						Tw9912_image_global_AUX_BACK[3][1]=Image_Config_AUX_BACK[2][valu];
					}
					ret = write_tw9912(&Tw9912_image);
					break;
				case SHARPNESS ://bad
				case CHROMA_U :
				case CHROMA_V :
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config_AUX_BACK[3][valu];
						Tw9912_image_global_AUX_BACK[2][1]=Image_Config_AUX_BACK[3][valu];
					}
					Tw9912_image[0]=0x13;
					ret = write_tw9912(&Tw9912_image);

					Tw9912_image[0]=0x14;
					ret = write_tw9912(&Tw9912_image);
					break;

			}
	}
//spin_unlock(&spin_chipe_config_lock);
mutex_unlock(&lock_chipe_config);
	return ret;
}
int init_tw9912_ent(Vedio_Channel Channel);
int flyVideoInitall_in(u8 Channel)
{
int ret=1 ;
//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
printk("tw9912:@@@@@flyVideoInitall_in(Channel=%d)\n",Channel);
	if (Channel>=YIN0 &&Channel<=NOTONE)
	{//Channel = SEPARATION;
		info_com_top_Channel = Channel;
		global_video_channel_flag = Channel;
	}
	else
	{
		info_com_top_Channel = NOTONE;
		global_video_channel_flag = NOTONE;
		printk("%s: you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
	}
//spin_unlock(&spin_chipe_config_lock);
mutex_unlock(&lock_chipe_config);
return ret;
//success return 1 fail return -1
}
int init_tw9912_ent(Vedio_Channel Channel)
{
int ret=-1 ;
printk("tw9912:@@@@@init_tw9912_ent(Channel=%d)\n",Channel);
printk("tw9912:init_tw9912_ent()-->Tw9912_init()\n");
	switch (Channel)
	{
		case YIN0:
			info_Vedio_Channel = YIN0;
			ret = Tw9912_init(PAL_I,YIN0);
			printk("TW9912:Channel selet YIN0\n");
			break;
		case YIN1:
			info_Vedio_Channel = YIN1;
			ret = Tw9912_init(PAL_I,YIN1);
			printk("TW9912:Channel selet YIN1\n");
			break;
		case YIN2:
			info_Vedio_Channel = YIN2;
			ret = Tw9912_init(PAL_I,YIN2);
			printk("TW9912:Channel selet YIN2\n");
			break;
		case YIN3:
			info_Vedio_Channel = YIN3;
			ret = Tw9912_init(PAL_I,YIN3);
			printk("TW9912:Channel selet YIN3\n");
			break;
		case SEPARATION:
			info_Vedio_Channel = SEPARATION;
			ret = Tw9912_init(NTSC_P,SEPARATION);
			printk("TW9912:Channel selet SEPARATION\n");
			break;
		default :
			printk("%s: you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
			break;
	}
return ret;
//success return 1 fail return -1
}
Vedio_Format camera_open_video_signal_test_in(void)
{
down(&sem);
return camera_open_video_signal_test_in_2();
}
Vedio_Format flyVideoTestSignalPin_in(u8 Channel)
{Vedio_Format ret= NOTONE;
//spin_lock(&spin_chipe_config_lock);
//return PAL_I;
mutex_lock(&lock_chipe_config);
	switch (Channel)
	{
		case 0: 
			info_Vedio_Channel = YIN0;
			ret=  testing_video_signal(YIN0);	
			break;
		case 1:
			info_Vedio_Channel = YIN1;
			ret=  testing_video_signal(YIN1);
			break;
		case 2:
			info_Vedio_Channel = YIN2;
			ret=  testing_video_signal(YIN2);
			break;
		case 3:
			info_Vedio_Channel = YIN3;
			ret=  testing_video_signal(YIN3);
			break;
		case 4:
			info_Vedio_Channel = SEPARATION;
			ret=  testing_video_signal(SEPARATION);
			break;
		default :
			info_Vedio_Channel = NOTONE;
			printk("%s:you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
			break;
	}
printk("tw9912:flyVideoTestSignalPin_in(Channel=%d) back %d\n",Channel,ret);
//spin_unlock(&spin_chipe_config_lock);
mutex_unlock(&lock_chipe_config);
//global_video_format_flag=ret;//Transmitted Jiang  Control
return ret;
}
int read_chips_signal_status(u8 cmd)
{int ret=0;
	mutex_lock(&lock_chipe_config);
	 ret = read_tw9912_chips_status(cmd);//return 0 or 1  ,if back 1 signal have change 
	 mutex_unlock(&lock_chipe_config);
 return ret;//have change return 1 else retrun 0
}
void video_init_config_in(Vedio_Format config_pramat)
{int i,j;
printk("tw9912:@@@@@video_init_config_in(config_pramat=%d)\n",config_pramat);
//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
	if(config_pramat != STOP_VIDEO)
	{	
		if(info_com_top_Channel == NOTONE)
		{
		Tw9912_init_PALi();
		printk("TW9912:warning -->info_com_top_Channel == NOTONE,Tw9912 Ignore\n");
		}
	    	else
	    	{
	    	printk("tw9912:video_init_config_in()-->init_tw9912_ent()\n");
		init_tw9912_ent(info_com_top_Channel);
		}
	VideoImage();
//msleep(300);//wait for video Steady display
/*
		printk("\r\n");
		printk("TW9912:info_Vedio_Channel=%d\n",info_Vedio_Channel);
		printk("TW9912:signal_is_how[%d].Channel=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Channel);
		printk("TW9912:signal_is_how[%d].Format=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Format);
		printk("TW9912:signal_is_how[%d].vedio_source=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].vedio_source);
*/
		if(info_Vedio_Channel<=SEPARATION)
		{
				if(tw9912_signal_unstabitily_for_Tw9912_init_flag) //find colobar flag signal bad
				{
					printk("video_init_config:video input  signal unstabitily,now shouw RED\n"); 
					 TC358_init(COLORBAR+2);//rea
					 tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;
				}
				else
				{
				//switch (flyVideoSignalPinTest(info_Vedio_Channel))
					switch (signal_is_how[info_Vedio_Channel].Format)
					{
					case NTSC_I: TC358_init(NTSC_I);
						   //TC358_init(PAL_Interlace);
						break;
					case PAL_I: TC358_init(PAL_I);
						break;
					case NTSC_P: TC358_init(NTSC_P);
						break;
					case PAL_P: TC358_init(PAL_P);
						break;
					default :printk("video not signal input\n"); 
						   TC358_init(COLORBAR+1);//blue
						break;
					}
				}
		}//if(info_Vedio_Channel<=SEPARATION)
		else
		{
			printk("Video_init_config:TW9912 not config!\n"); 
			 TC358_init(COLORBAR);
		}
		
	}
	else
	{ 
	TC358_init(COLORBAR);
	  //TC358_init(STOP_VIDEO);
	}
//Correction_Parameter_fun(signal_is_how[info_Vedio_Channel].Format);
//spin_unlock(&spin_chipe_config_lock);
up(&sem);
mutex_unlock(&lock_chipe_config);
}
void Video_Show_Output_Color(void)
{
mutex_lock(&lock_chipe_config);
printk("tw9912:error ******************************\n");
printk("tw9912:error Video_Show_Output_Color()\n");
Tw9912_init_PALi();
TC358_init(COLORBAR);
//colorbar_init_blue();
mutex_unlock(&lock_chipe_config);
}
