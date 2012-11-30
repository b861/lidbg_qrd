#include "lidbg.h"

#ifdef DEBUG_TVP5150

#ifdef SOC_VIN_DETECT
static struct task_struct *vin_task;

bool vin_detect()
{

    u8 reg_addr = 0x88;
    u8 reg_value = 0;
    //u32 count = 0;
    //FUNCTION_IN;

    //ADC_I2C_Read(NULL, reg_addr, &reg_value, 1);
    //RETAILMSG(0, (TEXT("SOC_VIN_Detect:%x\r\n"), reg_value));

    SOC_I2C_Rec(1, 0x5c, reg_addr, &reg_value , 1);
    //lidbg("%x\n", reg_value);

    if(
        //((reg_value&(0x01 << 4)) == 1) || //20111121
        ((reg_value&(0x01 << 4)) ) ||
        ((reg_value&(0x01 << 3)) == 0) ||
        ((reg_value&(0x01 << 2)) == 0) ||
        ((reg_value&(0x01 << 1)) == 0)
    )
        return 0;
    else
    {
#if 1
        //\u0152ì²âµ\u0153ÊÓÆµ

        if((reg_value&(0x01 << 5)))
        {
            // lidbg("PAL 50Hz\n");

            //RETAILMSG(0, (TEXT("PAL 50Hz\r\n")));
            //vin_type = PAL;
        }
        else
        {
            //  lidbg("NTSC 60Hz\n");

            //RETAILMSG(0, (TEXT("NTSC 60Hz\r\n")));
            // vin_type = NTSC;
        }
#endif
        return 1;

    }
    // FUNCTION_OUT;

}






int thread_vin(void *data)
{
    bool old_ret = 0;

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
#if 0

            static u32 tmp = 0;
            lidbg("bl %d\n", tmp);
            SOC_BL_Set(tmp);
            msleep(2000);
            tmp = (tmp + 10) & 0xff;
#endif


#if 0
            bool ret;

            ret =  vin_detect();
            if(old_ret == ret)
            {
                msleep(200);
            }
            else
            {


                if(ret)//lcd on
                {
#ifdef FLY_BOARD_2ST
                    //soc_io_output(0, 4, 0);//PWM a4
                    c
                    SOC_BL_Set(128);
                    SOC_IO_Output(5, 5, 1);//PANNE_PEN f5
                    SOC_IO_Output(6, 2, 1);//LCD_IDLE g2
#endif
#ifdef FLY_BOARD_3ST
                    // soc_io_output(0, 4, 0);//PWM a4
                    SOC_BL_Set(128);
                    SOC_IO_Output(4, 7, 1);//PANNE_PEN f5
                    SOC_IO_Output(6, 2, 0);//LCD_IDLE g2
#endif
                }
                else//lcd off
                {
                    lidbg("lcd off\n");
#ifdef FLY_BOARD_2ST
                    SOC_IO_Output(6, 2, 0);//LCD_IDLE g2
                    SOC_IO_Output(5, 5, 0);//PANNE_PEN f5
                    SOC_IO_Output(0, 4, 1);//PWM a4
#endif
#ifdef FLY_BOARD_3ST
                    SOC_IO_Output(6, 2, 1);//LCD_IDLE g2
                    SOC_IO_Output(4, 7, 0);//PANNE_PEN f5
                    SOC_IO_Output(0, 4, 1);//PWM a4
#endif
                }
                msleep(2000);

            }

            old_ret = ret;
#endif
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}



void vin_thread_start()
{
    int err;
    vin_task = kthread_create(thread_vin, NULL, "vin_task");
    if(IS_ERR(vin_task))
    {
        lidbg("Unable to start kernel thread.\n");
        err = PTR_ERR(vin_task);
        vin_task = NULL;
        //return err;
        //exit;
    }
    wake_up_process(vin_task);




}




#endif






//CHANNEL reg - 0x0
#define TVP5150_COMPOSITE0 0x0   //cam
#define TVP5150_COMPOSITE1 0x2   //dvd
#define TVP5150_SVIDEO     0x1

#define TVP5150_DVD


u8 ADC_CVBS_INIT[] =
{
#ifdef TVP5150_DVD
    0x00, TVP5150_COMPOSITE1,//CHANNEL
#else
    0x00, TVP5150_COMPOSITE0,//CHANNEL
#endif
    // 0x03, 0x0D,
    0x03, 0x2D,//gpcl output 0s
    0x08, 0x04,
    0x0a, 0xe6,//color
    0x0c, 0x80,//contrast
#ifdef TVP5150_DVD
    0x09, 0x68,//brightness
#else
    0x09, 50,//brightness  cam
#endif
    0x0b, 0x00, //hue
    //0x0f, 0x00,
    0x0f, 0x02, // disable intr
    0x12, 0x04,
    //  0xFF		// end flag

};

int video_codec_init(void)
{
#if 1
    u32 i = 0;
    u8 data = 0;

    lidbg("tvp5150 init+\n");
    //power
    //SOC_IO_Output(5, 24, 1);  //dvd reset
    DVD_RESET;
    TVP5150_POWER_UP;   //power

    msleep(100);
    //reset
    TVP5150_RESET;


    for(i = 0; i < SIZE_OF_ARRAY(ADC_CVBS_INIT) / 2; i++)
    {
        SOC_I2C_Send(1, 0x5c, &ADC_CVBS_INIT[i*2] , 2);
    }

    for(i = 0; i < SIZE_OF_ARRAY(ADC_CVBS_INIT) / 2; i++)
    {
        data = 0xff;
        SOC_I2C_Rec(1, 0x5c, ADC_CVBS_INIT[i*2], &data , 1);
        lidbg("%x %x\n", ADC_CVBS_INIT[i*2], data);
    }

#ifdef SOC_VIN_DETECT
    vin_thread_start();
#endif
    lidbg("tvp5150 init-\n");

    return 1;
#endif
}


void tvp5150_set_high_z(bool status)
{
    if(status)
    {
        //tvp5150 high impedance
        u8 buf[2];
        buf[0] = 0x03;//reg_addr
        buf[1] = 0x00;//reg_data
        SOC_I2C_Send(1, 0x5c, buf , 2);
    }
    else
    {
        //tvp5150 output enable
        u8 buf[2];
        buf[0] = 0x03;//reg_addr
        buf[1] = 0x0d;//reg_data
        SOC_I2C_Send(1, 0x5c, buf , 2);

    }

}


#endif



























