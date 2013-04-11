#include "i2c_io.h"
#include "tw9912.h"
#include "tc358746xbg_config.h"
#include "TC358746XBG.h"
#define I2C_US_IO
#define APAT_BUS_ID 1
static int tc358746_reset_flag = 0;
struct TC358_register_struct colorbar_init_user_tab[] =
{
    //80 pixel of hui
    {0x00e8, 0x00, register_value_width_16},
    {0x00e8, 0x94, register_value_width_16},
};
#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)\
do { \
long timeout = (nMilliSec) * HZ / 1000; \
while(timeout > 0) \
{ \
timeout = schedule_timeout(timeout); \
} \
}while(0);
#endif


#define COLOR_BLACK 0
#define COLOR_RED 2
#define COLOR_BLUE 4
#define COLOR_PINK 6
#define COLOR_GREEN 8
#define COLOR_LIGHT_BLUE 10
#define COLOR_YELLOW 12
#define COLOR_WHITE 14

void TC358_Hardware_Rest(void)
{
    tc358_RESX_DOWN;
    //	udelay(100);
    //	msleep(100);
    //msleep(10000);
    tc358_RESX_UP;
    //	udelay(100);
    //	msleep(100);
}
static void Power_contorl(void)
{
    RTC358_CS_DOWN;
    tc358_MSEL_UP;//NULL now  1: Par_in -> CSI-2 TX
    TC358_Hardware_Rest();
}
i2c_ack TC358_Register_Read(u16 add, char *buf, u8 flag)
{
i2c_ack ret ;
    //int buf_change;
    if(flag == register_value_width_16)
    {
      ret =  i2c_read_2byte(APAT_BUS_ID, TC358746_I2C_ChipAdd, add, buf, 2);
    }
    else if (flag == register_value_width_32)
    {
      ret =  i2c_read_2byte(APAT_BUS_ID, TC358746_I2C_ChipAdd, add, buf, 4);
        //	buf_change=*buf>>16;
        //	*buf=*buf<<16|buf_change;
    }
    else
        tc358746_dbg("you set regitset value width have error\n");
return ret;
}

i2c_ack TC358_Register_Write(u16 *add, u32 *valu, u8 flag)
{
i2c_ack ret;
    u8 BUF_tc358[6]; // address 2 valu=4;
    //u8 huang;
    if(flag == register_value_width_16)
    {
        //tc358746_dbg("TC358_Register_Write add=%x valu=%x width=%d\n\n",*add,(U16)*valu,flag);
        BUF_tc358[0] = ((*add) >> 8) & 0xff; //add MSB first
        BUF_tc358[1] = (*add) & 0xff;

        BUF_tc358[2] = (((u16) * valu) >> 8) & 0xff;
        BUF_tc358[3] = ((u16) * valu) & 0xff;
        //for(huang=0;huang<4;huang++)TC358746_colorbar_640_480_fps60_init_tab
        //{
        //	 tc358746_dbg("BUF_tc358[%d]=%x\n",huang,BUF_tc358[huang]);
        //}
        //tc358746_dbg("APAT_BUS_ID=%d,TC358746_I2C_ChipAdd=0x%x,sub_addr=0x%.2x%.2x,volu=0x%.2x%.2x\n",APAT_BUS_ID,TC358746_I2C_ChipAdd,BUF_tc358[0],BUF_tc358[1],BUF_tc358[2],BUF_tc358[3]);
       ret = i2c_write_2byte(APAT_BUS_ID, TC358746_I2C_ChipAdd, BUF_tc358, 4);
    }
    else if (flag == register_value_width_32)
    {
        //tc358746_dbg("TC358_Register_Write add=%x valu=%x width=%d\n\n",*add,*valu,flag);
        BUF_tc358[0] = (*add >> 8) & 0xff; //add MSB first
        BUF_tc358[1] = (*add) & 0xff;

        BUF_tc358[4] = (*valu >> 24) & 0xff; //32bit data valu MSB
        BUF_tc358[5] = (*valu >> 16) & 0xff;
        BUF_tc358[2] = (*valu >> 8) & 0xff;
        BUF_tc358[3] = (*valu) & 0xff;
        /*
        //在写i2c 32bit宽度的寄存器时，数据先发送  低  16 位高 八位，低八位 再发送   高   16位高八位，低八位。
        //
        */
        //for(huang=0;huang<6;huang++)
        //	{
        //		 tc358746_dbg("BUF_tc358[%d]=%x\n",huang,BUF_tc358[huang]);
        //        }
        //tc358746_dbg("APAT_BUS_ID=%d,TC358746_I2C_ChipAdd=0x%x,sub_addr=0x%.2x%.2x,volu=0x%.2x%.2x%.2x%.2x\n",APAT_BUS_ID,TC358746_I2C_ChipAdd,BUF_tc358[0],BUF_tc358[1],BUF_tc358[4],BUF_tc358[5],BUF_tc358[2],BUF_tc358[3]);
        ret = i2c_write_2byte(APAT_BUS_ID, TC358746_I2C_ChipAdd, BUF_tc358, 6);
    }
    else
    {
        tc358746_dbg("you set regitset value width have error\n");
	return NACK;
    }
return ret;
}
static void TC358_Software_Rest(void)
{
    u16 add = 0x0002;
    u32 valu = 1;
    TC358_Register_Write(&add, &valu, register_value_width_16	); //reset
    msleep(1);
    valu = 0;
    TC358_Register_Write(&add, &valu, register_value_width_16	); //normal
    msleep(1);
}
static int TC358_Register_config(struct TC358_register_struct *TC358746_init_tab)
{
int ret;
i2c_ack back_ret;
register int i = 0;
#ifdef tc358746_debug
    u8 valu[4];
#endif
  //  TC358_Software_Rest();
    while(TC358746_init_tab[i].add_reg != 0xffff)//write
    {

      back_ret =  TC358_Register_Write(&(TC358746_init_tab[i].add_reg), \
                             &(TC358746_init_tab[i].add_val), TC358746_init_tab[i].registet_width);
      if(back_ret  == NACK) goto NACK_BREAK;

        i++;
    }
#ifdef tc358746_debug
    i = 0;
    while(TC358746_init_tab[i].add_reg != 0xffff)//read
    {

        if(TC358746_init_tab[i].add_reg == 0x500 || TC358746_init_tab[i].add_reg == 0x204 || TC358746_init_tab[i].add_reg == 0x518)
        {

            tc358746_dbg("TC358 read register=%02x,write only\n", TC358746_init_tab[i].add_reg);

        }
        else
        {
            back_ret = TC358_Register_Read((TC358746_init_tab[i].add_reg), valu, TC358746_init_tab[i].registet_width);
  	    if(back_ret  == NACK) goto NACK_BREAK;
            if(TC358746_init_tab[i].registet_width == 32)
                tc358746_dbg("r a=%x,v=%02x%02x%02x%02x\n", TC358746_init_tab[i].add_reg, valu[2], valu[3], valu[0], valu[1]);
            if(TC358746_init_tab[i].registet_width == 16)
                tc358746_dbg("r a=%x,v=%02x%02x\n", TC358746_init_tab[i].add_reg, valu[0], valu[1]);
        }
        i++;
    }
#endif
return 0;
NACK_BREAK:
printk("interuppt config because TC358746 NACK\n");
return -1;
}
static int TC358_id(void)
{
    u8 valu[4];
	int ret;
	
    int  retry;
    for(retry = 0; retry < 5; retry++)
    {
	    TC358_Register_Read((tc358746_id[0].add_reg), valu, tc358746_id[0].registet_width);
	    if(valu[0] == tc358746_id[0].add_val >> 8)
	    {
	        printk("TC358746xbg ID=%02x%02x\n", valu[0], valu[1]);
			ret = 1;
	    }
	    else
	    {
	        printk("TC358746xbg Read Back ID=0x%02x%02x\n", valu[0], valu[1]);
	        printk("****************error TC358746xbg devieces is not fond********************\n");
			ret = -1;
	    }

		
        if (ret < 0)
        {
        	msleep(50);
            continue;
        }
        else
            return ret;

		
    }
	return ret;

}
void TC358_data_output_enable(u8 flag)
{
    u16 sub_addr = 0x4;
    u32 valu;
    if(flag == 1)
    {
        printk("TC358 output enable\n");
        valu = 0x1044; //enable
        TC358_Register_Write(&sub_addr, &valu, register_value_width_16);

    }
    else
    {
        printk("TC358 output disable\n");
        valu = 0x1014; //disable
        TC358_Register_Write(&sub_addr, &valu, register_value_width_16);

    }
}

void colorbar_init(void)
{
    u16 add_reg_1;
    u32 add_val_1;
    u8 i;
    register u8 j;
    i2c_ack ret;
   ret = TC358_Register_config(lingceng_init_tab);
     if(ret == NACK) goto NACK_BREAK;
    printk("\n\nTC358746:parameter is lingceng_init_tab!\n");
/* */
    //for(i=0;i<8;i++)
    for(i = 4; i < 7; i += 2)
    {
        //for(j=48;j>0;j--)
        for(j = 180; j > 0; j--)
        {
           ret = TC358_Register_Write(&(colorbar_init_tab[2*i].add_reg), &(colorbar_init_tab[2*i].add_val), colorbar_init_tab[2*i].registet_width);
             if(ret == NACK) goto NACK_BREAK;
           ret = TC358_Register_Write(&(colorbar_init_tab[2*i+1].add_reg), &(colorbar_init_tab[2*i+1].add_val), colorbar_init_tab[2*i+1].registet_width);
             if(ret == NACK) goto NACK_BREAK;
        }
    }

    add_reg_1 = 0x00e0; //使能colobar
    add_val_1 = 0xc1df;
    TC358_Register_Write(&add_reg_1, &add_val_1, register_value_width_16);
NACK_BREAK:
	;
}
void colorbar_init_blue(u8 color_flag)
{
    u16 add_reg_1;
    u32 add_val_1;
    u16 i;
    register u16 j;
    int ret_back;
    i2c_ack ret;
    printk("TC358746:parameter is lingceng_init_tab!\n");
   ret_back = TC358_Register_config(lingceng_init_tab);
   if(ret_back == -1) goto NACK_BREAK;

    i = color_flag - 1;
    for(j = 360; j > 0; j--)
    {
        ret =TC358_Register_Write(&(colorbar_init_tab[2*i].add_reg), &(colorbar_init_tab[2*i].add_val), colorbar_init_tab[2*i].registet_width);
	  if(ret == NACK) goto NACK_BREAK;
        ret =TC358_Register_Write(&(colorbar_init_tab[2*i+1].add_reg), &(colorbar_init_tab[2*i+1].add_val), colorbar_init_tab[2*i+1].registet_width);
	  if(ret == NACK) goto NACK_BREAK;
    }
    add_reg_1 = 0x00e0; //使能colobar
    add_val_1 = 0xc1df;
    TC358_Register_Write(&add_reg_1, &add_val_1, register_value_width_16);
return ;
NACK_BREAK:
printk("ERROR TC358 NACK :%s\n",__func__);
}
static void colorbar_init_1(void)
{
    u16 add_reg_1;
    u32 add_val_1;
    int ret_back;
    printk("TC358746:parameter is lingceng_init_tab!\n");
   ret_back = TC358_Register_config(lingceng_init_tab);
   if(ret_back == -1) goto NACK_BREAK;
   
    add_reg_1 = 0x00e0; //使能colobar
    add_val_1 = 0xc1df;
    TC358_Register_Write(&add_reg_1, &add_val_1, register_value_width_16);
return ;
NACK_BREAK:
printk("ERROR TC358 NACK :%s\n",__func__);
}
static void colorbar_init_user(void)
{
    u16 add_reg_1;
    u32 add_val_1;
    int ret_back;
    register u16 j;
    i2c_ack ret;
    printk("TC358746:parameter is lingceng_init_tab!\n");
   ret_back = TC358_Register_config(lingceng_init_tab);
   if(ret_back == -1) goto NACK_BREAK;
   printk("colorbar_init_user_tab[0] = 0x%.2x\n",colorbar_init_user_tab[0].add_val);
      printk("colorbar_init_user_tab[1] = 0x%.2x\n",colorbar_init_user_tab[1].add_val);

       for(j = 360; j > 0; j--)
    {
        ret =TC358_Register_Write(&(colorbar_init_user_tab[0].add_reg), &(colorbar_init_user_tab[0].add_val), colorbar_init_user_tab[0].registet_width);
	  if(ret == NACK) goto NACK_BREAK;
        ret =TC358_Register_Write(&(colorbar_init_user_tab[1].add_reg), &(colorbar_init_user_tab[1].add_val), colorbar_init_user_tab[1].registet_width);
	  if(ret == NACK) goto NACK_BREAK;
    }
    add_reg_1 = 0x00e0; //使能colobar
    add_val_1 = 0xc1df;
    TC358_Register_Write(&add_reg_1, &add_val_1, register_value_width_16);
return ;
NACK_BREAK:
printk("ERROR TC358 NACK :%s\n",__func__);
}
void TC358_init(Vedio_Format flag)
{
int ret;
    printk("TC358 inital begin\n");
    tc358746_dbg("flag= %d\n", flag);
    Power_contorl();
    ret = TC358_id();
	if(ret < 0)
		return;
    if(flag <= COLORBAR + TC358746XBG_WHITE+1)
    {
        switch (flag)
        {
        case NTSC_I:
        case NTSC_P:
            TC358_Register_config(NTSCp_init_tab);
            printk("\n\nTC358746:parameter is NTSCp_init_tab!\n\n");
            break;

        case PAL_I:
        case PAL_P:
            TC358_Register_config(PALp_init_tab);
            printk("\n\nTC358746:parameter is PALp_init_tab!\n\n");
            break;

        case STOP_VIDEO:
            TC358_Register_config(Stop_tab);
            printk("\n\nTC358746:parameter is is Stop_tab!\n\n");
            break;
        case COLORBAR:
            colorbar_init();
            printk("\n\nTC358746:parameter is is COLORBAR!\n\n");
            break;
        case COLORBAR+TC358746XBG_BLUE:
            colorbar_init_blue(TC358746XBG_BLUE);
            printk("\n\nTC358746:parameter is is COLORBAR TC358746XBG_BLUE!\n\n");
            break;
        case COLORBAR+TC358746XBG_RED:
            colorbar_init_blue(TC358746XBG_RED);
            printk("\n\nTC358746:parameter is is COLORBAR TC358746XBG_RED!\n\n");
            break;
        case COLORBAR+TC358746XBG_GREEN:
            colorbar_init_blue(TC358746XBG_GREEN);
            printk("\n\nTC358746:parameter is is COLORBAR TC358746XBG_GREEN!\n\n");
            break;
        case COLORBAR+TC358746XBG_LIGHT_BLUE:
            colorbar_init_blue(TC358746XBG_LIGHT_BLUE);
            printk("TC358746:parameter is is COLORBAR TC358746XBG_LIGHT_BLUE!\n\n");
            break;
        case COLORBAR+TC358746XBG_BLACK:
	    colorbar_init_user();
	    printk("TC358746:parameter is colorbar_init_user!\n\n");
            break;
        case COLORBAR+TC358746XBG_YELLOW:
      	    colorbar_init_blue(TC358746XBG_YELLOW);
       	    printk("\n\nTC358746:parameter is is COLORBAR TC358746XBG_YELLOW!\n\n");
        break;
        case COLORBAR + TC358746XBG_WHITE+1://number 15
      	    colorbar_init_1();
       	    printk("\n\nTC358746:parameter only lingceng_init_tab!\n\n");
        break;
		
        default :
            colorbar_init();
            printk("\n\nTC358746:parameter is default NTSCi_init_tab!\n\n");
            break;
        }
    }
    else
    {
        printk("\n\nTC358746:error:you input TC358746 parameter is not have !\n\n");
    }
    tc358746_dbg(KERN_INFO "Build Time: %s %s  %s \n", __FUNCTION__, __DATE__, __TIME__);
    printk("TC358 inital done\n");

}
#if 0
void TC358_exit( void )
{
    tc358746_dbg("Now stop thread :sent_TC358_VBsram_Thread\n");
    //	kthread_stop(sent_TC358_VBsram_Thread_piont);
    //  sent_TC358_VBsram_Thread_piont = NULL;
    RTC358_CS_DOWN;
    TC358_Sleep();
    //RTC358_CS_UP;

}
#endif


