#include "video_init_config.h"
static int flag_io_config=0;
static Vedio_Channel info_Vedio_Channel = NOTONE;
static Vedio_Channel info_com_top_Channel = SEPARATION;
extern TW9912_Signal signal_is_how[5];
//spinlock_t spin_chipe_config_lock;
struct mutex lock_chipe_config;
void video_io_i2c_init_in(void)
{
	if (!flag_io_config)
	{	
		//spin_lock_init(&spin_chipe_config_lock);
		mutex_init(&lock_chipe_config);
		i2c_io_config_init();
		flag_io_config=1;
	}
}
int flyVideoImageQualityConfig_in(u8 cmd ,u8 valu)
{
//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
	if(cmd ==0) return valu;
//spin_unlock(&spin_chipe_config_lock);
mutex_unlock(&lock_chipe_config);
	return 0;
}
int init_tw9912_ent(Vedio_Channel Channel);
int flyVideoInitall_in(u8 Channel)
{

int ret=1 ;
//spin_lock(&spin_chipe_config_lock);
mutex_lock(&lock_chipe_config);
	switch (Channel)
	{
		case 0:
			info_com_top_Channel = YIN0;
			init_tw9912_ent(YIN0);
			printk("TW9912:Channel selet YIN0\n");
			break;
		case 1:
			info_com_top_Channel = YIN1;
			init_tw9912_ent(YIN1);
			printk("TW9912:Channel selet YIN1\n");
			break;
		case 2:
			info_com_top_Channel = YIN2;
			init_tw9912_ent(YIN2);
			printk("TW9912:Channel selet YIN2\n");
			break;
		case 3:
			info_com_top_Channel = YIN3;
			init_tw9912_ent(YIN3);
			printk("TW9912:Channel selet YIN3\n");
			break;
		case 4:
			info_com_top_Channel = SEPARATION;
			init_tw9912_ent(SEPARATION);
			printk("TW9912:Channel selet SEPARATION\n");
			break;
		default :
			info_com_top_Channel = NOTONE;
			Tw9912_init_PALi();
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
int flyVideoTestSignalPin_in(u8 Channel)
{int ret= NOTONE;
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
return ret;
}
void video_init_config_in(Vedio_Format config_pramat)
{
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
		init_tw9912_ent(info_com_top_Channel);
	
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
	   ;//TC358_init(STOP_VIDEO);
	}
//spin_unlock(&spin_chipe_config_lock);
mutex_unlock(&lock_chipe_config);
}
