/* drivers/input/touchscreen/goodix_touch.c
 *
 * Copyright (C) 2010 Goodix, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *111
 */
//#define SOC_COMPILE

#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"

#else
#include "lidbg_def.h"

#include "lidbg_enter.h"

LIDBG_DEFINE;
#endif



#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/i2c.h>


#include <asm/io.h>
#include <asm/irq.h>
#include <mach/hardware.h>


#include <mach/irqs.h>



#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/earlysuspend.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <mach/gpio.h>

#include <linux/irq.h>
#include <linux/syscalls.h>
#include <linux/reboot.h>
#include <linux/proc_fs.h>


#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/completion.h>
#include <asm/uaccess.h>

#include "goodix_touch.h"
#include "goodix_queue.h"

#define RECORVERY_MODULE

#ifdef RECORVERY_MODULE
#include "touch.h"
touch_t touch = {0, 0, 0};
#endif



#ifndef GUITAR_SMALL
#error The code does not match the hardware version.
#endif
int g_New_PosX[5] = {0};
int g_New_PosY[5] = {0};
uint8_t  point_data[35] = { 0 }, finger = 0;
unsigned int pressure[MAX_FINGER_NUM] = {0};
unsigned int  touch_cnt = 0;

extern  unsigned int FLAG_FOR_15S_OFF;
extern  bool is_ts_load;
extern void SOC_Log_Dump(int cmd);

static struct workqueue_struct *goodix_wq;
int lidbg_i2c_running;


/********************************************
*	管理当前手指状态的伪队列，对当前手指根据时间顺序排�?*	适用于Guitar小屏		*/
static struct point_queue  finger_list;	//record the fingers list
/*************************************************/

const char *s3c_ts_name = "Goodix TouchScreen of Guitar";
struct i2c_client *i2c_connect_client = NULL;
EXPORT_SYMBOL(i2c_connect_client);

#ifdef CONFIG_HAS_EARLYSUSPEND
static void goodix_ts_early_suspend(struct early_suspend *h);
static void goodix_ts_late_resume(struct early_suspend *h);
#endif

/*******************************************************
功能�?
	读取从机数据
	每个读操作用两条i2c_msg组成，第1条消息用于发送从机地址�?	�?条用于发送读取地址和取回数据；每条消息前发送起始信�?参数�?	client:	i2c设备，包含设备地址
	buf[0]�?首字节为读取地址
	buf[1]~buf[len]：数据缓冲区
	len�?读取数据长度
return�?	执行消息�?*********************************************************/
/*Function as i2c_master_send */
static int i2c_read_bytes(struct i2c_client *client, uint8_t *buf, int len)
{
    struct i2c_msg msgs[2];
    int ret = -1;
    //发送写地址
    msgs[0].flags = !I2C_M_RD; //写消�?	msgs[0].addr=client->addr;
    msgs[0].len = 1;
    msgs[0].buf = &buf[0];
    //接收数据
    msgs[1].flags = I2C_M_RD; //读消�?	msgs[1].addr=client->addr;
    msgs[1].len = len - 1;
    msgs[1].buf = &buf[1];

    ret = i2c_transfer(client->adapter, msgs, 2);
    return ret;
}

/*******************************************************
功能�?	向从机写数据
参数�?	client:	i2c设备，包含设备地址
	buf[0]�?首字节为写地址
	buf[1]~buf[len]：数据缓冲区
	len�?数据长度
return�?	执行消息�?*******************************************************/
/*Function as i2c_master_send */
static int i2c_write_bytes(struct i2c_client *client, uint8_t *data, int len)
{
    struct i2c_msg msg;
    int ret = -1;
    //发送设备地址
    msg.flags = !I2C_M_RD; //写消�?	msg.addr=client->addr;
    msg.len = len;
    msg.buf = data;

    ret = i2c_transfer(client->adapter, &msg, 1);
    return ret;
}

/*******************************************************
功能�?	Guitar初始化函数，用于发送配置信息，获取版本信息
参数�?	ts:	client私有数据结构�?return�?	执行结果码，0表示正常执行
*******************************************************/


//#define	ZCC_1444
//#define	ZCC_1466
//#define ZCC_1466_93
#define ZCC_1466_83


static int goodix_init_panel(struct goodix_ts_data *ts)
{
    int ret = -1;

    printk("\n\n\ncome to init ts<-------goodix_init_panel_ftf!\n\n\n");


    //#define GUITAR_CONFIG_43
#ifdef GUITAR_CONFIG_43
    uint8_t config_info[54] = {0x30,
                               0x19, 0x05, 0x06, 0x28, 0x02, 0x14, 0x14, 0x10, 0x28, 0xB0,
                               0x14, 0x00, 0x1E, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,
                               0xCD, 0xE1, 0x00, 0x00, 0x00, 0x00, 0x1D, 0xCF, 0x20, 0x0B,
                               0x0B, 0x8B, 0x50, 0x3C, 0x1E, 0x28, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x01
                              };
#else //TCL_5.0 inch
    /*
    	uint8_t config_info[54]={0x30,	0x19,0x05,0x06,0x28,0x02,0x14,0x14,0x10,0x40,0xB8,0x14,0x00,0x1E,0x00,0x01,0x23,
    									0x45,0x67,0x89,0xAB,0xCD,0xE1,0x00,0x00,0x00,0x00,0x0D,0xCF,0x20,0x03,0x05,0x83,
    									0x50,0x3C,0x1E,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    									0x00,0x00,0x00,0x00,0x01};

    	uint8_t config_info[55]={0x30,	0x19,0x05,0x05,0x28,0x02,0x14,0x14,0x10,0x32,0xF8,0x14,0x00,0x1E,0x00,0xED,0xCB,
    									0xA9,0x87,0x65,0x43,0x21,0x01,0x00,0x00,0x35,0x2E,0x4D,0xC0,0x20,0x01,0x01,0x83,
    									0x50,0x3C,0x1E,0x28,0x00,0x33,0x2C,0x01,0xEC,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,
    									0x00,0x00,0x00,0x00,0x01,0x64};
    //deiyi ~
    uint8_t config_info[55] = {0x30,0x19,0x05,0x05,0x28,0x02,0x14,0x14,0x10,0x32, 0xF8, 0x01, 0xE0, 0x03, 0x20, 0xED, 0xCB,
                               0xA9,0x87,0x65,0x43,0x21,0x01,0x00,0x00,0x35,0x2E, 0x4D, 0xC0, 0x20, 0x01, 0x01, 0x83,
                               0x50,0x3C,0x1E,0x28,0x00,0x33,0x2C,0x01,0xEC,0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00,0x00,0x00,0x00,0x01,0x64
                              };
    //ZCC_1444
    //zhongchu heise~
    uint8_t config_info[55] = {0x30,0x14,0x05,0x03,0x28,0x02,0x14,0x14,0x10,0x3C,0xBA,0x01,0xE0,0x03,0x20,0x01,0x23,
                               0x45,0x67,0x89,0xAB,0xCD,0xE0,0x00,0x00,0x00,0x00,0x4D,0xC1,0x20,0x01,0x03,0x8B,
                               0x00,0x00,0x00,0x28,0x00,0x34,0x2C,0x01,0xEC,0x00,0x46,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00,0x01
                              };
    //ZCC_1466
    uint8_t config_info[55] = {0x30,0x0B,0x05,0x06,0x28,0x02,0x14,0x14,0x10,0x28,0xB2,0x01,0xE0,0x03,0x20,0x01,0x23,0x45,0x67,0x89,0xAB,
    					  	   0xCD,0xE0,0x00,0x00,0x00,0x00,0x4D,0xC1,0x20,0x01,0x01,0x41,0x64,0x3C,0x1E,0x28,0x0E,0x00,0x00,0x00,0x00,
    					       0x50,0x3C,0x32,0x71,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    					      };
    //ZCC_1466_83
    uint8_t config_info[55] = {0x30,0x0F,0x05,0x05,0x28,0x02,0x14,0x14,0x10,0x32,0xB2,0x01,0xE0,0x03,0x20,0x01,0x23,0x45,0x67,0x89,0xAB,
    					  	   0xCD,0xE1,0x00,0x00,0x00,0x00,0x4D,0xCF,0x20,0x01,0x01,0x83,0x64,0x3C,0x1E,0x28,0x0E,0x25,0x2D,0x01,0x9E,
    					       0x50,0x46,0x32,0x71,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    					      };

    //ZCC_1466_93
    uint8_t config_info[55] = {0x30,0x0F,0x05,0x06,0x28,0x02,0x14,0x14,0x10,0x28,0xB2,0x01,0xE0,0x03,0x20,0x01,0x23,0x45,0x67,0x89,0xAB,
    					  	   0xCD,0xE0,0x00,0x00,0x00,0x00,0x4D,0xC1,0x20,0x01,0x01,0x41,0x64,0x3C,0x1E,0x28,0x0E,0x00,0x00,0x00,0x00,
    					       0x50,0x37,0x32,0x71,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    					  	  };

    //ZCC_1466_93_40
    uint8_t config_info[55] = {0x30,0x0F,0x05,0x06,0x28,0x02,0x14,0x14,0x10,0x28,0xB2,0x01,0xE0,0x03,0x20,0x01,0x23,0x45,0x67,0x89,0xAB,
    						   0xCD,0xE0,0x00,0x00,0x00,0x00,0x4D,0xC1,0x20,0x01,0x01,0x41,0x64,0x3C,0x1E,0x28,0x0E,0x00,0x00,0x00,0x00,
    						   0x50,0x28,0x32,0x71,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    					  	  };
    */   //end
    //ZCC_1466_93_30
#if 0
    uint8_t config_info[55] = {0x30, 0x0F, 0x05, 0x06, 0x28, 0x02, 0x14, 0x14, 0x10, 0x28, 0xB2, 0x01, 0xE0, 0x03, 0x20, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,
                               0xCD, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x4D, 0xC1, 0x20, 0x01, 0x01, 0x41, 0x64, 0x3C, 0x1E, 0x28, 0x0E, 0x00, 0x00, 0x00, 0x00,
                               0x50, 0x0a, 0x32, 0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
                              };
#else

    uint8_t config_info[] = {0x30,
					0x13,0x25,0x07,0x28,0x02,0x14,0x14,0x10,0x3C,0xB2,
					0x02,0x58,0x04,0x00,0x01,0x23,0x45,0x67,0x89,0xAB,
					0xCD,0xE1,0x00,0x00,0x32,0x2D,0x4F,0xCF,0x20,0x83,
					0x80,0x80,0x50,0x3C,0x1E,0xB4,0x00,0x30,0x2C,0x01,
					0xEC,0x00,0x50,0x32,0x71,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x01
                            };
#endif
#endif
    //printk("start to send ---------->35\n");
    //lsw
    ret = SOC_I2C_Send(1, 0x55, /*config_info[0],*/ config_info, 54 ); /*How many bytes to write ��ȥdev_addr��reg��ַ��*/
    //i2c_api_do_send(1, 0x55, &config_info[53], &config_info[54],2); /*How many bytes to write ��ȥdev_addr��reg��ַ��*/

    //ret=i2c_write_bytes(ts->client,config_info,54);
    if (ret < 0)
        goto error_i2c_transfer;
    //	msleep(1);
    //printk("start to send -----sucess----->35\n");
    return 0;

error_i2c_transfer:
    printk("===========error_i2c_transfer\n");
    return ret;
}

static int  goodix_read_version(struct goodix_ts_data *ts)
{
    int ret = 0;
    uint8_t version[2] = {0x69, 0xff};	//command of reading Guitar's version
    uint8_t version_data[41];		//store touchscreen version infomation
    memset(version_data, 0 , sizeof(version_data));
    version_data[0] = 0x6A;
    ret = i2c_write_bytes(ts->client, version, 2);
    if (ret < 0)
        goto error_i2c_version;
    //msleep(1);
    ret = i2c_read_bytes(ts->client, version_data, 40);
    if (ret < 0)
        goto error_i2c_version;
    dev_info(&ts->client->dev, " Guitar Version: %s\n", &version_data[1]);
    version[1] = 0x00;				//cancel the command
    i2c_write_bytes(ts->client, version, 2);
    return 0;

error_i2c_version:
    return ret;
}

/*******************************************************
功能�?	触摸屏工作函�?	由中断触发，接受1组坐标数据，校验后再分析输出
参数�?	ts:	client私有数据结构�?return�?	执行结果码，0表示正常执行
********************************************************/


static void goodix_ts_work_func(struct work_struct *work)
{
    struct goodix_ts_data *ts = container_of(work, struct goodix_ts_data, work);
    struct input_dev *dev = ts->input_dev;
    int i = 0;
    //printk("ts_goodix_ts_work_func=[%x]",(u32)ts);
    //printk("lsw come into --->goodix_ts_work_func<----\n");
    //return 0;
    //printk("come into --->read IIC data~!<----\n");
    SOC_I2C_Rec(1, 0x55, 0x00, point_data, 32);

    {
        finger = 0;
        if(point_data[0] == 0x00)
        {
            finger = 0;
            goto up;
        }
        if((point_data[0] && 0x01) == 0x01)
        {
            finger = 1;
        }
        for (i = 0; i < 4; i++)
        {
            point_data[0] = (point_data[0] >> 1);
            if((point_data[0] && 0x01) == 0x01)
            {
                finger++;
            }
        }
    }

    //printk("\nfinger=[%d]\n",finger);
    //return 0;
#ifdef FLY_DEBUG
    if(finger == 3)
    {
        SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);
    }
    if (finger == 4)
    {
        printk("SOC_Log_Dump\n");
        SOC_Log_Dump(LOG_DMESG);
    }
    if(finger == 5)
    {
        SOC_Key_Report(KEY_MENU, KEY_PRESSED_RELEASED);
    }
#endif

    switch(finger)
    {
    case 5:
        g_New_PosX[4] = ((( point_data[28]) << 8 ) + point_data[29]  );
        g_New_PosY[4] = ((( point_data[30]) << 8 ) + point_data[31]  );
        // pressure[4] = (unsigned int) (point_data[32]);//add
        if(g_New_PosY[4] != 0 && g_New_PosX[4] != 0)
        {
            input_report_key(dev, ABS_MT_TRACKING_ID, 4);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, g_New_PosY[4]);
            input_report_abs(dev, ABS_MT_POSITION_Y, g_New_PosX[4]);
            input_mt_sync(dev);
        }
    case 4:
        g_New_PosX[3] = ((( point_data[17]) << 8 ) + point_data[24]  );
        g_New_PosY[3] = ((( point_data[25]) << 8 ) + point_data[26]  );
        // pressure[3] = (unsigned int) (point_data[27]);//add

        if(g_New_PosY[3] != 0 && g_New_PosX[3] != 0)
        {
            input_report_key(dev, ABS_MT_TRACKING_ID, 3);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, g_New_PosY[3]);
            input_report_abs(dev, ABS_MT_POSITION_Y, g_New_PosX[3]);
            input_mt_sync(dev);
        }
    case 3:
        g_New_PosX[2] = ((( point_data[12]) << 8 ) + point_data[13]  );
        g_New_PosY[2] = ((( point_data[14]) << 8 ) + point_data[15]  );
        // pressure[2] = (unsigned int) (point_data[16]);//add

        if(g_New_PosY[2] != 0 && g_New_PosX[2] != 0)
        {
            input_report_key(dev, ABS_MT_TRACKING_ID, 2);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, g_New_PosY[2]);
            input_report_abs(dev, ABS_MT_POSITION_Y, g_New_PosX[2]);
            input_mt_sync(dev);
        }
    case 2:
        g_New_PosX[1] = ((( point_data[7]) << 8 ) + point_data[8]  );
        g_New_PosY[1] = ((( point_data[9]) << 8 ) + point_data[10]	);
        // pressure[1] = (unsigned int) (point_data[11]);//add

        if(g_New_PosY[1] != 0 && g_New_PosX[1] != 0)
        {
            input_report_key(dev, ABS_MT_TRACKING_ID, 1);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, g_New_PosY[1]);
            input_report_abs(dev, ABS_MT_POSITION_Y, g_New_PosX[1]);
            input_mt_sync(dev);
        }
    case 1:
    {
        g_New_PosX[0] = ((( point_data[2]) << 8 ) + point_data[3]  );
        g_New_PosY[0] = ((( point_data[4]) << 8 ) + point_data[5]  );
        if(g_New_PosY[0] != 0 && g_New_PosX[0] != 0)
        {
            input_report_abs(ts->input_dev, ABS_MT_POSITION_X, g_New_PosY[0]);
            input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, g_New_PosX[0]);
            input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 255);
            input_report_abs(ts->input_dev, ABS_MT_TRACKING_ID, 0);
            input_mt_sync(ts->input_dev);
            input_report_key(ts->input_dev, BTN_TOUCH, 1);
            input_sync(ts->input_dev);
            //  input_sync(dev);
            touch_cnt++;
            if (touch_cnt > 50)
            {
                touch_cnt = 0;
                printk("[%d,%d]\n", g_New_PosY[0], g_New_PosX[0]);
            }

        }

        FLAG_FOR_15S_OFF++;
        if(FLAG_FOR_15S_OFF >= 1000)
        {
            FLAG_FOR_15S_OFF = 1000;
        }
        //printk("\nFLAG_FOR_15S_OFF===[%d]\n",FLAG_FOR_15S_OFF);
        if(FLAG_FOR_15S_OFF < 0)
        {
            printk("\nerr:FLAG_FOR_15S_OFF===[%d]\n", FLAG_FOR_15S_OFF);
        }
#ifdef RECORVERY_MODULE
        if( (g_New_PosY[0] >= 0) && (g_New_PosX[0] >= 0) )
        {
            touch.x = g_New_PosY[0];
            touch.y = g_New_PosX[0];
            touch.pressed = 1;
            set_touch_pos(&touch);
        }
#endif
    }
    break;
    case 0:
up:
        //input_report_key(dev, BTN_TOUCH, 0);
        //input_report_key(dev, BTN_2, 0);
        finger = 0;
        input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0);
        input_mt_sync(ts->input_dev);
        input_report_key(ts->input_dev, BTN_TOUCH, 0);
        input_sync(ts->input_dev);
        //input_report_abs(dev, ABS_MT_WIDTH_MAJOR, 0);
        //input_report_abs(dev, ABS_MT_POSITION_X, 0);
        // input_report_abs(dev, ABS_MT_POSITION_Y, 0);
        // input_mt_sync(dev);
        // input_sync(dev);
        // printk("finger up\n");
#ifdef RECORVERY_MODULE
        {
            touch.pressed = 0;
            set_touch_pos(&touch);
        }
#endif
        break;
    default:
    {
        printk("default finger=[%d]\n", finger);
        break;
    }


    }


#if 0
    ret = i2c_read_bytes(ts->client, point_data, 35);
    if(ret <= 0)
    {
        dev_err(&(ts->client->dev), "I2C transfer error. Number:%d\n ", ret);
        ts->bad_data = 1;
        ts->retry++;
        goodix_init_panel(ts);
        goto start_read_i2c;
    }
#endif
    ts->bad_data = 0;
#if 0
    {
        int i = 0;

#if 0
        int state = 0;
        if(point_data[0] != 0)
        {
            state = 1;
        }
        else
        {
            state = 0;
        }
        for(i = 0; i < 5; i++)
        {
            printk("%d[%d,%d]R[%d]->", i + 1, g_New_PosX[i], g_New_PosY[i], pressure[i]);
        }
        printk("\n-----------------c finger=[%d]\n", finger);
#endif

        switch(finger)
        {
        case 5:
        case 4:
            input_report_key(dev, ABS_MT_TRACKING_ID, 3);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, event->x4);
            input_report_abs(dev, ABS_MT_POSITION_Y, event->y4);
            input_mt_sync(dev);
        case 3:
            input_report_key(dev, ABS_MT_TRACKING_ID, 2);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, event->x3);
            input_report_abs(dev, ABS_MT_POSITION_Y, event->y3);
            input_mt_sync(dev);
        case 2:
            input_report_key(dev, ABS_MT_TRACKING_ID, 1);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, event->x2);
            input_report_abs(dev, ABS_MT_POSITION_Y, event->y2);
            input_mt_sync(dev);
        case 1:
            input_report_key(dev, ABS_MT_TRACKING_ID, 0);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 250);
            input_report_abs(dev, ABS_MT_POSITION_X, event->x1);
            input_report_abs(dev, ABS_MT_POSITION_Y, event->y1);
            input_mt_sync(dev);
        default:
        {


        }
        }




        printk("%d.[%d,%d].[%d]->", finger, g_New_PosX[i], g_New_PosY[i], pressure[i]);



        input_sync(dev);

#if 0
        {
            if(state == 1)
            {
                input_report_abs(ts->input_dev, ABS_MT_POSITION_X, g_New_PosX[0]);
                input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, g_New_PosY[0]);
            }
            input_report_abs(ts->input_dev, ABS_MT_PRESSURE, pressure[0]);
            input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, pressure[0]);
            input_mt_sync(ts->input_dev);



            for(count = 1; count < finger; count++)
            {
                //if (finger_list.length > count)
                {
                    //if(finger_list.pointer[count].state == FLAG_DOWN)
                    if(state == 1)
                    {
                        input_report_abs(ts->input_dev, ABS_MT_POSITION_X, g_New_PosX[count]);
                        input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, g_New_PosY[count]);
                    }
                    input_report_abs(ts->input_dev, ABS_MT_PRESSURE, pressure[count]);
                    input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, pressure[count]);
                    input_mt_sync(ts->input_dev);
                }
            }
        }
#endif
    }
#endif

#ifdef GOODIX_DEBUG
    if((finger_bit ^ point_data[1]) != 0)
    {
        for(count = 0; count < finger_list.length; count++)
            printk(KERN_INFO "Index:%d, No:%d, State:%d\n", count, finger_list.pointer[count].num, finger_list.pointer[count].state);
        printk(KERN_INFO "\n");
    }
#endif

    // del_point(&finger_list);
    //finger_bit = point_data[1];
    return 0;
    //XFER_ERROR:
NO_ACTION:
    if(ts->use_irq)
        enable_irq(ts->client->irq);

}

/*******************************************************
功能�?	计时器响应函�?	由计时器触发，调度触摸屏工作函数运行；之后重新计�?参数�?	timer：函数关联的计时�?
return�?	计时器工作模式，HRTIMER_NORESTART表示不需要自动重�?********************************************************/
static enum hrtimer_restart goodix_ts_timer_func(struct hrtimer *timer)
{
    struct goodix_ts_data *ts = container_of(timer, struct goodix_ts_data, timer);

    queue_work(goodix_wq, &ts->work);
    hrtimer_start(&ts->timer, ktime_set(0, 10000000), HRTIMER_MODE_REL);//16->10 by futengfei
    return HRTIMER_NORESTART;
}

/*******************************************************
功能�?	中断响应函数
	由中断触发，调度触摸屏处理函数运�?参数�?	timer：函数关联的计时�?
return�?	计时器工作模式，HRTIMER_NORESTART表示不需要自动重�?********************************************************/
static irqreturn_t goodix_ts_irq_handler(int irq, void *dev_id)
{
    struct goodix_ts_data *ts = dev_id;
    struct input_dev *dev = ts->input_dev;

#define USE_QUEUE_WORK
#ifdef USE_QUEUE_WORK
    {
        //struct goodix_ts_data *ts = dev_id;
        //printk("come into ts2 IRQ----->zong\n");  //by futengfei
        disable_irq_nosync(ts->client->irq);
        queue_work(goodix_wq, &ts->work);
    }
#else
    {


        //printk("come to IRQ_work!\n");
        //struct goodix_ts_data *ts = container_of(work, struct goodix_ts_data, work);
        // struct input_dev *dev = ts->input_dev;
        int i = 0;
        // read data

        //printk("\nftf:lidbg_i2c_running==[%d]\n",lidbg_i2c_running);
#if 1
        //2: chongtu ~
        if (lidbg_i2c_running == 1)
        {
            //printk("goodix_ts_irq_handler:lidbg_i2c_running============================[1]\n");
            return IRQ_HANDLED;
        }
#endif

        SOC_I2C_Rec(1, 0x55, 0x00, point_data, 32);
        //i2c_master_recv(ts->client,point_data,32);
        switch(point_data[0])
        {
        case 0x1f:
            finger = 5;
            break;
        case 0x0f:
            finger = 4;
            break;
        case 0x07:
            finger = 3;
#if 0
            SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);
            goto up_irq;
#endif
            break;
        case 0x03:
            finger = 2;
            break;
        case 0x01:
            finger = 1;
            break;
        default:
        {
            finger = 0;
            goto up_irq;
        }
        }



        //deal data
        switch(finger)
        {
        case 5:
            g_New_PosX[4] = ((( point_data[28]) << 8 ) + point_data[29]  );
            g_New_PosY[4] = ((( point_data[30]) << 8 ) + point_data[31]  );
            //pressure[4] = (unsigned int) (point_data[32]);//add
        case 4:
            g_New_PosX[3] = ((( point_data[17]) << 8 ) + point_data[24]  );
            g_New_PosY[3] = ((( point_data[25]) << 8 ) + point_data[26]  );
            //pressure[3] = (unsigned int) (point_data[27]);//add
        case 3:
            g_New_PosX[2] = ((( point_data[12]) << 8 ) + point_data[13]  );
            g_New_PosY[2] = ((( point_data[14]) << 8 ) + point_data[15]  );
            //pressure[2] = (unsigned int) (point_data[16]);//add
        case 2:
            g_New_PosX[1] = ((( point_data[7]) << 8 ) + point_data[8]  );
            g_New_PosY[1] = ((( point_data[9]) << 8 ) + point_data[10]	);
            //pressure[1] = (unsigned int) (point_data[11]);//add
        case 1:
            g_New_PosX[0] = ((( point_data[2]) << 8 ) + point_data[3]  );
            g_New_PosY[0] = ((( point_data[4]) << 8 ) + point_data[5]  );
            //pressure[0] = (unsigned int) (point_data[6]);//add
        default:
        {
            //	printk("==touch_point default =\n");
            break;
        }
        }

        // to report
        if(finger)
        {
            for( i = 0; i < finger; i++)
            {
                input_report_key(ts->input_dev, ABS_MT_TRACKING_ID, i);
                input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, 1);
                input_report_abs(ts->input_dev, ABS_MT_POSITION_X, g_New_PosY[i]);
                input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, g_New_PosX[i]);
                input_mt_sync(ts->input_dev);
                //printk("%d.[%d,%d].[%d]->", finger, g_New_PosY[i] , g_New_PosX[i], pressure[i]);
            }
            input_sync(ts->input_dev);
            //printk("\n");
        }
        else
        {

up_irq:
            input_report_key(dev, BTN_TOUCH, 0);
            input_report_key(dev, BTN_2, 0);
            input_report_abs(dev, ABS_MT_TOUCH_MAJOR, 0);
            input_report_abs(dev, ABS_MT_WIDTH_MAJOR, 0);
            input_report_abs(dev, ABS_MT_POSITION_X, 0);
            input_report_abs(dev, ABS_MT_POSITION_Y, 0);

            input_mt_sync(dev);
            input_sync(dev);
            // printk("finger up\n");
        }



    }//end
#endif
    return IRQ_HANDLED;
}

/*******************************************************
功能�?	触摸屏探测函�?	在注册驱动时调用（要求存在对应的client）；
	用于IO,中断等资源申请；设备注册；触摸屏初始化等工作
参数�?	client：待驱动的设备结构体
	id：设备ID
return�?	执行结果码，0表示正常执行
********************************************************/
static int screen_x=0;
static int screen_y=0;
static int goodix_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
//static int goodix_ts_probe()//lsw
{
    struct goodix_ts_data *ts;
    int ret = 0, retry = 0, count = 0;
    struct goodix_i2c_rmi_platform_data *pdata;

    printk(  " come into goodix_ts_probe---futengfei\n");

    //Check I2C function

#if 0
    ret = gpio_request(SHUTDOWN_PORT, "TS_SHUTDOWN");	//Request IO
    if (ret < 0)
    {
        printk(KERN_ALERT "Failed to request GPIO:%d, ERRNO:%d\n", (int)SHUTDOWN_PORT, ret);
        goto err_check_functionality_failed;
    }
    gpio_direction_output(SHUTDOWN_PORT, 0);	//Touchscreen is waiting to wakeup
    ret = gpio_get_value(SHUTDOWN_PORT);
    if (ret)
    {
        printk(KERN_ALERT  "Cannot set touchscreen to work.\n");
        goto err_check_functionality_failed;
    }

#endif
#if 0
    //SOC_IO_Output(SHUTDOWN_PORT_GROUP, SHUTDOWN_PORT_INDEX, 0); //temprory
    ret = SOC_IO_Input(SHUTDOWN_PORT_GROUP, SHUTDOWN_PORT_INDEX, GPIO_PULLDOWN); //temprory
    if (ret)
    {
        printk(KERN_ALERT  "SHUTDOWN PIN IS hight!Cannot set touchscreen to work.\n");
        goto err_check_functionality_failed;
    }


#endif

    //waite guitar start
    if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
    {
        printk( "System need I2C function.\n");
        ret = -ENODEV;
        goto err_check_functionality_failed;
    }

    i2c_connect_client = client;	//used by Guitar Updating.

    //gpio_set_value(SHUTDOWN_PORT, 1);		//suspend
    //SOC_IO_Output(SHUTDOWN_PORT_GROUP,SHUTDOWN_PORT_INDEX,1);//temprory

    ts = kzalloc(sizeof(*ts), GFP_KERNEL);
    //printk("ts_kzalloc=[%x]",(u32)ts);
    if (ts == NULL)
    {
        printk( "err_alloc_data_failed.\n");
        ret = -ENOMEM;
        goto err_alloc_data_failed;
    }

    INIT_WORK(&ts->work, goodix_ts_work_func);
    ts->client = client;
    i2c_set_clientdata(client, ts);
    ts->client->addr = 0x55;
    pdata = client->dev.platform_data;

    ts->input_dev = input_allocate_device();
    if (ts->input_dev == NULL)
    {
        ret = -ENOMEM;
        printk( "err_input_dev_alloc_failed.\n");
        goto err_input_dev_alloc_failed;
    }

    ts->input_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS) ;
    ts->input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
    ts->input_dev->absbit[0] = BIT(ABS_X) | BIT(ABS_Y) | BIT(ABS_PRESSURE);
    //#ifdef GOODIX_MULTI_TOUCH	//used by android 1.x for multi-touch, not realized
    //ts->input_dev->absbit[0]=BIT(ABS_HAT0X) |BIT(ABS_HAT0Y);
    //ts->input_dev->keybit[BIT_WORD(BTN_2)] = BIT_MASK(BTN_2);
    //#endif

    input_set_abs_params(ts->input_dev, ABS_X, 0, 1024, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_Y, 0, 600 , 0, 0);
    input_set_abs_params(ts->input_dev, ABS_PRESSURE, 0, 38, 0, 0);

#ifdef GOODIX_MULTI_TOUCH

    //  set_bit(BTN_2, ts->input_dev->keybit);
#define 	RESOLUTION_X	(1024)  //this
#define 	RESOLUTION_Y	(600)
#define GOODIX_TOUCH_WEIGHT_MAX 		(150)
screen_x=RESOLUTION_X;
screen_y=RESOLUTION_Y;
SOC_Display_Get_Res(&screen_x, &screen_y);
#endif
    input_set_abs_params(ts->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_X, 0, screen_x  , 0, 0); //ts->abs_y_max
    input_set_abs_params(ts->input_dev, ABS_MT_POSITION_Y, 0, screen_y, 0, 0);	//ts->abs_x_max

    //	sprintf(ts->phys, "input/ts)");
    printk("check your screen [%d*%d]=================futengfei===\n", screen_x, screen_y);

    sprintf(ts->phys, "input/ts");
    ts->input_dev->name = s3c_ts_name;
    ts->input_dev->phys = ts->phys;
    ts->input_dev->id.bustype = BUS_I2C;
    ts->input_dev->id.vendor = 0xDEAD;
    ts->input_dev->id.product = 0xBEEF;
    ts->input_dev->id.version = 10427;	//screen firmware version

    finger_list.length = 0;
    ret = input_register_device(ts->input_dev);
    if (ret)
    {
        printk( "err_input_register_device_failed.\n");
        goto err_input_register_device_failed;
    }

    ts->use_irq = 0;
    ts->retry = 0;
    ts->bad_data = 0;
    //client->irq=TS_INT;
    client->irq = GPIOEIT;

#if 0

    if (client->irq)
    {
        ret = gpio_request(INT_PORT, "TS_INT");	//Request IO
        if (ret < 0)
        {
            dev_err(&client->dev, "Failed to request GPIO:%d, ERRNO:%d\n", (int)INT_PORT, ret);
            goto err_gpio_request_failed;
        }
        ret = s3c_gpio_cfgpin(INT_PORT, INT_CFG);	//Set IO port function
        ret  = request_irq(TS_INT, goodix_ts_irq_handler ,  IRQ_TYPE_EDGE_RISING,
                           client->name, ts);
        if (ret != 0)
        {
            dev_err(&client->dev, "Can't allocate touchscreen's interrupt!ERRNO:%d\n", ret);
            gpio_free( INT_PORT);
            goto err_gpio_request_failed;
        }
        else
        {
            disable_irq(TS_INT);
            ts->use_irq = 1;
            dev_dbg(&client->dev, "Reques EIRQ %d succesd on GPIO:%d\n", TS_INT, INT_PORT);
        }
    }

#else

#endif

    //err_gpio_request_failed:
    // if (!ts->use_irq)
#if 0
    {
        hrtimer_init(&ts->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        ts->timer.function = goodix_ts_timer_func;
        hrtimer_start(&ts->timer, ktime_set(1, 0), HRTIMER_MODE_REL);
    }
#endif
    //-------------------------------------------------------------------------------
    //gpio_set_value(SHUTDOWN_PORT, 0);
    // SOC_IO_Output(SHUTDOWN_PORT_GROUP, SHUTDOWN_PORT_INDEX, 0); //temprory

    msleep(10);
    for(count = 0; count < 3; count++)
    {
        //printk("come into ----------->35\n");
        ret = goodix_init_panel(ts);
        if(ret != 0)		//Initiall failed
            continue;
        else
        {
            if(ts->use_irq)
                //enable_irq(TS_INT);
                SOC_IO_ISR_Enable(GPIOEIT);
            break;
        }
    }
    if(ret != 0)
    {
        ts->bad_data = 1;
        goto err_init_godix_ts;
    }
    goodix_read_version(ts);
    msleep(30);
    SOC_IO_Input(0, GPIOEIT, GPIO_CFG_PULL_UP);

    ret = SOC_IO_ISR_Add(GPIOEIT, IRQF_TRIGGER_FALLING, goodix_ts_irq_handler, ts);
    if(ret == 0)
    {
        printk("[futengfei]gt811:------->SOC_IO_ISR_Add err\n");
    }
    ts->use_irq = 1;
    ts->client->irq = GPIOEIT;
#ifdef CONFIG_HAS_EARLYSUSPEND
    ts->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    ts->early_suspend.suspend = goodix_ts_early_suspend;
    ts->early_suspend.resume = goodix_ts_late_resume;
    register_early_suspend(&ts->early_suspend);
#endif

    dev_dbg(&client->dev, "Start  %s in %s mode\n", ts->input_dev->name, ts->use_irq ? "Interrupt" : "Polling\n");
    printk(  " out goodix_ts_probe---futengfei\n\n");
    return 0;

err_init_godix_ts:
    if(ts->use_irq)
        SOC_IO_ISR_Del(GPIOEIT);
    //free_irq(TS_INT,ts);
    //gpio_request(INT_PORT,"TS_INT");
    //gpio_free(INT_PORT);

err_input_register_device_failed:
    input_free_device(ts->input_dev);

err_input_dev_alloc_failed:
    i2c_set_clientdata(client, NULL);
    kfree(ts);
err_i2c_failed:
    //gpio_direction_input(SHUTDOWN_PORT);
    //gpio_free(SHUTDOWN_PORT);
err_alloc_data_failed:
err_check_functionality_failed:
    return ret;
}


/*******************************************************
功能�?	驱动资源释放
参数�?	client：设备结构体
return�?	执行结果码，0表示正常执行
********************************************************/
static int goodix_ts_remove(struct i2c_client *client)
{

#if 0
    struct goodix_ts_data *ts = i2c_get_clientdata(client);
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&ts->early_suspend);
#endif
    if (ts->use_irq)
        free_irq(client->irq, ts);
    else
        hrtimer_cancel(&ts->timer);

    gpio_direction_input(SHUTDOWN_PORT);
    gpio_free(SHUTDOWN_PORT);

    if (ts->use_irq)
        gpio_free(INT_PORT);

    dev_notice(&client->dev, "The driver is removing...\n");
    i2c_set_clientdata(client, NULL);
    input_unregister_device(ts->input_dev);
    if(ts->input_dev)
        kfree(ts->input_dev);
    kfree(ts);

#endif
    return 0;
}

//停用设备
static int goodix_ts_suspend(struct i2c_client *client, pm_message_t mesg)
{
    int ret = 0;
    struct goodix_ts_data *ts = i2c_get_clientdata(client);
    printk(" [%s]========futengfei=======\n\n\n", __func__);

    return 0;
}
//重新唤醒
static int goodix_ts_resume(struct i2c_client *client)
{
    int ret = 0, retry = 0, init_err = 0;
    uint8_t GT811_check[6] = {0x55};
    struct goodix_ts_data *ts = i2c_get_clientdata(client);
    printk("come into [%s]========futengfei====== [futengfei]=\n", __func__);
    printk(KERN_INFO "Build Time: %s %s  %s \n", __FUNCTION__, __DATE__, __TIME__);

    for(retry = 0; retry < 10; retry++)
    {
        goodix_init_panel(ts);
        init_err = SOC_I2C_Rec(1, 0x55, 0x68, GT811_check, 6 );
        ret = 0;
        //if( GT811_check[0] == 0xff&&GT811_check[1] == 0xff&&GT811_check[2] == 0xff&&GT811_check[3] == 0xff&&GT811_check[4] == 0xff&&GT811_check[5] == 0xff)
        if(init_err < 0)
        {
            printk("[futengfei]goodix_init_panel:goodix_init_panel failed====retry=[%d] init_err=%d\n", retry, init_err);
            ret = 1;
        }
        else
        {
            printk("[futengfei]goodix_init_panel:goodix_init_panel success====retry=[%d] init_err=%d\n\n\n", retry, init_err);
            ret = 0;
        }

        msleep(8);
        if(ret != 0)	//Initiall failed
        {
            printk("[futengfei]goodix_init_panel:goodix_init_panel failed=========retry=[%d]===ret[%d]\n", retry, ret);
            SOC_IO_Output(0, 26, 0);
            msleep(300);
            SOC_IO_Output(0, 26, 1);
            msleep(700);
            continue;
        }

        else
            break;

        printk("[futengfei] goodix_ts_resume:if this is appear ,that is say the continue no goto for directly!\n");

    }

    if(ret != 0)
    {
        printk("goodix_init_panel:Initiall failed============");
        ts->bad_data = 1;
        //goto err_init_godix_ts;
    }

    /*
    	if (ts->power) {
    		ret = ts->power(ts, 1);
    		if (ret < 0)
    			printk("goodix_ts_resume power on failed\n");
    	}

    	if (ts->use_irq)
    		//enable_irq(client->irq);
    #ifndef STOP_IRQ_TYPE
    		gt811_irq_enable(ts);     //KT ADD 1202
    #elif
    		enable_irq(client->irq);
    #endif
    	else
    		hrtimer_start(&ts->timer, ktime_set(1, 0), HRTIMER_MODE_REL);
    */
    return 0;
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void goodix_ts_early_suspend(struct early_suspend *h)
{
    struct goodix_ts_data *ts;
    printk("\n\n\n[futengfei]come into================ [%s]\n", __func__);
    ts = container_of(h, struct goodix_ts_data, early_suspend);
    goodix_ts_suspend(ts->client, PMSG_SUSPEND);
}

static void goodix_ts_late_resume(struct early_suspend *h)
{
    struct goodix_ts_data *ts;
    printk("\n\n\n[futengfei]come into================ [%s]\n", __func__);
    ts = container_of(h, struct goodix_ts_data, early_suspend);
    goodix_ts_resume(ts->client);
}
#endif

//可用于该驱动�?设备名—设备ID 列表
//only one client
static const struct i2c_device_id goodix_ts_id[] =
{
    { GOODIX_I2C_NAME, 0 },
    { }
};

static struct i2c_driver goodix_ts_driver =
{
    .probe		= goodix_ts_probe,
    .remove		= goodix_ts_remove,
#ifndef CONFIG_HAS_EARLYSUSPEND
    .suspend	= goodix_ts_suspend,
    .resume		= goodix_ts_resume,
#endif
    .id_table	= goodix_ts_id,
    .driver =
    {
        .name	= GOODIX_I2C_NAME,
        .owner = THIS_MODULE,
    },
};

/*******************************************************
功能�?	驱动加载函数
return�?	执行结果码，0表示正常执行
********************************************************/
static int __devinit goodix_ts_init(void)
{
    int ret = 0;
    unsigned int flag_irq = 0;
    uint8_t device_check[2] = {0x55};


#ifndef SOC_COMPILE
    LIDBG_GET;
#endif
    is_ts_load = 1;
    printk("\n\n==in=GT801.KO===============touch INFO=======================peizhi=futengfei\n");


    //SOC_IO_Output(SHUTDOWN_PORT_GROUP, SHUTDOWN_PORT_INDEX, 1); //temprory by futengfei
    goodix_wq = create_workqueue("goodix_wq");
    if (!goodix_wq)
    {
        printk(KERN_ALERT "creat workqueue faiked\n");
        return -ENOMEM;

    }
    SOC_IO_Input(0, GPIOEIT, GPIO_CFG_PULL_UP);
    ret = i2c_add_driver(&goodix_ts_driver);
    //  SOC_IO_Output(SHUTDOWN_PORT_GROUP, SHUTDOWN_PORT_INDEX, 0); //temprory
    return ret;
}

/*******************************************************
功能�?	驱动卸载函数
参数�?	client：设备结构体
********************************************************/
static void __exit goodix_ts_exit(void)
{
    printk(KERN_DEBUG "Touchscreen driver of guitar is exiting...\n");
    i2c_del_driver(&goodix_ts_driver);
    if (goodix_wq)
        destroy_workqueue(goodix_wq);
}

late_initcall(goodix_ts_init);
module_exit(goodix_ts_exit);

MODULE_DESCRIPTION("Goodix Touchscreen Driver");
MODULE_LICENSE("GPL");



