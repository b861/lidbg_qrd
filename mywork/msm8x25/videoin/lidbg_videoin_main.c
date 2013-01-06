

#include "lidbg_enter.h"
#include "video_init_config.h"

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
void lidbg_video_main_in(int argc, char **argv)
{
printk("In lidbg_video_main()\n");
 if(!strcmp(argv[0], "xxx"))
	{
		;
	}
  if(!strcmp(argv[0], "checkoutYin3"))
	{
	 flyVideoInitall_in(YIN3);
	}
  if(!strcmp(argv[0], "checkoutYin0"))
	{
	 flyVideoInitall_in(YIN0);
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
}


int lidbg_video_init(void)
{
	printk("lidbg_video_init modules ismod\n");
	LIDBG_GET;
	set_func_tbl();
	video_io_i2c_init();
	flyVideoChannelInitall(SEPARATION);
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



