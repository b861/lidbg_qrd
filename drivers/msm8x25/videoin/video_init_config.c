#include "video_init_config.h"
#include "video_init_config_tab.h"
static int flag_io_config = 0;
static Vedio_Channel info_Vedio_Channel = NOTONE;
static Vedio_Channel info_com_top_Channel = YIN2;
extern TW9912_Signal signal_is_how[5];
extern Last_config_t the_last_config;
extern TW9912Info global_tw9912_info;
//static struct task_struct *Signal_Test = NULL;
static u8 flag_now_config_channal_AUX_or_Astren=0; //0 is Sstren 1 is AUX

static TW9912_Image_Parameter TW9912_Image_Parameter_fly[6] = {
																{BRIGHTNESS,5},
																{CONTRAST,5},
																{SHARPNESS,5},
																{CHROMA_U,5},
																{CHROMA_V,5},
																{HUE,5},
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
        flag_io_config = 1;
    }
}
void VideoReset_in(void)
{
    printk("reset tw9912 and tc358746\n");
    //Tw9912Reset_in();
    TC358_Hardware_Rest();
}
int static Change_channel(void)
{
    //   mutex_lock(&lock_chipe_config);
    printk("TC358:Change_channel() \n");
    //Disabel_video_data_out();//tw9912

    TC358_data_output_enable(DISABLE);
    //TC358_init(COLORBAR + TC358746XBG_BLACK);
    //msleep(1);
    //TC358_data_output_enable(ENABLE);
    printk("%s:tw9912_RESX_DOWN\n", __func__);

    tw9912_RESX_DOWN;//\u8fd9\u91cc\u5bf9tw9912\u590d\u4f4d\u7684\u539f\u56e0\u662f\u89e3\u51b3\u5012\u8f66\u9000\u56deDVD\u65f6\u89c6\u9891\u5361\u6b7b\u3002
    tw9912_RESX_UP;
    //msleep(20);
    //  mutex_unlock(&lock_chipe_config);
    return 0;
}
int static VideoImageParameterConfig(void)
{
	if (info_com_top_Channel == YIN3)
	{
	/**************************************Astren************************************************/
		if(TW9912_Image_Parameter_fly[1].valu == 240)
		{
		printk("Astern\n");
	//	SOC_Write_Servicer(VIDEO_PASSAGE_ASTERN);
		flag_now_config_channal_AUX_or_Astren = 0;
		     if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
		        {
		        Tw9912_image_global_AUX_BACK[0][1] = 0x00;//honda xiyu is 31 // is good 00
		        Tw9912_image_global_AUX_BACK[1][1] = 0x58;//honda xiyu 5c //is good 58
			 Tw9912_image_global_AUX_BACK[2][1] = 0x00;
		        Tw9912_image_global_AUX_BACK[3][1] = 0xdf;
			 Tw9912_image_global_AUX_BACK[4][1] = 0xdf;
			// printk("Tw9912_image_global_AUX_BACK reset valu from NTSC_I\n");
		     	 }
		     else//PALi
		     	{
		        Tw9912_image_global_AUX_BACK_PAL_I[0][1]= 0x00;// is good 00
		        Tw9912_image_global_AUX_BACK_PAL_I[1][1]= 0x58;//is good 58
		        Tw9912_image_global_AUX_BACK_PAL_I[2][1]= 0x00;
		        Tw9912_image_global_AUX_BACK_PAL_I[3][1]= 0xdf;
		        Tw9912_image_global_AUX_BACK_PAL_I[4][1]= 0xdf;
			// printk("Tw9912_image_global_AUX_BACK reset valu from PAL_I\n");
			 }
			 return 1;
		}
		else if(TW9912_Image_Parameter_fly[1].valu> 10)
		{
			printk("Error at %s you input valu = %d paramter have Problems",__func__,TW9912_Image_Parameter_fly[1].valu);
			return -1;
		}
	/**************************************AUX********************************************/
		else
		{
			printk("AUX\n");
		//	SOC_Write_Servicer(VIDEO_PASSAGE_AUX);
		   	 flag_now_config_channal_AUX_or_Astren = 1;
		        if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
		        {u8 i =0;
			        for (i = BRIGHTNESS;i<=HUE;i++)
			        { 
			        switch (i)
					{
						case BRIGHTNESS:	
						Tw9912_image_global_AUX_BACK[0][1] =\
							Image_Config_AUX_BACK[0][10-TW9912_Image_Parameter_fly[BRIGHTNESS-1].valu];
							break;
						case CONTRAST:
						Tw9912_image_global_AUX_BACK[1][1] =\
							Image_Config_AUX_BACK[1][TW9912_Image_Parameter_fly[CONTRAST-1].valu];
							break;
						case HUE:
						Tw9912_image_global_AUX_BACK[3][1] =\
							Image_Config_AUX_BACK[3][TW9912_Image_Parameter_fly[CHROMA_U-1].valu];
							Tw9912_image_global_AUX_BACK[4][1] =\
						Image_Config_AUX_BACK[4][TW9912_Image_Parameter_fly[CHROMA_U-1].valu];
							break;
						case CHROMA_U:
						Tw9912_image_global_AUX_BACK[2][1] =\
							Image_Config_AUX_BACK[2][10-TW9912_Image_Parameter_fly[HUE-1].valu];
							break;
						default :
							break;
					}
				}	            
		        }
		        else//PAL_i
		        {
		        u8 i =0;
			        for (i = BRIGHTNESS;i<=HUE;i++)
			        { 
					switch (i)
					{
						case BRIGHTNESS:	
						Tw9912_image_global_AUX_BACK_PAL_I[0][1] =\
							Image_Config_AUX_BACK_PAL_I[0][10-TW9912_Image_Parameter_fly[BRIGHTNESS-1].valu];
							break;
						case CONTRAST:
						Tw9912_image_global_AUX_BACK_PAL_I[1][1] =\
							Image_Config_AUX_BACK_PAL_I[1][TW9912_Image_Parameter_fly[CONTRAST-1].valu];
							break;
						case CHROMA_U:
						Tw9912_image_global_AUX_BACK_PAL_I[3][1] =\
							Image_Config_AUX_BACK_PAL_I[3][TW9912_Image_Parameter_fly[CHROMA_U-1].valu];
						Tw9912_image_global_AUX_BACK_PAL_I[4][1] =\
							Image_Config_AUX_BACK_PAL_I[4][TW9912_Image_Parameter_fly[CHROMA_U-1].valu];
							break;
						case HUE:
						Tw9912_image_global_AUX_BACK_PAL_I[2][1] =\
							Image_Config_AUX_BACK_PAL_I[2][10-TW9912_Image_Parameter_fly[HUE-1].valu];
							break;
						default :
							break;
					}
				}	            
		        }
			return 1;
		}
	}
	/**************************************DVDS********************************************/
	 else 
	{u8 i =0;
		printk("DVD\n");
	//	SOC_Write_Servicer(VIDEO_PASSAGE_DVD);
	        for (i = BRIGHTNESS;i<=HUE;i++)
	        {
			switch (i)
			{
			case  CONTRAST:	
				Tw9912_image_global_separation[0][1] =\
					Image_Config_separation[0][TW9912_Image_Parameter_fly[BRIGHTNESS-1].valu];
				break;
			case BRIGHTNESS:
				Tw9912_image_global_separation[1][1] =\
					Image_Config_separation[1][TW9912_Image_Parameter_fly[CONTRAST-1].valu];
				break;
			case HUE:
				Tw9912_image_global_separation[2][1] =\
					Image_Config_separation[2][TW9912_Image_Parameter_fly[HUE-1].valu];
				break;
			case CHROMA_U:
				Tw9912_image_global_separation[3][1] =\
					Image_Config_separation[3][TW9912_Image_Parameter_fly[CHROMA_U-1].valu];
				Tw9912_image_global_separation[4][1] =\
					Image_Config_separation[4][TW9912_Image_Parameter_fly[CHROMA_U-1].valu];
				break;
			default :
				break;
			}
		}
		  
	}
return 1;
}
int static VideoImage(void)
{
    int ret;
    register int i = 0;
    u8 Tw9912_image[2] = {0x17, 0x87,}; //default input pin selet YIN0ss
    printk("VideoImage()\n");
    VideoImageParameterConfig();
    for(i = 0; i < 5; i++)
	    {
	        if(info_com_top_Channel == YIN3)//back or AUX
	        {
	            if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
	                ret = write_tw9912((char *)&Tw9912_image_global_AUX_BACK[i]);
	            else
	                ret = write_tw9912((char *)&Tw9912_image_global_AUX_BACK_PAL_I[i]);
	        }
	        else //DVD SEPARATION
	            ret = write_tw9912((char *)&Tw9912_image_global_separation[i]);
    }

    if(flag_now_config_channal_AUX_or_Astren == 0)//Astren
	{
		if(signal_is_how[info_com_top_Channel].Format == NTSC_I)
			{
				Tw9912_image[0] = 0x12;
				Tw9912_image[1] = 0x1f;
				ret = write_tw9912((char *)&Tw9912_image);
				Tw9912_image[0] = 0x08;
				Tw9912_image[1] = global_tw9912_info.reg_val;//form qcamerahwi_preview.cpp
				ret = write_tw9912((char *)&Tw9912_image);
			}
		else
			{
				Tw9912_image[0] = 0x12;
				Tw9912_image[1] = 0xff;
				ret = write_tw9912((char *)&Tw9912_image);
			}
	}

#ifdef BOARD_V1
	    if(info_com_top_Channel == YIN3)// back or AUX
	    {
	        ret = write_tw9912((char *)Tw9912_image);
	        Tw9912_image[0] = 0x08;
	        Tw9912_image[1] = 0x14; // image down 5 line
	        ret = write_tw9912((char *)Tw9912_image);
	        Tw9912_image[0] = 0x0a;
	        Tw9912_image[1] = 0x22; // image down 5 line
	        ret = write_tw9912((char *)Tw9912_image);
	    }
	    else if(info_com_top_Channel == YIN2)//DVD
	    {
	        u8 Tw9912_image[2] = {0x08, 0x1a,}; //image reft 5 line
	        ret = write_tw9912((char *)Tw9912_image);
	    }
	    else
	    {
	        ;
	    }
#endif
return ret;
}
int flyVideoImageQualityConfig_in(Vedio_Effect cmd , u8 valu)
{
    printk("flyVideoImage(%d,%d)\n", cmd, valu);
	switch(cmd)
	{
		case CONTRAST:TW9912_Image_Parameter_fly[CONTRAST-1].valu = valu;
			break;
		case BRIGHTNESS:TW9912_Image_Parameter_fly[BRIGHTNESS-1].valu= valu;
			break;
		case SHARPNESS:TW9912_Image_Parameter_fly[SHARPNESS-1].valu = valu;
			break;
		case CHROMA_U:TW9912_Image_Parameter_fly[CHROMA_U-1].valu= valu;
			break;
		case CHROMA_V:TW9912_Image_Parameter_fly[CHROMA_V-1].valu = valu;
			break;
		case HUE:TW9912_Image_Parameter_fly[HUE-1].valu = valu;
			break;
		default : printk("Error at %s you input cmd = %d paramter have Problems",__func__,cmd);
			break;
	}
	if(cmd == BRIGHTNESS)//wait all set doen befor application
		VideoImage();
return 0;
}
int init_tw9912_ent(Vedio_Channel Channel);
int flyVideoInitall_in(u8 Channel)
{
    int ret = 1 ;
    //spin_lock(&spin_chipe_config_lock);
    mutex_lock(&lock_chipe_config);
    printk("flyVideoInitall_in(%d)\n", Channel);
    if (Channel >= YIN0 && Channel <= NOTONE)
    {
        //Channel = SEPARATION;
        info_com_top_Channel = Channel;
        if(Channel == YIN2)//dvd
        {
            info_com_top_Channel = SEPARATION;
            //info_com_top_Channel = YIN0;
	    // SOC_Write_Servicer(VIDEO_PASSAGE_DVD);
	//            global_video_channel_flag = DVD;
        }
    }
    else
    {
        info_com_top_Channel = NOTONE;
   //     global_video_channel_flag = OTHER;
        printk("%s: you input TW9912 Channel=%d error!\n", __FUNCTION__, Channel);
    }
    //spin_unlock(&spin_chipe_config_lock);
    mutex_unlock(&lock_chipe_config);
    return ret;
    //success return 1 fail return -1
}
int init_tw9912_ent(Vedio_Channel Channel)
{
    int ret = -1 ;
    video_config_debug("init_tw9912_ent(%d)\n", Channel);
    switch (Channel)
    {
    case YIN0:
        info_Vedio_Channel = YIN0;
        ret = Tw9912_init(PAL_I, YIN0);
        video_config_debug("TW9912:Channel selet YIN0\n");
        break;
    case YIN1:
        info_Vedio_Channel = YIN1;
        ret = Tw9912_init(PAL_I, YIN1);
        video_config_debug("TW9912:Channel selet YIN1\n");
        break;
    case YIN2:
        info_Vedio_Channel = YIN2;
        ret = Tw9912_init(PAL_I, YIN2);
        video_config_debug("TW9912:Channel selet YIN2\n");
        break;
    case YIN3:
        info_Vedio_Channel = YIN3;
        ret = Tw9912_init(PAL_I, YIN3);
        video_config_debug("TW9912:Channel selet YIN3\n");
        break;
    case SEPARATION:
        info_Vedio_Channel = SEPARATION;
        ret = Tw9912_init(NTSC_P, SEPARATION);
        video_config_debug("TW9912:Channel selet SEPARATION\n");
        break;
    default :
        video_config_debug("%s: you input TW9912 Channel=%d error!\n", __FUNCTION__, Channel);
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
{
    static Vedio_Format printk_count_flag_next = 0;
    static u8 printk_format_count =0;
    Vedio_Format ret = NOTONE;
    static u8 Format_count = 0;
    static u8 Format_count_flag = 0;
    //spin_lock(&spin_chipe_config_lock);
    //  return NTSC_I;
    mutex_lock(&lock_chipe_config);
    if(
        ( (the_last_config.Channel == YIN2 || the_last_config.Channel == SEPARATION) && Channel == YIN3) || \
        (the_last_config.Channel == YIN3  && (Channel == YIN2 || Channel == SEPARATION))
    )
    {
        //SOC_Write_Servicer(VIDEO_SHOW_BLACK);
        Change_channel();

    }

 //  mutex_unlock(&lock_chipe_config);
//  return NTSC_I;
    if(Channel == SEPARATION || Channel == YIN2)
    {
        ret = testing_NTSCp_video_signal();
    }
    else
    {static u8 goto_agian_test = 0;
AGAIN_TEST_FOR_BACK_NTSC_I:    
	goto_agian_test ++;
        switch (Channel)
        {
        case 0:
            //	info_Vedio_Channel = YIN0;
            ret =  testing_video_signal(YIN0);
            break;
        case 1:
            //	info_Vedio_Channel = YIN1;
            ret =  testing_video_signal(YIN1);
            break;
        case 2:
            //	info_Vedio_Channel = YIN2;
            ret =  testing_video_signal(YIN2);
            break;
        case 3:
            //	info_Vedio_Channel = YIN3;
            ret =  testing_video_signal(YIN3);
            break;
        case 4:
            //	info_Vedio_Channel = SEPARATION;
            ret =  testing_video_signal(SEPARATION);
            break;
        default :
            //	info_Vedio_Channel = NOTONE;
            printk("%s:you input TW9912 Channel=%d error!\n", __FUNCTION__, Channel);
            break;
        }
		if(ret == 1 && goto_agian_test <= 2 ) 
			goto AGAIN_TEST_FOR_BACK_NTSC_I; 
		else goto_agian_test = 0;
    }
	if (printk_count_flag_next != ret || printk_format_count > 20)
		{
		printk_format_count =0;
		printk_count_flag_next = ret;
    		printk("C=%d,F=%d\n", Channel, ret);
		}
	else printk_format_count++;
    //spin_unlock(&spin_chipe_config_lock);
    mutex_unlock(&lock_chipe_config);
    //global_video_format_flag=ret;//Transmitted Jiang  Control
    
    if( ret > 4 ) 
	{
		Format_count ++;
		if(Format_count > 30 && Format_count_flag == 0)
		{
			  Format_count = 0 ;
			  Format_count_flag = 1;
			  printk("warning : Test Input Signal Fail Now Reconfig Tw9912 \n");
			 // video_init_config_in( PAL_P);
			if(info_com_top_Channel == SEPARATION || info_com_top_Channel == YIN2)
				Tw9912_init_NTSCp();
			else
	        		Tw9912_init(PAL_I, YIN3);
		  
		}
	}
	else
	{
		Format_count = 0;
		Format_count_flag = 0;//When the AUX does not enter ,you can not repeat the configuration
		signal_is_how[info_com_top_Channel].Format = ret;
	}
    return ret;
}
int read_chips_signal_status(u8 cmd)
{
    int ret = 0;
    mutex_lock(&lock_chipe_config);
    ret = read_tw9912_chips_status(cmd);//return 0 or 1  ,if back 1 signal have change
    mutex_unlock(&lock_chipe_config);
    return ret;//have change return 1 else retrun 0
}
int IfInputSignalNotStable(void)
{
    int i;
    Vedio_Format ret2;
    static u8 IfInputSignalNotStable_count = 1;
    static u8 SignalNotFind_count = 0;
    video_config_debug("IfInputSignalNotStable(info_com_top_Channel =%d)\n", info_com_top_Channel);
    if( (info_com_top_Channel != SEPARATION && info_com_top_Channel != YIN2))
    {
        //YIN3 CVBS
        if(read_chips_signal_status(1) == 0 )
        {
            video_config_debug("Signal is good\n");
            goto SIGNALISGOOD;
        }
        SignalNotFind_count = 0;
        if(IfInputSignalNotStable_count == 2)
        {
            IfInputSignalNotStable_count = 1;    //if IfInputSignalNotStable_count ==1 the is first find signal bad,
            goto SIGNALINPUT;
        }
        //IfInputSignalNotStable_count ==2 is signal recovery
        IfInputSignalNotStable_count ++;
        video_config_debug("signal astable\n");
        mutex_lock(&lock_chipe_config);
        //Disabel_video_data_out();
        //TC358_data_output_enable(0);
        mutex_unlock(&lock_chipe_config);
        while(global_camera_working_status)//camera not exit
        {
            SignalNotFind_count++;
            if(SignalNotFind_count > 50) goto BREAKTHEWHILE;
            ret2 = Tw9912TestingChannalSignal(info_com_top_Channel) ;
            switch( ret2 )
            {
            case NTSC_I:
                video_config_debug("find signal NTSC_I\n");
                goto BREAKTHEWHILE;
                break;
            case PAL_I:
                video_config_debug("find signal PAL_I\n");
                goto BREAKTHEWHILE;
                break;
            case NTSC_P:
                video_config_debug("find signal NTSC_P\n");
                goto BREAKTHEWHILE;
                break;
            default :
                for(i = 0; i < 10; i++)
                {
                    if(!global_camera_working_status) goto BREAKTHEWHILE;
                    msleep(1);
                }
                video_config_debug("not signal\n");
                break;
            }
        }
BREAKTHEWHILE:
        mutex_lock(&lock_chipe_config);
        //Enabel_video_data_out();
        //TC358_data_output_enable(1);
        mutex_unlock(&lock_chipe_config);
    }
    else//DVD YUV
    {
        ;
    }
SIGNALISGOOD:
SIGNALINPUT:
    return 0;
}
/*static int thread_signal_test(void *data)
{
    int i = 0;
    long int timeout;
    video_config_debug("thread_signal_test()\n");
    while(!kthread_should_stop())
    {
        timeout = 10;
        while(timeout > 0)
        {
            //delay
            timeout = schedule_timeout(timeout);
        }

        i = read_chips_signal_status(1);

        if(i)
        {
            printk("The backlight off!\n");
            SOC_F_LCD_Light_Con(0);
        }
    }
    return 0;
}*/
void video_init_config_in(Vedio_Format config_pramat)
{
    printk("\n\nVideo Module Build Time: %s %s  %s \n", __FUNCTION__, __DATE__, __TIME__);
    video_config_debug("tw9912:config channal is %d\n", info_com_top_Channel);
    //spin_lock(&spin_chipe_config_lock);
    mutex_lock(&lock_chipe_config);
    //SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
    if(info_com_top_Channel == SEPARATION || info_com_top_Channel == YIN2)
    {  if(testing_NTSCp_video_signal() == NTSC_P)
    	{
        Tw9912_init_NTSCp();
        VideoImage();
	 SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
        TC358_init(NTSC_P);
	 printk("Vedio Format Is NTSCp\n");
    	}
	else
	{
 	printk("Waring Not find DVD video signal..\n");
	TC358_init(COLORBAR + TC358746XBG_YELLOW); //blue
	}
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
            mutex_unlock(&lock_chipe_config);
            init_tw9912_ent(info_com_top_Channel);
            mutex_lock(&lock_chipe_config);
        }
        VideoImage();

	printk("global_video_channel_flag = %x\n",global_video_channel_flag);
	if(global_video_channel_flag == TV_4KO)
	{u8 Tw9912_register_valu[] = {0x08, 0x17,}; //default input pin selet YIN0
		write_tw9912((char *)Tw9912_register_valu);
		Tw9912_register_valu[0] =0xa;
		Tw9912_register_valu[1] =0x1e;
		write_tw9912((char *)Tw9912_register_valu);
	}

        //msleep(300);//wait for video Steady display
        /*
        		printk("\r\n");
        		printk("TW9912:info_Vedio_Channel=%d\n",info_Vedio_Channel);
        		printk("TW9912:signal_is_how[%d].Channel=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Channel);
        		printk("TW9912:signal_is_how[%d].Format=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].Format);
        		printk("TW9912:signal_is_how[%d].vedio_source=%d\n",info_Vedio_Channel,signal_is_how[info_Vedio_Channel].vedio_source);
        */
        if(info_Vedio_Channel <= SEPARATION)
        {
            if(tw9912_signal_unstabitily_for_Tw9912_init_flag == 1) //find colobar flag signal bad
            {
                printk("video_init_config:video input  signal unstabitily,now shouw RED\n");
                TC358_init(COLORBAR + TC358746XBG_RED); //rea
                tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;
            }
            else if(tw9912_signal_unstabitily_for_Tw9912_init_flag == 2) //NTSCp not find!
            {
                printk("tw9912:Configure NTSCp but after configuration cannot detect a progressive signal,now shouw TC358746XBG_GREEN\n");
                TC358_init(COLORBAR + TC358746XBG_GREEN); //rea
                tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;
            }
            else
            {
                //switch (flyVideoSignalPinTest(info_Vedio_Channel))
                if(1)
                {
                    switch (signal_is_how[info_Vedio_Channel].Format)
                    {
                    case NTSC_I:
			   SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
                        TC358_init(NTSC_I);
                        //TC358_init(PAL_Interlace);
                        break;
                    case PAL_I:
			   SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
                        TC358_init(PAL_I);
                        break;
                    case NTSC_P:
			   SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
                        TC358_init(NTSC_P);
                        break;
                    case PAL_P:
			   SOC_Write_Servicer(VIDEO_NORMAL_SHOW);
                        TC358_init(PAL_P);
                        break;
                    default :
                        printk("video not signal input..\n");
			   SOC_Write_Servicer(VIDEO_SHOW_BLACK);
                        TC358_init(COLORBAR + TC358746XBG_BLACK); //blue
                        break;
                    }
                }
                else if(0)
                    TC358_init(COLORBAR);
                else
                {
                    TC358_Hardware_Rest();
                    printk("Erorr timeout hkjing\n");
                }
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
void Video_Show_Output_Color(u8 color_flag)
{
    mutex_lock(&lock_chipe_config);
    printk("flyvideo:error Video_Show_Output_Color()\n");
    //  Tw9912_init_PALi();
    TC358_init(color_flag);
    //colorbar_init_blue();
    mutex_unlock(&lock_chipe_config);
}
