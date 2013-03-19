#include "video_init_config.h"
static int flag_io_config=0;
static Vedio_Channel info_Vedio_Channel = NOTONE;
static Vedio_Channel info_com_top_Channel = YIN2;
extern TW9912_Signal signal_is_how[5];
extern Last_config_t the_last_config;
static struct task_struct * Signal_Test = NULL;
static char Channel_tab[][20]={
					"YIN0",
					"YIN1",
					"DVD(YIN2)",
					"BACK(YIN3)",
					"DVD(YUV)",
					"NOTONE",
					 };
static char Format_tab[][20]={
					"NTSC_I",
					"PAL_I",
					"NTSC_P",
					"PAL_P",
					"STOP_VIDEO",
					"COLORBAR",
					"OTHER",
					 };
#ifndef BOARD_V2 // V1
u8 Image_Config[5][11]={
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0xe9,		0Xf0,		0xf1,		0xf2,		0xf3,		0xf5,		0xf6,		0xf7,	        0xf8,		0xf9,		0xfd,	}, //BRIGHTNESS ed
					{	0x40,		0x45,		0x50,		0x5a,		0x65,        0x6e,		0x78,		0x85	,	0x94,		0x9f,		0xa5,},//CONTRAST 9f
					{	0x81,		0x96,		0xb9,		0xdf,		0xe7,		0xff,		0x0,   	0xa,		0x35,		0x63,		0x7f,},//HUE
					{	0x0,		0x15,		0x30,		0x45,		0x65,		0x7f,		0x95,		0xa5	,	0xd5,		0xe0,		0xff,},//SHARPNESS
					{	0x0,		0x15,		0x30,		0x45,		0x65,		0x7f,		0x95,		0xa5	,	0xd5,		0xe0,		0xff,},//SHARPNESS
					   };
static u8 Tw9912_image_global[5][2]={
									{0x10,0xf5},//BRIGHTNESS
									{0x11,0x6e},//CONTRAST
																	
									{0x15,0xff},//HUE
									{0x13,0x7f},//SHARPNESS
									{0x14,0x7f},//SHARPNESS
								};
u8 Image_Config_AUX_BACK[5][11]={
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0x15,		0X0f, 	0x09,		0x05,		0x03,		0xff,		0xfa,		0xf0,		0xe9,	        0xe0,		0xc9,	}, //BRIGHTNESS
					{	0x39,		0x40,		0x45,		0x50,		0x59,		0x60,		0x65,		0x6a	,	0x75,		0x8a,		0x90,},//CONTRAST
					{	0xa9,		0xb5,		0xc5,		0xe0,		0xf1,		0x00,		0x20,		0x30	,	0x40,		0x50,		0x60,},//HUE
					{	0x30,		0x40,		0x50,		0x60,		0x70,		0x80,		0x90,		0xa0	,	0xb0,		0xc0,		0xd0,},//SHARPNESS
					{	0x30,		0x40,		0x50,		0x60,		0x70,		0x80,		0x90,		0xa0	,	0xb0,		0xc0,		0xd0,},//SHARPNESS
					   };
static u8 Tw9912_image_global_AUX_BACK[5][2]={
									{0x10,0xff},//BRIGHTNESS
									{0x11,0x60},//CONTRAST
																	
									{0x15,0x00},//HUE
									{0x13,0x80},//SHARPNESS
									{0x14,0x80},//SHARPNESS
								};
u8 Image_Config_AUX_BACK_PAL_I[5][11]={
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0x15,		0X0f, 	0x09,		0x05,		0x03,		0x05,		0xfa,		0xf0,		0xe9,	        0xe0,		0xc9,	}, //BRIGHTNESS
					{	0x25,		0x30,		0x35,		0x39,		0x40,		0x44,		0x45,		0x50,		0x59,		0x65,		0x6a,},//CONTRAST
					{	0xa9,		0xb5,		0xc5,		0xe0,		0xf1,		0x00,		0x20,		0x30	,	0x40,		0x50,		0x60,},//HUE
					{	0x50,		0x60,		0x70,		0x80,		0x90,		0xa0,		0xb0,		0xc0,		0xd0,		0xe0,		0xf0,},//SHARPNESS
					{	0x50,		0x60,		0x70,		0x80,		0x90,		0xa0,		0xb0,		0xc0,		0xd0,		0xe0,		0xf0,},//SHARPNESS
					   };
static u8 Tw9912_image_global_AUX_BACK_PAL_I[5][2]={
									{0x10,0x05},//BRIGHTNESS
									{0x11,0x44},//CONTRAST
																	
									{0x15,0x00},//HUE
									{0x13,0xa0},//SHARPNESS
									{0x14,0xa0},//SHARPNESS
								};

#else// V2
u8 Image_Config[5][11]={//DVD YIN2
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0xc5,		0Xd5,	0xdd,		0xe5,		0xed,		0xef,		0xf2,		0xf5,	        0xf8,		0xfb,		0xff,	}, //BRIGHTNESS ed
					{	0x55,		0x65,		0x6d,		0x75,		0x7d,        0x80,		0x87,		0x8e	,	0x95,		0x9c,		0xa5,},//CONTRAST 9f
					{	0x81,		0x96,		0xb9,		0xdf,		0xe7,		0xff,		0x0,   	0xa,		0x35,		0x63,		0x7f,},//HUE
					{	0x20,		0x4a,		0x64,		0x7e,		0x98,		0xb6, 	0xbf,		0xc6,		0xd6,		0xe6,		0xf6,},//SHARPNESS
					{	0x20,		0x4a,		0x64,		0x7e,		0x98,		0xb6, 	0xbf,		0xc6,		0xd6,		0xe6,		0xf6,},//SHARPNESS
					   };
static u8 Tw9912_image_global[5][2]={//DVD YIN2
									{0x10,0xef},//BRIGHTNESS
									{0x11,0x80},//CONTRAST
																	
									{0x15,0xff},//HUE
									{0x13,0xb6},//SHARPNESS
									{0x14,0xb6},//SHARPNESS
								};
u8 Image_Config_AUX_BACK[5][11]={//back or AUX
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0x15,		0X0f, 	0x09,		0x05,		0x03,		0xf6,		0xfa,		0xf0,		0xe9,	        0xe0,		0xc9,	}, //BRIGHTNESS
					{	0x33,		0x35,		0x39,		0x40,		0x45,		0x50,		0x59,		0x65,		0x6a	,	0x75,		0x8a,	},//CONTRAST
					{	0xa9,		0xb5,		0xc5,		0xe0,		0xf1,		0x00,		0x20,		0x30	,	0x40,		0x50,		0x60,},//HUE
					{	0x50,		0x60,		0x70,		0x80,		0x90,		0xa0,		0xb0,		0xc0,		0xd0,		0xe0,		0xf0,},//SHARPNESS
					{	0x50,		0x60,		0x70,		0x80,		0x90,		0xa0,		0xb0,		0xc0,		0xd0,		0xe0,		0xf0,},//SHARPNESS
					   };
static u8 Tw9912_image_global_AUX_BACK[5][2]={
									{0x10,0xF6},//BRIGHTNESS
									{0x11,0x50},//CONTRAST
																	
									{0x15,0x00},//HUE
									{0x13,0xa0},//SHARPNESS
									{0x14,0xa0},//SHARPNESS
								};
u8 Image_Config_AUX_BACK_PAL_I[5][11]={
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0x15,		0X0f, 	0x09,		0x05,		0x03,		0x05,		0xfa,		0xf0,		0xe9,	        0xe0,		0xc9,	}, //BRIGHTNESS
					{	0x25,		0x30,		0x35,		0x39,		0x40,		0x44,		0x45,		0x50,		0x59,		0x65,		0x6a,},//CONTRAST
					{	0xa9,		0xb5,		0xc5,		0xe0,		0xf1,		0x00,		0x20,		0x30	,	0x40,		0x50,		0x60,},//HUE
					{	0x50,		0x60,		0x70,		0x80,		0x90,		0xa0,		0xb0,		0xc0,		0xd0,		0xe0,		0xf0,},//SHARPNESS
					{	0x50,		0x60,		0x70,		0x80,		0x90,		0xa0,		0xb0,		0xc0,		0xd0,		0xe0,		0xf0,},//SHARPNESS
					   };
static u8 Tw9912_image_global_AUX_BACK_PAL_I[5][2]={
									{0x10,0x05},//BRIGHTNESS
									{0x11,0x44},//CONTRAST
																	
									{0x15,0x00},//HUE
									{0x13,0xa0},//SHARPNESS
									{0x14,0xa0},//SHARPNESS
								};
#endif
u8 Image_Config_separation[5][11]={//DVD separation
						/*0*/	/*1*/	/*2*/	/*3*/	/*4*/	/*5*/	/*6*/	/*7*/	/*8*/	/*9*/	/*10*/
					{	0x50,		0x58,		0x65,		0x70,		0x75,        0x80,		0x85,		0x90	,	0x95,		0xa5,		0xb0,},//BRIGHTNESS ed
					{	0xc0,		0xc5,		0Xc9,	0xcf,		0xd0,		0xd5,		0xe0,		0xe2,	        0xe5,		0xe9,		0x00,	},//CONTRAST 9f
					{	0x87,		0x96,		0xb9,		0xdf,		0xe7,		0xff,		0x0,   	0xa,		0x35,		0x63,		0x7f,},//HUE
					{	0x20,		0x4a,		0x64,		0x7e,		0x98,		0xb6, 	0xbf,		0xc6,		0xd6,		0xe6,		0xf6,},//SHARPNESS
					{	0x20,		0x4a,		0x64,		0x7e,		0x98,		0xb6, 	0xbf,		0xc6,		0xd6,		0xe6,		0xf6,},//SHARPNESS
					   };
static u8 Tw9912_image_global_separation[5][2]={//DVD separation
									{0x11,0x80},//BRIGHTNESS
									{0x10,0x00},//CONTRAST
																	
									{0x15,0xff},//HUE
									{0x13,0xb6},//SHARPNESS
									{0x14,0xb6},//SHARPNESS
								};
//spinlock_t spin_chipe_config_lock;
struct mutex lock_chipe_config;
extern struct mutex lock_com_chipe_config;
struct semaphore sem;
void video_io_i2c_init_in(void)
{
	if (!flag_io_config)
	{	
		//spin_lock_init(&spin_chipe_config_lock);
		mutex_init(&lock_chipe_config);
		mutex_init(&lock_com_chipe_config);
		sema_init(&sem, 0);
		i2c_io_config_init();
		flag_io_config=1;
	}
}
int static Change_channel(void)
{
printk("TC358:Change_channel() \n");
//Disabel_video_data_out();

TC358_data_output_enable(DISABLE);
TC358_init(COLORBAR+TC358746XBG_BLACK);
//msleep(1);
TC358_data_output_enable(ENABLE);
tw9912_RESX_DOWN;//\u8fd9\u91cc\u5bf9tw9912\u590d\u4f4d\u7684\u539f\u56e0\u662f\u89e3\u51b3\u5012\u8f66\u9000\u56deDVD\u65f6\u89c6\u9891\u5361\u6b7b\u3002
tw9912_RESX_UP;
msleep(20);
}
int static VideoImage(void)
{int ret;
 int i=0;
 u8 Tw9912_image[2]={0x17,0x87,};//default input pin selet YIN0ss
 printk("fly_video_tw9912 The location of the image set ,key parameter info_com_top_Channel =%s.\n",Channel_tab[info_com_top_Channel]);
 	for(i=0;i<5;i++)
 		{
 		if(info_com_top_Channel == YIN3)//back or AUX
 			{
				if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
						ret = write_tw9912(&Tw9912_image_global_AUX_BACK[i]);
				else
						ret = write_tw9912(&Tw9912_image_global_AUX_BACK_PAL_I[i]);
			}
			else if(info_com_top_Channel == YIN2)//DVD YIN2
				ret = write_tw9912(&Tw9912_image_global[i]);
			else //DVD SEPARATION
				ret = write_tw9912(&Tw9912_image_global_separation[i]);
		}

	if(info_com_top_Channel == YIN3)// back or AUX
		{
			ret = write_tw9912(Tw9912_image);
			Tw9912_image[0]=0x08;
			Tw9912_image[1]=0x14;// image down 5 line
			ret = write_tw9912(Tw9912_image);
			Tw9912_image[0]=0x0a;
			Tw9912_image[1]=0x22;// image down 5 line
			ret = write_tw9912(Tw9912_image);
		}
	else if(info_com_top_Channel == YIN2)//DVD
		{
		u8 Tw9912_image[2]={0x08,0x1a,};//image reft 5 line
			ret = write_tw9912(Tw9912_image);
		}
	else 
		{
		;
		}

#ifdef BOARD_V2
	if(info_com_top_Channel == YIN2)//dvd cvbs
	{
	Tw9912_image[0]=0x08;//image dowd 3 line
	Tw9912_image[1]=0x15;// image down 3 line
	ret = write_tw9912(Tw9912_image);
	Tw9912_image[0]=0x09;//image dowd 3 line
	Tw9912_image[1]=0xfa;// image down 3 line
	ret = write_tw9912(Tw9912_image);

       	Tw9912_image[0]=0x0B;//image dowd 3 line
	Tw9912_image[1]=0xec;// image down 3 line
	ret = write_tw9912(Tw9912_image);
	}
	else 	if(info_com_top_Channel == YIN3)//aux back_cvbs
	{
		if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
		{
			Tw9912_image[0]=0x08;//image dowd 3 line
			Tw9912_image[1]=0x10;// image down 3 line
			ret = write_tw9912(Tw9912_image);
			Tw9912_image[0]=0x09;//image dowd 3 line
			Tw9912_image[1]=0xf9;// image down 3 line
			ret = write_tw9912(Tw9912_image);

			Tw9912_image[0]=0x0A;//image dowd 3 line
			Tw9912_image[1]=0x23;// image down 3 line
			ret = write_tw9912(Tw9912_image);
			
		       	Tw9912_image[0]=0x0B;//image dowd 3 line
			Tw9912_image[1]=0xec;// image down 3 line
			ret = write_tw9912(Tw9912_image);
			/**/
		}
		else//pal
		{;
		/*	Tw9912_image[0]=0x07;//image dowd 3 line
			Tw9912_image[1]=0x22;// image down 3 line
			ret = write_tw9912(Tw9912_image);
			Tw9912_image[0]=0x08;//image dowd 3 line
			Tw9912_image[1]=0x12;// image down 3 line
			ret = write_tw9912(Tw9912_image);
			Tw9912_image[0]=0x09;//image dowd 3 line
			Tw9912_image[1]=0x40;// image down 3 line
			ret = write_tw9912(Tw9912_image);

			Tw9912_image[0]=0x0a;//image dowd 3 line
			Tw9912_image[1]=0x1c;// image down 3 line
			ret = write_tw9912(Tw9912_image);
		       	Tw9912_image[0]=0x0B;//image dowd 3 line
			Tw9912_image[1]=0xe6;// image down 3 line
			ret = write_tw9912(Tw9912_image);
		*/
		//config at tw9912.c function --> i2c_ack Correction_Parameter_fun(Vedio_Format format)
		}

	}
#endif 
return ret;
}
int flyVideoImageQualityConfig_in(Vedio_Effect cmd ,u8 valu)
{
int ret;
u8 Tw9912_image[2]={0,0,};//default input pin selet YIN0ss
	printk("fly_video HAL Layer Image quality config (cmd =%d,valu=%d)\n",cmd,valu);
	//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
	
	if(valu >10)
		{
			printk("\ntw9912:flyVideoImageQualityConfig_in() input valu is bad 10 errror\n\n");
			mutex_unlock(&lock_chipe_config);
			return -1;
		}
	if(info_com_top_Channel == YIN3)
	{
		if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
				{
				switch (cmd)
					{
						
						case BRIGHTNESS ://ok
							Tw9912_image[0]=0x10;
							Tw9912_image[1]=Image_Config_AUX_BACK[0][10-valu];
							
							Tw9912_image_global_AUX_BACK[0][1]=Image_Config_AUX_BACK[0][10-valu];//remember
							ret = write_tw9912(&Tw9912_image);
							break;
						case CONTRAST ://ok
							Tw9912_image[0]=0x11;
							//if(global_video_format_flag = NTSC_I)
							{
								Tw9912_image[1]=Image_Config_AUX_BACK[1][valu];
								Tw9912_image_global_AUX_BACK[1][1]=Image_Config_AUX_BACK[1][valu];
							}
							ret = write_tw9912(&Tw9912_image);
							break;
						case HUE ://bad
							Tw9912_image[0]=0x15;
							//if(global_video_format_flag = NTSC_I)
							{
								Tw9912_image[1]=Image_Config_AUX_BACK[2][valu];
								Tw9912_image_global_AUX_BACK[2][1]=Image_Config_AUX_BACK[2][valu];
							}
							ret = write_tw9912(&Tw9912_image);
							break;
						case SHARPNESS ://bad
						case CHROMA_U :
						case CHROMA_V :
							//if(global_video_format_flag = NTSC_I)
							{
								Tw9912_image[1]=Image_Config_AUX_BACK[3][valu];
								Tw9912_image_global_AUX_BACK[3][1]=Image_Config_AUX_BACK[3][valu];
							}
							Tw9912_image[0]=0x13;
							ret = write_tw9912(&Tw9912_image);
							
								Tw9912_image[1]=Image_Config_AUX_BACK[4][valu];
								Tw9912_image_global_AUX_BACK[4][1]=Image_Config_AUX_BACK[4][valu];
								
							Tw9912_image[0]=0x14;
							ret = write_tw9912(&Tw9912_image);
							break;
					}
				}
		else
			{
			switch (cmd)
				{
					
					case BRIGHTNESS ://ok
						Tw9912_image[0]=0x10;
						Tw9912_image[1]=Image_Config_AUX_BACK_PAL_I[0][10-valu];
						
						Tw9912_image_global_AUX_BACK[0][1]=Image_Config_AUX_BACK_PAL_I[0][10-valu];//remember
						ret = write_tw9912(&Tw9912_image);
						break;
					case CONTRAST ://ok
						Tw9912_image[0]=0x11;
						//if(global_video_format_flag = NTSC_I)
						{
							Tw9912_image[1]=Image_Config_AUX_BACK_PAL_I[1][valu];
							Tw9912_image_global_AUX_BACK[1][1]=Image_Config_AUX_BACK_PAL_I[1][valu];
						}
						ret = write_tw9912(&Tw9912_image);
						break;
					case HUE ://bad
						Tw9912_image[0]=0x15;
						//if(global_video_format_flag = NTSC_I)
						{
							Tw9912_image[1]=Image_Config_AUX_BACK_PAL_I[2][valu];
							Tw9912_image_global_AUX_BACK[2][1]=Image_Config_AUX_BACK_PAL_I[2][valu];
						}
						ret = write_tw9912(&Tw9912_image);
						break;
					case SHARPNESS ://bad
					case CHROMA_U :
					case CHROMA_V :
						//if(global_video_format_flag = NTSC_I)
						{
							Tw9912_image[1]=Image_Config_AUX_BACK_PAL_I[3][valu];
							Tw9912_image_global_AUX_BACK[3][1]=Image_Config_AUX_BACK_PAL_I[3][valu];
						}
						Tw9912_image[0]=0x13;
						ret = write_tw9912(&Tw9912_image);
						
							Tw9912_image[1]=Image_Config_AUX_BACK_PAL_I[4][valu];
							Tw9912_image_global_AUX_BACK[4][1]=Image_Config_AUX_BACK_PAL_I[4][valu];
							
						Tw9912_image[0]=0x14;
						ret = write_tw9912(&Tw9912_image);
						break;
				}
			}
	}
	else if(info_com_top_Channel == YIN2)
	{
			switch (cmd)
			{
				case BRIGHTNESS ://ok
					Tw9912_image[0]=0x10;
					Tw9912_image[1]=Image_Config[0][valu];
					
					Tw9912_image_global[0][1]=Image_Config[0][valu];//remember
					ret = write_tw9912(&Tw9912_image);
					break;
				case CONTRAST ://ok
					if(valu>=5)	valu=10;
					Tw9912_image[0]=0x11;
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config[1][valu];
						Tw9912_image_global[1][1]=Image_Config[1][valu];
					}
					ret = write_tw9912(&Tw9912_image);
					break;
				case HUE ://bad
					Tw9912_image[0]=0x15;
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config[2][valu];
						Tw9912_image_global[2][1]=Image_Config[2][valu];
					}
					ret = write_tw9912(&Tw9912_image);
					break;
				case SHARPNESS ://bad
				case CHROMA_U :
				case CHROMA_V :
					//if(global_video_format_flag = NTSC_I)
					{
						Tw9912_image[1]=Image_Config[3][valu];
						Tw9912_image_global[3][1]=Image_Config[3][valu];
					}
					Tw9912_image[0]=0x13;
					ret = write_tw9912(&Tw9912_image);

						Tw9912_image[1]=Image_Config[4][valu];
						Tw9912_image_global[4][1]=Image_Config[4][valu];
					Tw9912_image[0]=0x14;
					ret = write_tw9912(&Tw9912_image);
					break;

			}
	}
	else //YUV
	{
		switch (cmd)
		{
			case BRIGHTNESS ://ok
				Tw9912_image[0]=0x11;
				Tw9912_image[1]=Image_Config_separation[0][valu];
				
				Tw9912_image_global_separation[0][1]=Image_Config_separation[0][valu];//remember
				ret = write_tw9912(&Tw9912_image);
				break;
			case CONTRAST ://ok	
				if(valu>=5)	valu=10;
				Tw9912_image[0]=0x10;
				//if(global_video_format_flag = NTSC_I)
				{
					Tw9912_image[1]=Image_Config_separation[1][valu];
					Tw9912_image_global_separation[1][1]=Image_Config_separation[1][valu];
				}
				ret = write_tw9912(&Tw9912_image);
				break;
			case HUE ://bad
				Tw9912_image[0]=0x15;
				//if(global_video_format_flag = NTSC_I)
				{
					Tw9912_image[1]=Image_Config_separation[2][valu];
					Tw9912_image_global_separation[2][1]=Image_Config_separation[2][valu];
				}
				ret = write_tw9912(&Tw9912_image);
				break;
			case SHARPNESS ://bad
			case CHROMA_U :
			case CHROMA_V :
				//if(global_video_format_flag = NTSC_I)
				{
					Tw9912_image[1]=Image_Config_separation[3][valu];
					Tw9912_image_global_separation[3][1]=Image_Config_separation[3][valu];
				}
				Tw9912_image[0]=0x13;
				ret = write_tw9912(&Tw9912_image);

					Tw9912_image[1]=Image_Config_separation[4][valu];
					Tw9912_image_global_separation[4][1]=Image_Config_separation[4][valu];
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
printk("tw9912:@@@@@flyVideoInitall_in(Channel=%s)\n",Channel_tab[Channel]);
	if (Channel>=YIN0 &&Channel<=NOTONE)
	{//Channel = SEPARATION;
		info_com_top_Channel = Channel;
		if(Channel == YIN2)//dvd
		{
		info_com_top_Channel = SEPARATION;
		//info_com_top_Channel = YIN0;

		}
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
	switch (Channel)
	{
		case YIN0:
			info_Vedio_Channel = YIN0;
			ret = Tw9912_init(PAL_I,YIN0);
			printk("fly_video_tw9912:Channel selet YIN0\n");
			break;
		case YIN1:
			info_Vedio_Channel = YIN1;
			ret = Tw9912_init(PAL_I,YIN1);
			printk("fly_video_tw9912:Channel selet YIN1\n");
			break;
		case YIN2:
			info_Vedio_Channel = YIN2;
			ret = Tw9912_init(PAL_I,YIN2);
			printk("fly_video_tw9912:Channel selet YIN2\n");
			break;
		case YIN3:
			info_Vedio_Channel = YIN3;
			ret = Tw9912_init(PAL_I,YIN3);
			printk("fly_video_tw9912:Channel selet YIN3\n");
			break;
		case SEPARATION:
			info_Vedio_Channel = SEPARATION;
			ret = Tw9912_init(NTSC_P,SEPARATION);
			printk("fly_video_tw9912:Channel selet SEPARATION\n");
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
	if(
		( (the_last_config.Channel == YIN2 ||the_last_config.Channel == SEPARATION) &&Channel == YIN3)||\
		(the_last_config.Channel == YIN3  && (Channel == YIN2 || Channel == SEPARATION))
	   )
	   {
	   	SOC_Write_Servicer(VIDEO_SHOW_BLACK);
	   	Change_channel();
		
	   }
	if(Channel == SEPARATION||Channel == YIN2)
	{
		ret= testing_NTSCp_video_signal();
	}
	else
	{
		switch (Channel)
		{
			case 0: 
			//	info_Vedio_Channel = YIN0;
				ret=  testing_video_signal(YIN0);	
				break;
			case 1:
			//	info_Vedio_Channel = YIN1;
				ret=  testing_video_signal(YIN1);
				break;
			case 2:
			//	info_Vedio_Channel = YIN2;
				ret=  testing_video_signal(YIN2);
				break;
			case 3:
			//	info_Vedio_Channel = YIN3;
				ret=  testing_video_signal(YIN3);
				break;
			case 4:
			//	info_Vedio_Channel = SEPARATION;
				ret=  testing_video_signal(SEPARATION);
				break;
			default :
			//	info_Vedio_Channel = NOTONE;
				printk("%s:you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
				break;
		}
	}
printk("fly_video HAL Layer teset Channel =%s ,Format = %s\n",Channel_tab[Channel],Format_tab[ret-1]);
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
static int thread_signal_test(void *data) 
{int i=0;
  long int timeout;
  	 printk("tw9912:thread_signal_test()\n");
	while(!kthread_should_stop())
	{
		timeout=10;
		while(timeout > 0) 
		{ //delay
				timeout = schedule_timeout(timeout); 
		} 

		i=read_chips_signal_status(1);
		
		if(i)
		{
			printk("The backlight off!\n");
			SOC_F_LCD_Light_Con(0);
		}
	}
return 0;
}
void video_init_config_in(Vedio_Format config_pramat)
{int i,j;   
printk( "\n\nVideo Module Build Time: %s %s  %s \n\n", __FUNCTION__, __DATE__, __TIME__);
printk("fly_video Tw9912 and Tc358 initall, key parameter info_com_top_Channel = %s.\n",Channel_tab[info_com_top_Channel]);
//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);

SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
		
if(info_com_top_Channel ==SEPARATION||info_com_top_Channel ==YIN2)
	{
printk("fly_video Enter configuration NTSCp branch\n");
Tw9912_init_NTSCp();
VideoImage();
TC358_init(NTSC_P);
	}
else	if(config_pramat != STOP_VIDEO)
	{	
		if(info_com_top_Channel == NOTONE)
		{
		Tw9912_init_PALi();
		printk("TW9912:warning -->info_com_top_Channel == NOTONE,Tw9912 Ignore\n");
		}
	    	else
	    	{
printk("fly_video Enter configuration NTSCi or PALi branch\n");
		mutex_unlock(&lock_chipe_config);
		init_tw9912_ent(info_com_top_Channel);
		mutex_lock(&lock_chipe_config);
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
				if(tw9912_signal_unstabitily_for_Tw9912_init_flag == 1) //find colobar flag signal bad
				{
					printk("video_init_config:video input  signal unstabitily,now shouw RED\n"); 
					 TC358_init(COLORBAR+TC358746XBG_RED);//rea
					 tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;
				}
				else if(tw9912_signal_unstabitily_for_Tw9912_init_flag == 2) //NTSCp not find!
				{
					printk("tw9912:Configure NTSCp but after configuration cannot detect a progressive signal,now shouw TC358746XBG_GREEN\n"); 
					 TC358_init(COLORBAR+TC358746XBG_GREEN);//rea
					 tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;
				}
				else
				{
				//switch (flyVideoSignalPinTest(info_Vedio_Channel))
				if(1){
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
						   TC358_init(COLORBAR+TC358746XBG_BLACK);//blue
						break;
					}
				}
				else
				  TC358_init(COLORBAR);
				/**/
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
	printk("TW9912:warning -->config_pramat == STOP_VIDEO\n");
	TC358_init(COLORBAR);
	  //TC358_init(STOP_VIDEO);
	}
//Signal_Test = kthread_run(thread_signal_test,NULL,"flyvideo_test");  
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
