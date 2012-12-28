

#include "i2c_io.h"
#include "tw9912.h"
#include "tw9912_config.h"
#define TW9912_I2C_ChipAdd 0x44 //SIAD = 0-->0x44  SIAD =1-->0x45
TW9912_input_info tw9912_input_information;
TW9912_Signal signal_is_how[5]={//用于记录四个通道的信息
							{Notone,Other,source_other},//Yin0
							{Notone,Other,source_other},//Yin1
							{Notone,Other,source_other},//Yin2
							{Notone,Other,source_other},//Yin3
							{Notone,Other,source_other},//Yuv
							};
TW9912_initall_status tw9912_status={TW9912_initall_not,Notone,Other};
Last_config_t the_last_config ={Notone,Other};
i2c_ack read_tw9912(unsigned int sub_addr, char *buf )
{
	i2c_ack ret;
	ret=i2c_read_byte(1,TW9912_I2C_ChipAdd, sub_addr, buf,1);
return ret;
}
i2c_ack write_tw9912(char *buf )
{	
i2c_ack ret;
		ret=i2c_write_byte(1, TW9912_I2C_ChipAdd,buf, 2);
return ret;
}

void tw9912_get_input_info(TW9912_input_info *input_information)
{
	
	input_information->chip_status1.index = 0x01;	//register index
	input_information->chip_status2.index = 0x31;
	input_information->standard_selet.index = 0x1c;
	input_information->component_video_format.index = 0x1e;
	input_information->macrovision_detection.index = 0x30;
	input_information->input_detection.index = 0xc1;
	
	read_tw9912(input_information->chip_status1.index,\
				&input_information->chip_status1.valu);
	
	read_tw9912(input_information->chip_status2.index,\
				&input_information->chip_status2.valu);
	
	read_tw9912(input_information->standard_selet.index,\
				&input_information->standard_selet.valu);
	
	read_tw9912(input_information->component_video_format.index,\
				&input_information->component_video_format.valu);
	
	read_tw9912(input_information->macrovision_detection.index,\
				&input_information->macrovision_detection.valu);

	read_tw9912(input_information->input_detection.index,\
				&input_information->input_detection.valu);

}
void Tw9912_analysis_input_signal(TW9912_input_info *input_information,Vedio_Channel channel)
{
u8 channel_1;
u8 format_1;

	signal_is_how[channel].Channel =Notone;
	signal_is_how[channel].Format = Other;
	signal_is_how[channel].vedio_source =source_other;
	
	if(input_information->chip_status1.valu & 0x08 )//bit3=1 Vertical logi is locked to the incoming video soruce
	{
		printk("tw9912:Signal is lock<<<<<<<<<<<<<<\n");
		if(input_information->chip_status1.valu & 0x01) signal_is_how[channel].vedio_source=source_50Hz;
		else signal_is_how[channel].vedio_source=source_60Hz;

		if(channel <= Yuv)
		signal_is_how[channel].Channel =channel;
		else
		printk("tw9912:error channal>>>>>>>>>>>>>>>>>>>More than Yuv\n");
		
		read_tw9912(0x02,&channel_1);//register 0x02 channel selete
		channel_1 =(channel_1&0x0c) >>2 ;
		if(channel_1 == channel ||channel == Yuv)
		{
			if(channel != Yuv)
			{
				format_1=input_information->component_video_format.valu & 0x70;
				if(format_1 == 0x00)
					{
					 signal_is_how[channel].Format = NTSC_i;
					}
				else if(format_1 == 0x10)
					{
					signal_is_how[channel].Format = PAL_i;
					}
			}
			else
			{
				format_1=input_information->input_detection.valu&0x07;
				if(format_1 == 0x2)
					{
					signal_is_how[channel].Format = NTSC_p;
					 signal_is_how[channel].Channel =Yuv;
					}
				else if(format_1 == 0x3)
					{
					signal_is_how[channel].Format = PAL_p;
					 signal_is_how[channel].Channel =Yuv;
					}
				else
					{
					printk("tw9912:Signal is lock but testing >>>>>>>>>>>>failed\n");
					}
			}
		
		}
		else
		{
			printk("tw9912:testing channal and config channal >>>>>>>>>>>>not the same");
			signal_is_how[channel].Format = Other;
			signal_is_how[channel].Channel =Notone;
			signal_is_how[channel].vedio_source=source_other;
		}
	}
	else
	{
		printk("\r\rtw9912:testing NOT lock>>>>>>>>>>>\n");
	}
}
void display_tw9912_info(void)
{
u8 i;
tw9912_dbg("\n\r\r\r\r\r\n");
	for(i=0;i<5;i++)
	{

		switch(i)
			{
					case Yin0:  tw9912_dbg("\n\nYin0\n");
						break;
					case Yin1:  tw9912_dbg("\n\nYin1\n");
						break;
					case Yin2: tw9912_dbg("\n\nYin2\n");
						break;
					case Yin3: tw9912_dbg("\n\nYin3\n");
						break;
					case Yuv: tw9912_dbg("\n\nYuv\n");
						break;
			}
		switch(signal_is_how[i].Channel)
			{
					case Yin0: tw9912_dbg("signal_is_how.Channel =Yin0\n");
						break;
					case Yin1: tw9912_dbg("signal_is_how.Channel =Yin1\n");
						break;
					case Yin2: tw9912_dbg("signal_is_how.Channel =Yin2\n");
						break;
					case Yin3: tw9912_dbg("signal_is_how.Channel =Yin3\n");
						break;
					case Yuv: tw9912_dbg("signal_is_how.Channel =Yuv\n");
						break;
					default : tw9912_dbg("signal_is_how.Channel =Notonein\n");
						break;
			}
		switch(signal_is_how[i].Format)
			{
					case PAL_p : tw9912_dbg("signal_is_how.Format = PAL_p\n");
						break;
					case PAL_i: tw9912_dbg("signal_is_how.Format = PAL_i\n");
						break;
					case NTSC_p: tw9912_dbg("ignal_is_how.Format = NTSC_p\n");
						break;
					case NTSC_i: tw9912_dbg("signal_is_how.Format = NTSC_i\n");
						break;
					default: tw9912_dbg("signal_is_how.Format = other\n");
						break;
			}
		switch(signal_is_how[i].vedio_source)
			{
					case source_50Hz: tw9912_dbg("signal_is_how.vedio_source = source_50Hz\n\n");
						break;
					case source_60Hz: tw9912_dbg("signal_is_how.vedio_source = source_60Hz\n\n");
						break;
					case source_other: tw9912_dbg("ignal_is_how.vedio_source = source_other\n\n");
						break;
			}
	}

	tw9912_dbg("chip_status1(%.2x)=%.2x\n",tw9912_input_information.chip_status1.index
									,tw9912_input_information.chip_status1.valu);
	
	tw9912_dbg("chip_status2(%.2x)=%.2x\n",tw9912_input_information.chip_status2.index
									,tw9912_input_information.chip_status2.valu);
	
	tw9912_dbg("standard_selet(%.2x)=%.2x\n",tw9912_input_information.standard_selet.index
									,tw9912_input_information.standard_selet.valu);
	
	tw9912_dbg("component_video_format(%.2x)=%.2x\n",tw9912_input_information.component_video_format.index
									,tw9912_input_information.component_video_format.valu);
	
	tw9912_dbg("macrovision_detection(%.2x)=%.2x\n",tw9912_input_information.macrovision_detection.index,
									tw9912_input_information.macrovision_detection.valu);
	
	tw9912_dbg("input_detection(%.2x)=%.2x\n",tw9912_input_information.input_detection.index,
									tw9912_input_information.input_detection.valu);
/**/

}

int testing_video_signal(Vedio_Channel Channel)
{
int ret =-1;
u8 channel_1;
u8 format_1;
u8 Tw9912_input_pin_selet[]={0x02,0x40,};//default input pin selet YIN0
TW9912_Signal signal_is_how_1;
TW9912_input_info tw9912_input_information_1;

	 tw9912_dbg("testing_signal!\n");

		signal_is_how_1.Channel =Notone;
		signal_is_how_1.Format = Other;
		signal_is_how_1.vedio_source =source_other;
	
	read_tw9912(0x02,&channel_1);//register 0x02 channel selete
	channel_1 =(channel_1&0x0c) >>2 ;//read back now config Channel
	if( (( (channel_1 != Channel ) && Channel != Notone) \
		|| (tw9912_status.flag != TW9912_initall_yes)) \
		&& Channel !=Yuv)//if now config Channel is not testing Channel
																// or tw9912 is not have initall
		{
		if(tw9912_status.flag == TW9912_initall_not)
			{
				tw9912_status.flag = TW9912_initall_yes;
				tw9912_status.format = PAL_i;
				tw9912_status.Channel = Channel;
				tw9912_dbg("first initalll!\n");
				Tw9912_init_PALi();//initall all register 
			}
			
		the_last_config.Channel =Channel;	
		switch(Channel)//Independent testing
			{
				case 0: 	//	 YIN0
					if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
					break;
				case 1: //	 YIN1
					Tw9912_input_pin_selet[1]=0x44;//register valu selete Yin1
					if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
					break;
				case 2: //	 YIN2
					Tw9912_input_pin_selet[1]=0x48;
					if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;

					break;
				case 3: //	 YIN3
					Tw9912_input_pin_selet[1]=0x4c;
					if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
					
					Tw9912_input_pin_selet[0]=0xe8;
					Tw9912_input_pin_selet[1]=0x3f;//disable YOUT buffer
					if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
					break;
				default :
					tw9912_dbg("%s:you input Channel = %d >>>>>>>>>>>>>>error!\n",__FUNCTION__,Channel);
					break;
			}
			msleep(400);//Wait for video Stability
		}
		else if( (Channel == Yuv ) && the_last_config.Channel !=Yuv )
		{
			Tw9912_init_NTSCp();//initall all register 
			msleep(400);
		}
	tw9912_get_input_info(&tw9912_input_information_1);
	if(tw9912_input_information_1.chip_status1.valu & 0x08 )//bit3=1 Vertical logi is locked to the incoming video soruce
	{
		if(tw9912_input_information_1.chip_status1.valu & 0x01)  signal_is_how_1.vedio_source=source_50Hz;
		else signal_is_how_1.vedio_source=source_60Hz;

		switch (Channel)//bit[3:2]
		{
			case 0: signal_is_how_1.Channel =Yin0;
				break;
			case 1: signal_is_how_1.Channel =Yin1;
				break;
			case 2: signal_is_how_1.Channel =Yin2;
				break;
			case 3: signal_is_how_1.Channel =Yin3;
				break;
			default :
				printk("tw9912:*****");
				break;
			
		}
		read_tw9912(0x02,&channel_1);//register 0x02 channel selete
		channel_1 =(channel_1&0x0c) >>2 ;
		if(channel_1 == Channel)
		{
				format_1=tw9912_input_information_1.component_video_format.valu & 0x70;
				if(format_1 == 0x00)
					{
					signal_is_how_1.Format = NTSC_i;
					}
				else if(format_1 == 0x10)
					{
					signal_is_how_1.Format = PAL_i;
					}
				else if(format_1 == 0x20)
					{
					signal_is_how_1.Format = NTSC_p;
					signal_is_how_1.Channel =Yuv;
					}
				else if(format_1 == 0x30)
					{
					signal_is_how_1.Format = PAL_p;
					signal_is_how_1.Channel =Yuv;
					}

			}
			else
			{
				signal_is_how_1.Format = Other;
				signal_is_how_1.Channel =Notone;
				signal_is_how_1.vedio_source=source_other;
			}
		}
 	switch(signal_is_how_1.Format)
 		{
			case NTSC_i:   ret =1;
				break;
			case PAL_i:      ret =2;
				break;
				
			case NTSC_p:  ret =3;
				break;
			case PAL_p:     ret =4;
				break;
				
			default:     ret =5;
				break;
		}
	return ret;
CONFIG_not_ack_fail:
	tw9912_dbg("testing_video_signal()--->NACK error\n");
	ret =-1;
	return ret;
}
int Tw9912_appoint_pin_testing_video_signal(Vedio_Channel Channel)
{
int ret =-1;
u8 channel_1;
u8 Tw9912_input_pin_selet[]={0x02,0x40,};//default input pin selet YIN0
u8 manually_initiate_auto_format_detection[]={0x1d,0xff,};//default input pin selet YIN0
tw9912_dbg("Tw9912_appoint_pin_testing_video_signal!\n");

	if(Channel !=Yuv)
	{
		read_tw9912(0x02,&channel_1);//register 0x02 channel selete
		channel_1 =(channel_1&0x0c) >>2 ;//read back now config Channel
		if( ( (channel_1 != Channel ) && Channel != Notone) \
			|| (tw9912_status.flag != TW9912_initall_yes) )//if now config Channel is not testing Channel
																	// or tw9912 is not have initall
			{
				if(tw9912_status.flag == TW9912_initall_not )
					{
						tw9912_status.flag = TW9912_initall_yes;
						tw9912_status.Channel = Channel;
						tw9912_status.format = PAL_i;
						tw9912_dbg("first initalll!\n");
						Tw9912_init_PALi();//initall all register 
					}
					
			the_last_config.Channel =Channel;	
			switch(Channel)//Independent testing
				{
					case Yin0: 	//	 YIN0
						if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
						break;
					case  Yin1: //	 YIN1
						Tw9912_input_pin_selet[1]=0x44;//register valu selete Yin1
						if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
						break;
					case  Yin2: //	 YIN2
						Tw9912_input_pin_selet[1]=0x48;
						if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;

						break;
					case  Yin3: //	 YIN3
						Tw9912_input_pin_selet[1]=0x4c;
						if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
						
						Tw9912_input_pin_selet[0]=0xe8;
						Tw9912_input_pin_selet[1]=0x3f;//disable YOUT buffer
						if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
						break;
					default :
						tw9912_dbg("%s:you input Channel = %d error!\n",__FUNCTION__,Channel);
						break;
						
				}
				msleep(400);//Wait for video Stability
			}
	}
	//else if( the_last_config.Channel !=Yuv )
	else
	{
		printk("\r\r\n\n");
		printk("tw9912:testing NTSCp\n");
		Tw9912_init_NTSCp();//initall all register 
		tw9912_status.flag = TW9912_initall_yes;
		tw9912_status.Channel = Yuv;
		tw9912_status.format = NTSC_p;
		msleep(400);
	}
	write_tw9912(manually_initiate_auto_format_detection);
	msleep(100);
	tw9912_get_input_info(&tw9912_input_information);
	Tw9912_analysis_input_signal(&tw9912_input_information,Channel);
 	display_tw9912_info();
 	switch(signal_is_how[Channel].Format)
 		{
			case NTSC_i:   ret =1;
				break;
			case PAL_i:      ret =2;
				break;
				
			case NTSC_p:  ret =3;
				break;
			case PAL_p:     ret =4;
				break;
				
		 default:     ret =5;
				break;
		}
	return ret;
CONFIG_not_ack_fail:
	tw9912_dbg("Tw9912_appoint_pin_testing_video_signal()--->NACK error\n");
	ret =-1;
	return ret;
}
static void TC9912_id(void)
{	
u8 TW9912_ID[]={0x00,0x00};
u8 valu;
		read_tw9912(TW9912_ID[0], &TW9912_ID[1]);
		valu = TW9912_ID[1]>>4;//ID = 0x60
		if( valu ==0x6)
		printk("TW9912 ID=0x%.2x\n",TW9912_ID[1]);
		else 
		printk("TW9912 Communication error!(0x%.2x)\n",TW9912_ID[1]);

}

int Tw9912_init_NTSCp(void)
{
    u32 i = 0;
    u8 *config_pramat_piont=NULL;
	tw9912_dbg("Tw9912_init_NTSCp initall tw9912+\n");
	TC9912_id();
	tw9912_RESX_DOWN;
	tw9912_RESX_UP;   
	the_last_config.Channel =Yuv;
	the_last_config.format = NTSC_p;
	config_pramat_piont=TW9912_INIT_NTSC_progressive_input; 
	while(TW9912_INIT_Public[i*2] != 0xfe)
		{    
			if(write_tw9912(&TW9912_INIT_Public[i*2])==NACK) goto CONFIG_not_ack_fail;
			i++;
		}	
	while(config_pramat_piont[i*2] != 0xfe)
	{    
		if(write_tw9912(&config_pramat_piont[i*2])==NACK) goto CONFIG_not_ack_fail;
		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
		i++;
	}
	tw9912_dbg("Tw9912_init_NTSCp initall tw9912-\n");
//msleep(400);
//Tw9912_appoint_pin_testing_video_signal(Yuv);
   return 1;
CONFIG_not_ack_fail:
	tw9912_dbg("%s:have NACK error!\n",__FUNCTION__);
   return -1;
}
int Tw9912_init_PALi(void)
{
    u32 i = 0;
    u8 *config_pramat_piont=NULL;
	tw9912_dbg("Tw9912_init_PALi initall tw9912+\n");
	TC9912_id();
	tw9912_RESX_DOWN;
	tw9912_RESX_UP;   
	the_last_config.Channel =Yin3;
	the_last_config.format = PAL_i;
	config_pramat_piont=TW9912_INIT_PAL_Interlaced_input; 
	while(TW9912_INIT_Public[i*2] != 0xfe)
		{    
			if(write_tw9912(&TW9912_INIT_Public[i*2])==NACK) goto CONFIG_not_ack_fail;
			i++;
		}	
	while(config_pramat_piont[i*2] != 0xfe)
	{    
		if(write_tw9912(&config_pramat_piont[i*2])==NACK) goto CONFIG_not_ack_fail;
		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
		i++;
	}
	tw9912_dbg("Tw9912_appoint_pin_testing_video_signal initall tw9912-\n");
   return 1;
CONFIG_not_ack_fail:
	tw9912_dbg("%s:have NACK error!\n",__FUNCTION__);
   return -1;
}
int Tw9912_init(Vedio_Format config_pramat,Vedio_Channel Channel)
{   

    u32 i = 0;
    int ret=0;
    u8 *config_pramat_piont=NULL;
    u8 Tw9912_input_pin_selet[]={0x02,0x40,};//default input pin selet YIN0
	printk("tw9912: init+\n");
	printk("\r\r\n\n");
	TC9912_id();
	//if(Channel == Notone&&tw9912_status.flag == TW9912_initall_not)
	if(Channel == Notone)
	{ //随便初始一下，防止i2c由于tw9912不工作影响通信。
	Tw9912_init_PALi();//initall all register
	//神码情况
	}
	else if(config_pramat != STOP_Vedio)
	{
		ret = Tw9912_appoint_pin_testing_video_signal(Channel);//bad
		if(ret==5)//the channel is not signal input
			goto NOT_signal_input;
		if(ret==-1)
			goto CONFIG_not_ack_fail;

		switch(ret)
		{
			case NTSC_i:
				printk("tw9912:testing NTSC_i\n");
				break;
			case NTSC_p:
				printk("tw9912:testing NTSC_p\n");
				break;
			case PAL_i:
				printk("tw9912:testing PAL_i\n");
				break;
			case PAL_p:
				printk("tw9912:testing PAL_p\n");
				break;
			default:
				;
				break;
			
		}
/*		
if(tw9912_status.flag == TW9912_initall_yes &&\
		signal_is_how[Channel].Format == tw9912_status.format ) //now config is old config
goto CONFIG_is_old;
*/			
		tw9912_RESX_DOWN;
		tw9912_RESX_UP;   
		switch(signal_is_how[Channel].Format)
		{
			case NTSC_i:
				tw9912_status.flag = TW9912_initall_yes;
				tw9912_status.Channel = Channel;
				tw9912_status.format = NTSC_i;
				config_pramat_piont=TW9912_INIT_NTSC_Interlaced_input; 
				//config_pramat_piont=TW9912_INIT_PAL_Interlaced_input; 
				tw9912_dbg("%s:config_pramat->NTSC_Interlace\n",__func__);
				break;
			
			case PAL_i:
				tw9912_status.flag = TW9912_initall_yes;
				tw9912_status.Channel = Channel;
				tw9912_status.format = PAL_i;
				config_pramat_piont=TW9912_INIT_PAL_Interlaced_input; 
				printk("%s:config_pramat->PAL_Interlace\n",__func__);
				break;
			
			case NTSC_p:
				tw9912_status.flag = TW9912_initall_yes;
				tw9912_status.Channel = Channel;
				tw9912_status.format = NTSC_p;
				config_pramat_piont=TW9912_INIT_NTSC_progressive_input;
				printk("%s:config_pramat->NTSC_Progressive\n",__func__);
				break;
			
			case PAL_p:
				tw9912_status.flag = TW9912_initall_yes;
				tw9912_status.Channel = Channel;
				tw9912_status.format = PAL_p;
				config_pramat_piont=TW9912_INIT_PAL_progressive_input;
				printk("%s:config_pramat->PAL_Progressive\n",__func__);
				break;
			default:
				printk("Format is Invalid ******\n");
				printk("%s:signal_is_how[Channel].Format=%d\n",__func__,signal_is_how[Channel].Format);
				goto NOT_signal_input;
				break;
		}
		
		the_last_config.Channel =Channel;
		the_last_config.format = signal_is_how[Channel].Format;
		
		while(TW9912_INIT_Public[i*2] != 0xfe)
			{    
				if(write_tw9912(&TW9912_INIT_Public[i*2])==NACK) goto CONFIG_not_ack_fail;
				i++;
			}	
		//for(;i<SIZE_OF_ARRAY(config_pramat_piont)/2;i++)
		while(config_pramat_piont[i*2] != 0xfe)
			{    
				if(write_tw9912(&config_pramat_piont[i*2])==NACK) goto CONFIG_not_ack_fail;
				tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
				i++;
			}
		
			if(signal_is_how[Channel].Format ==PAL_i ||signal_is_how[Channel].Format == NTSC_i)
			{
				switch(Channel)//Independent testing
					{
						case 0: 	//	 YIN0
							if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
							break;
						case 1: //	 YIN1
							Tw9912_input_pin_selet[1]=0x44;//register valu selete Yin1
							if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
							break;
						case 2: //	 YIN2
							Tw9912_input_pin_selet[1]=0x48;
							if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;

							break;
						case 3: //	 YIN3
							Tw9912_input_pin_selet[1]=0x4c;
							if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
				
							Tw9912_input_pin_selet[0]=0xe8;//only selet Yin3 neet set
							Tw9912_input_pin_selet[1]=0x3f;//disable YOUT buffer
							if(write_tw9912(Tw9912_input_pin_selet)==NACK) goto CONFIG_not_ack_fail;
							break;
						default : 
							tw9912_dbg("%s:you input Channel = %d error!\n",__FUNCTION__,Channel);
							break;
					}
			}
		//	msleep(400);//wait for vedio signal Stable 
	}
	/*else
	{
		tw9912_status.flag = TW9912_initall_not;
		tw9912_status.Channel = Notone;
		tw9912_status.format = Other;
		config_pramat_piont=TW9912_Stop;
		tw9912_dbg("%s:config_pramat->STOP_Vedio\n",__func__);
	}
	*/
#ifdef DEBUG_PLOG_TW9912
	i=0;
	while(config_pramat_piont[i*2] != 0xfe)
	{  u8 data = 0;
		data = 0xff;
		read_tw9912(config_pramat_piont[i*2], &data);
		tw9912_dbg("r a=%x ,v= %x\n",config_pramat_piont[i*2] , data);
		i++;
	}
#endif
	printk("tw9912: init-\n");

//CONFIG_is_old:
    return 1;
CONFIG_not_ack_fail:
	tw9912_dbg("%s:have NACK error!\n",__FUNCTION__);
   return -2;
NOT_signal_input:
	tw9912_dbg("%s:the channal=%d not have video signal!\n",__FUNCTION__,Channel);
   return -1;
}













