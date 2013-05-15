
#include "i2c_io.h"
#include "tw9912.h"
#include "tw9912_config.h"
#include "lidbg_enter.h"
static int tw9912_reset_flag = 0;
struct mutex lock_com_chipe_config;
static struct task_struct *tw9912_Correction_Parameter_fun = NULL;
u8 tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;
static int read_tw9912_chips_status_flag = 0 , read_tw9912_chips_status_flag_1 = 0;
u8 tw9912_reset_flag_jam = 0;
/*
\u56e0\u4e3a\u914d\u7f6eNTSCp\u65f6\u64ad\u653e\u5361\u6b7b\u7684\u95ee\u9898\u9700\u8981\u5bf9tw9912\u590d\u4f4d\uff0c
\u5728\u7b2c\u4e00\u6b21\u8fdb\u5165DVD\u65f6\u5e94\u4e3a\u672a\u80fd\u6267\u884c\u5230int static Change_channel(void)\u5b9e\u73b0tw9912\u7684\u590d\u4f4d\uff0c
\u7279\u52a0\u5165\u6b64\u6807\u5fd7\u4f4d\u5bf9tw9912\u7b2c\u4e00\u6b21\u8fdb\u5165\u505a\u590d\u4f4d\u6807\u5fd7\u3002
*/
#define TW9912_I2C_ChipAdd 0x44 //SIAD = 0-->0x44  SIAD =1-->0x45
TW9912_input_info tw9912_input_information;
tw9912_run_flag tw912_run_sotp_flag;
TW9912_Signal signal_is_how[5] = //用于记录四个通道的信息
{
    {NOTONE, OTHER, source_other}, //YIN0
    {NOTONE, OTHER, source_other}, //YIN1
    {NOTONE, OTHER, source_other}, //YIN2
    {NOTONE, OTHER, source_other}, //YIN3
    {NOTONE, OTHER, source_other}, //SEPARATION
};
TW9912_initall_status tw9912_status = {TW9912_initall_not, NOTONE, OTHER};
Last_config_t the_last_config = {NOTONE, OTHER};
void Tw9912Reset_in(void)
{	
    tw9912_dbg("%s:tw9912_RESX_DOWN\n",__func__);
    tw9912_RESX_DOWN;
    msleep(20);
    tw9912_RESX_UP;
}
void Tw9912_hardware_reset(void)
{	
    tw9912_dbg("%s:tw9912_RESX_DOWN\n",__func__);
    tw9912_RESX_DOWN;
    tw9912_RESX_UP;
    //Tw9912_init_agin();
    Tw9912_init_NTSCp();
    tw9912_reset_flag_jam = 0; //\u5524\u9192\u540e\u7b2c\u4e00\u6b21\u8fdb\u5165DVD\u5bf9tw9912\u8fdb\u884c\u590d\u4f4d
}
i2c_ack read_tw9912(unsigned int sub_addr, char *buf )
{
    i2c_ack ret;
    ret = i2c_read_byte(1, TW9912_I2C_ChipAdd, sub_addr, buf, 1);
    tw9912_dbg("tw9912:read addr=0x%.2x value=0x%.2x\n", sub_addr, buf[0]);
    return ret;
}
void read_NTSCp(void)
{
    u8 buf;
    register int i = 0;
    i2c_ack ret;
    for(i = 0; TW9912_INIT_NTSC_Progressive_input[i] != 0xfe; i += 2)
    {
        ret = read_tw9912(TW9912_INIT_NTSC_Progressive_input[i], &buf);
	if (ret == NACK) goto NACK_BREAK;
        //printk("tw9912: TW9912_INIT_NTSC_Progressive_input[%d]=0x%.2x != readback =0x%.2xd",i,TW9912_INIT_NTSC_Progressive_input[i],buf);
        if(buf != TW9912_INIT_NTSC_Progressive_input[i+1])
            tw9912_dbg("tw9912: worning TW9912_INIT_NTSC_Progressive_input[0x%.2x]=0x%.2x != readback =0x%.2x\n", TW9912_INIT_NTSC_Progressive_input[i], TW9912_INIT_NTSC_Progressive_input[i+1], buf);
    }
NACK_BREAK:
printk("tw9912:%s:interrupt becouts NACK\n",__func__);
}
i2c_ack write_tw9912(char *buf )
{
    i2c_ack ret;
    u8 buf_back;
    static int again_write_count = 0;
    tw9912_dbg("tw9912:write addr=0x%.2x value=0x%.2x\n", buf[0], buf[1]);
    ret = i2c_write_byte(1, TW9912_I2C_ChipAdd, buf, 2);
#ifdef DEBUG_TW9912_CHECK_WRITE
    if (ret == ACK && the_last_config.Channel == SEPARATION)
    {
        ret = i2c_read_byte(1, TW9912_I2C_ChipAdd, buf[0], &buf_back, 1);
        tw9912_dbg("TW9912:  Register (0x%.2x) back (0x%.2x) and write (0x%.2x) \n", buf[0], buf_back, buf[1]);
        if(buf_back != buf[1] )
        {
            if(
                ( (buf[0] == 0x1c || buf[0] == 0x1c ) &&  (buf_back & 0xf) != (buf[1] & 0xf ) ) || \
                (  buf[0] == 0x1d && (buf_back & 0x7f != buf[1] & 0x7f ))\
            )
            {
                msleep(10);
                tw9912_dbg("TW9912: error Read (0x%.2x) back (0x%.2x) and write (0x%.2x) in data inequality\n", buf[0], buf_back, buf[1]);
                if(again_write_count < 2)
                    //	write_tw9912(buf);
                    i2c_write_byte(1, TW9912_I2C_ChipAdd, buf, 2);
                again_write_count ++;
            }
        }
        if(again_write_count >= 2) again_write_count = 0;
    }
#endif
    /**/
    return ret;
}
#if 0
i2c_ack Correction_Parameter_fun(Vedio_Format format)
{
    i2c_ack ret;
    u8 Tw9912_Parameter[] = {0, 0,};

    if( format == PAL_I )
    {
        //msleep(100);
        tw9912_dbg("Correction_Parameter_fun() PAL\n");
        Tw9912_Parameter[0] = 0xff;
        Tw9912_Parameter[1] = 0x00;
        ret = write_tw9912(Tw9912_Parameter);
	if (ret == NACK) goto NACK_BREAK;

        Tw9912_Parameter[0] = 0x07;
        Tw9912_Parameter[1] = 0x22;
        ret = write_tw9912(Tw9912_Parameter);
	if (ret == NACK) goto NACK_BREAK;

        Tw9912_Parameter[0] = 0x0a;
        Tw9912_Parameter[1] = 0x17;
        ret = write_tw9912(Tw9912_Parameter);
	if (ret == NACK) goto NACK_BREAK;

        Tw9912_Parameter[0] = 0x07;
        Tw9912_Parameter[1] = 0x22;
        ret = write_tw9912(Tw9912_Parameter);
	if (ret == NACK) goto NACK_BREAK;

        Tw9912_Parameter[0] = 0x08;
        Tw9912_Parameter[1] = 0x12;
        ret = write_tw9912(Tw9912_Parameter);
	if (ret == NACK) goto NACK_BREAK;

        Tw9912_Parameter[0] = 0x09;
        Tw9912_Parameter[1] = 0x40;
        ret = write_tw9912(Tw9912_Parameter);
    }
    else if(format == NTSC_I )
    {
        tw9912_dbg("Correction_Parameter_fun() NTSC\n");
        //		Tw9912_Parameter[0]=0x0a;//image dowd 3 line
        //		Tw9912_Parameter[1]=0x17;// image down 3 line
        //		ret = write_tw9912(Tw9912_Parameter);

        /*		Tw9912_Parameter[0]=0x09;//image dowd 3 line
        		Tw9912_Parameter[1]=0xf9;// image down 3 line
        		ret = write_tw9912(Tw9912_Parameter);

        		Tw9912_Parameter[0]=0x0A;//image dowd 3 line
        		Tw9912_Parameter[1]=0x29;// image down 3 line
        		ret = write_tw9912(Tw9912_Parameter);

        		Tw9912_Parameter[0]=0x0B;//image dowd 3 line
        		Tw9912_Parameter[1]=0xec;// image down 3 line
        		ret = write_tw9912(Tw9912_Parameter);
        	*/
    }
    else//separation
    {
        ;
    }
return ret;
NACK_BREAK:
return NACK;
}

static int thread_tw9912_Correction_Parameter_fun(void *data)
{
    int i = 0;
    long int timeout;
    tw9912_dbg("tw9912:thread_tw9912_Correction_Parameter_fun()\n");
    while(!kthread_should_stop())
    {
        timeout = 10;
        while(timeout > 0)
        {
            //delay
            timeout = schedule_timeout(timeout);
        }
        if (tw912_run_sotp_flag.run == 1)
        {
            // printk("tw9912 is run again and format is PALi\n");
            Correction_Parameter_fun(tw912_run_sotp_flag.format);
            tw912_run_sotp_flag.run = 0;
            kthread_stop(tw9912_Correction_Parameter_fun);
        }

    }
    return 0;
}
#endif
void tw9912_get_input_info(TW9912_input_info *input_information)
{
i2c_ack ret;
    input_information->chip_status1.index = 0x01;	//register index
    input_information->chip_status2.index = 0x31;
    input_information->standard_selet.index = 0x1c;
    input_information->component_video_format.index = 0x1e;
    input_information->macrovision_detection.index = 0x30;
    input_information->input_detection.index = 0xc1;

	ret = read_tw9912(input_information->chip_status1.index, \
	            &input_information->chip_status1.valu);
	if (ret == NACK) goto NACK_BREAK;

//	ret = read_tw9912(input_information->chip_status2.index, \
//	            &input_information->chip_status2.valu);
//	if (ret == NACK) goto NACK_BREAK;

//	ret =read_tw9912(input_information->standard_selet.index, \
//	            &input_information->standard_selet.valu);
//	if (ret == NACK) goto NACK_BREAK;

	ret = read_tw9912(input_information->component_video_format.index, \
	            &input_information->component_video_format.valu);
	if (ret == NACK) goto NACK_BREAK;

	ret = read_tw9912(input_information->macrovision_detection.index, \
	            &input_information->macrovision_detection.valu);
//	if (ret == NACK) goto NACK_BREAK;

//	read_tw9912(input_information->input_detection.index, \
//	            &input_information->input_detection.valu);
NACK_BREAK:
;
}
void Tw9912_analysis_input_signal(TW9912_input_info *input_information, Vedio_Channel channel)
{
    u8 channel_1;
    u8 format_1;

    if(channel > SEPARATION)
    {
        printk("tw9912:%s:input chanel paramal have error!\n",__func__);
        goto CHANNEL_FAILD;
    }
    signal_is_how[channel].Channel = channel;
    signal_is_how[channel].Format = OTHER;
    signal_is_how[channel].vedio_source = source_other;

    if(input_information->chip_status1.valu & 0x08 )//bit3=1 Vertical logi is locked to the incoming video soruce
    {
        printk("tw9912:Signal is lock<<<<<<<<<<<<<<\n");
        if(input_information->chip_status1.valu & 0x01) signal_is_how[channel].vedio_source = source_50Hz;
        else signal_is_how[channel].vedio_source = source_60Hz;

        if(channel <= SEPARATION)
            signal_is_how[channel].Channel = channel;
        else
            printk("tw9912:error channal>>>>>>>>>>>>>>>>>>>More than SEPARATION\n");

        read_tw9912(0x02, &channel_1); //register 0x02 channel selete
        channel_1 = (channel_1 & 0x0c) >> 2 ;
        if(channel_1 == channel || channel == SEPARATION)
        {
            if(channel != SEPARATION)
                //if(1)
            {
                format_1 = input_information->component_video_format.valu & 0x70;
                if(format_1 == 0x00)
                {
                    signal_is_how[channel].Format = NTSC_I;
                }
                else if(format_1 == 0x10)
                {
                    signal_is_how[channel].Format = PAL_I;
                }
                else if(format_1 == 0x20)
                {
                    signal_is_how[channel].Format = NTSC_P;
                }
                else if(format_1 == 0x30)
                {
                    signal_is_how[channel].Format = PAL_P;
                }
                else
                {
                    signal_is_how[channel].Format = OTHER;
                }
            }
            else
            {
                format_1 = input_information->input_detection.valu & 0x07;
                if(format_1 == 0x2)
                {
                    signal_is_how[channel].Format = NTSC_P;
                }
                else if(format_1 == 0x3)
                {
                    signal_is_how[channel].Format = PAL_P;
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
            signal_is_how[channel].Format = OTHER;
            signal_is_how[channel].vedio_source = source_other;
        }
    }
    else
    {
        printk("\r\rtw9912:testing NOT lock>>>>>>>>>>>\n");
        signal_is_how[channel].Format = OTHER;
        signal_is_how[channel].vedio_source = source_other;
    }
    if(channel == SEPARATION  )
    {
        signal_is_how[channel].Format = NTSC_P;
    }
CHANNEL_FAILD:
    ;
}
void Disabel_video_data_out(void)
{
    u8 disabel[] = {0x03, 0x27,};
    tw9912_dbg("Disabel_video_data_out()\n");
    read_tw9912(0x03, &disabel[1]); //register 0x02 channel selete
    disabel[1] |=0x07;
    write_tw9912(disabel);
}
void Enabel_video_data_out(void)
{
    u8 disabel[] = {0x03, 0x0,};
    tw9912_dbg("Enabel_video_data_out()\n");
    read_tw9912(0x03, &disabel[1]); //register 0x02 channel selete
    disabel[1] &=0xf0;
    write_tw9912(disabel);
}
void display_tw9912_info(void)
{
    u8 i;
    tw9912_dbg("\n\r\r\r\r\r\n");
    for(i = 0; i < 5; i++)
    {

        switch(i)
        {
        case YIN0:
            tw9912_dbg("\n\nYIN0\n");
            break;
        case YIN1:
            tw9912_dbg("\n\nYIN1\n");
            break;
        case YIN2:
            tw9912_dbg("\n\nYIN2\n");
            break;
        case YIN3:
            tw9912_dbg("\n\nYIN3\n");
            break;
        case SEPARATION:
            tw9912_dbg("\n\nSEPARATION\n");
            break;
        }
        switch(signal_is_how[i].Channel)
        {
        case YIN0:
            tw9912_dbg("signal_is_how.Channel =YIN0\n");
            break;
        case YIN1:
            tw9912_dbg("signal_is_how.Channel =YIN1\n");
            break;
        case YIN2:
            tw9912_dbg("signal_is_how.Channel =YIN2\n");
            break;
        case YIN3:
            tw9912_dbg("signal_is_how.Channel =YIN3\n");
            break;
        case SEPARATION:
            tw9912_dbg("signal_is_how.Channel =SEPARATION\n");
            break;
        default :
            tw9912_dbg("signal_is_how.Channel =NOTONEin\n");
            break;
        }
        switch(signal_is_how[i].Format)
        {
        case PAL_P :
            tw9912_dbg("signal_is_how.Format = PAL_P\n");
            break;
        case PAL_I:
            tw9912_dbg("signal_is_how.Format = PAL_I\n");
            break;
        case NTSC_P:
            tw9912_dbg("ignal_is_how.Format = NTSC_P\n");
            break;
        case NTSC_I:
            tw9912_dbg("signal_is_how.Format = NTSC_I\n");
            break;
        default:
            tw9912_dbg("signal_is_how.Format = other\n");
            break;
        }
        switch(signal_is_how[i].vedio_source)
        {
        case source_50Hz:
            tw9912_dbg("signal_is_how.vedio_source = source_50Hz\n\n");
            break;
        case source_60Hz:
            tw9912_dbg("signal_is_how.vedio_source = source_60Hz\n\n");
            break;
        case source_other:
            tw9912_dbg("ignal_is_how.vedio_source = source_other\n\n");
            break;
        }
    }

    tw9912_dbg("chip_status1(%.2x)=%.2x\n", tw9912_input_information.chip_status1.index
               , tw9912_input_information.chip_status1.valu);

    tw9912_dbg("chip_status2(%.2x)=%.2x\n", tw9912_input_information.chip_status2.index
               , tw9912_input_information.chip_status2.valu);

    tw9912_dbg("standard_selet(%.2x)=%.2x\n", tw9912_input_information.standard_selet.index
               , tw9912_input_information.standard_selet.valu);

    tw9912_dbg("component_video_format(%.2x)=%.2x\n", tw9912_input_information.component_video_format.index
               , tw9912_input_information.component_video_format.valu);

    tw9912_dbg("macrovision_detection(%.2x)=%.2x\n", tw9912_input_information.macrovision_detection.index,
               tw9912_input_information.macrovision_detection.valu);

    tw9912_dbg("input_detection(%.2x)=%.2x\n", tw9912_input_information.input_detection.index,
               tw9912_input_information.input_detection.valu);
    /**/

}
static int TW9912_Channel_Choices(Vedio_Channel channel)
{
    u8 Tw9912_input_pin_selet[] = {0x02, 0x40,}; //default input pin selet YIN0
    int ret;
    tw9912_dbg("fly_video now channal choices %d",channel);
    the_last_config.Channel = channel;
    switch(channel)//Independent testing
    {
    case YIN0: 	//	 YIN0
        if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
        break;
    case YIN1: //	 YIN1
        Tw9912_input_pin_selet[1] = 0x44; //register valu selete YIN1
        if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
        break;
    case YIN2: //	 YIN2
        Tw9912_input_pin_selet[1] = 0x48;
        if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

        break;
    case YIN3: //	 YIN3
        Tw9912_input_pin_selet[1] = 0x4c;
        if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

        Tw9912_input_pin_selet[0] = 0xe8;
        Tw9912_input_pin_selet[1] = 0x3f; //disable YOUT buffer
        if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
        break;
      case SEPARATION: 	//	 YUV
              Tw9912_input_pin_selet[1] = 0x70;
        if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
        break;
    default :
        tw9912_dbg("%s:you input Channel = %d >>>>>>>>>>>>>>error!\n", __FUNCTION__, channel);
        break;
    }
    return 0;
CONFIG_not_ack_fail:
    printk("tw9912:TW9912_Channel_Choices()--->NACK error\n");
    return -1;
}
Vedio_Format Tw9912TestingChannalSignal(Vedio_Channel Channel)
{
    Vedio_Format ret;
    u8 signal = 0, valu;
    register u8 i;
    u8 tw9912_register[] = {0x1c, 0x07,}; //default input pin selet YIN0
 printk("Tw9912TestingChannalSignal(channel = %d)\n",Channel);   
 if(Channel ==YIN2 ||Channel == SEPARATION)
 return NTSC_P;
mutex_lock(&lock_com_chipe_config);

    if(the_last_config.Channel  != Channel){TW9912_Channel_Choices(Channel);}
    else {tw9912_dbg("the_last_config.Channel  == Channel so not have change channal\n");}
	
    for(i = 0; i < 5; i++)
    {
        read_tw9912(0x01, &signal); //register 0x02 channel selete
        if(signal & 0x80) printk("tw9912:fly_video at channal (%d) not find signal\n", Channel);
        else
        {
            printk("tw9912:fly_video at channal (%d) find signal\n", Channel);
            goto break_for;
        }
	mutex_unlock(&lock_com_chipe_config);
        msleep(20);
	mutex_lock(&lock_com_chipe_config);

    }
if(i==5)goto SIGNAL_NOT_LOCK;
break_for:
    write_tw9912(tw9912_register);// Auto detection
mutex_unlock(&lock_com_chipe_config);
    msleep(40);//waiting......
mutex_lock(&lock_com_chipe_config);
    read_tw9912(0x1c, &signal);
    signal = signal & 0x70;
    switch(signal)
    {
    case 0:
        ret = NTSC_I;
        printk("\nSTDNOW is NTSC(M)\n");
        break;
    case 0x10:
        ret = PAL_I;
        printk("\nSTDNOW is PAL (B,D,G,H,I)\n");
        break;
    case 0x20:
        ret = OTHER;
        printk("\nSTDNOW is SECAM\n");
        break;
    case 0x30:
        ret = NTSC_I;
        printk("\nSTDNOW is NTSC4.43\n");
        break;
    case 0x40:
        ret = PAL_I;
        printk("\nSTDNOW is PAL (M)\n");
        break;
    case 0x50:
        ret = PAL_I;
        printk("\nSTDNOW is PAL (CN)\n");
        break;
    case 0x60:
        ret = PAL_I;
        printk("\nSTDNOW is PAL 60\n");
        break;
    default :
        ret = OTHER;
        printk("\nSTDNOW is NA\n");
        break;
    }
  //  if((Channel == YIN2 || Channel == SEPARATION) && ret == NTSC_I)
    if(Channel == YIN2 || Channel == SEPARATION)
    	{
	   read_tw9912(0xc1, &valu);
	    if(valu & 0x08) //bit3 -->Composite Sync detection status
		    {
		        switch(valu & 0x7) //bit[2:0]
		        {
		        case 0:
		            ret = NTSC_I;
		            break;
		        case 1:
		            ret = PAL_I;
		            break;

		        case 2:
		            ret = NTSC_P;
			  printk("\nDVD is NTSC_P\n");
		            break;
		        case 3:
		            ret = PAL_P;
		            break;

		        default:
		            ret = OTHER;
		            break;
		        }
		        printk("tw9912:testing_NTSCp_video_signal() singal lock back %d\n", ret);
		    }
	}
    //    return NTSC_P;
    printk("tw9912:fly_video test signal is %d",ret);
mutex_unlock(&lock_com_chipe_config);
     return ret;
SIGNAL_NOT_LOCK:
mutex_unlock(&lock_com_chipe_config);
	return OTHER;
}

Vedio_Format camera_open_video_signal_test_in_2(void)
{
    Vedio_Format ret = OTHER;
    u8 channel_1;
    u8 format_1;
    u8 Tw9912_input_pin_selet[] = {0x02, 0x40,}; //default input pin selet YIN0
    TW9912_Signal signal_is_how_1 = {NOTONE, OTHER, source_other};
    TW9912_input_info tw9912_input_information_1;

    tw9912_dbg("camera_open_video_signal_test()!\n");

    tw9912_get_input_info(&tw9912_input_information_1);
    if(tw9912_input_information_1.chip_status1.valu & 0x08 )//bit3=1 Vertical logi is locked to the incoming video soruce
    {
        if(tw9912_input_information_1.chip_status1.valu & 0x01)  signal_is_how_1.vedio_source = source_50Hz;
        else signal_is_how_1.vedio_source = source_60Hz;

        read_tw9912(0x02, &channel_1); //register 0x02 channel selete
        channel_1 = (channel_1 & 0x0c) >> 2 ;
        if(channel_1 == tw9912_status.Channel)
        {
            format_1 = tw9912_input_information_1.component_video_format.valu & 0x70;
            if(format_1 == 0x00)
            {
                signal_is_how_1.Format = NTSC_I;
            }
            else if(format_1 == 0x10)
            {
                signal_is_how_1.Format = PAL_I;
            }
            else if(format_1 == 0x20)
            {
                signal_is_how_1.Format = NTSC_P;
            }
            else if(format_1 == 0x30)
            {
                signal_is_how_1.Format = PAL_P;
            }

        }
        else
        {
            signal_is_how_1.Format = OTHER;
        }
    }

    return signal_is_how_1.Format;
}
int read_tw9912_chips_status(u8 cmd)
{
    static TW9912_input_info tw9912_input_information_status;
    static TW9912_input_info tw9912_input_information_status_next;
    tw9912_get_input_info(&tw9912_input_information_status_next);
    if(cmd)
    {
        /*
        	if(tw9912_input_information_status_next.input_detection.valu != tw9912_input_information_status.input_detection.valu)
        	{

        		 printk("worning:input_detection(0xc1) have change --old = %d,new =%d\n", \
        		 	tw9912_input_information_status.input_detection.valu,\
        		 	tw9912_input_information_status_next.input_detection.valu);

        		  tw9912_input_information_status.input_detection.valu = \
        		  	tw9912_input_information_status_next.input_detection.valu;
        		  return 1;
        	}

        	if(tw9912_input_information_status_next.component_video_format.valu != tw9912_input_information_status.component_video_format.valu)
        	{

        		 printk("worning:component_video_format(0x1e) have change --old = %d,new =%d\n", \
        		 	tw9912_input_information_status.component_video_format.valu,\
        		 	tw9912_input_information_status_next.component_video_format.valu);

        		 tw9912_input_information_status.component_video_format.valu = \
        		 	tw9912_input_information_status_next.component_video_format.valu;
        		   return 1;
        	}
        */
        if(tw9912_input_information_status_next.macrovision_detection.valu != tw9912_input_information_status.macrovision_detection.valu)
        {

            printk("worning:macrovision_detection(0x30) have change --old = %d,new =%d\n",
                   tw9912_input_information_status.macrovision_detection.valu,
                   tw9912_input_information_status_next.macrovision_detection.valu);

            tw9912_input_information_status.macrovision_detection.valu =
                tw9912_input_information_status_next.macrovision_detection.valu;
            //   return tw9912_input_information_status_next.macrovision_detection.valu;
            return 1;
        }
        return 0;
    }
    else
    {
        return tw9912_input_information_status_next.macrovision_detection.valu;
    }
}
Vedio_Format testing_NTSCp_video_signal()
{
    Vedio_Format ret = OTHER;
    u8 valu;//default input pin selet YIN0
    tw9912_dbg("testing_NTSCp_video_signal()\n");
    if(the_last_config.Channel != SEPARATION)
       // Tw9912_init_NTSCp();
       Tw9912_YIN3ToYUV_init_agin();
    read_tw9912(0xc1, &valu);
    if(valu & 0x08) //bit3 -->Composite Sync detection status
    {
        switch(valu & 0x7) //bit[2:0]
        {
        case 0:
            ret = NTSC_I;
            break;
        case 1:
            ret = PAL_I;
            break;

        case 2:
            ret = NTSC_P;
            break;
        case 3:
            ret = PAL_P;
            break;

        default:
            ret = OTHER;
            break;
        }
        printk("testing_NTSCp_video_signal() singal lock back %d\n", ret);
    }
    return ret;
}
Vedio_Format testing_video_signal(Vedio_Channel Channel)
{
    Vedio_Format ret = OTHER;
    u8 channel_1;
    u8 format_1;
    u8 Tw9912_input_pin_selet[] = {0x02, 0x40,}; //default input pin selet YIN0
    TW9912_Signal signal_is_how_1 = {NOTONE, OTHER, source_other};
    TW9912_input_info tw9912_input_information_1;
    mutex_lock(&lock_com_chipe_config);
    //if(the_last_config.Channel != Channel && the_last_config.Channel == SEPARATION)
    if(the_last_config.Channel != Channel)
    {
        Tw9912_init_agin();
    }

    if(Channel > SEPARATION) goto CHANNAL_ERROR;
    signal_is_how_1.Channel = Channel;

    signal_is_how_1.Format = OTHER;
    signal_is_how_1.vedio_source = source_other;
/*
    read_tw9912(0x02, &channel_1); //register 0x02 channel selete
    channel_1 = (channel_1 & 0x0c) >> 2 ; //read back now config Channel
    if( (( (channel_1 != Channel ) && Channel != NOTONE) \
            || (tw9912_status.flag != TW9912_initall_yes)) \
      )//if now config Channel is not testing Channel
        // or tw9912 is not have initall
    {
        if(tw9912_status.flag == TW9912_initall_not)
        {
            tw9912_status.flag = TW9912_initall_yes;
            tw9912_status.format = PAL_I;
            tw9912_status.Channel = Channel;
            tw9912_dbg("first initalll!\n");
            Tw9912_init_PALi();//initall all register
        }

	
	the_last_config.Channel = Channel;
        switch(Channel)//Independent testing
        {
        case 0: 	//	 YIN0
            if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
            break;
        case 1: //	 YIN1
            Tw9912_input_pin_selet[1] = 0x44; //register valu selete YIN1
            if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
            break;
        case 2: //	 YIN2
            Tw9912_input_pin_selet[1] = 0x48;
            if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

            break;
        case 3: //	 YIN3
            Tw9912_input_pin_selet[1] = 0x4c;
            if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

            Tw9912_input_pin_selet[0] = 0xe8;
            Tw9912_input_pin_selet[1] = 0x3f; //disable YOUT buffer
            if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
            break;
        default :
            tw9912_dbg("%s:you input Channel = %d >>>>>>>>>>>>>>error!\n", __FUNCTION__, Channel);
            break;
        }
		
        //msleep(400);//Wait for video Stability ok
        //msleep(100);// ok
        //msleep(50);
    }
    else if( (Channel == SEPARATION ) && the_last_config.Channel != SEPARATION )
    {
        Tw9912_init_NTSCp();//initall all register
        msleep(400);
    }
 */
    tw9912_get_input_info(&tw9912_input_information_1);
    if(tw9912_input_information_1.chip_status1.valu & 0x08 )//bit3=1 Vertical logi is locked to the incoming video soruce
    {
        if(tw9912_input_information_1.chip_status1.valu & 0x01)  signal_is_how_1.vedio_source = source_50Hz;
        else signal_is_how_1.vedio_source = source_60Hz;

    //    read_tw9912(0x02, &channel_1); //register 0x02 channel selete
   //     channel_1 = (channel_1 & 0x0c) >> 2 ;
 //       if(channel_1 == Channel)
    //    {
            format_1 = tw9912_input_information_1.component_video_format.valu & 0x70;
            if(format_1 == 0x00)
            {
                signal_is_how_1.Format = NTSC_I;
            }
            else if(format_1 == 0x10)
            {
                signal_is_how_1.Format = PAL_I;
            }
            else if(format_1 == 0x20)
            {
                signal_is_how_1.Format = NTSC_P;
            }
            else if(format_1 == 0x30)
            {
                signal_is_how_1.Format = PAL_P;
            }

  //      }
   //     else
   //     {
    //        signal_is_how_1.Format = OTHER;
    //    }
    }

    tw9912_dbg("testing_signal(): back %d\n", signal_is_how_1.Format);
    mutex_unlock(&lock_com_chipe_config);
    return signal_is_how_1.Format;
CONFIG_not_ack_fail:
    mutex_unlock(&lock_com_chipe_config);
    tw9912_dbg("tw9912:testing_video_signal()--->NACK error\n");
    ret = OTHER;
CHANNAL_ERROR:
    mutex_unlock(&lock_com_chipe_config);
    tw9912_dbg("tw9912:testing_video_signal()--->Channel input error\n");
    ret = OTHER;
    return ret;
}
int Tw9912_appoint_pin_testing_video_signal(Vedio_Channel Channel)
{
    int ret = -1;
    u8 channel_1;
    u8 Tw9912_input_pin_selet[] = {0x02, 0x40,}; //default input pin selet YIN0
    u8 manually_initiate_auto_format_detection[] = {0x1d, 0xff,}; //default input pin selet YIN0
    mutex_lock(&lock_com_chipe_config);
    tw9912_dbg("@@@@@Tw9912_appoint_pin_testing_video_signal!\n");

    if(Channel != SEPARATION)
    {
        read_tw9912(0x02, &channel_1); //register 0x02 channel selete
        channel_1 = (channel_1 & 0x0c) >> 2 ; //read back now config Channel
        if( ( (channel_1 != Channel ) && Channel != NOTONE) \
                || (tw9912_status.flag != TW9912_initall_yes) )//if now config Channel is not testing Channel
            // or tw9912 is not have initall
        {
            if(tw9912_status.flag == TW9912_initall_not )
            {
                tw9912_status.flag = TW9912_initall_yes;
                tw9912_status.Channel = Channel;
                tw9912_status.format = PAL_I;
                tw9912_dbg("first initalll!\n");
                Tw9912_init_PALi();//initall all register
            }

            the_last_config.Channel = Channel;
            switch(Channel)//Independent testing
            {
            case YIN0: 	//	 YIN0
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
                break;
            case  YIN1: //	 YIN1
                Tw9912_input_pin_selet[1] = 0x44; //register valu selete YIN1
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
                break;
            case  YIN2: //	 YIN2
                Tw9912_input_pin_selet[1] = 0x48;
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

                break;
            case  YIN3: //	 YIN3
                Tw9912_input_pin_selet[1] = 0x4c;
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

                Tw9912_input_pin_selet[0] = 0xe8;
                Tw9912_input_pin_selet[1] = 0x3f; //disable YOUT buffer
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
                break;
            default :
                tw9912_dbg("%s:you input Channel = %d error!\n", __FUNCTION__, Channel);
                break;

            }
            //msleep(400);//Wait for video Stability
        }
    }
    //else if( the_last_config.Channel !=SEPARATION )
    else
    {
        printk("\r\r\n\n");
        printk("tw9912:testing NTSCp The default detection!\n");
        //	Tw9912_init_NTSCp();//initall all register
        tw9912_status.flag = TW9912_initall_yes;
        tw9912_status.Channel = SEPARATION;
        tw9912_status.format = NTSC_P;
        //	msleep(1000);
        signal_is_how[SEPARATION].Format = NTSC_P;
        ret = NTSC_P;
        goto TEST_NTSCp;
    }
    write_tw9912(manually_initiate_auto_format_detection);
    //msleep(100);
    tw9912_get_input_info(&tw9912_input_information);
    Tw9912_analysis_input_signal(&tw9912_input_information, Channel);
    display_tw9912_info();
    switch(signal_is_how[Channel].Format)
    {
    case NTSC_I:
        ret = 1;
        break;
    case PAL_I:
        ret = 2;
        break;

    case NTSC_P:
        ret = 3;
        break;
    case PAL_P:
        ret = 4;
        break;

    default:
        ret = 5;
        break;
    }
TEST_NTSCp:
    mutex_unlock(&lock_com_chipe_config);
    return ret;
CONFIG_not_ack_fail:
    mutex_unlock(&lock_com_chipe_config);
    tw9912_dbg("Tw9912_appoint_pin_testing_video_signal()--->NACK error\n");
    ret = -1;
    return ret;
}
static void TC9912_id(void)
{
    u8 TW9912_ID[] = {0x00, 0x00};
    u8 valu;
    read_tw9912(TW9912_ID[0], &TW9912_ID[1]);
    valu = TW9912_ID[1] >> 4; //ID = 0x60
    if( valu == 0x6)
        printk("TW9912 ID=0x%.2x\n", TW9912_ID[1]);
    else
        printk("TW9912 Communication error!(0x%.2x)\n", TW9912_ID[1]);

}
static int bug_return_for_PALi(void)
{
    int ret;
    u32 i = 0;
    u8 *config_pramat_piont = NULL;
    u8 Tw9912_image[2] = {0x17, 0x87,}; //default input pin selet YIN0ss
    config_pramat_piont = TW9912_INIT_NTSC_Interlaced_input;
    while(config_pramat_piont[i*2] != 0xfe)
    {
        if(write_tw9912(&config_pramat_piont[i*2]) == NACK);
        //		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
        i++;
    }
    tw9912_dbg("Several register allocation, avoid returning the DVD video stop\n");
    Tw9912_image[0] = 0x08; //image dowd 3 line
    Tw9912_image[1] = 0x10; // image down 3 line
    ret =  write_tw9912(Tw9912_image);
    Tw9912_image[0] = 0x09; //image dowd 3 line
    Tw9912_image[1] = 0xf9; // image down 3 line
    ret = write_tw9912(Tw9912_image);

    Tw9912_image[0] = 0x0A; //image dowd 3 line
    Tw9912_image[1] = 0x23; // image down 3 line
    ret = write_tw9912(Tw9912_image);

    Tw9912_image[0] = 0x0B; //image dowd 3 line
    Tw9912_image[1] = 0xec; // image down 3 line
    ret = write_tw9912(Tw9912_image);
    return ret;
}
int Tw9912_init_NTSCp(void)
{
    u32 i = 0;
    u8 *config_pramat_piont = NULL;
    if (tw9912_reset_flag_jam == 0)
    {
        tw9912_reset_flag_jam = 1;
	tw9912_dbg("%s:tw9912_RESX_DOWN\n",__func__);
        tw9912_RESX_DOWN;//\u8fd9\u91cc\u5bf9tw9912\u590d\u4f4d\u7684\u539f\u56e0\u662f\u89e3\u51b3\u5012\u8f66\u9000\u56deDVD\u65f6\u89c6\u9891\u5361\u6b7b\u3002
        tw9912_RESX_UP;
      //  msleep(50);
    }
    tw9912_dbg("Tw9912_init_NTSCp initall tw9912+\n");
    TC9912_id();
    printk("the_last_config.Channel  =%d\n", the_last_config.Channel );
    the_last_config.Channel = SEPARATION;
    the_last_config.format = NTSC_P;
    //	bug_return_for_PALi();
    config_pramat_piont = TW9912_INIT_NTSC_Progressive_input;
    while(TW9912_INIT_Public[i*2] != 0xfe)
    {
        if(write_tw9912(&TW9912_INIT_Public[i*2]) == NACK) goto CONFIG_not_ack_fail;
        i++;
    }
    while(config_pramat_piont[i*2] != 0xfe)
    {
        if(write_tw9912(&config_pramat_piont[i*2]) == NACK) goto CONFIG_not_ack_fail;
        //		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
        i++;
    }

#ifdef DEBUG_PLOG_TW9912
    i = 0;
    while(config_pramat_piont[i*2] != 0xfe)
    {
        u8 data = 0;
        data = 0xff;
        read_tw9912(config_pramat_piont[i*2], &data);
        tw9912_dbg("r a=%x ,v= %x\n", config_pramat_piont[i*2] , data);
        i++;
    }
#endif
    tw9912_dbg("Tw9912_init_NTSCp initall tw9912-\n");
    //msleep(400);
    //Tw9912_appoint_pin_testing_video_signal(SEPARATION);
    return 1;
CONFIG_not_ack_fail:
    tw9912_dbg("%s:have NACK error!\n", __FUNCTION__);
    return -1;
}
int Tw9912_init_agin(void)
{
    u32 i = 0;
    u8 *config_pramat_piont = NULL;
    tw9912_dbg("Tw9912_init_agin +\n");
    //TC9912_id();
    the_last_config.Channel = YIN3;
    the_last_config.format = NTSC_I;
    config_pramat_piont=TW9912_INIT_AGAIN;
   // config_pramat_piont = TW9912_INIT_NTSC_Interlaced_input;
    while(config_pramat_piont[i*2] != 0xfe)
    {
        if(write_tw9912(&config_pramat_piont[i*2]) == NACK) goto CONFIG_not_ack_fail;
        //		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
        i++;
    }
    tw9912_dbg("Tw9912_init_agin -\n");
    return 1;
CONFIG_not_ack_fail:
    tw9912_dbg("%s:have NACK error!\n", __FUNCTION__);
    return -1;
}
int Tw9912_YIN3ToYUV_init_agin(void)
{
    u32 i = 0;
    u8 *config_pramat_piont = NULL;
    tw9912_dbg("Tw9912_YIN3ToYUV_init_agin +\n");
   // TC9912_id();
   the_last_config.Channel = SEPARATION;
   the_last_config.format = NTSC_P;
    config_pramat_piont=TW9912_YIN3ToYUV_INIT_AGAIN;
   // config_pramat_piont = TW9912_INIT_NTSC_Interlaced_input;
    while(config_pramat_piont[i*2] != 0xfe)
    {
        if(write_tw9912(&config_pramat_piont[i*2]) == NACK) goto CONFIG_not_ack_fail;
        //		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
        i++;
    }
    tw9912_dbg("Tw9912_YIN3ToYUV_init_agin -\n");
    return 1;
CONFIG_not_ack_fail:
    tw9912_dbg("%s:have NACK error!\n", __FUNCTION__);
    return -1;
}
int Tw9912_init_PALi(void)
{
    u32 i = 0;
    u8 *config_pramat_piont = NULL;
    tw9912_dbg("Tw9912_init_PALi initall tw9912+\n");
    TC9912_id();
    the_last_config.Channel = YIN3;
    the_last_config.format = PAL_I;
    config_pramat_piont = TW9912_INIT_PAL_Interlaced_input;
    while(TW9912_INIT_Public[i*2] != 0xfe)
    {
        if(write_tw9912(&TW9912_INIT_Public[i*2]) == NACK) goto CONFIG_not_ack_fail;
        i++;
    }
    while(config_pramat_piont[i*2] != 0xfe)
    {
        if(write_tw9912(&config_pramat_piont[i*2]) == NACK) goto CONFIG_not_ack_fail;
        //		tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
        i++;
    }
    tw9912_dbg("Tw9912_appoint_pin_testing_video_signal initall tw9912-\n");
    return 1;
CONFIG_not_ack_fail:
    tw9912_dbg("%s:have NACK error!\n", __FUNCTION__);
    return -1;
}
static void TW9912_inital_time_test_signal_stability(Vedio_Channel Channel)
{
 int ret = 0;
 while(1)
 	{
            ret = read_tw9912_chips_status(1);//return register valu
            msleep(10);
            read_tw9912_chips_status_flag_1++;

            //printk("tw9912:read_tw9912_chips_status back %.2x\n",ret);
            if( ret )
            {
                read_tw9912_chips_status_flag ++;
                read_tw9912_chips_status_flag_1=0;
                printk("tw9912:worning Channel = %d input  signal unstabitily! %d\n", Channel, read_tw9912_chips_status_flag_1);
            }
            else
            {
                printk("tw9912: input  signal stabitily! %d ,%d\n", read_tw9912_chips_status_flag, read_tw9912_chips_status_flag_1);
            }
            if(read_tw9912_chips_status_flag > 5 || read_tw9912_chips_status_flag_1 > 10)
            {
                if (read_tw9912_chips_status_flag_1 >= 10)
                    ;//tw9912_signal_unstabitily_for_Tw9912_init_flag = 1;//find colobar flag signal bad
                break;
            }
 	}
 	

	 read_tw9912_chips_status_flag = 0;
        read_tw9912_chips_status_flag_1 = 0;
}
int Tw9912_init(Vedio_Format config_pramat, Vedio_Channel Channel)
{
    Vedio_Format ret_format;
    u32 i = 0;
    int ret = 0, delte_signal_count = 0;
    u8 *config_pramat_piont = NULL;
    u8 Tw9912_input_pin_selet[] = {0x02, 0x40,}; //default input pin selet YIN0
    printk("tw9912: inital begin\n");
    mutex_lock(&lock_com_chipe_config);
    TC9912_id();
    //if(Channel == NOTONE&&tw9912_status.flag == TW9912_initall_not)
    if(Channel == NOTONE)
    {
        //随便初始一下，防止i2c由于tw9912不工作影响通信。
        Tw9912_init_PALi();//initall all register
        //神码情况
    }
    else if(config_pramat != STOP_VIDEO)
    {
    	TW9912_Channel_Choices(Channel);
	if(0)
	{
	    TW9912_inital_time_test_signal_stability(Channel);
    	}
        tw9912_dbg("tw9912:Tw9912_init()-->Tw9912_appoint_pin_testing_video_signal(%d)\n", Channel);
        mutex_unlock(&lock_com_chipe_config);
SIGNAL_DELTE_AGAIN:
        tw9912_dbg("tw9912 inital befor test signal count:%d;", delte_signal_count);
        ret = Tw9912_appoint_pin_testing_video_signal(Channel);//bad
        delte_signal_count++;
        msleep(20);
        if(ret == 5 && delte_signal_count < 4) goto SIGNAL_DELTE_AGAIN;
        delte_signal_count = 0;
        mutex_lock(&lock_com_chipe_config);
        if(ret == 5) //the channel is not signal input
        {
		tw9912_signal_unstabitily_for_Tw9912_init_flag = 0;//find colobar flag signal bad
		mutex_unlock(&lock_com_chipe_config);
		ret_format = Tw9912TestingChannalSignal(Channel);
		mutex_lock(&lock_com_chipe_config);
		if(ret_format == OTHER )
			goto NOT_signal_input;
		else
			signal_is_how[Channel].Format = ret_format;
        }
        if(ret == -1)
            goto CONFIG_not_ack_fail;

        switch(ret)
        {
        case NTSC_I:
            printk("\nVideoFormaIs NTSC_I\n\n");
            break;
        case NTSC_P:
            printk("\nVideoFormatIs NTSC\n\n");
            break;
        case PAL_I:
            printk("\nVideoFormatIs PAL_I\n\n");
            break;
        case PAL_P:
            printk("\nVideoFormatIs PAL_P\n\n");
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
        switch(signal_is_how[Channel].Format)
        {
        case NTSC_I:
            tw9912_status.flag = TW9912_initall_yes;
            tw9912_status.Channel = Channel;
            tw9912_status.format = NTSC_I;
            config_pramat_piont = TW9912_INIT_NTSC_Interlaced_input;
            //config_pramat_piont=TW9912_INIT_PAL_Interlaced_input;
            //printk("tw9912:%s:config_pramat->NTSC_Interlace\n", __func__);
            break;

        case PAL_I:
            tw9912_status.flag = TW9912_initall_yes;
            tw9912_status.Channel = Channel;
            tw9912_status.format = PAL_I;
            config_pramat_piont = TW9912_INIT_PAL_Interlaced_input;
            //config_pramat_piont=TW9912_INIT_NTSC_Interlaced_input;
            //printk("tw9912:%s:config_pramat->PAL_Interlace\n", __func__);
            break;

        case NTSC_P:
            tw9912_status.flag = TW9912_initall_yes;
            tw9912_status.Channel = Channel;
            tw9912_status.format = NTSC_P;
            config_pramat_piont = TW9912_INIT_NTSC_Progressive_input;
           //printk("tw9912:%s:config_pramat->NTSC_Progressive\n", __func__);
            break;

        case PAL_P:
            tw9912_status.flag = TW9912_initall_yes;
            tw9912_status.Channel = Channel;
            tw9912_status.format = PAL_P;
            config_pramat_piont = TW9912_INIT_PAL_Progressive_input;
            //printk("tw9912:%s:config_pramat->PAL_Progressive\n", __func__);
            break;

        default:
            printk("Format is Invalid ******\n");
            printk("tw9912:%s:signal_is_how[Channel].Format=%d\n", __func__, signal_is_how[Channel].Format);
            goto NOT_signal_input;
            break;
        }

        the_last_config.Channel = Channel;
        the_last_config.format = signal_is_how[Channel].Format;

        while(TW9912_INIT_Public[i*2] != 0xfe)
        {
            if(write_tw9912(&TW9912_INIT_Public[i*2]) == NACK) goto CONFIG_not_ack_fail;
            i++;
        }
        //for(;i<SIZE_OF_ARRAY(config_pramat_piont)/2;i++)
        while(config_pramat_piont[i*2] != 0xfe)
        {
            if(write_tw9912(&config_pramat_piont[i*2]) == NACK) goto CONFIG_not_ack_fail;
            //				tw9912_dbg("w a=%x,v=%x\n",config_pramat_piont[i*2],config_pramat_piont[i*2+1]);
            if(signal_is_how[Channel].Format == NTSC_P \
                    && config_pramat_piont[i*2] > 0x24\
                    && config_pramat_piont[i*2] < 0x2d)
                usleep(100);
            i++;
        }
        if(Channel == SEPARATION)
        {
            u8 manually_initiate_auto_format_detection[] = {0x1d, 0x89,}; //bit7
            //Writing 1 to this bit will manually initiate the auto format detection process
            //bit3 and bit0
            //only  enable recognition of NTSC
            TW9912_input_info tw9912_input_information_NTSCp;
            write_tw9912(manually_initiate_auto_format_detection);
            //msleep(100);
            tw9912_get_input_info(&tw9912_input_information_NTSCp);
            if(tw9912_input_information_NTSCp.input_detection.valu & 0x08)//Composite Sync detection status
            {
                unsigned char Input_source_format;//Register 0xc1 bit[1:0]
                /*
                	input source format detection in the case of composite sync.
                	0 = 480i
                	1 = 576i
                	2 = 480p
                	3 = 576p
                	4 = 1080i
                	5 = 720p
                	6 = 1080p 7 = none of above
                */
                Input_source_format = tw9912_input_information_NTSCp.input_detection.valu & 0x07 ;//bit[2:0]
                switch (Input_source_format)
                {
                case 0x2://480p
                    printk("tw9912 initall NTSCp config is dong and find NTSCp signal\n");
                    break;
                case 0x0://480i
                case 0x1://567i
                case 0x3://576p
                case 0x4://1080i
                case 0x5://720p
                case 0x6://1080p
                case 0x7://none of above
                    goto NOT_signal_input;
                    break;
                }
            }

        }
        else if(Channel != YIN3 &&( signal_is_how[Channel].Format == PAL_I || signal_is_how[Channel].Format == NTSC_I) ) //Set channel
        {
            switch(Channel)
            {
            case 0: 	//	 YIN0
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
                break;
            case 1: //	 YIN1
                Tw9912_input_pin_selet[1] = 0x44; //register valu selete YIN1
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
                break;
            case 2: //	 YIN2
                Tw9912_input_pin_selet[1] = 0x48;
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

                break;
            case 3: //	 YIN3
                Tw9912_input_pin_selet[1] = 0x4c;
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;

                Tw9912_input_pin_selet[0] = 0xe8; //only selet YIN3 neet set
                Tw9912_input_pin_selet[1] = 0x3f; //disable YOUT buffer
                if(write_tw9912(Tw9912_input_pin_selet) == NACK) goto CONFIG_not_ack_fail;
                break;
            default :
                printk("tw9912:%s:you input Channel = %d error!\n", __FUNCTION__, Channel);
                break;
            }

        }

    }
    tw912_run_sotp_flag.format = signal_is_how[Channel].Format;
    if(signal_is_how[Channel].Format == PAL_I)
    {
    ;
    /*
        u8 Tw9912_Parameter[] = {0, 0,};

        Tw9912_Parameter[0] = 0x0a;
        Tw9912_Parameter[1] = 0x1b;
        ret = write_tw9912(Tw9912_Parameter);

        Tw9912_Parameter[0] = 0x09;
        Tw9912_Parameter[1] = 0x27;
        ret = write_tw9912(Tw9912_Parameter);

        tw912_run_sotp_flag.run = 1;
        printk("Create a new thread\n");
        tw9912_Correction_Parameter_fun = kthread_run(thread_tw9912_Correction_Parameter_fun, NULL, "flyvideo_Parameter");
    */
    }
    else if(signal_is_how[Channel].Format == NTSC_I)
    {
    ;
    /*
        u8 Tw9912_Parameter[] = {0, 0,};

        Tw9912_Parameter[0] = 0x0a;
        Tw9912_Parameter[1] = 0x19;
        ret = write_tw9912(Tw9912_Parameter);

        tw912_run_sotp_flag.run = 1;
        printk("Create a new thread\n");
        tw9912_Correction_Parameter_fun = kthread_run(thread_tw9912_Correction_Parameter_fun, NULL, "flyvideo_Parameter");
     */
    }
    else
    {
        ;
    }
#ifdef DEBUG_PLOG_TW9912
    i = 0;
    while(config_pramat_piont[i*2] != 0xfe)
    {
        u8 data = 0;
        data = 0xff;
        read_tw9912(config_pramat_piont[i*2], &data);
        tw9912_dbg("r a=%x ,v= %x\n", config_pramat_piont[i*2] , data);
        i++;
    }
#endif
    printk("tw9912: inital done\n");
    //Correction_Parameter_fun(signal_is_how[Channel].Format);
    //CONFIG_is_old:
    mutex_unlock(&lock_com_chipe_config);
    return 1;
CONFIG_not_ack_fail:
    mutex_unlock(&lock_com_chipe_config);
    tw9912_dbg("%s:have NACK error!\n", __FUNCTION__);
    return -2;
NOT_signal_input:
    mutex_unlock(&lock_com_chipe_config);
    tw9912_dbg("%s:the channal=%d not have video signal!\n", __FUNCTION__, Channel);

    return -1;
}













