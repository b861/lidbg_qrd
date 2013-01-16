

#include "lidbg_enter.h"
#include "video_init_config.h"
static struct task_struct * Vedio_Signal_Test = NULL; 
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
	while(!kthread_should_stop())
	{
	        timeout=100;
		while(timeout > 0) 
		{ //delay
			timeout = schedule_timeout(timeout); 
		} 
		printk("tw9912:test Yin3 back format :%d\n", flyVideoTestSignalPin_in(YIN2));
		printk("tw9912:global_video_format_flag :%d",	global_video_format_flag);
		printk("  %d\n",i++);
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
	global_video_format_flag = NTSC_I;
	global_video_channel_flag = YIN2;//DVD

}


int lidbg_video_init(void)
{
	printk("lidbg_video_init modules ismod\n");
	LIDBG_GET;
	set_func_tbl();
	video_io_i2c_init();
	flyVideoChannelInitall(YIN2); // DVD
	    return 0;

}

int lidbg_video_deinit(void)
{
	return 0;

}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");
module_init(lidbg_video_init);
module_exit(lidbg_video_deinit);



