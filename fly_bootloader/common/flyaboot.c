#include "fastboot.h"
#include "fly_private.h"
#include "fly_common.h"
/*
//#define FACTORY_MODEL
#define 		RED_COL 		0XFF0000
#define 		WHITE_COL 		0XFFFFFF
#define 		BLUE_COL 		0X0000FF
#define 		GREEN_COL 		0X00FF00
#define 		BLACK_COL 		0X000000
#define		PINK_COL 		0XFF8080
//#define 		fontsize16		10	

*/

#ifdef MEMBASE
#define EMMC_BOOT_IMG_HEADER_ADDR (0xFF000+(MEMBASE))
#else
#define EMMC_BOOT_IMG_HEADER_ADDR 0xFF000
#endif

#if FLY_SCREEN_SIZE_1024
int fly_screen_w = 1024;
int fly_screen_h= 600;
#else
int fly_screen_w = 800;
int fly_screen_h=  480;
#endif

const char *model_message ="Press any key or touch screen to enter";
const char *NO_SYS_MEG1 = "Found no system";
const char *NO_SYS_MEG2  = "please enter the recovery to restore system!";
const char *FASTBOOT_MEG = "You can execute some fastboot commands";
const char *INTO_FASTBOT = "fastboot model.......";
const char *INTO_REC= "entering recovery.......";
const char *INTO_FLYREC = "back_up_recovery model......";
int bp_meg = 0;
 
const char *open_system_print_message  = "console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x37";
const char *android_system_unprintf   = " androidboot.hardware=qcom loglevel=1";

extern  bool target_use_signed_kernel(void);

enum col_select{
		RED1,
		WHITE,
		BLUE,
		GREEN,
		BLACK,
};

void test_display()
{
	int num = 0;
	int count_down_time = 50;
	
	while(1)
	{
		if(num >= 60000) num = 0;
		switch(num%5)
		{
			case RED1:
			{
				fly_setBcol(RED_COL);
			//	fly_text_lk(30,200,"this is yi ge da ben dang 1",BLACK_COL);
				break;
			}
			case WHITE:
			{
				fly_setBcol(WHITE_COL);
				//fly_text_lk(30,300,"this is yi zhong da ben dang 2",BLACK_COL);
				break;
			}	
			case BLUE:
			{
				fly_setBcol(BLUE_COL);
				//fly_text_lk(30,400,"this is yi ge xiao ben dang 3",BLACK_COL);
				break;		
			}
			case GREEN:
			{
				fly_setBcol(GREEN_COL);
				//fly_text_lk(30,500,"this is yi ge xiao xiao ben dang 4",BLACK_COL);
				break;		
			}	
			case BLACK:
			{
				fly_setBcol(BLACK);
				//fly_text_lk(30,200,"this is yi ge xiao xiao xiao ben dang 5",WHITE_COL);
				break;		
			}	
			default:
				break;
		}
		
	while(judge_key_state() == -1);
	
	while(!judge_key_state())
	{
		if(!(count_down_time--))
		{			
			break;
		}
		mdelay(100);
	}
	
	if(count_down_time<=0) 
	{
		reboot_device(0);
		break;
	}
	else count_down_time = 50;
	num++;
     }
	return ;
}

void display_colour(int model)
{
		int count = 0;
		int len = strlen(model_message);
		switch(model)
		{
			case RecoveryModel:
			{
				count = fontsize*(sizeof(" Recovery: ") + len);
				
				fly_version(1024-30-count, fly_screen_h -10,"%s Recovery: ",BLACK_COL,model_message);
				break;
			}
			case FastbootModel:
			{
				count = fontsize*(sizeof(" Fastboot: ") + len);
				fly_version(1024-30-count, fly_screen_h -10,"%s Fastboot: ",BLACK_COL,model_message);
				break;
			}
			case FlyRecoveryModel:
			{
				count = fontsize*(sizeof(" FlyRecovery: ") + len);
				fly_version(1024 -30 -count, fly_screen_h -10,"%s FlyRecovery: ",BLACK_COL,model_message);
				break;
			}
			case PreRecoveryModel:
                       {
                               count = fontsize*(sizeof(" Recovery: ") + len);
                               fly_version(1024 -30 -count, fly_screen_h -10,"%s Recovery: ",RED_COL,model_message);
                               break;
                       }
			default:
				break;
		}

//		dprintf(INFO,"[FLYADIO]dcz ====>> len = %d count = %d \n",len,count);
		return ;
}
void display_enter_recovery_count(int count)
{
       drawRect(1024-30,fly_screen_h -30,40,30,BLACK_COL,BLACK_COL);
#if (LOGO_FORMAT== RGB888)
       fly_version(1024-30, fly_screen_h -10,"%d",0X818181,count);
#else
       fly_version(1024-30, fly_screen_h -10,"%d",0Xffffff,count);
#endif

       return ;
}
void display_count(int model,int count)
{
		switch(model)
		{
			case RecoveryModel:
			{
				drawRect(1024-30,fly_screen_h -30,40,30,GREEN_COL,GREEN_COL);
				fly_version(1024-30, fly_screen_h -10,"%d",BLACK_COL,count);
				//dprintf(INFO,"[dczhou]====>>>recovery number ....");
				break;
			}
			case FastbootModel:
			{
				drawRect(1024-30,fly_screen_h -30,40,30,BLUE_COL,BLUE_COL);
				fly_version(1024-30, fly_screen_h -10,"%d",BLACK_COL,count);
				//dprintf(INFO,"[dczhou]====>>>fastboot  number ....");
				break;
			}
			case FlyRecoveryModel:
			{
				drawRect(1024-30,fly_screen_h -30,40,30,RED_COL,RED_COL);
				fly_version(1024-30, fly_screen_h -10,"%d",BLACK_COL,count);
				break;
			}
			default:
				break;
	}
		return ;
}
void test_display_logo()
{
  	while(1)
  	{
		if(!judge_key_state())
		{
			mdelay(300);
			if(!judge_key_state())
				break;
		}
	}
}

void prompt_meg()
{
	int fsize = 0;
	fly_setBcol(WHITE_COL);
	fsize = fontsize*strlen(NO_SYS_MEG1);
	fly_text_lk((fly_screen_w - fsize)/2, (fly_screen_h +16)/2 - 25,NO_SYS_MEG1,RED_COL);//please enter the recovery to restore system!
	fsize = fontsize*strlen(NO_SYS_MEG2);
	fly_text_lk((fly_screen_w - fsize)/2, (fly_screen_h +16)/2,NO_SYS_MEG2,RED_COL);
	fly_text_lk(8, (fly_screen_h -10),INTO_REC,BLACK_COL);
}

void display_fastboot_meg()
{
	int fsize = 0;
	fly_setBcol(WHITE_COL);
	fsize = fontsize*strlen(FASTBOOT_MEG);
	fly_text_lk((1024 - fsize)/2,(fly_screen_h +16)/2 + 20,FASTBOOT_MEG,BLACK_COL);
	fly_text_lk(8, (fly_screen_h -10),INTO_FASTBOT,BLACK_COL);
}

void erase_flypartition()
{
	fly_erase("recovery");
	fly_erase("persist");
	fly_erase("boot");
	fly_erase("system");
	fly_erase("cache");
	fly_erase("misc");
	fly_erase("logo");
	fly_erase("flydata");
	fly_erase("flyrwdata");
	fly_erase("flysystem");
	fly_erase("flyapdata");
	fly_erase("userdata");
	fly_erase("flyparameter");	
}
extern unsigned page_size ;
int test_system()
{
#if 0
	struct boot_img_hdr *hdr = (void*) buf;
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;


	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	   ptn = ptable_find(ptable, "boot");
	    if (ptn == NULL) {
		        dprintf(CRITICAL, "ERROR: No boot partition found\n");
		        return -1;

	}

	if (flash_read(ptn, offset, buf, page_size)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
		return -1;
	}

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(CRITICAL, "ERROR: Invalid boot image header\n");
		return -1;
	}

	return  0x869;

#endif
}

char *dbg_msg_set(const char *system_cmd)
{
	return dbg_msg_en(system_cmd, bp_meg);
}

void flyaboot_init(unsigned *boot_into_recovery,bool *boot_into_fastboot)
{
	int model = 0;
	int arg = 0;
	int Adcnum = 0;
	int count_down_time = 4;
	int bofore_recovery_time = 4;
	int hw_info = -1;
	dprintf(INFO, "----- LK Build Time: %s %s -----\n", __DATE__, __TIME__);

	flyaudio_hw_init();

	fly_fbcon_clear();
	//if(get_extra_recovery_message(&RecoveryMeg))
	if(ptn_read("flyparameter", 0, sizeof(RecoveryMeg), &RecoveryMeg))
	{
		dprintf(INFO,"flyaboot init autoUp.val = %d  upName.val = %d\n",RecoveryMeg.bootParam.autoUp.val,RecoveryMeg.bootParam.upName.val);
	}
	/****************************************************************/
	dprintf(INFO,"flyaboot init autoUp.val = 0x%x  upName.val = %d\n",RecoveryMeg.bootParam.autoUp.val,RecoveryMeg.bootParam.upName.val);
	dprintf(INFO,"flyaboot init RecoveryMeg.recovery_headers.flags: %s\n",RecoveryMeg.recovery_headers.flags);
	dprintf(INFO,"flyaboot init RecoveryMeg.bpTimes.flags: %s\n",RecoveryMeg.bpTimes.flags);


	/****************************add check UpMdel***************************/
	if(strncmp(RecoveryMeg.bootParam.bootParamsLen.flags,"BOOTLEN",strlen("BOOTLEN"))  != 0)
	{
		dprintf(INFO,"LK bootParamsLen.flags error : %s\n",RecoveryMeg.bootParam.bootParamsLen.flags); 

		memcpy(RecoveryMeg.bootParam.bootParamsLen.flags,"BOOTLEN",strlen("BOOTLEN"));
		RecoveryMeg.bootParam.bootParamsLen.val = 0;

		memcpy(RecoveryMeg.bootParam.autoUp.flags,"AUTOUP",strlen("AUTOUP"));
		RecoveryMeg.bootParam.autoUp.val = 0x5432;

		memcpy(RecoveryMeg.bootParam.upName.flags,"AUNAME",strlen("AUNAME"));
		RecoveryMeg.bootParam.upName.val = 2;
	}
	/****************************************************************/
//	hw_info = atoi(&RecoveryMeg.hwInfo.info[4]);
	hw_info = (RecoveryMeg.hwInfo.info[4]-'0');
	if(hw_info < 0)
		dprintf(INFO,"Error hardware information type !\n");
	else
		dprintf(INFO,"****** flyaboot init hw_info = %d ******\n",hw_info);

	arg = RecoveryMeg.bootParam.autoUp.val;

	if(arg != FlySystemModel)
	{
			model  = RecoveryModel;
			dprintf(INFO,"autoUp model check mdel = %x\n",model);
	}

	bp_meg = RecoveryMeg.bootParam.upName.val;
	dprintf(INFO,"flyaboot init bp_meg = %d\n",bp_meg);

	send_hw_info(hw_info);
	ctp_type_get();
	/*
	*don't show static logo before displaying green screen when auto-up
	*/
	if(arg == FlySystemModel)
	{
		if(*boot_into_fastboot == 0)
		show_logo();
	}
	mdelay(1000);
	mdelay(500);
	backlight_enable();
    if((arg == FlySystemModel)){
               if(*boot_into_fastboot == 0){
                       while(--bofore_recovery_time)
                       {
                               //display_colour(PreRecoveryModel);
                               display_enter_recovery_count(bofore_recovery_time);
                               if((bofore_recovery_time == 1) || (!judge_key_state())){
                                       Adcnum = get_boot_mode();
                                       break;
                               }
                               //mdelay(700);
                       }
               }
       }
#ifdef BAK_FLYLK_FLAG
	model = RecoveryModel;

	dprintf(INFO,"*** Auto run bak-flylk boot into recovery *** \n");
	display_colour(model);
#else

	display_colour(Adcnum);
	while(Adcnum&&(count_down_time--))
	{
		display_count(Adcnum,count_down_time);
//		mdelay(700);
		if( !judge_key_state())
		{
			model = Adcnum;
			dprintf(INFO,"flyaboot init model = %x\n",model);
			break;
		}
	}
#endif
	switch(model)
	{
		case FlyRecoveryModel:
			dprintf(INFO,"Boot into FlyRecoveryModel\r\n");
			fly_setBcol(WHITE_COL);
			fly_text_lk(8, (fly_screen_h -10),INTO_FLYREC,BLACK_COL);
			boot_flyrecovery_from_mmc();
			break;
		case RecoveryModel:
			*boot_into_recovery = 1;
			dprintf(INFO,"Boot into RecoveryModel\r\n");
			break;
		case FastbootModel:
			*boot_into_fastboot = true;
			dprintf(INFO,"Boot into LoaderModel\r\n");
			break;
		case 0:
			dprintf(INFO,"Boot into NormalModel\r\n");
			break;
		default:
			dprintf(INFO,"Unknown Model!\n");
			break;
	}

	/*
	{
		char c = '\0';
		getc(&c);
		dprintf(ALWAYS,"[LSH]:get '%c' from UART,send 'f/F' then goto fastboot!\n",c);
		if((c == 'f') || (c == 'F'))
			boot_into_fastboot = true;
	}
	*/

	if(!*boot_into_recovery){
			memset(fb_base_get()+fly_screen_w*(fly_screen_h-30)*FBCON_BPP/8,0x00,fly_screen_w*30*FBCON_BPP/8);
		       //show_logo();
		       //backlight_enable();
	}
	else{
		fly_setBcol(WHITE_COL);
		fly_text_lk(8, (fly_screen_h -10),INTO_REC,BLACK_COL);
	}

	if(*boot_into_fastboot == true)
		display_fastboot_meg();

	return;
}


