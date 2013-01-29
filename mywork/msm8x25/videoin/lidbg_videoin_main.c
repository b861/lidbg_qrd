

#include "lidbg_enter.h"
#include "lidbg_def.h"
#include "video_init_config.h"
static struct task_struct * Vedio_Signal_Test = NULL; 
extern tw9912_run_flag tw912_run_sotp_flag;
LIDBG_DEFINE;
static void video_config_init(Vedio_Format config_pramat,u8 Channal)
{
	
			Video_RESX_DOWN;
			usleep(5000);
			Video_RESX_UP;
			usleep(500);
			#ifdef DEBUG_TW9912
				Tw9912_init(config_pramat,Channal);
			#endif
				TC358_init(config_pramat);



}
static int thread_vedio_signal_test(void *data)  
{int i=0;
  long int timeout;
  	 printk("tw9912:thread_vedio_signal_test()\n");
	while(!kthread_should_stop())
	{
	timeout=1;
	while(timeout > 0) 
	{ //delay
			timeout = schedule_timeout(timeout); 
	} 
	/*
	i=read_chips_signal_status(1);
		if(i)
		{
			printk("thread_vedio_signal_test find tw9912 have change!\n");
			  TC358_init(COLORBAR+TC358746XBG_LIGHT_BLUE);//blue
			while(read_chips_signal_status(0) !=0x0 )
				{
				msleep(100);
				}
			switch (tw912_run_sotp_flag.format) // valu com form tw9912.c 919 line
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
			msleep(10000);
		}
		msleep(10);*/
		
	printk("global_camera_working_status = %d\n",global_camera_working_status);
	}
return 0;
}
void lidbg_video_main_in(int argc, char **argv)
{
printk("In lidbg_video_main()\n");

	 if(!strcmp(argv[0], "image"))
	{
		u32 valu;
	    if(!strcmp(argv[1], "BRIGHTNESS"))
	    	{
			valu = simple_strtoul(argv[2], 0, 0);
			flyVideoImageQualityConfig_in(BRIGHTNESS,valu);
			printk("BRIGHTNESS valu=%d",valu);
		}
	    if(!strcmp(argv[1], "CONTRAST"))
	    	{
			valu = simple_strtoul(argv[2], 0, 0);
			flyVideoImageQualityConfig_in(CONTRAST,valu);
			printk("CONTRAST valu=%d",valu);
		}
	    if(!strcmp(argv[1], "SHARPNESS"))
	    	{
			valu = simple_strtoul(argv[2], 0, 0);
			flyVideoImageQualityConfig_in(SHARPNESS,valu);
			printk("SHARPNESS valu=%d",valu);
		}
	   if(!strcmp(argv[1], "CHROMA_U"))
	    	{
			valu = simple_strtoul(argv[2], 0, 0);
			flyVideoImageQualityConfig_in(CHROMA_U,valu);
			printk("CHROMA_U valu=%d",valu);
		}
           if(!strcmp(argv[1], "CHROMA_V"))
	    	{
			valu = simple_strtoul(argv[2], 0, 0);
			flyVideoImageQualityConfig_in(CHROMA_V,valu);
			printk("CHROMA_V valu=%d",valu);
		}
           if(!strcmp(argv[1], "HUE"))
	    	{
			valu = simple_strtoul(argv[2], 0, 0);
			flyVideoImageQualityConfig_in(HUE,valu);
			printk("HUE valu=%d",valu);
		}

	}
 if(!strcmp(argv[0], "test"))
	{
	printk("tw9912:test test back format :%d", camera_open_video_signal_test());
	}
 if(!strcmp(argv[0], "on"))
	{
		  Vedio_Signal_Test = kthread_run(thread_vedio_signal_test,NULL,"flyvideo");  
	}
  if(!strcmp(argv[0], "off"))
	{
		   kthread_stop(Vedio_Signal_Test);  
	}
   if(!strcmp(argv[0], "testYin3"))
	{
	printk("tw9912:test Yin3 back format :%d", flyVideoTestSignalPin_in(YIN3));
	}
  if(!strcmp(argv[0], "testYin0"))
	{
	printk("tw9912:test Yin3 back format :%d", flyVideoTestSignalPin_in(YIN0));
	}
    if(!strcmp(argv[0], "testYuv"))
	{
	printk("tw9912:test Yin3 back format :%d", flyVideoTestSignalPin_in(SEPARATION));
	}
	
  if(!strcmp(argv[0], "checkoutYin3"))
	{
	 flyVideoInitall_in(YIN3);
	}
  if(!strcmp(argv[0], "checkoutYin0"))
	{
	 flyVideoInitall_in(YIN0);
	}
    if(!strcmp(argv[0], "checkoutYin2"))
	{
	 flyVideoInitall_in(YIN2);
	}
    if(!strcmp(argv[0], "checkoutYuv"))
	{
	 flyVideoInitall_in(SEPARATION);
	}
 #ifdef DEBUG_TW9912
	else if(!strcmp(argv[0], "TW9912"))
	{
	if(!strcmp(argv[1], "initNTSCp"))
		{
			printk("Inital TW9912 NTSCp\n");
			Tw9912_init_NTSCp();
		}
	if(!strcmp(argv[1], "testingyin0"))
		{
			Tw9912_appoint_pin_testing_video_signal(YIN0);
		}
	if(!strcmp(argv[1], "testingyin3"))
		{
			Tw9912_appoint_pin_testing_video_signal(YIN3);
		}
	if(!strcmp(argv[1], "write"))
		{
		u8 buf[2];
		
		u16 sub_addr;
		u32 valu;
			sub_addr = simple_strtoul(argv[2], 0, 0);
			valu = simple_strtoul(argv[3], 0, 0);
			buf[0]=sub_addr;
			buf[1]=valu;
			i2c_write_byte(1, 0x45, buf , 2);
			i2c_read_byte(1,0x45, sub_addr , &valu,1);
				printk("read adder=0x%02x, valu=0x%02x\n",buf[0],valu);
		}	
		
		if(!strcmp(argv[1], "Read"))
		{
		u16 sub_addr;
		u8 buf[2]={0,0};
			sub_addr = simple_strtoul(argv[2], 0, 0);
			i2c_read_byte(1,0x45, sub_addr ,buf,1);
			printk("read adder=0x%02x,valu=0x%02x\n",sub_addr,buf[0]);
		}
	}
#endif
#ifdef DEBUG_TC358
	else if (!strcmp(argv[0], "TC358"))
	{
		if(!strcmp(argv[1], "pp"))
		{
				printk("TC358_init(PAL_Progressive);\n\n");
				TC358_init(PAL_P);
		}
		if(!strcmp(argv[1], "np"))
		{
				printk("TC358_init(PAL_Progressive);\n\n");
				TC358_init(NTSC_P);
		}
	}

	else if(!strcmp(argv[0], "ResetNTSCi"))
		{
				printk("Reset Vedio NTSCi\n\n");
				video_config_init(NTSC_I,YIN3);
		}
	else if(!strcmp(argv[0], "ResetPALi"))
		{
				printk("Reset Vedio PALi\n\n");
				video_config_init(PAL_I,YIN3);

		}
	else if(!strcmp(argv[0], "ResetNTSCp"))
		{
				printk("Reset Vedio NTSCp\n\n");
				video_config_init(NTSC_P,YIN3);

		}
	else if(!strcmp(argv[0], "ResetPALp"))
		{
				printk("Reset Vedio PALp\n\n");
				video_config_init(PAL_P,YIN3);

		}
	else if(!strcmp(argv[0], "StopVedio"))
		{
				printk("StopVedio\n\n");
				video_config_init(STOP_VIDEO,YIN3);
		}
	else if(!strcmp(argv[0], "Reset720*480"))
		{
		 	  	printk("call TC358_init();\n\n");
				TC358_init(2);
		}
	else if(!strcmp(argv[0], "Reset640*480"))
		{
				printk("call TC358_init();\n\n");
				TC358_init(3);
		}
#endif//#ifdef DEBUG_TC358
	else 
		{
			;
		}
 }



static void set_func_tbl(void)
{
//video
	plidbg_dev->soc_func_tbl.pfnlidbg_video_main = lidbg_video_main_in;
	plidbg_dev->soc_func_tbl.pfnvideo_io_i2c_init = video_io_i2c_init_in;
	plidbg_dev->soc_func_tbl.pfnflyVideoInitall = flyVideoInitall_in;
	plidbg_dev->soc_func_tbl.pfnflyVideoTestSignalPin = flyVideoTestSignalPin_in;
	plidbg_dev->soc_func_tbl.pfnflyVideoImageQualityConfig = flyVideoImageQualityConfig_in;
	plidbg_dev->soc_func_tbl.pfnvideo_init_config = video_init_config_in;
	plidbg_dev->soc_func_tbl.pfncamera_open_video_signal_test = camera_open_video_signal_test_in;
	plidbg_dev->soc_func_tbl.pfncamera_open_video_color = Video_Show_Output_Color;
	plidbg_dev->soc_func_tbl.pfnread_tw9912_chips_signal_status = read_chips_signal_status;
	global_video_format_flag = NTSC_I;
	global_video_channel_flag = YIN2;//DVD
	global_camera_working_status = 0;//stop
}
static int video_dev_probe(struct platform_device *pdev)
{
DUMP_BUILD_TIME;
return 0;
}
static int video_dev_remove(struct platform_device *pdev)
{
DUMP_BUILD_TIME;
return 0;
}
static int  video_dev_suspend(struct platform_device *pdev, pm_message_t state)
{
DUMP_BUILD_TIME;
return 0;
}
static int video_dev_resume(struct platform_device *pdev)
{
printk("resume tw9912 reset\n");
Tw9912_hardware_reset();

return 0;
}
static struct platform_driver video_driver =
{
    .probe = video_dev_probe,
    .remove = video_dev_remove,
    .suspend =  video_dev_suspend,
    .resume =  video_dev_resume,
    .driver = {
        .name = "video_devices",
        .owner = THIS_MODULE,

    },


};
struct platform_device video_devices =
{
    .name			= "video_devices",
    .id 			= 0,
};

int lidbg_video_init(void)
{
	printk("lidbg_video_init modules ismod\n");
	LIDBG_GET;
	set_func_tbl();
//	TC358_init(PAL_I);
	video_io_i2c_init();
	Tw9912_hardware_reset();
	flyVideoChannelInitall(YIN2); // DVD
platform_driver_register(&video_driver);
platform_device_register(&video_devices);
	    return 0;

}

int lidbg_video_deinit(void)
{
	printk("lidbg_video_deinit module exit.....\n");
    
	return 0;

}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");
module_init(lidbg_video_init);
module_exit(lidbg_video_deinit);



