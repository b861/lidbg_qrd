#include "fastboot.h"
#include "fly_private.h"

#define	LDHOPEN		0

#define EXPAND(NAME) #NAME
#define TARGET(NAME) EXPAND(NAME)
#define DEFAULT_CMDLINE "mem=100M console=null";

#ifdef MEMBASE
#define EMMC_BOOT_IMG_HEADER_ADDR (0xFF000+(MEMBASE))
#else
#define EMMC_BOOT_IMG_HEADER_ADDR 0xFF000
#endif

static int last_press_flag = 0;
extern int ctp_config_index;

int touch_points_get()
{
	u8 touch_points_reg[] = {0x81, 0x4E};
	u8 clear_data[] = {0x81, 0x4E, 0x00};

	int i = 0;
	int cnt = 0;
	int points = 0;
	int press_confirm = 0;
	char points_data[2] = {0};
	int ctp_addr = 0;

	ctp_addr = g_bootloader_hw.ctp_info.chip_data[ctp_config_index].ctp_slave_add;
	clear_data[0] = g_bootloader_hw.ctp_info.chip_data[ctp_config_index].point_data_add >> 8;
	clear_data[1] = g_bootloader_hw.ctp_info.chip_data[ctp_config_index].point_data_add & 0xff;
	touch_points_reg[0] = g_bootloader_hw.ctp_info.chip_data[ctp_config_index].point_data_add >> 8;
	touch_points_reg[1] = g_bootloader_hw.ctp_info.chip_data[ctp_config_index].point_data_add & 0xff;

	while(cnt < 20){
			mdelay(50);
			ctp_read(ctp_addr, touch_points_reg, points_data, 2);
			points = points_data[0] & 0xf;
			ctp_write(ctp_addr, clear_data, sizeof(clear_data)/sizeof(clear_data[0]));
//			dprintf(INFO, "***** cnt = %d, points = %d *****\n", cnt, points);
			//released
			if((points == 0) && (!adc_get()) && (last_press_flag != 0)){
					for(i=0; i<50; i++){
							mdelay(10);
							ctp_read(ctp_addr, touch_points_reg, points_data, 2);
							press_confirm = points_data[0] & 0xf;
							ctp_write(ctp_addr, clear_data, sizeof(clear_data)/sizeof(clear_data[0]));
							if(press_confirm){
								dprintf(INFO, "*****  press released confirm, press_confirm=%d, i=%d *****\n",press_confirm ,i );
								last_press_flag = 1;
								return 1;
							}
					}
					dprintf(INFO, "*****  press released *****\n");
					last_press_flag = 0;
					return 0;
			}

			//pressed
			if((points != 0) || adc_get()){
					dprintf(INFO, "*****  press pressed *****\n");
					last_press_flag = 1;
					return 1;
			}
			points = 0;
			cnt++;
	}
	return 0;
}
int  fly_back_key()
{
	int meg = 0;
	//if((fly_get_adc(38 , 6) <= 3200000 ) | (fly_get_adc(39 ,7) <= 3200000 ))
	if(touch_points_get())
		meg = 1;
	return meg;
}

static int fly_back_key_meg()
{
	int meg = 1;

	if(fly_back_key())
		meg = 0;

	return meg;
}

int  get_boot_mode(void)
{
	int pass_time;
	int read_num = 0;
	char tem[150];
	pass_time = 0;   //press_time
	dprintf(INFO,"get boot mode !\n");

	while(pass_time < 240)
	{
		if(!fly_back_key())
		{
			dprintf(INFO,"No key !\n");
			break;
		}
		mdelay(50);
		if(pass_time>20 && pass_time<70)
		{
			dprintf(INFO,"RecoveryModel GREEN_COL\r\n");
			read_num = RecoveryModel;
			fly_setBcol(GREEN_COL);
		}
		if(pass_time>=70 && pass_time<120)
		{
			dprintf(INFO,"LoaderModel BLUE_COL\r\n");
			read_num = FastbootModel;
			fly_setBcol(BLUE_COL);
		}
		if(pass_time>=120 && pass_time<220)
		{
			dprintf(INFO,"FastbootModel RED_COL\r\n");
			read_num = FlyRecoveryModel;
			fly_setBcol(RED_COL);
		}

		if( pass_time>=220)
		{
				read_num = 0;
				dprintf(INFO,"*** Read key timeout, read_num = %d ***\n", read_num);
				break;
		}

	       else
		{
			pass_time++;
			continue;
		}
	}

	return read_num;
}

/*
void fly_thread_key_reboot()
{
	if(!fly_back_key_meg())
	{
		dprintf(INFO, "[FLYADIO]dcz====>> start to rebooting the device\n");
		reboot_device(0);
	}
}
*/
int judge_key_state()
{
	return fly_back_key_meg();
}
