
#include "lidbg_servicer.h"
#include "lidbg_insmod.h"

typedef enum
{
    GPT_SIEZ_ORIGIN,
    GPT_SIEZ_1024M,
    GPT_SIEZ_1536M,
    GPT_SIEZ_2048M,
    GPT_SIEZ_UNUSED,
} GPT_PARTITION_SIZE;

int main(int argc, char **argv)
{
	int parted_size = GPT_SIEZ_ORIGIN;

	if(argc < 1)
	{
		printf("usage:lidbg_parted argv\n");
		return -1;
	}

	parted_size = strtoul(argv[1], 0, 0);

	if(parted_size != GPT_SIEZ_ORIGIN)
	{
		lidbg("######### gpt start #########\n");
		system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 print");
		if(parted_size == 1024){
			lidbg("######### gpt set userdata to 1G #########\n");
			system("umount /cache");
			//rm mmcblk0p29 and mmcblk0p30
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 rm 29");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 rm 30");
			//create userdata from 1879MB ~ 2953MB
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 mkpartfs userdata ext2 1879MB 2953MB");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 name 29 userdata");
			//create flash from 2953MB ~ 31.3GB
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 mkpartfs flash ext2 2953MB 31.3GB");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 name 30 flash");
		}else if(parted_size == 1536){
			lidbg("######### gpt set userdata to 1.5G #########\n");
			system("umount /cache");
			//rm mmcblk0p29 and mmcblk0p30
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 rm 29");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 rm 30");
			//create userdata from 1879MB ~ 3465MB
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 mkpartfs userdata ext2 1879MB 3465MB");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 name 29 userdata");
			//create flash from 3465MB ~ 31.3GB
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 mkpartfs flash ext2 3465MB 31.3GB");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 name 30 flash");
		}else if(parted_size == 2048){
			lidbg("######### gpt set userdata to 2G #########\n");
			system("umount /cache");
			//rm mmcblk0p29 and mmcblk0p30
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 rm 29");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 rm 30");
			//create userdata from 1879MB ~ 3977MB
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 mkpartfs userdata ext2 1879MB 3977MB");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 name 29 userdata");
			//create flash from 3977MB ~ 31.3GB
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 mkpartfs flash ext2 3977MB 31.3GB");
			system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 name 30 flash");
		}else
			lidbg("######### Undefined gpt userdata size #########\n");

	lidbg("######### gpt set end #########\n");
	system("/system/lib/modules/out/parted -s /dev/block/mmcblk0 print");
	}
	return 0;
}
