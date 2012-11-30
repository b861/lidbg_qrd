#include "lidbg.h"

#define SCAN_TIME (500)
#define TS_I2C_BUS (1)

#define TS_RESET  do{   	SOC_IO_Output(0,26,0);\
							msleep(500);\
							SOC_IO_Output(0,26,1);\
					}while(0)

struct probe_device
{
    char chip_addr;
    unsigned int sub_addr;
    int cmd;
};

 struct probe_device ts_probe_dev[] =
{
	//rmi
	{0x2c,0x68,LOG_CAP_TS_RMI},//sku7
	//ft5x06
	{0x38,0x00,LOG_CAP_TS_FT5X06_SKU7},//sku7
	{0x39,0x00,LOG_CAP_TS_FT5X06},//flycar
	//gt811
	{0x5d,0x00,LOG_CAP_TS_GT811},//flycar
	{0x55,0x00,LOG_CAP_TS_GT801},//flycar
};

bool scan_on=1;
unsigned int FLAG_FOR_15S_OFF = 0; //for  flycar
bool is_ts_load = 0;

void ts_scan(void)
{
	static unsigned int loop=0;
	int i;
	int32_t rc1,rc2;
	u8 tmp;
	
	for(i=0;i<SIZE_OF_ARRAY(ts_probe_dev);i++)
	{
		rc1 = SOC_I2C_Rec_Simple(TS_I2C_BUS,ts_probe_dev[i].chip_addr,&tmp, 1 );
		rc2 = SOC_I2C_Rec(TS_I2C_BUS,ts_probe_dev[i].chip_addr,ts_probe_dev[i].sub_addr,&tmp, 1 );
		
		lidbg("rc1=%x,rc2=%x\n",rc1,rc2);
		 
		if ((rc1 < 0)&&(rc2 < 0))
			    lidbg("i2c_addr 0x%x probe fail!\n",ts_probe_dev[i].chip_addr);
		else
		{
			lidbg("i2c_addr 0x%x found!\n",ts_probe_dev[i].chip_addr);
			scan_on=0;
			SOC_I2C_Rec(TS_I2C_BUS,0x12,0x00,&tmp, 1 );//let i2c bus release
			SOC_Capts_Insmod(ts_probe_dev[i].cmd);
			break;
		}
	}
	
	loop++;
       lidbg("ts_scan_time:%d\n",loop);

	//if(loop > 10) {scan_on=0;}
}

int ts_probe_thread(void *data)
{
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()){break;} 
		
		if(scan_on==1)
		{
			TS_RESET;
			msleep(SCAN_TIME);
			ts_scan();
			
		}
		else
		{
			msleep(200);
			if(is_ts_load == 0)
			{
				scan_on = 1;
				lidbg("ts not load,scan again...\n");
			}
			else
				break;

		}
    }
    return 0;
}

static int ts_probe_init(void)
{
	static struct task_struct *scan_task;
	DUMP_BUILD_TIME;
	 
	scan_task = kthread_create(ts_probe_thread, NULL, "ts_scan_task");
       wake_up_process(scan_task);
	return 0;
}

static void ts_probe_exit(void){}

module_init(ts_probe_init);
module_exit(ts_probe_exit);

EXPORT_SYMBOL(FLAG_FOR_15S_OFF);
EXPORT_SYMBOL(is_ts_load);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("lsw.");
