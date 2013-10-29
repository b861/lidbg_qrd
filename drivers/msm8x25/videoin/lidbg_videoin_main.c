

#include "lidbg.h"
#include "video_init_config.h"
static struct task_struct *Vedio_Signal_Test = NULL;
//static struct task_struct *RunTimeTw9912Status = NULL;
extern tw9912_run_flag tw912_run_sotp_flag;
extern struct TC358_register_struct colorbar_init_user_tab[2];
extern Vedio_Channel info_Vedio_Channel;
extern TW9912_Signal signal_is_how[5];
extern Vedio_Channel info_com_top_Channel;

#define MAJOR_Tw9912 0
//#define MINOR_LED 1
#define DEV_NAME "tw9912config"
extern struct lidbg_hal *plidbg_dev; //add by huangzongqiang
dev_t tw9912_dev;
struct class *tw9912_class;
struct cdev *tw9912_cdev;

TW9912Info global_tw9912_info_for_NTSC_I={
						0x08,
						0x17,
						false,//true is find black line;
						true,//true is neet again find the black line;
						true,//first open the camera
						};
TW9912Info global_tw9912_info_for_PAL_I={
						0x08,
						0x17,
						false,//true is find black line;
						true,//true is neet again find the black line;
						true,//first open the camera
						};

LIDBG_DEFINE;
static void video_config_init(Vedio_Format config_pramat, u8 Channal)
{

    Video_RESX_DOWN;
    usleep(5000);
    Video_RESX_UP;
    usleep(500);
#ifdef DEBUG_TW9912
    Tw9912_init(config_pramat, Channal);
#endif
    TC358_init(config_pramat);



}
static int thread_vedio_signal_test(void *data)
{
    long int timeout;
    printk("tw9912:thread_vedio_signal_test()\n");
    while(!kthread_should_stop())
    {
        timeout = 1;
        while(timeout > 0)
        {
            //delay
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

      //  printk("global_camera_working_status = %d\n", global_camera_working_status);
     // if(read_chips_signal_status(1))
	//printk("read_chips_signal_status = %d\n",read_chips_signal_status(1));
	read_chips_signal_status(1);
	//printk("Tw9912TestingChannalSignal() = %d\n",Tw9912TestingChannalSignal(SEPARATION));

    }
    return 0;
}
/*
static int TestTw9912SignalStatusThread(void *data)
{
    long int timeout;
    printk("%s()\n",__func__);
    while(!kthread_should_stop())
    {
    if(global_camera_working_status)//camera is run
    	{
		timeout = 1;
		while(timeout > 0)
		{
		//delay
			timeout = schedule_timeout(timeout);
		}
		IfInputSignalNotStable();
	}
	else//camera is stop
	{ video_config_debug("global_camera_working_status = 0 \n");
		timeout = 600;
	        while(timeout > 0)
	        {
	            //delay
	            timeout = schedule_timeout(timeout);
	        }
	}
    }
    return 0;
}

static void CreadChipStatusFunction(void)
{
	printk("%s()\n",__func__);
        RunTimeTw9912Status = kthread_run(TestTw9912SignalStatusThread, NULL, "flyvideo");

}
static void CleanChipStatusFunction(void)
{
		printk("%s()\n",__func__);
                kthread_stop(RunTimeTw9912Status);
}
*/
#ifdef DEBUG_TW9912
void lidbgVideoTerminalImageConfig(int argc, char **argv)
{
    u32 valu;
    if(!strcmp(argv[2], "BRIGHTNESS"))
    {
        valu = simple_strtoul(argv[3], 0, 0);
        flyVideoImageQualityConfig_in(BRIGHTNESS, valu);
        printk("BRIGHTNESS valu=%d", valu);
    }
    if(!strcmp(argv[2], "CONTRAST"))
    {
        valu = simple_strtoul(argv[3], 0, 0);
        flyVideoImageQualityConfig_in(CONTRAST, valu);
        printk("CONTRAST valu=%d", valu);
    }
    if(!strcmp(argv[2], "SHARPNESS"))
    {
        valu = simple_strtoul(argv[3], 0, 0);
        flyVideoImageQualityConfig_in(SHARPNESS, valu);
        printk("SHARPNESS valu=%d", valu);
    }
    if(!strcmp(argv[2], "CHROMA_U"))
    {
        valu = simple_strtoul(argv[3], 0, 0);
        flyVideoImageQualityConfig_in(CHROMA_U, valu);
        printk("CHROMA_U valu=%d", valu);
    }
    if(!strcmp(argv[2], "CHROMA_V"))
    {
        valu = simple_strtoul(argv[3], 0, 0);
        flyVideoImageQualityConfig_in(CHROMA_V, valu);
        printk("CHROMA_V valu=%d", valu);
    }
    if(!strcmp(argv[2], "HUE"))
    {
        valu = simple_strtoul(argv[3], 0, 0);
        flyVideoImageQualityConfig_in(HUE, valu);
        printk("HUE valu=%d", valu);
    }

}
#endif

#ifdef DEBUG_TC358
void lidbgVideoTc358746TerminalConfig(int argc, char **argv)
{
printk("lidbgVideoTc358746TerminalConfig()\n");
    if(!strcmp(argv[1], "pp"))
    {
        printk("TC358_init(PAL_Progressive);\n\n");
        TC358_init(PAL_P);
    }
    else if(!strcmp(argv[1], "np"))
    {
        printk("TC358_init(PAL_Progressive);\n\n");
        TC358_init(NTSC_P);
    }
    else if(!strcmp(argv[1], "write"))
    {
        u8 buf[4] = {0, 0, 0, 0}, flag;
        u16 sub_addr;
        u32 valu;

        sub_addr = simple_strtoul(argv[2], 0, 0);
        valu = simple_strtoul(argv[3], 0, 0);
        flag =  simple_strtoul(argv[4], 0, 0);
        if(flag == 16)
        {
            printk("TC358 write in adder=0x%02x, valu=0x%02x\n", sub_addr, valu);
            TC358_Register_Write(&sub_addr, &valu, register_value_width_16);
            TC358_Register_Read(sub_addr, buf, register_value_width_16);
            printk("TC358 read back adder=0x%02x, valu=0x%02x%02x%02x%02x\n", sub_addr, buf[2], buf[3], buf[0], buf[1]);
        }
        else
        {
            printk("TC358 write in adder=0x%02x, valu=0x%02x\n", sub_addr, valu);
            TC358_Register_Write(&sub_addr, &valu, register_value_width_32);
            TC358_Register_Read(sub_addr, buf, register_value_width_32);
            printk("TC358 read back adder=0x%02x, valu=0x%02x%02x%02x%02x\n", sub_addr, buf[2], buf[3], buf[0], buf[1]);
        }

    }
    else if(!strcmp(argv[1], "Read"))
    {
        u8 flag, buf[4] = {0, 0, 0, 0};
        u16 sub_addr;
        sub_addr = simple_strtoul(argv[2], 0, 0);
        flag =  simple_strtoul(argv[3], 0, 0);
        if(flag == 16)
        {
            TC358_Register_Read(sub_addr, buf, register_value_width_16);
            printk("TC358 read back adder=0x%02x, valu=0x%02x%02x%02x%02x\n", sub_addr, buf[2], buf[3], buf[0], buf[1]);
        }
        else
        {
            TC358_Register_Read(sub_addr, buf, register_value_width_32);
            printk("TC358 read back adder=0x%02x, valu=0x%02x%02x%02x%02x\n", sub_addr, buf[2], buf[3], buf[0], buf[1]);
        }
    }
    else if(!strcmp(argv[1], "Reset720*480"))
    {
        printk("call TC358_init();\n\n");
        TC358_init(2);
    }
    else if(!strcmp(argv[1], "Reset640*480"))
    {
        printk("call TC358_init();\n\n");
        TC358_init(3);
    }
    else if(!strcmp(argv[1], "ShowColor"))
    {
	 u16 cmd;
	printk("call TC358_init();\n\n");
        cmd = simple_strtoul(argv[2], 0, 0);
        TC358_init(cmd);
    }
    else if(!strcmp(argv[1], "ShowColordebug"))
    {
	 u16 cmd;
	printk("call TC358_init();\n\n");
        cmd = simple_strtoul(argv[2], 0, 0);
	colorbar_init_user_tab[0].add_val=cmd;
	cmd = simple_strtoul(argv[3], 0, 0);
	colorbar_init_user_tab[1].add_val=cmd;
        TC358_init(7);
    }
	
}
#endif
#ifdef DEBUG_TW9912
void lidbgVideoTw9912TerminalConfig(int argc, char **argv)
{
    if(!strcmp(argv[1], "flyVideoTestSignalPinYin3"))
    {
        printk("tw9912:test Yin3 back format :%d", flyVideoTestSignalPin_in(YIN3));
    }
    else if(!strcmp(argv[1], "flyVideoTestSignalPinYin0"))
    {
        printk("tw9912:test Yin3 back format :%d", flyVideoTestSignalPin_in(YIN0));
    }
    else if(!strcmp(argv[1], "flyVideoTestSignalPinYuv"))
    {
        printk("tw9912:test Yin3 back format :%d", flyVideoTestSignalPin_in(SEPARATION));
    }
    else if(!strcmp(argv[1], "flyVideoInitallYin3"))
    {
        flyVideoInitall_in(YIN3);
    }
    else if(!strcmp(argv[1], "flyVideoInitallYin0"))
    {
        flyVideoInitall_in(YIN0);
    }
    else if(!strcmp(argv[1], "flyVideoInitallYin2"))
    {
        flyVideoInitall_in(YIN2);
    }
    else if(!strcmp(argv[1], "flyVideoInitallYuv"))
    {
        flyVideoInitall_in(SEPARATION);
    }
    else if(!strcmp(argv[1], "read_all_NTSCp"))
    {
        printk("TW9912 Read All register\n");
        read_NTSCp();
    }
	else if(!strcmp(argv[1], "read_all_register"))
	{
		printk("TW9912 Read All register+\n\n");
		TW9912_read_all_register();
		printk("TW9912 Read All register-\n\n");
	}
    else if(!strcmp(argv[1], "initNTSCp"))
    {
        printk("Inital TW9912 NTSCp\n");
        Tw9912_init_NTSCp();
    }
    else if(!strcmp(argv[1], "write"))
    {
        u8 buf[2];

        u16 sub_addr;
        u32 valu;
        sub_addr = simple_strtoul(argv[2], 0, 0);
        valu = simple_strtoul(argv[3], 0, 0);
        buf[0] = sub_addr;
        buf[1] = valu;
		#ifdef FLY_VIDEO_BOARD_V3
	 int ret_send=0;
	 int ret_rec=0;
	 //ret_send=i2c_api_do_send(3, 0x44, buf[0],buf,2);
	 i2c_write_byte(3, 0x44, buf , 2);
	 printk("FLY_VIDEO_BOARD_V3 write adder=0x%02x, valu=0x%02x\n", buf[0], buf[1]);
	// ret_rec=i2c_api_do_recv(3, 0x44, sub_addr , (char*)&valu, 1);
	//i2c_read_byte(3, 0x44, sub_addr , (char*)&valu, 1);
	 printk("FLY_VIDEO_BOARD_V3 read adder=0x%02x, valu=0x%02x\n", buf[0], valu);
	 #else
        i2c_write_byte(1, 0x44, buf , 2);
        i2c_read_byte(1, 0x44, sub_addr , (char*)&valu, 1);
        printk("read adder=0x%02x, valu=0x%02x\n", buf[0], valu);
	#endif
    }
    else if(!strcmp(argv[1], "Read"))
    {
        u16 sub_addr;
        u8 buf[2] = {0, 0};
        sub_addr = simple_strtoul(argv[2], 0, 0);
			#ifdef FLY_VIDEO_BOARD_V3
			int ret;
			//ret=SOC_I2C_Rec(3, 0x44, sub_addr , buf, 1);
			// ret=i2c_api_do_recv(3, 0x44, sub_addr , buf, 1);
			i2c_read_byte(3, 0x44, sub_addr , buf, 1);
       	printk(" FLY_VIDEO_BOARD_V3 read adder=0x%02x,valu=0x%02x ret=%d \n", sub_addr, buf[0],ret);
			 #else
        i2c_read_byte(1, 0x44, sub_addr , buf, 1);
        printk("read adder=0x%02x,valu=0x%02x\n", sub_addr, buf[0]);
			#endif
    }
    else if(!strcmp(argv[1], "image"))
    {
        lidbgVideoTerminalImageConfig(argc, argv);
    }
	else if(!strcmp(argv[1], "I2cRate"))
	{
		#ifdef FLY_VIDEO_BOARD_V3
		int	rate;
		rate= simple_strtoul(argv[2], 0, 0);
		SOC_I2C_Set_Rate(3, rate);
		printk(" FLY_VIDEO_BOARD_V3 I2C_Rate_Debug rate=%d\n", rate);
		#else
		
		#endif
	}
    else
    {
        ;
    }
}
#endif
void lidbg_video_main_in(int argc, char **argv)
{
    u8 ret;
    printk("In lidbg_video_main()\n");
    if(!strcmp(argv[0], "agian_initall"))
    {
        video_init_config_in(NTSC_P);
    }
#ifdef DEBUG_TW9912
    else if(!strcmp(argv[0], "Tw9912"))
    {
        lidbgVideoTw9912TerminalConfig(argc, argv);
    }
#endif
#ifdef DEBUG_TC358
    else if(!strcmp(argv[0], "Tc358746"))
    {
        lidbgVideoTc358746TerminalConfig(argc, argv);
    }
#endif
    else if(!strcmp(argv[0], "TestingPresentChannalSignal"))
    {
        ret =(u8) camera_open_video_signal_test();
        switch(ret)
        {
        case 1:
            printk("TestingPresentChannalSignal format :NTSC_i\n");
            break;
        case 2:
            printk("TestingPresentChannalSignal format :PAL_i\n");
            break;
        case 3:
            printk("TestingPresentChannalSignal format :NTSC_p\n");
            break;

        }
    }
    else if(!strcmp(argv[0], "on"))
    {
        Vedio_Signal_Test = kthread_run(thread_vedio_signal_test, NULL, "flyvideo");
    }
    else if(!strcmp(argv[0], "off"))
    {
        kthread_stop(Vedio_Signal_Test);
    }
#ifdef DEBUG_FLYVIDEO
    else if(!strcmp(argv[0], "ResetNTSCi"))
    {
        printk("Reset Vedio NTSCi\n\n");
        video_config_init(NTSC_I, YIN3);
    }
    else if(!strcmp(argv[0], "ResetPALi"))
    {
        printk("Reset Vedio PALi\n\n");
        video_config_init(PAL_I, YIN3);

    }
    else if(!strcmp(argv[0], "ResetNTSCp"))
    {
        printk("Reset Vedio NTSCp\n\n");
        video_config_init(NTSC_P, YIN3);

    }
    else if(!strcmp(argv[0], "ResetPALp"))
    {
        printk("Reset Vedio PALp\n\n");
        video_config_init(PAL_P, YIN3);

    }
    else if(!strcmp(argv[0], "StopVedio"))
    {
        printk("StopVedio\n\n");
        video_config_init(STOP_VIDEO, YIN3);
    }
#endif
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
    plidbg_dev->soc_func_tbl.pfnVideoReset= VideoReset_in;
    global_video_format_flag = NTSC_I;
    global_video_channel_flag = TV_4KO;//DVD
    global_camera_working_status = 0;//stop
}


struct early_suspend early_suspend;
static int  video_early_suspend(struct early_suspend *h)
{
    DUMP_BUILD_TIME;
    return 0;
}
static int video_late_resume(struct early_suspend *h)
{
    printk("video_late_resume tw9912 reset\n");
    Tw9912_hardware_reset();
    flyVideoChannelInitall(YIN2); // DVD
    video_init_config_in(NTSC_P);//first initall tw9912 all register
    return 0;
}

static int video_dev_probe(struct platform_device *pdev)
{
    DUMP_BUILD_TIME;

	
#ifdef CONFIG_HAS_EARLYSUSPEND
		early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
		early_suspend.suspend = video_early_suspend;
		early_suspend.resume = video_late_resume;
		register_early_suspend(&early_suspend);
#endif
	
		//fake suspend
	//SOC_Fake_Register_Early_Suspend(&early_suspend);
    return 0;
}
static int video_dev_remove(struct platform_device *pdev)
{
    DUMP_BUILD_TIME;
    return 0;
}
static int  video_dev_suspend(struct platform_device *pdev,pm_message_t state)
{
    DUMP_BUILD_TIME;
    return 0;
}
static int video_dev_resume(struct platform_device *pdev)
{
	printk("resume video chip environment variables reset\n");
	//Tw9912_hardware_reset();
	global_tw9912_info_for_NTSC_I.flag=true;//true is neet again find the black line;
	global_tw9912_info_for_NTSC_I.reg_val=0x17;
	global_tw9912_info_for_NTSC_I.this_is_first_open = true;
	global_tw9912_info_for_PAL_I.flag=true;//true is neet again find the black line;
	global_tw9912_info_for_PAL_I.reg_val=0x12;
	global_tw9912_info_for_PAL_I.this_is_first_open = true;//first open the camera

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
int tw9912_open(struct inode *inode, struct file *filp)
{
    /*\u5c06\u8bbe\u5907\u7ed3\u6784\u4f53\u6307\u9488\u8d4b\u503c\u7ed9\u6587\u4ef6\u79c1\u6709\u6570\u636e\u6307\u9488*/
	//printk("tw9912config open------>\n");
    return 0;
}
static ssize_t tw9912_read(struct file *filp, char __user *buf, size_t size,
                          loff_t *ppos)
{
    unsigned int count = size;
    ssize_t ret;
	if(signal_is_how[info_Vedio_Channel].Format == NTSC_I)
	{
  //  if (copy_to_user(buf, (void *)(&global_tw9912_info), count))
	if (copy_to_user(buf, (void *)(&global_tw9912_info_for_NTSC_I), count))
		{
			printk("TW9912config : copy_to_user ERR\n");
			ret =  - EFAULT;
		}
		else
		{
			printk("TW9912config : NTSC_I paramter copy to user : %.2x%.2x\n",\
				global_tw9912_info_for_NTSC_I.reg,global_tw9912_info_for_NTSC_I.reg_val);
			ret = count;
		}
	}
	else if(signal_is_how[info_Vedio_Channel].Format == PAL_I)
	{
		if (copy_to_user(buf, (void *)(&global_tw9912_info_for_PAL_I), count))
		{
			printk("TW9912config : copy_to_user ERR\n");
			ret =  - EFAULT;
		}
		else
		{
			printk("TW9912config : PAL_I paramter copy to user : %.2x%.2x\n",\
				global_tw9912_info_for_PAL_I.reg,global_tw9912_info_for_PAL_I.reg_val);
			ret = count;
		}
	}
	else
	{
		if (copy_to_user(buf, (void *)(&global_tw9912_info_for_PAL_I), count))
		{
			printk("TW9912config : copy_to_user ERR\n");
			ret =  - EFAULT;
		}
		else
		{
			printk("TW9912config : PAL_I paramter copy to user : 0x%.2x\n",\
				global_tw9912_info_for_PAL_I.this_is_first_open);
			ret = count;
		}
		//printk("TW9912config :read false\n\n");
	}
    return count;
}
static ssize_t tw9912_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	//struct tw9912_config_data cdata;
	u8 para[] = {0x0, 0x0,};

	if(signal_is_how[info_Vedio_Channel].Format == NTSC_I)
	{
		if (copy_from_user(&global_tw9912_info_for_NTSC_I, buf, count))
		{
			printk("TW9912config : copy_from_user ERR\n");
		}
		/*  */
		if(global_tw9912_info_for_NTSC_I.reg_val  > 0x10 && global_tw9912_info_for_NTSC_I.reg == 0x8\
			&& info_com_top_Channel != SEPARATION && info_com_top_Channel != YIN2)
		{
			printk("TW9912config : paramter is %.2x%.2x NOW write in the register\n",\
				global_tw9912_info_for_NTSC_I.reg,global_tw9912_info_for_NTSC_I.reg_val);
			para[0]=global_tw9912_info_for_NTSC_I.reg;
			para[1]=global_tw9912_info_for_NTSC_I.reg_val;
			write_tw9912(para);
		}
		else
		{
			 printk("TW9912config : paramter is %.2x%.2x NOT write in the regitster\n",\
			 global_tw9912_info_for_NTSC_I.reg,global_tw9912_info_for_NTSC_I.reg_val);
		}
	}
	else if(signal_is_how[info_Vedio_Channel].Format == PAL_I)
	{
		if (copy_from_user(&global_tw9912_info_for_PAL_I, buf, count))
		{
			printk("TW9912config : copy_from_user ERR\n");
		}
		/*  */
		if(global_tw9912_info_for_PAL_I.reg_val  > 0x10 && global_tw9912_info_for_PAL_I.reg == 0x8\
			&& info_com_top_Channel != SEPARATION && info_com_top_Channel != YIN2)
		{
			printk("TW9912config : paramter is %.2x%.2x NOW write in the register\n",\
				global_tw9912_info_for_PAL_I.reg,global_tw9912_info_for_PAL_I.reg_val);
			para[0]=global_tw9912_info_for_PAL_I.reg;
			para[1]=global_tw9912_info_for_PAL_I.reg_val;
			write_tw9912(para);
		}
		else
		{
			 printk("TW9912config : paramter is %.2x%.2x NOT write in the regitster\n",\
			 global_tw9912_info_for_PAL_I.reg,global_tw9912_info_for_PAL_I.reg_val);
		}

	}
	else
	{TW9912Info global_tw9912_info;
		if (copy_from_user(&global_tw9912_info, buf, count))
		{
			printk("TW9912config : copy_from_user ERR\n");
		}
		global_tw9912_info_for_PAL_I.this_is_first_open= global_tw9912_info.this_is_first_open;
		printk("TW9912config :Synchronous DVD open count\n");
	}
return 0;
}
static const struct file_operations tw9912_fops =
{
	.owner = THIS_MODULE,
	.llseek = NULL,
	.read = tw9912_read,
	.write = tw9912_write,
	.open = tw9912_open,
	.release = NULL,
};
int tw9912_index_init(void)
{  int err;
  err=alloc_chrdev_region(&tw9912_dev,0,1,DEV_NAME);//\u5411\u5185\u6838\u6ce8\u518c\u4e00\u4e2a\u8bbe\u5907
    if(err<0)
		{
			printk("register_chrdev tw9912config is error!\n ");
			return err;
		}
   tw9912_cdev = cdev_alloc();
   tw9912_cdev->owner= THIS_MODULE;
   tw9912_cdev->ops = &tw9912_fops;
   err = cdev_add(tw9912_cdev,tw9912_dev,1);
   if(err)  
   printk(KERN_NOTICE "Error for cdec_add() -->adding tw9912config %d \n", MINOR(tw9912_dev));
   tw9912_class =class_create(THIS_MODULE,"tw9912config_class");
   if(IS_ERR(tw9912_class))
	    {
	        printk("Err: failed in creating mlidbg class.\n");
	        goto fail_class_create;
	    }
    device_create(tw9912_class,NULL,tw9912_dev,NULL,DEV_NAME); //\u521b\u5efa\u4e00\u4e2a\u8bbe\u5907\u8282\u70b9\uff0c\u8282\u70b9\u540d\u4e3aDEV_NAME "%d"
	printk("tw9912config device_create ok\n");
	
    lidbg_chmod("/dev/tw9912config");

return 0;
fail_class_create:
    unregister_chrdev_region(tw9912_dev, 1);
    return -1;
}
int lidbg_video_init(void)
{
   
    printk("lidbg_video_init modules ismod\n");
//#ifdef	BOARD_V3
//	return 0;
//#endif
    LIDBG_GET;
    tw9912_index_init();
    set_func_tbl();
    //	TC358_init(PAL_I);
    video_io_i2c_init();
    Tw9912_hardware_reset();
    flyVideoChannelInitall(YIN2); // DVD
    video_init_config_in(NTSC_P);//first initall tw9912 all register
    platform_driver_register(&video_driver);
    platform_device_register(&video_devices);
    //CreadChipStatusFunction();
return 0;
}
void tw9912_exit( void )
{	
	printk("tw9912config_exit_now.....\n");
	//void cdev_put(struct cdev *p)
	
	cdev_del(tw9912_cdev);
	device_destroy(tw9912_class,tw9912_dev);
	class_destroy(tw9912_class);
	//cdev_put(led_cdev);
	unregister_chrdev_region(tw9912_dev, 1);
}

void lidbg_video_deinit(void)
{
    printk("lidbg_video_deinit module exit.....\n");
    //CleanChipStatusFunction();
    tw9912_exit();
   // return 0;

}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");
module_init(lidbg_video_init);
module_exit(lidbg_video_deinit);



