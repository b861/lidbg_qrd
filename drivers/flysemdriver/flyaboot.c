#include <assert.h>
#include <bits.h>
#include <stdlib.h>
#include <string.h>
#include <dev/keys.h>
#include <dev/gpio.h>
#include <dev/gpio_keypad.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <platform.h>


#include <app.h>
#include <debug.h>
#include <arch/arm.h>
#include <dev/udc.h>
#include <string.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include <dev/flash.h>
#include <lib/ptable.h>
#include <dev/keys.h>
#include <dev/fbcon.h>
#include <baseband.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <platform.h>
#include <assert.h>
#include <bits.h>
#include <stdlib.h>
#include <string.h>
#include <dev/keys.h>
#include <dev/gpio.h>
#include <dev/gpio_keypad.h>

#include <kernel/event.h>
#include <kernel/timer.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <platform.h>
#include <boot_stats.h>

#if DEVICE_TREE
#include <libfdt.h>
#include <dev_tree.h>
#endif

#include <crypto_hash.h>
#include <smem.h> //ML
#include "image_verify.h"
#include "bootimg.h"
#include "fastboot.h"
#include "sparse_format.h"
#include "mmc.h"
#include "scm.h"
#include "flylk.h"

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

//#define BAK_FLYLK_FLAG

#ifdef MEMBASE
#define EMMC_BOOT_IMG_HEADER_ADDR (0xFF000+(MEMBASE))
#else
#define EMMC_BOOT_IMG_HEADER_ADDR 0xFF000
#endif

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

#if DEVICE_TREE
BUF_DMA_ALIGN(dt_buf, 4096);
#endif


#if FLY_SCREEN_SIZE_1024
int fly_screen_w = 1024;
int fly_screen_h= 600;
#else
int fly_screen_w = 800;
int fly_screen_h=  480;
#endif

const char *model_message = "again to press the key go to";
const char *NO_SYS_MEG1 = "Found no system";
const char *NO_SYS_MEG2  = "please enter the recovery to restore system!";
const char *FASTBOOT_MEG = "You can execute some fastboot commands";
const char *INTO_FASTBOT = "fastboot model.......";
const char *INTO_REC= "entering recovery.......";
const char *INTO_FLYREC = "back_up_recovery model......";
int bp_meg = 0;
 
static unsigned char buf[4096]; //Equal to max-supported pagesize
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
			default:
				break;
		}

//		dprintf(INFO,"[FLYADIO]dcz ====>> len = %d count = %d \n",len,count);
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


}

int boot_flyrecovery_from_mmc(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct boot_img_hdr *uhdr;
	unsigned offset = 0;
	int rcode;
	unsigned long long ptn = 0;
	int index = INVALID_PTN;
	const char *cmdline;

	unsigned char *image_addr = 0;
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	unsigned imagesize_actual;
	unsigned second_actual = 0;

#if DEVICE_TREE
	struct dt_table *table;
	struct dt_entry dt_entry;
	unsigned dt_table_offset;
	uint32_t dt_actual;
	uint32_t dt_hdr_size;
#endif
	dprintf(CRITICAL,"****** Boot into flyrecovery model ******\n");

	if (!boot_into_recovery) {
		memset(ffbm_mode_string, '\0', sizeof(ffbm_mode_string));
		rcode = get_ffbm(ffbm_mode_string, sizeof(ffbm_mode_string));
		if (rcode <= 0) {
			boot_into_ffbm = false;
			if (rcode < 0)
				dprintf(CRITICAL,"failed to get ffbm cookie");
		} else
			boot_into_ffbm = true;
	} else
		boot_into_ffbm = false;
	uhdr = (struct boot_img_hdr *)EMMC_BOOT_IMG_HEADER_ADDR;
	if (!memcmp(uhdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(INFO, "Unified boot method!\n");
		hdr = uhdr;
		goto unified_boot;
	}

	index = partition_get_index("flyrecovery");
	dprintf(INFO,"partition flyrecovery index = %d \n",index);
	ptn = partition_get_offset(index);
	dprintf(INFO,"partition flyrecovery ptn = %d \n",ptn);

	if(ptn == 0) {
		dprintf(CRITICAL, "ERROR: No flyrecovery partition found\n");
                return -1;
	}

	if (mmc_read(ptn + offset, (unsigned int *) buf, page_size)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
                return -1;
	}

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(CRITICAL, "ERROR: Invalid boot image header\n");
                return -1;
	}

	if (hdr->page_size && (hdr->page_size != page_size)) {
		page_size = hdr->page_size;
		page_mask = page_size - 1;
	}

	/*
	 * Update the kernel/ramdisk/tags address if the boot image header
	 * has default values, these default values come from mkbootimg when
	 * the boot image is flashed using fastboot flash:raw
	 */
	update_ker_tags_rdisk_addr(hdr);

	/* Get virtual addresses since the hdr saves physical addresses. */
	hdr->kernel_addr = VA((addr_t)(hdr->kernel_addr));
	hdr->ramdisk_addr = VA((addr_t)(hdr->ramdisk_addr));
	hdr->tags_addr = VA((addr_t)(hdr->tags_addr));

	kernel_actual  = ROUND_TO_PAGE(hdr->kernel_size,  page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);

	/* Check if the addresses in the header are valid. */
	if (check_aboot_addr_range_overlap(hdr->kernel_addr, kernel_actual) ||
		check_aboot_addr_range_overlap(hdr->ramdisk_addr, ramdisk_actual))
	{
		dprintf(CRITICAL, "kernel/ramdisk addresses overlap with aboot addresses.\n");
		return -1;
	}

#ifndef DEVICE_TREE
	if (check_aboot_addr_range_overlap(hdr->tags_addr, MAX_TAGS_SIZE))
	{
		dprintf(CRITICAL, "Tags addresses overlap with aboot addresses.\n");
		return -1;
	}
#endif

	/* Authenticate Kernel */
	dprintf(INFO, "use_signed_kernel=%d, is_unlocked=%d, is_tampered=%d.\n",
		(int) target_use_signed_kernel(),
		device.is_unlocked,
		device.is_tampered);

	if(target_use_signed_kernel() && (!device.is_unlocked))
	{
		offset = 0;

		image_addr = (unsigned char *)target_get_scratch_address();

#if DEVICE_TREE
		dt_actual = ROUND_TO_PAGE(hdr->dt_size, page_mask);
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual + dt_actual);

		if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_actual))
		{
			dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
			return -1;
		}
#else
		imagesize_actual = (page_size + kernel_actual + ramdisk_actual);

#endif

		dprintf(INFO, "Loading boot image (%d): start\n", imagesize_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_START);

		if (check_aboot_addr_range_overlap(image_addr, imagesize_actual))
		{
			dprintf(CRITICAL, "Boot image buffer address overlaps with aboot addresses.\n");
			return -1;
		}

		/* Read image without signature */
		if (mmc_read(ptn + offset, (void *)image_addr, imagesize_actual))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image\n");
				return -1;
		}

		dprintf(INFO, "Loading boot image (%d): done\n", imagesize_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		offset = imagesize_actual;

		if (check_aboot_addr_range_overlap(image_addr + offset, page_size))
		{
			dprintf(CRITICAL, "Signature read buffer address overlaps with aboot addresses.\n");
			return -1;
		}

		/* Read signature */
		if(mmc_read(ptn + offset, (void *)(image_addr + offset), page_size))
		{
			dprintf(CRITICAL, "ERROR: Cannot read boot image signature\n");
			return -1;
		}

		verify_signed_bootimg(image_addr, imagesize_actual);

		/* Move kernel, ramdisk and device tree to correct address */
		memmove((void*) hdr->kernel_addr, (char *)(image_addr + page_size), hdr->kernel_size);
		memmove((void*) hdr->ramdisk_addr, (char *)(image_addr + page_size + kernel_actual), hdr->ramdisk_size);

		#if DEVICE_TREE
		if(hdr->dt_size) {
			dt_table_offset = ((uint32_t)image_addr + page_size + kernel_actual + ramdisk_actual + second_actual);
			table = (struct dt_table*) dt_table_offset;

			if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			/* Find index of device tree within device tree table */
			if(dev_tree_get_entry_info(table, &dt_entry) != 0){
				dprintf(CRITICAL, "ERROR: Device Tree Blob cannot be found\n");
				return -1;
			}

			/* Validate and Read device device tree in the "tags_add */
			if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size))
			{
				dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
				return -1;
			}

			memmove((void *)hdr->tags_addr, (char *)dt_table_offset + dt_entry.offset, dt_entry.size);
		} else {
			/*
			 * If appended dev tree is found, update the atags with
			 * memory address to the DTB appended location on RAM.
			 * Else update with the atags address in the kernel header
			 */
			void *dtb;
			dtb = dev_tree_appended((void*) hdr->kernel_addr,
						hdr->kernel_size,
						(void *)hdr->tags_addr);
			if (!dtb) {
				dprintf(CRITICAL, "ERROR: Appended Device Tree Blob not found\n");
				return -1;
			}
		}
		#endif
	}
	else
	{
		second_actual  = ROUND_TO_PAGE(hdr->second_size,  page_mask);

		dprintf(INFO, "Loading boot image (%d): start\n",
				kernel_actual + ramdisk_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_START);

		offset = page_size;

		/* Load kernel */
		if (mmc_read(ptn + offset, (void *)hdr->kernel_addr, kernel_actual)) {
			dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
					return -1;
		}
		offset += kernel_actual;

		/* Load ramdisk */
		if(ramdisk_actual != 0)
		{
			if (mmc_read(ptn + offset, (void *)hdr->ramdisk_addr, ramdisk_actual)) {
				dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
				return -1;
			}
		}
		offset += ramdisk_actual;

		dprintf(INFO, "Loading boot image (%d): done\n",
				kernel_actual + ramdisk_actual);
		bs_set_timestamp(BS_KERNEL_LOAD_DONE);

		if(hdr->second_size != 0) {
			offset += second_actual;
			/* Second image loading not implemented. */
			ASSERT(0);
		}

		#if DEVICE_TREE
		if(hdr->dt_size != 0) {
			/* Read the first page of device tree table into buffer */
			if(mmc_read(ptn + offset,(unsigned int *) dt_buf, page_size)) {
				dprintf(CRITICAL, "ERROR: Cannot read the Device Tree Table\n");
				return -1;
			}
			table = (struct dt_table*) dt_buf;

			if (dev_tree_validate(table, hdr->page_size, &dt_hdr_size) != 0) {
				dprintf(CRITICAL, "ERROR: Cannot validate Device Tree Table \n");
				return -1;
			}

			table = (struct dt_table*) memalign(CACHE_LINE, dt_hdr_size);
			if (!table)
				return -1;

			/* Read the entire device tree table into buffer */
			if(mmc_read(ptn + offset,(unsigned int *) table, dt_hdr_size)) {
				dprintf(CRITICAL, "ERROR: Cannot read the Device Tree Table\n");
				return -1;
			}

			/* Find index of device tree within device tree table */
			if(dev_tree_get_entry_info(table, &dt_entry) != 0){
				dprintf(CRITICAL, "ERROR: Getting device tree address failed\n");
				return -1;
			}

			/* Validate and Read device device tree in the "tags_add */
			if (check_aboot_addr_range_overlap(hdr->tags_addr, dt_entry.size))
			{
				dprintf(CRITICAL, "Device tree addresses overlap with aboot addresses.\n");
				return -1;
			}

			if(mmc_read(ptn + offset + dt_entry.offset,
						 (void *)hdr->tags_addr, dt_entry.size)) {
				dprintf(CRITICAL, "ERROR: Cannot read device tree\n");
				return -1;
			}
			#ifdef TZ_SAVE_KERNEL_HASH
			aboot_save_boot_hash_mmc(hdr->kernel_addr, kernel_actual,
				       hdr->ramdisk_addr, ramdisk_actual,
				       ptn, offset, hdr->dt_size);
			#endif /* TZ_SAVE_KERNEL_HASH */

		} else {
			/*
			 * If appended dev tree is found, update the atags with
			 * memory address to the DTB appended location on RAM.
			 * Else update with the atags address in the kernel header
			 */
			void *dtb;
			dtb = dev_tree_appended((void*) hdr->kernel_addr,
						kernel_actual,
						(void *)hdr->tags_addr);
			if (!dtb) {
				dprintf(CRITICAL, "ERROR: Appended Device Tree Blob not found\n");
				return -1;
			}
		}
		#endif
	}

	if (boot_into_recovery && !device.is_unlocked && !device.is_tampered)
		target_load_ssd_keystore();

unified_boot:
	if(hdr->cmdline[0]) {
		cmdline = (char*) hdr->cmdline;
	} else {
		cmdline = DEFAULT_CMDLINE;
	}

	if(bp_meg == 1)cmdline = open_system_print_message; 
	dprintf(INFO, "cmdline = '%s'\n", cmdline);
	
	boot_linux((void *)hdr->kernel_addr, (void *)hdr->tags_addr,
		   (const char *)cmdline, board_machtype(),
		   (void *)hdr->ramdisk_addr, hdr->ramdisk_size);

	return 0;
}

void flyaboot_init(unsigned *boot_into_recovery,bool *boot_into_fastboot)
{
	int model = 0;
	int arg = 0;
	int count_down_time = 20;

	fbcon_clear();

	//if(get_extra_recovery_message(&RecoveryMeg))
	if(emmc_get_extra_recovery_msg("flyparameter",sizeof(RecoveryMeg),&RecoveryMeg))
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

	arg = RecoveryMeg.bootParam.autoUp.val;

	if(arg != FlySystemModel)
	{
			model  = RecoveryModel;
			dprintf(INFO,"autoUp model check mdel = %x\n",model);
	}

	bp_meg = RecoveryMeg.bootParam.upName.val;
	dprintf(INFO,"flyaboot init bp_meg = %d\n",bp_meg);

	if(bp_meg == 1)
			dprintf(INFO,"System print is enabled !\n");
	else
			dprintf(INFO,"System print is disabled !\n");

	displaylight();

#ifdef BAK_FLYLK_FLAG
	model = RecoveryModel;

	dprintf(INFO,"*** Auto run bak-flylk boot into recovery *** \n");
	display_colour(model);
#else
	int Adcnum = adcread();

	display_colour(Adcnum);
	while(Adcnum&&(count_down_time--))
	{
		display_count(Adcnum,count_down_time);
		mdelay(100);
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
			dprintf(INFO,"Boot into FastbootModel\r\n");
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
			show_logo();
			displaylight();
	}
	else
	{
		fly_setBcol(WHITE_COL);
		fly_text_lk(8, (fly_screen_h -10),INTO_REC,BLACK_COL);
	}

	if(*boot_into_fastboot == true)
		display_fastboot_meg();

	return;
}


