/**
 *
 * Synaptics Register Mapped Interface (RMI4) I2C Physical Layer Driver.
 * Copyright (c) 2007-2011, Synaptics Incorporated
 *
 */
/*
 * This file is licensed under the GPL2 license.
 *
 *#############################################################################
 * GPL
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 *#############################################################################
 */

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/i2c.h>


#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "rmi_i2c.h"
#include "rmi_drvr.h"
#include "rmi_platformdata.h"
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio_event.h>
#include <linux/leds.h>
#include <linux/i2c/atmel_mxt_ts.h>
#include <linux/i2c.h>
#include <linux/input/rmi_platformdata.h>
#include <linux/input/rmi_i2c.h>
#include <linux/delay.h>
#include <linux/atmel_maxtouch.h>
#include <linux/input/ft5x06_ts.h>
#include <asm/gpio.h>
#include <asm/mach-types.h>
#include <mach/rpc_server_handset.h>
#include <mach/pmic.h>


#define DRIVER_NAME "rmi4_ts"
#define rmiresetpin 26

#define DEVICE_NAME "rmi4_ts"

/* Used to lock access to the page address.*/
/* TODO: for multiple device support will need a per-device mutex */
static DEFINE_MUTEX(page_mutex);
#define FT5X06_REG_FW_VER	0xA6


static const struct i2c_device_id rmi_i2c_id_table[] =
{
    { DEVICE_NAME, 0 },
    { },
};
MODULE_DEVICE_TABLE(i2c, rmi_i2c_id_table);


/* Used to count the number of I2C modules we get.
 */
static int device_count;


/*
 * This is the data kept on a per instance (client) basis.  This data is
 * always accessible by using the container_of() macro of the various elements
 * inside.
 */
struct instance_data
{
    int instance_no;
    int irq;
    struct rmi_phys_driver rmiphysdrvr;
    struct i2c_client *i2cclient; /* pointer to i2c_client for later use in
					read, write, read_multiple, etc. */
    int page;
};

/*
 * RMI devices have 16-bit addressing, but some of the physical
 * implementations (like SMBus) only have 8-bit addressing.  So RMI implements
 * a page address at 0xff of every page so we can reliable page addresses
 * every 256 registers.  This function sets the page.
 *
 * The page_mutex lock must be held when this function is entered.
 *
 * param[in] id - The pointer to the instance_data struct
 * param[in] page - The new page address.
 * returns zero on success, non-zero on failure.
 */
/** Writing to page select is giving errors in some configurations.  It's
 * not needed for basic operation, so we've turned it off for the moment.
 */
#if	defined(USE_PAGESELECT)
int
rmi_set_page(struct instance_data *instancedata, unsigned int page)
{
    char txbuf[2];
    int retval;
    txbuf[0] = 0xff;
    txbuf[1] = page;
    retval = i2c_master_send(instancedata->i2cclient, txbuf, 2);
    if (retval != 2)
    {
        dev_err(&instancedata->i2cclient->dev,
                "%s: Set page failed: %d.", __func__, retval);
    }
    else
    {
        retval = 0;
        instancedata->page = page;
    }
    return retval;
}
#else
int
rmi_set_page(struct instance_data *instancedata, unsigned int page)
{
    return 0;
}
#endif

/*
 * Read a single register through i2c.
 *
 * param[in] pd - The pointer to the rmi_phys_driver struct
 * param[in] address - The address at which to start the data read.
 * param[out] valp - Pointer to the buffer where the data will be stored.
 * returns zero upon success (with the byte read in valp), non-zero upon error.
 */
static int
rmi_i2c_read(struct rmi_phys_driver *physdrvr, unsigned short address, char *valp)
{
    struct instance_data *instancedata =
        container_of(physdrvr, struct instance_data, rmiphysdrvr);

    char txbuf[2];
    int retval = 0;
    int retry_count = 0;

    /* Can't have anyone else changing the page behind our backs */
    mutex_lock(&page_mutex);

    if (((address >> 8) & 0xff) != instancedata->page)
    {
        /* Switch pages */
        retval = rmi_set_page(instancedata, ((address >> 8) & 0xff));
        if (retval)
            goto exit;
    }

retry:
    txbuf[0] = address & 0xff;
    retval = i2c_master_send(instancedata->i2cclient, txbuf, 1);

    if (retval != 1)
    {
        dev_err(&instancedata->i2cclient->dev, "%s: Write fail: %d\n",
                __func__, retval);
        goto exit;
    }
    retval = i2c_master_recv(instancedata->i2cclient, txbuf, 1);

    if (retval != 1)
    {
        if (++retry_count == 5)
        {
            dev_err(&instancedata->i2cclient->dev,
                    "%s: Read of 0x%04x fail: %d\n",
                    __func__, address, retval);
        }
        else
        {
            mdelay(10);
            rmi_set_page(instancedata, ((address >> 8) & 0xff));
            goto retry;
        }
    }
    else
    {
        retval = 0;
        *valp = txbuf[0];
    }
exit:

    mutex_unlock(&page_mutex);
    return retval;
}

/*
 * Same as rmi_i2c_read, except that multiple bytes are allowed to be read.
 *
 * param[in] pd - The pointer to the rmi_phys_driver struct
 * param[in] address - The address at which to start the data read.
 * param[out] valp - Pointer to the buffer where the data will be stored.  This
 *     buffer must be at least size bytes long.
 * param[in] size - The number of bytes to be read.
 * returns zero upon success (with the byte read in valp), non-zero upon error.
 *
 */
static int rmi_i2c_read_multiple(struct rmi_phys_driver *physdrvr, unsigned short address, char *valp, int size)
{
    struct instance_data *instancedata = container_of(physdrvr, struct instance_data, rmiphysdrvr);

    char txbuf[2];
    int retval = 0;
    int retry_count = 0;

    //printk("[futengfei] rmi_i2c_read_multiple1 ---------------------------->[%s]\n", __func__);

    /* Can't have anyone else changing the page behind our backs */
    mutex_lock(&page_mutex);

    if (((address >> 8) & 0xff) != instancedata->page)
    {
        /* Switch pages */
        retval = rmi_set_page(instancedata, ((address >> 8) & 0xff));
        if (retval)
        {
            printk("[futengfei] rmi_i2c_read_multiple12 -----retval=[%d]--goto exit;---->[%s]\n", retval, __func__);
            goto exit;
        }
    }

retry:
    txbuf[0] = address & 0xff;
    retval = i2c_master_send(instancedata->i2cclient, txbuf, 1);
    //printk("[futengfei] rmi_i2c_read_multiple13 ---i2c_master_send--retval=[%d]1--i2caddr=[0x%02x]-->[%s]\n",retval,instancedata->i2cclient->addr, __func__);
    if (retval != 1)
    {

        printk("[futengfei] rmi_i2c_read_multiple14 -----retval=[%d]--goto exit2;---->[%s]\n", retval, __func__);
        printk("[futengfei] %s: Write_fail: %d\n", __func__, retval);
        goto exit;
    }

    retval = i2c_master_recv(instancedata->i2cclient, valp, size);
    //printk("[futengfei] rmi_i2c_read_multiple15 -i2c_master_recv-->retval=[%d]6-size=[%d]6.i2caddr=[0x%02x]->[%s]\n",retval,size,instancedata->i2cclient->addr, __func__);
    if (retval != size)
    {
        if (++retry_count == 5)
        {
            dev_err(&instancedata->i2cclient->dev, "[futengfei]%s: Read of 0x%04x size %d fail: %d\n", __func__, address, size, retval);
        }
        else
        {
            mdelay(70);
            rmi_set_page(instancedata, ((address >> 8) & 0xff));

            printk("[futengfei] rmi_i2c_read_multiple1 ---------retry-[%d]-------------->[%s]\n", retry_count, __func__);
            goto retry;
        }
    }
    else
    {
        retval = 0;
    }
exit:

    mutex_unlock(&page_mutex);
    return retval;
}


/*
 * Write a single register through i2c.
 * You can write multiple registers at once, but I made the functions for that
 * seperate for performance reasons.  Writing multiple requires allocation and
 * freeing.
 *
 * param[in] pd - The pointer to the rmi_phys_driver struct
 * param[in] address - The address at which to start the write.
 * param[in] data - The data to be written.
 * returns one upon success, something else upon error.
 */
static int
rmi_i2c_write(struct rmi_phys_driver *physdrvr, unsigned short address, char data)
{
    struct instance_data *instancedata =
        container_of(physdrvr, struct instance_data, rmiphysdrvr);

    unsigned char txbuf[2];
    int retval = 0;

    /* Can't have anyone else changing the page behind our backs */
    mutex_lock(&page_mutex);

    if (((address >> 8) & 0xff) != instancedata->page)
    {
        /* Switch pages */
        retval = rmi_set_page(instancedata, ((address >> 8) & 0xff));
        if (retval)
            goto exit;
    }

    txbuf[0] = address & 0xff;
    txbuf[1] = data;
    retval = i2c_master_send(instancedata->i2cclient, txbuf, 2);

    /* TODO: Add in retry on writes only in certian error return values */
    if (retval != 2)
    {
        dev_err(&instancedata->i2cclient->dev, "%s: Write fail: %d\n",
                __func__, retval);
        goto exit; /* Leave this in case we add code below */
    }
    else
    {
        retval = 1;
    }
exit:

    mutex_unlock(&page_mutex);
    return retval;
}

/*
 * Write multiple registers.
 *
 * For fast writes of 16 bytes of less we will re-use a buffer on the stack.
 * For larger writes (like for RMI reflashing) we will need to allocate a
 * temp buffer.
 *
 * param[in] pd - The pointer to the rmi_phys_driver struct
 * param[in] address - The address at which to start the write.
 * param[in] valp - A pointer to a buffer containing the data to be written.
 * param[in] size - The number of bytes to write.
 * returns one upon success, something else upon error.
 */
static int
rmi_i2c_write_multiple(struct rmi_phys_driver *physdrvr, unsigned short address,
                       char *valp, int size)
{
    struct instance_data *instancedata =
        container_of(physdrvr, struct instance_data, rmiphysdrvr);

    unsigned char *txbuf;
    unsigned char txbuf_most[17]; /* Use this buffer for fast writes of 16
					bytes or less.  The first byte will
					contain the address at which to start
					the write. */
    int retval = 0;
    int i;

    if (size < sizeof(txbuf_most))
    {
        /* Avoid an allocation if we can help it. */
        txbuf = txbuf_most;
    }
    else
    {
        /* over 16 bytes write we'll need to allocate a temp buffer */
        txbuf = kzalloc(size + 1, GFP_KERNEL);
        if (!txbuf)
            return -ENOMEM;
    }

    /* Yes, it stinks here that we have to copy the buffer */
    /* We copy from valp to txbuf leaving
    the first location open for the address */
    for (i = 0; i < size; i++)
        txbuf[i + 1] = valp[i];

    /* Can't have anyone else changing the page behind our backs */
    mutex_lock(&page_mutex);

    if (((address >> 8) & 0xff) != instancedata->page)
    {
        /* Switch pages */
        retval = rmi_set_page(instancedata, ((address >> 8) & 0xff));
        if (retval)
            goto exit;
    }

    txbuf[0] = address & 0xff; /* put the address in the first byte */
    retval = i2c_master_send(instancedata->i2cclient, txbuf, size + 1);

    /* TODO: Add in retyr on writes only in certian error return values */
    if (retval != 1)
    {
        dev_err(&instancedata->i2cclient->dev, "%s: Write fail: %d\n",
                __func__, retval);
        goto exit;
    }
exit:

    mutex_unlock(&page_mutex);
    if (txbuf != txbuf_most)
        kfree(txbuf);
    return retval;
}

/*
 * This is the Interrupt Service Routine.  It just notifies the application
 * layer that attention is required.
 */
static irqreturn_t
i2c_attn_isr(int irq, void *info)
{
    struct instance_data *instancedata = info;
    // printk("[futengfei] come into -----IRQ_HANDLED------------>[%s]\n", __func__);

    disable_irq_nosync(instancedata->irq);

    if (instancedata->rmiphysdrvr.attention)
    {
        instancedata->rmiphysdrvr.attention(&instancedata->rmiphysdrvr, instancedata->instance_no);
    }

    return IRQ_HANDLED;
}

/* The Driver probe function - will allocate and initialize the instance
 * data and request the irq and set the instance data as the clients
 * platform data then register the physical driver which will do a scan of
 * the RMI4 Physical Device Table and enumerate any RMI4 functions that
 * have data sources associated with them.
 */
/*
static int ft5x06_i2c_read(struct i2c_client *client, char *writebuf,
		   int writelen, char *readbuf, int readlen)
{
int ret;

if (writelen > 0)
{
	struct i2c_msg msgs[] = {
		{
		 .addr = client->addr,
		 .flags = 0,
		 .len = writelen,
		 .buf = writebuf,
		 },
		{
		 .addr = client->addr,
		 .flags = I2C_M_RD,
		 .len = readlen,
		 .buf = readbuf,
		 },
	};
	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret < 0)
		dev_err(&client->dev, "f%s: i2c read error.\n",
			__func__);
}
else
	{
	struct i2c_msg msgs[] = {
		{
		 .addr = client->addr,
		 .flags = I2C_M_RD,
		 .len = readlen,
		 .buf = readbuf,
		 },
	};
	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s:i2c read error.\n", __func__);
}
return ret;
}
*/



static int rmi_i2c_probe(struct i2c_client *client, const struct i2c_device_id *dev_id)
{

    struct instance_data *instancedata;
    int retval = 0, err = 0;
    int irqtype = 0;



    struct rmi_i2c_platformdata *platformdata;
    struct rmi_sensordata *sensordata;


    printk("[futengfei] come into11 ---------------------------->[%s]\n", __func__);
    //msleep(8000);
    if (client == NULL)
    {
        printk( "%s: Invalid NULL client received.\n", __func__);
        return -EINVAL;
    }

    printk( "%s: Probing i2c RMI device, addr: 0x%02x\n", __func__, client->addr);


    gpio_tlmm_config(GPIO_CFG(rmiresetpin, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_16MA), GPIO_CFG_ENABLE);
    err = gpio_request(rmiresetpin, "rmireset");
    if(err)
    {
        printk("[futengfei] err--------------------------gpio_request(26, rmireset)\n" );
    }
    err = gpio_direction_output(rmiresetpin, 1);
    if(err)
    {
        printk("[futengfei] outputerr--------------------------gpio_request(26, rmireset)\n" );
    }
    gpio_set_value_cansleep(rmiresetpin, 1);
    msleep(300);
    /* Allocate and initialize the instance data for this client */
    instancedata = kzalloc(sizeof(*instancedata), GFP_KERNEL);
    if (!instancedata)
    {
        printk("%s: Out of memory trying to allocate instance_data.\n", __func__);
        return -ENOMEM;
    }
    printk("[futengfei] come into ----------2------------->[%s]\n", __func__);

    instancedata->rmiphysdrvr.name           = DRIVER_NAME;
    instancedata->rmiphysdrvr.write          = rmi_i2c_write;
    instancedata->rmiphysdrvr.read           = rmi_i2c_read;
    instancedata->rmiphysdrvr.write_multiple = rmi_i2c_write_multiple;
    instancedata->rmiphysdrvr.read_multiple  = rmi_i2c_read_multiple;
    instancedata->rmiphysdrvr.module         = THIS_MODULE;
    printk("[futengfei] come into ----------3------------->[%s]\n", __func__);
    /* Set default to polling in case no matching platform data is located
    for this device. We'll still work but in polling mode since we didn't
    find any irq info */
    instancedata->rmiphysdrvr.polling_required = true;

    instancedata->page = 0xffff; /* Force a set page the first time */
    printk("[futengfei] come into ----------4------------->[%s]\n", __func__);
    /* cast to our struct rmi_i2c_platformdata so we know
    the fields (see rmi_ic2.h) */
    platformdata = client->dev.platform_data;
    if (platformdata == NULL)
    {
        printk( "%s: CONFIGURATION ERROR - platform data is NULL.\n", __func__);
        return -EINVAL;
    }
    sensordata = platformdata->sensordata;
    /* Egregiously horrible delay here that seems to prevent I2C disasters on
     * certain broken dev systems.  In most cases, you can safely leave this
     * as zero.
     */
    if (platformdata->delay_ms > 0)
        mdelay(platformdata->delay_ms);

    printk( "%s: sensor addr: 0x%02x irq: 0x%x type: %d\n", __func__, platformdata->i2c_address, platformdata->irq, platformdata->irq_type);
    if (client->addr != platformdata->i2c_address)
    {
        printk( "%s: CONFIGURATION ERROR - client I2C address 0x%02x doesn't match platform data address 0x%02x.\n", __func__, client->addr, platformdata->i2c_address);
        return -EINVAL;
    }
    printk("[futengfei] come into ----------5------------>[%s]\n", __func__);
    instancedata->instance_no = device_count++;

    /* set the device name using the instance_no appended
    to DEVICE_NAME to make a unique name */
    dev_set_name(&client->dev, "rmi4-i2c%d", instancedata->instance_no);
    printk("[futengfei] come into ----------6------------>[%s]\n", __func__);

    /* Determine if we need to poll (inefficient) or use interrupts.
    */
    if (platformdata->irq)
    {
        instancedata->irq = platformdata->irq;
        switch (platformdata->irq_type)
        {
        case IORESOURCE_IRQ_HIGHEDGE:
            irqtype = IRQF_TRIGGER_RISING;
            break;
        case IORESOURCE_IRQ_LOWEDGE:
            irqtype = IRQF_TRIGGER_FALLING;
            break;
        case IORESOURCE_IRQ_HIGHLEVEL:
            irqtype = IRQF_TRIGGER_HIGH;
            break;
        case IORESOURCE_IRQ_LOWLEVEL:
            irqtype = IRQF_TRIGGER_LOW;
            break;
        default:
            printk("%s: Invalid IRQ flags in platform data.\n", __func__);
            kfree(instancedata);
            return -ENXIO;
        }

        instancedata->rmiphysdrvr.polling_required = false;
        instancedata->rmiphysdrvr.irq = instancedata->irq;

    }
    else
    {
        instancedata->rmiphysdrvr.polling_required = true;
        printk(	"%s: No IRQ info given. Polling required.\n", __func__);
    }

    /* Store the instance data in the i2c_client - we need to do this prior
    * to calling register_physical_driver since it may use the read, write
    * functions. If nothing was found then the id fields will be set to 0
    * for the irq and the default  will be set to polling required so we
    * will still work but in polling mode. */
    i2c_set_clientdata(client, instancedata);

    /* Copy i2c_client pointer into instance_data's i2c_client pointer for
    later use in rmi4_read, rmi4_write, etc. */
    instancedata->i2cclient = client;

    /* Call the platform setup routine, to do any setup that is required before
     * interacting with the device.
     */
    printk("[futengfei] come into ----------7------------>[%s]\n", __func__);

    if (sensordata && sensordata->rmi_sensor_setup)
    {
        printk("[futengfei] come into ----------7.1------%d---->[%s]\n", retval, __func__);
        retval = sensordata->rmi_sensor_setup();
        printk("[futengfei] come into ----------7.2------%d---->[%s]\n", retval, __func__);
        if (retval)
        {
            printk( "[futengfei] %s: sensor setup failed with code %d.\n", __func__, retval);
            return retval;
        }
    }
    printk("[futengfei] come into ----------8------------>[%s]\n", __func__);
#if 0

    /////////////////////////TEST IIC////////////////////////////////
    for (retval = 0; retval < 100; retval++)
    {

        u8 testdata[15];
        int err2 = 0;
        err2 = i2c_master_recv(client, testdata, 15);
        if(err2 != 1)

        {
            u8 reg_addr = 0;
            u8 reg_value = 0;
            reg_addr = FT5X06_REG_FW_VER;
            err2 = ft5x06_i2c_read(client, &reg_addr, 1, &reg_value, 1);
        }
        if(err2)

        {
            printk("[futengfei] i2c_master_recv ERR! 11111111111111[%d]-reg_value=[0x%02x]---->[%s]\n", err2, testdata[0], __func__); //reg_value
        }
        else
        {
            printk("[futengfei] i2c_master_recv sucess! 222222222222222[%d]-reg_value=[0x%02x]---->[%s]\n", err2, testdata[0], __func__); //reg_value
        }
        msleep(120);

    }
#endif

    /* Register sensor drivers - this will call the detect function that
    * will then scan the device and determine the supported RMI4 sensors
    * and functions.
    */
    retval = rmi_register_sensor(&instancedata->rmiphysdrvr, platformdata->sensordata);
    if (retval)
    {
        printk("[futengfei] %s: ----------Failed to Register [%s] sensor drivers\n", __func__, instancedata->rmiphysdrvr.name);
        i2c_set_clientdata(client, NULL);
        kfree(instancedata);
        return retval;
    }
    printk("[futengfei] come into ----------9------------>[%s]\n", __func__);

    if (instancedata->rmiphysdrvr.polling_required == false)
    {

        printk("[futengfei] come into ----------10------------>[%s]\n", __func__);
        retval = request_irq(instancedata->irq, i2c_attn_isr, irqtype, "rmi_i2c", instancedata);
        if (retval)
        {
            printk( "[futengfei] %s: failed to obtain IRQ %d. Result: %d.\n", __func__, instancedata->irq, retval);
            printk( "[futengfei] %s: Reverting to polling.\n", __func__);
            instancedata->rmiphysdrvr.polling_required = true;
            /* TODO: Need to revert back to polling - create and start timer. */
        }
        else
        {
            printk("[futengfei] %s: got irq.sucess \n", __func__);
        }
    }

    printk("[futengfei] come into ----------11------------>[%s]\n", __func__);
    printk( "[futengfei] %s: Successfully registered %s sensor driver.\n",	__func__, instancedata->rmiphysdrvr.name);
    printk( "[futengfei] %s: Successfully registered %s sensor driver.\n", __func__, instancedata->rmiphysdrvr.name);

    return retval;
}

/* The Driver remove function.  We tear down the instance data and unregister
 * the phys driver in this call.
 */
static int
rmi_i2c_remove(struct i2c_client *client)
{
    struct instance_data *instancedata =
        i2c_get_clientdata(client);

    dev_dbg(&client->dev, "%s: Unregistering phys driver %s\n", __func__,
            instancedata->rmiphysdrvr.name);

    rmi_unregister_sensors(&instancedata->rmiphysdrvr);

    dev_dbg(&client->dev, "%s: Unregistered phys driver %s\n",
            __func__, instancedata->rmiphysdrvr.name);

    /* only free irq if we have an irq - otherwise the instance_data
    will be 0 for that field */
    if (instancedata->irq)
        free_irq(instancedata->irq, instancedata);

    kfree(instancedata);
    dev_dbg(&client->dev, "%s: Remove successful\n", __func__);

    return 0;
}

#ifdef CONFIG_PM
static int
rmi_i2c_suspend(struct i2c_client *client, pm_message_t mesg)
{
    /* Touch sleep mode */
    return 0;
}

static int
rmi_i2c_resume(struct i2c_client *client)
{
    /* Re-initialize upon resume */
    return 0;
}
#else
#define rmi_i2c_suspend	NULL
#define rmi_i2c_resume	NULL
#endif

/*
 * This structure tells the i2c subsystem about us.
 *
 * TODO: we should add .suspend and .resume fns.
 *
 */
static struct i2c_driver rmi_i2c_driver =
{
    .probe		= rmi_i2c_probe,
    .remove		= rmi_i2c_remove,
    .suspend	= rmi_i2c_suspend,
    .resume		= rmi_i2c_resume,
    .id_table	= rmi_i2c_id_table,
    .driver = {
        .name  = DEVICE_NAME,
        .owner = THIS_MODULE,
    },
};

/*
 * Register ourselves with i2c Chip Driver.
 *
 */
extern bool is_ts_load;
//bool is_ts_load;
int rmi_phys_i2c_init(void)
{
    printk("[futengfei] come into ---------------------------->[%s]\n", __func__);

    is_ts_load = 1;

    return i2c_add_driver(&rmi_i2c_driver);
}

/*
 * Un-register ourselves from the i2c Chip Driver.
 *
 */
#if 0
static void __exit rmi_phys_i2c_exit(void)
{
    i2c_del_driver(&rmi_i2c_driver);
}
#endif

//module_init(rmi_phys_i2c_init);
//module_exit(rmi_phys_i2c_exit);

MODULE_AUTHOR("Synaptics, Inc.");
MODULE_DESCRIPTION("RMI4 Driver I2C Physical Layer");
MODULE_LICENSE("GPL");
