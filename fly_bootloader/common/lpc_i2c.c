#include "i2c_gpio.h"
#include "soc.h"

static struct i2c_gpio_dev *lpc_i2c_devp = NULL;

unsigned char lpc_read(char *buf, unsigned int size)
{
	int ret;

	struct i2c_msg msg_buf[] = {
		{LPC_I2C_ADDR, I2C_M_RD, size, buf}
	};
	ret = bit_xfer(lpc_i2c_devp, msg_buf, 1);

	return ret;
}

unsigned char lpc_write( char *buf, unsigned int size)
{
    int ret;
	struct i2c_msg msg_buf[] = { {LPC_I2C_ADDR,
				      I2C_M_WR, size, buf}
	};
	ret  = bit_xfer(lpc_i2c_devp, msg_buf, 1);

	return ret;
}

static void lpc_i2c_config()
{
	int ret = 0;
	int i =0;
	dprintf(INFO, "><><>< config lpc i2c bus ><><><\n");

	gpio_set_direction(LPC_SDA_GPIO,GPIO_OUTPUT);
	gpio_set_direction(LPC_SCL_GPIO,GPIO_OUTPUT);
	lpc_i2c_devp = malloc(sizeof(struct i2c_gpio_dev));
	if (!lpc_i2c_devp) {
		dprintf(INFO, "Malloc space for lpc_i2c_devp failed.\n");
		return;
	}
	lpc_i2c_devp = memset(lpc_i2c_devp, 0, sizeof(struct i2c_gpio_dev));

	lpc_i2c_devp->name = "lpc_i2c_gpio";
	lpc_i2c_devp->scl_pin = LPC_SCL_GPIO;
	lpc_i2c_devp->sda_pin = LPC_SDA_GPIO;
	lpc_i2c_devp->retries = 5;
	lpc_i2c_devp->udelay = 1;
}

void send_hw_info(char hw_info)
{
	UINT8 Check_Sum = 0;
	UINT8 hwInfo[7]= {0xff,0x55,0x4,0x00, 0x25, hw_info};
	int i;

	lpc_i2c_config();
	dprintf(CRITICAL,"send hardware info {0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x}\n", hwInfo[0], hwInfo[1], hwInfo[2], hwInfo[3], hwInfo[4], hwInfo[5]);

	for(i= 2;i<6;i++)
	{
		Check_Sum += hwInfo[i];
	}
	hwInfo[6] = Check_Sum;

	lpc_write(hwInfo, sizeof(hwInfo));
}

void backlight_enable()
{
	UINT8 Check_Sum = 0;
	UINT8 back_light[7]= {0xff,0x55,0x4,0x02, 0x0d, 0x1};
	int i,j;

	dprintf(CRITICAL,"Open backlight !\n");	

	for(i= 2;i<6;i++)
	{
		Check_Sum += back_light[i];
	}
	
	back_light[6] = Check_Sum;

	lpc_write(back_light, sizeof(back_light));
}
