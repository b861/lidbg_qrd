#include "video_init_config.h"
static int flag_io_config=0;
static Vedio_Channel info_Vedio_Channel = NOTONE;
static Vedio_Channel info_com_top_Channel = YIN2;
extern TW9912_Signal signal_is_how[5];
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
int flyVideoImageQualityConfig_in(Vedio_Effect cmd ,u8 valu)
{
int ret;
u8 Tw9912_image[2]={0,0,};//default input pin selet YIN0ss

	printk("tw9912:@@@@@flyVideoImageQualityConfig_in(cmd =%d,valu=%d)\n",cmd,valu);
	//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
	if(cmd ==0) return valu;
	switch (cmd)
	{
		case BRIGHTNESS :
			Tw9912_image[0]=0x10;
			Tw9912_image[1]=valu;
			printk("cmd = %d, valu = %d",cmd,valu);
			ret = write_tw9912(&Tw9912_image);
			break;
		case CONTRAST :
			Tw9912_image[0]=0x11;
			Tw9912_image[1]=valu;
			printk("cmd = %d, valu = %d",cmd,valu);
			ret = write_tw9912(&Tw9912_image);
			break;
		case SHARPNESS :
			Tw9912_image[0]=0x12;
			Tw9912_image[1]=valu;
			printk("cmd = %d, valu = %d",cmd,valu);
			ret = write_tw9912(&Tw9912_image);
			break;
		case CHROMA_U :
			Tw9912_image[0]=0x13;
			Tw9912_image[1]=valu;
			printk("cmd = %d, valu = %d",cmd,valu);
			ret = write_tw9912(&Tw9912_image);
			break;
		case CHROMA_V :
			Tw9912_image[0]=0x14;
			Tw9912_image[1]=valu;
			printk("cmd = %d, valu = %d",cmd,valu);
			ret = write_tw9912(&Tw9912_image);
			break;
		case HUE :
			Tw9912_image[0]=0x15;
			Tw9912_image[1]=valu;
			printk("cmd = %d, valu = %d",cmd,valu);
			ret = write_tw9912(&Tw9912_image);
			break;
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
printk("tw9912:@@@@@flyVideoInitall_in(Channel=%d)\n",Channel);
mutex_lock(&lock_chipe_config);
	switch (Channel)
	{
		case 0:
			info_com_top_Channel = YIN0;
			//init_tw9912_ent(YIN0);
			break;
		case 1:
			info_com_top_Channel = YIN1;
			//init_tw9912_ent(YIN1);
			break;
		case 2:
			info_com_top_Channel = YIN2;
			//init_tw9912_ent(YIN2);
			break;
		case 3:
			info_com_top_Channel = YIN3;
			//init_tw9912_ent(YIN3);
			break;
		case 4:
			info_com_top_Channel = SEPARATION;
			//init_tw9912_ent(SEPARATION);
			break;
		default :
			info_com_top_Channel = NOTONE;
			//Tw9912_init_PALi();
			printk("%s: you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
			break;
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
printk("tw9912:@@@@@flyVideoTestSignalPin_in(Channel=%d)\n",Channel);
//spin_lock(&spin_chipe_config_lock);
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
//spin_unlock(&spin_chipe_config_lock);
mutex_unlock(&lock_chipe_config);
global_video_format_flag=ret;
return ret;
}
void video_init_config_in(Vedio_Format config_pramat)
{
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
	
	
		printk("\r\n");
		printk("TW9912:info_Vedio_Channel=%d\n",info_Vedio_Channel);
		printk("TW9912:signal_is_how[%d].Channel=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Channel);
		printk("TW9912:signal_is_how[%d].Format=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Format);
		printk("TW9912:signal_is_how[%d].vedio_source=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].vedio_source);
	
		if(info_Vedio_Channel<=SEPARATION)
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
					    TC358_init(COLORBAR);
					break;
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
mutex_unlock(&lock_chipe_config);
}
