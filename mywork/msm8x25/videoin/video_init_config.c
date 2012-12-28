#include "video_init_config.h"
static int flag_io_config=0;
static Vedio_Channel info_Vedio_Channel = Notone;
static Vedio_Channel info_com_top_Channel = Yin3;
extern TW9912_Signal signal_is_how[5];
spinlock_t spin_chipe_config_lock;
void video_io_i2c_init_in(void)
{
	if (!flag_io_config)
	{	
		spin_lock_init(&spin_chipe_config_lock);
		i2c_io_config_init();
		flag_io_config=1;
	}
}
int flyVideoImageQualityConfig_in(u8 cmd ,u8 valu)
{
spin_lock(&spin_chipe_config_lock);
	if(cmd ==0) return valu;
spin_unlock(&spin_chipe_config_lock);
	return 0;
}
int init_tw9912_ent(Vedio_Channel Channel);
int flyVideoInitall_in(u8 Channel)
{

int ret=1 ;
spin_lock(&spin_chipe_config_lock);
	switch (Channel)
	{
		case 0:
			info_com_top_Channel = Yin0;
			init_tw9912_ent(Yin0);
			printk("TW9912:Channel selet Yin0\n");
			break;
		case 1:
			info_com_top_Channel = Yin1;
			init_tw9912_ent(Yin1);
			printk("TW9912:Channel selet Yin1\n");
			break;
		case 2:
			info_com_top_Channel = Yin2;
			init_tw9912_ent(Yin2);
			printk("TW9912:Channel selet Yin2\n");
			break;
		case 3:
			info_com_top_Channel = Yin3;
			init_tw9912_ent(Yin3);
			printk("TW9912:Channel selet Yin3\n");
			break;
		case 4:
			info_com_top_Channel = Yuv;
			init_tw9912_ent(Yuv);
			printk("TW9912:Channel selet Yuv\n");
			break;
		default :
			info_com_top_Channel = Notone;
			Tw9912_init_PALi();
			printk("%s: you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
			break;
	}
spin_unlock(&spin_chipe_config_lock);
return ret;
//success return 1 fail return -1
}
int init_tw9912_ent(Vedio_Channel Channel)
{
int ret=-1 ;
	switch (Channel)
	{
		case Yin0:
			info_Vedio_Channel = Yin0;
			ret = Tw9912_init(PAL_i,Yin0);
			printk("TW9912:Channel selet Yin0\n");
			break;
		case Yin1:
			info_Vedio_Channel = Yin1;
			ret = Tw9912_init(PAL_i,Yin1);
			printk("TW9912:Channel selet Yin1\n");
			break;
		case Yin2:
			info_Vedio_Channel = Yin2;
			ret = Tw9912_init(PAL_i,Yin2);
			printk("TW9912:Channel selet Yin2\n");
			break;
		case Yin3:
			info_Vedio_Channel = Yin3;
			ret = Tw9912_init(PAL_i,Yin3);
			printk("TW9912:Channel selet Yin3\n");
			break;
		case Yuv:
			info_Vedio_Channel = Yuv;
			ret = Tw9912_init(NTSC_p,Yuv);
			printk("TW9912:Channel selet Yuv\n");
			break;
		default :
			printk("%s: you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
			break;
	}
return ret;
//success return 1 fail return -1
}
int flyVideoTestSignalPin_in(u8 Channel)
{int ret= Notone;
spin_lock(&spin_chipe_config_lock);
	switch (Channel)
	{
		case 0: 
			info_Vedio_Channel = Yin0;
			ret=  testing_video_signal(Yin0);	
			break;
		case 1:
			info_Vedio_Channel = Yin1;
			ret=  testing_video_signal(Yin1);
			break;
		case 2:
			info_Vedio_Channel = Yin2;
			ret=  testing_video_signal(Yin2);
			break;
		case 3:
			info_Vedio_Channel = Yin3;
			ret=  testing_video_signal(Yin3);
			break;
		case 4:
			info_Vedio_Channel = Yuv;
			ret=  testing_video_signal(Yuv);
			break;
		default :
			info_Vedio_Channel = Notone;
			printk("%s:you input TW9912 Channel=%d error!\n",__FUNCTION__,Channel);
			break;
	}
spin_unlock(&spin_chipe_config_lock);
return ret;
}
void video_init_config_in(Vedio_Format config_pramat)
{
spin_lock(&spin_chipe_config_lock);
	if(config_pramat != STOP_Vedio)
	{	
		if(info_com_top_Channel == Notone)
		{
		Tw9912_init_PALi();
		printk("TW9912:warning -->info_com_top_Channel == Notone,Tw9912 Ignore\n");
		}
	    	else
		init_tw9912_ent(info_com_top_Channel);
	
		printk("\r\n");
		printk("TW9912:info_Vedio_Channel=%d\n",info_Vedio_Channel);
		printk("TW9912:signal_is_how[%d].Channel=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Channel);
		printk("TW9912:signal_is_how[%d].Format=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Format);
		printk("TW9912:signal_is_how[%d].vedio_source=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].vedio_source);
	
		if(info_Vedio_Channel<=Yuv)
		{

				//switch (flyVedioTestSignalPin(info_Vedio_Channel))
				switch (signal_is_how[info_Vedio_Channel].Format)
				{
				case NTSC_i: TC358_init(NTSC_i);
					   //TC358_init(PAL_Interlace);
					break;
				case PAL_i: TC358_init(PAL_i);
					break;
				case NTSC_p: TC358_init(NTSC_p);
					break;
				case PAL_p: TC358_init(PAL_p);
					break;
				default :printk("video not signal input\n"); 
					    TC358_init(Colorbar);
					break;
				}
			
		}//if(info_Vedio_Channel<=Yuv)
		else
		{
			printk("Video_init_config:TW9912 not config!\n"); 
			 TC358_init(Colorbar);
		}
		
	}
	else
	{
	   ;//TC358_init(STOP_Vedio);
	}
spin_unlock(&spin_chipe_config_lock);
}
