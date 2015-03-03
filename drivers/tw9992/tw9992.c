/*
 * Copyright (C) 2011-2014 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#include <linux/fsl_devices.h>

#include <linux/sched.h> 
#include <linux/kthread.h>
#include <linux/workqueue.h>

#include "lidbg.h"
LIDBG_DEFINE;
struct tw9992_chipset {
    struct 		device 		*dev;
    struct 		i2c_client 	*client; 
};

#define TW9_PDN_PORT 4
struct reg_value {
    u8 reg;
    u8 value;
};
static struct reg_value tw9992_init_default[] = {
	{0x02,0x40},

	{0x04,0x00},{0x05,0x09},{0x06,0x00},{0x07,0x02},{0x08,0x12},
	{0x09,0xF0},{0x0A,0x09},{0x0B,0xD1},{0x0C,0xCC},{0x0D,0x00},
	{0x10,0x00},{0x11,0x64},{0x12,0x11},{0x13,0x80},{0x14,0x80},
	{0x15,0x00},{0x17,0x80},{0x18,0x44},{0x19,0x06},{0x1A,0x00},
	{0x1C,0x0F},{0x1D,0x7F},{0x1F,0x00},{0x20,0x50},{0x21,0x22},{0x22,0xF0},
	{0x23,0xD8},{0x24,0xBC},{0x25,0xB8},{0x26,0x44},{0x27,0x38},
	{0x28,0x00},{0x29,0x00},{0x2A,0x78},{0x2B,0x44},{0x2C,0x30},
	{0x2D,0x14},{0x2E,0xA5},{0x2F,0xE0},
	{0x33,0x05},{0x34,0x1A},{0x35,0x00},{0x36,0x5A},{0x37,0x18},{0x38,0xDD},
	{0x39,0x00},{0x3A,0x30},{0x3B,0x00},{0x3C,0x00},{0x3D,0x00},
	{0x3F,0x1A},{0x40,0x80},{0x41,0x00},{0x42,0x00},
	{0x48,0x02},{0x49,0x00},{0x4A,0x81},{0x4B,0x0A},{0x4C,0x00},{0x4D,0x01},
	{0x4E,0x01},
	{0x50,0x00},{0x51,0x00},{0x52,0x00},
	{0x56,0x00},{0x57,0x00},{0x58,0x00},
	{0x60,0x00},{0x61,0x00},{0x62,0x00},
	{0x70,0x01},{0x71,0xA5},{0x72,0xA0},{0x73,0x00},{0x74,0xF0},{0x75,0x00},
	{0x76,0x17},{0x77,0x05},{0x78,0x88},{0x79,0x06},{0x7A,0x28},
	{0x7B,0x46},{0x7C,0xB3},{0x7D,0x06},{0x7E,0x13},{0x7F,0x11},
	{0x80,0x05},{0x81,0xA0},{0x82,0x13},{0x83,0x11},{0x84,0x02},
	{0x85,0x0E},{0x86,0x08},{0x87,0x37},{0x88,0x00},{0x89,0x00},
	{0x8A,0x02},{0x8B,0x33},{0x8C,0x22},{0x8D,0x03},{0x8E,0x22},
	{0x8F,0x01},
	{0x90,0x00},{0x91,0x0C},{0x92,0x00},{0x93,0x0E},{0x94,0x07},{0x95,0xFF},
	{0x96,0x1A},
	{0x9B,0x02},
	{0xA0,0x00},{0xA1,0x00},{0xA2,0x30},{0xA3,0xC0},{0xA4,0x00},
	{0xC0,0x06},{0xC1,0x20},
};

struct reg_value tw9992_Decoder_NTSC[] = {
    //vdelay=0x015=21	vactive=0x0f0=240
    //hdelay=0x014=20	hactive=0x2d0=720
    {0x07,0x02},{0x08,0x15},{0x09,0xf0},{0x0a,0x14},{0x0b,0xd0}
};

struct reg_value tw9992_Decoder_PAL[] = {
	//vdelay=0x017=23	vactive=0x120=288
	//hdelay=0x00f=15	hactive=0x2d0=720
	{0x07,0x12},{0x08,0x17},{0x09,0x20},{0x0a,0x0f},{0x0b,0xd0}

};	


/*!
 * Maintains the information on the current state of the sesor.
 */
static struct tw9992_chipset *tw9992;

static int tw9992_probe(struct i2c_client *adapter, const struct i2c_device_id *device_id);
static int tw9992_remove(struct i2c_client *client);
static struct of_device_id intersil_match_table[] = {
	{ .compatible = "intersil,tw9992", },
	{ },
};

static const struct i2c_device_id tw9992_i2c_id[] = {
    {"tw9992", 0},
    {},
};

MODULE_DEVICE_TABLE(i2c, tw9992_i2c_id);

static struct i2c_driver tw9992_i2c_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name  = "tw9992",
        .of_match_table = intersil_match_table,
    },
    .probe  = tw9992_probe,
    .remove = tw9992_remove,
    .id_table = tw9992_i2c_id,
};


static inline int tw9992_i2c_read_reg(struct i2c_client *client, u8 reg)
{
    int ret;
	char val;
	
	ret = i2c_master_send(client, &reg, 1);
	if (ret < 0) {
		dev_dbg(&client->dev, "read reg error: ret = %d\n", ret);
		return -1;
	}
	ret = i2c_master_recv(client, &val, 1);
	if (ret < 0) {
		dev_dbg(&client->dev, "read reg error: ret = %d\n", ret);
		return -1;
	}

    return val;
}

static inline int tw9992_i2c_write_reg(struct i2c_client *client, u8 reg, u8 val)
{
    int ret;

	u8 reg_val[2];
	reg_val[0] = reg;
	reg_val[1] = val;
	ret = i2c_master_send(client,reg_val,2);

    return ret;
}

static inline int tw9992_i2c_write_block(struct i2c_client *client, struct reg_value *config, int size)
{
    int i;

	for(i=0; i < size; i++) { 
		tw9992_i2c_write_reg(client, config[i].reg, config[i].value);
	}

    return 0;
}

static inline int tw9992_i2c_read_block(struct i2c_client *client, struct reg_value *config, int size)
{
    int i;
   u8 value;
	for(i=0; i < size; i++) { 
		value=tw9992_i2c_read_reg(client, config[i].reg);
		lidbg(" config:0x%02X vualue :0x%02X\n", config[i].reg,value);
	}

    return 0;
}

static int tw9992_hw_probe(void)
{
    lidbg("========tw9992_hw_config=======\n");
	tw9992_i2c_write_block(tw9992->client, tw9992_init_default,
                                 ARRAY_SIZE(tw9992_init_default));

	tw9992_i2c_write_reg(tw9992->client, 0x02, 0x41);
	
	tw9992_i2c_write_block(tw9992->client, tw9992_Decoder_NTSC,
									 ARRAY_SIZE(tw9992_Decoder_NTSC));
    return 0;
}

static int tw9992_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    int ret = 0;
    u8 chip_id;
    u8 status;
    lidbg("========tw9992_probe=======\n");
    tw9992 = kzalloc(sizeof(struct tw9992_chipset), GFP_KERNEL);
    if (!tw9992)
        return -ENOMEM;
            client->addr=0x3d;
    tw9992->client = client;
	
  SOC_IO_Output(0, 4, 0);
  	msleep(1000);
    SOC_IO_Output(0, 64, 1);	
       SOC_IO_Output(0,4, 1);	
    	msleep(1000);
    if(tw9992_hw_probe() < 0)
       lidbg("tw9992_chip_init error...\r\n");
    else
       lidbg("tw9992_chip_init ok...\r\n");

	//-------------------------------------------------------------------------------------
	/* Chip ID Check */
    chip_id = tw9992_i2c_read_reg(client, 0x00);
    if (chip_id != 0x92) {
       lidbg("TVIN tw9992_mipi not found, chip_id read(0x00) = 0x%02x.\n", chip_id);
        goto err;
    }
   lidbg("TVIN tw9992_mipi(0x%02X) is found\n", chip_id);
   	msleep(1000);
	status = tw9992_i2c_read_reg(client, 0x03);
	 lidbg("TVIN tw9992_mipi_status(0x%02X) is found\n", status);
	 tw9992_i2c_read_block(client, tw9992_init_default,
                                 ARRAY_SIZE(tw9992_init_default));
    return ret;
	
err:
    
    kfree(tw9992);

    return ret;
}

/*!
 * tw9992 I2C detach function
 *
 * @param client            struct i2c_client *
 * @return  Error code indicating success or failure
 */
static int tw9992_remove(struct i2c_client *client)
{
    return 0;
}


/*!
 * tw9992 init function
 * Called by insmod tw9992_camera.ko.
 *
 * @return  Error code indicating success or failure
 */
static __init int tw9992_init(void)
{
    u8 err;
     lidbg("========tw9992_init=======\n");
     LIDBG_GET;
    err = i2c_add_driver(&tw9992_i2c_driver);
    if (err != 0)
        pr_err("%s:driver registration failed, error=%d\n",
               __func__, err);

    return err;
}

/*!
 * tw9992 cleanup function
 * Called on rmmod tw9992_camera.ko
 *
 * @return  Error code indicating success or failure
 */
static void __exit tw9992_clean(void)
{
    i2c_del_driver(&tw9992_i2c_driver);
}


module_init(tw9992_init);
module_exit(tw9992_clean);

MODULE_DESCRIPTION("GTP Series Driver");
MODULE_LICENSE("GPL");

