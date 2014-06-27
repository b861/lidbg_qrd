
#include "lidbg.h"

LIDBG_DEFINE;

#define GTP_RST_PORT_ACTIVE (1)
#define USE_TS_NUM (0)

#if (defined(BOARD_V1) || defined(BOARD_V2) || defined(BOARD_V3))
#define FLYHAL_CONFIG_PATH "/flydata/flyhalconfig"
#else
#define FLYHAL_CONFIG_PATH "/flysystem/flyconfig/default/lidbgconfig/flylidbgconfig.txt"
#endif

#define GTP_SWAP SWAP
#define GTP_REVERT(x, y)     do{\
         x = max_x-x;\
         y = max_y-y;\
       }while (0)
       
static LIST_HEAD(flyhal_config_list);
static int ts_scan_delayms;
static int ts_choose_touchscreen = 0;
int ts_should_revert = -1;
bool is_ts_load = false;

u32 max_x, max_y;


struct probe_device
{
    char chip_addr;
    unsigned int sub_addr;
    char *name;
    void (*reset)(void);
	void (*find_cb)(void);
};

void reset_high_active(void);
void reset_low_active(void);
void gt9xx_reset_high_active(void);
void ts_gpio_free(void);
struct probe_device ts_probe_dev[] =
{
    {0x14, 0x00, "gt911.ko", gt9xx_reset_high_active, ts_gpio_free},
    {0x5d, 0x00, "gt811.ko", reset_high_active, ts_gpio_free},
    {0x55, 0x00, "gt801.ko", reset_low_active, ts_gpio_free},
    {0x38, 0x00, "ft5x06_ts.ko", reset_high_active, ts_gpio_free}
    //{0x5d, 0x00, LOG_CAP_TS_GT910, "gt910new.ko",reset_high_active},
};


//zone below [method]
void gt9xx_reset_high_active(void)
{	
    SOC_IO_Output(0, GTP_RST_PORT, !GTP_RST_PORT_ACTIVE);
    msleep(200);
	#ifndef SOC_msm8x25
	SOC_IO_Output(0, GTP_INT_PORT, 1);
	usleep(200);
	#endif
    SOC_IO_Output(0, GTP_RST_PORT, GTP_RST_PORT_ACTIVE);
    msleep(300);
}
void reset_high_active(void)
{
    SOC_IO_Output(0, GTP_RST_PORT, !GTP_RST_PORT_ACTIVE);
    msleep(200);
    SOC_IO_Output(0, GTP_RST_PORT, GTP_RST_PORT_ACTIVE);
    msleep(300);
}

void reset_low_active(void)
{
    SOC_IO_Output(0, GTP_RST_PORT, GTP_RST_PORT_ACTIVE);
    msleep(200);
    SOC_IO_Output(0, GTP_RST_PORT, !GTP_RST_PORT_ACTIVE);
    msleep(300);
}
void ts_gpio_free(void)
{
   #ifndef SOC_msm8x25
   //gpio_free(GTP_INT_PORT);
   //gpio_free(GTP_RST_PORT);
   #endif
}
void parse_ts_info(struct probe_device *ts_info)
{
    char path[100];

	if(g_var.is_fly)
	{
	    sprintf(path, "/flysystem/lib/out/%s", ts_info->name);
	    lidbg_insmod( path );
	}
	else
	{
		sprintf(path, "/system/lib/modules/out/%s", ts_info->name);
		lidbg_insmod( path );
	}
    lidbg_fs_log(TS_LOG_PATH, "loadts=%s,USE_TS_NUM:%d,ts_choose_touchscreen:%d,ts_should_revert:%d\n", ts_info->name, USE_TS_NUM, ts_choose_touchscreen, ts_should_revert);
    fs_mem_log("loadts=%s,USE_TS_NUM:%d,ts_choose_touchscreen:%d,ts_should_revert:%d\n", ts_info->name, USE_TS_NUM, ts_choose_touchscreen, ts_should_revert);

}

struct probe_device *ts_scan(struct probe_device *tsdev, int size)
{
    static unsigned int loop = 0;
    int32_t rc1, rc2;
    int i;
    u8 tmp;

    for(i = 0; i < size; i++)
    {
        tsdev->reset();

        rc1 = SOC_I2C_Rec_Simple(TS_I2C_BUS, tsdev->chip_addr, &tmp, 1 );
        rc2 = SOC_I2C_Rec(TS_I2C_BUS, tsdev->chip_addr, tsdev->sub_addr, &tmp, 1 );
        SOC_I2C_Rec(TS_I2C_BUS, 0x12, 0x00, &tmp, 1 ); //let i2c bus release

        if ((rc1 < 0) && (rc2 < 0))
        {
            loop++;
            lidbg("fail.%d:[0x%x] rc1=%x,rc2=%x,delayms=%d\n", loop, tsdev->chip_addr, rc1, rc2, ts_scan_delayms);
        }
        else
        {
            lidbg("found:[0x%x,%s]\n", tsdev->chip_addr, tsdev->name);
	        tsdev->find_cb();
			return tsdev;
        }
        tsdev++;
    }
#ifdef SOC_msm8x25
    if(loop == 30)
    {
		if(g_var.is_fly)
			lidbg_insmod("/flysystem/lib/out/gt801.ko");
		else
	        lidbg_insmod("/system/lib/modules/out/gt801.ko");
    }
#endif

    return NULL;
}

void ts_probe_prepare(void)
{
    char buff[50] = {0};
    fs_fill_list(FLYHAL_CONFIG_PATH, FS_CMD_FILE_LISTMODE, &flyhal_config_list);
    FS_REGISTER_INT(ts_scan_delayms, "ts_scan_delayms", 500, NULL);
    FS_REGISTER_INT(ts_choose_touchscreen, "ts_choose_touchscreen", 0, NULL);

    ts_should_revert = fs_find_string(&flyhal_config_list, "TSMODE_XYREVERT");
    if(ts_should_revert > 0)
        LIDBG_WARN("<TS.XY will revert>\n");
    else
        LIDBG_WARN("<TS.XY will normal>\n");

    lidbg_insmod(get_lidbg_file_path(buff, "lidbg_ts_to_recov.ko"));
    fs_register_filename_list(TS_LOG_PATH, true);
}
//zone end
void ts_data_report(touch_type t,int id,int x,int y,int w)
{
	GTP_SWAP(x, y);
    if (1 == ts_should_revert)
		GTP_REVERT(x, y);
	lidbg_touch_handle(t, id,x, y, w);

	if(t == TOUCH_DOWN)
	{
		g_var.flag_for_15s_off++;
		if(g_var.flag_for_15s_off >= 1000)
		{
			g_var.flag_for_15s_off = 1000;
		}
	}
	
	if((id == 4) && (t == TOUCH_DOWN) && (!g_var.is_fly) && (!g_var.recovery_mode)) // 5 fingers
	{
		SOC_Key_Report(KEY_BACK,KEY_PRESSED_RELEASED);
	}
	
	if((id == 0)&&(1 == g_var.recovery_mode))
	{
		static struct tspara touch = {0, 0, 0} ;
		if(t == TOUCH_DOWN)
			touch.press = 0;
		else 
			touch.press = 1;
		touch.x = x;
		touch.y = y;
		//SOC_Set_Touch_Pos(&touch);  //not ready
	}
}

//zone below [logic]
int ts_probe_thread(void *data)
{
    struct probe_device *ts = NULL;
	
	SOC_Display_Get_Res(&max_x,&max_y);
	
    ts_probe_prepare();

    if (USE_TS_NUM == 0 && ts_choose_touchscreen == 0)
    {
        while(!is_ts_load)
        {
            if((ts = ts_scan(ts_probe_dev, SIZE_OF_ARRAY(ts_probe_dev))))
            {
                parse_ts_info(ts);
                msleep(5000);
                ts = NULL;
            }
            msleep(ts_scan_delayms);
        }
    }
    else
    {
        LIDBG_WARN("<disable ts scan work>\n");
        parse_ts_info(&ts_probe_dev[ts_choose_touchscreen > 0 ? ts_choose_touchscreen - 1 : USE_TS_NUM - 1 ]);
    }

    ssleep(10);
    LIDBG_WARN("<ts_probe_thread exited>\n");

    return 0;
}
//zone end


static int ts_probe_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;
    CREATE_KTHREAD(ts_probe_thread, NULL);
    return 0;
}

static void ts_probe_exit(void)
{
}

module_init(ts_probe_init);
module_exit(ts_probe_exit);

EXPORT_SYMBOL(is_ts_load);
EXPORT_SYMBOL(ts_should_revert);
EXPORT_SYMBOL(ts_data_report);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("lsw.");
