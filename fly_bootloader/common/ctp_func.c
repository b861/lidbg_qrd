#include "i2c_gpio.h"
#include "../soc.h"
#include "fly_private.h"

static struct i2c_gpio_dev *devp = NULL;

#ifdef CTP_DBG
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MAX_LENGTH 240
#define GTP_REG_CONFIG_DATA   0x8047

static u8 config_data[GTP_ADDR_LENGTH]
    = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

static u8 configed_data[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
    = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
#endif

unsigned char ctp_read(char *buf,  unsigned char *sub_addr,unsigned int size)
{
	int ret;
//	sub_addr = sub_addr & 0xff;
	struct i2c_msg msg_buf[] = {
		{g_bootloader_hw.ctp_info.ctp_slave_add, I2C_M_WR, 2, sub_addr},
		{g_bootloader_hw.ctp_info.ctp_slave_add, I2C_M_RD, size, buf}
	};
	ret = bit_xfer(devp, msg_buf, 2);
	return ret;
}

unsigned char ctp_write( char *buf, unsigned int size)
{
       int ret;
	struct i2c_msg msg_buf[] = {
		{g_bootloader_hw.ctp_info.ctp_slave_add,I2C_M_WR, size, buf},
	};
	ret  = bit_xfer(devp, msg_buf, 1);
	return ret;
}

static void ctp_i2c_config()
{
	int ret = 0;
	int i =0;
	dprintf(INFO, "><><>< config ctp i2c bus ><><><\n");

	gpio_set_direction(g_bootloader_hw.ctp_info.ctp_sda,GPIO_OUTPUT);
	gpio_set_direction(g_bootloader_hw.ctp_info.ctp_scl,GPIO_OUTPUT);
	devp = malloc(sizeof(struct i2c_gpio_dev));
	if (!devp) {
		return NULL;
	}
	devp = memset(devp, 0, sizeof(struct i2c_gpio_dev));

	devp->name = "ctp_i2c_gpio";
	devp->scl_pin = g_bootloader_hw.ctp_info.ctp_scl;
	devp->sda_pin = g_bootloader_hw.ctp_info.ctp_sda;
	devp->retries = 5;
	devp->udelay = 1;

#ifdef CTP_DBG
	ret = ctp_read(configed_data,  config_data, 186);

    for(i=0; i<188; i++)
		dprintf(INFO, " reg[%x] = %x\n",(0x8047+i), configed_data[i+2]);
#endif
}

int ctp_reset()
{
	//reset ctp
	gpio_set_direction(g_bootloader_hw.ctp_info.ctp_rst,GPIO_OUTPUT);
	//	gpio_set(rst_pin, GPIO_OUTPUT);
	gpio_set_val(g_bootloader_hw.ctp_info.ctp_rst, 0);

	gpio_set_direction(g_bootloader_hw.ctp_info.ctp_int,GPIO_OUTPUT);
	//	gpio_set(int_pin, GPIO_OUTPUT);
	mdelay(100);
	gpio_set_val(g_bootloader_hw.ctp_info.ctp_int, 0);
	udelay(200);
	gpio_set_val(g_bootloader_hw.ctp_info.ctp_rst, 1);
	mdelay(100);

	//pul down int_rst
	gpio_set_val(g_bootloader_hw.ctp_info.ctp_int, 0);
	//	gpio_set(gpio, 0);

	mdelay(50);
	ctp_i2c_config();
	mdelay(100);
}
