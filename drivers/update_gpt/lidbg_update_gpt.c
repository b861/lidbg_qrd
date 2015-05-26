#include "lidbg.h"
#include "lidbg_update_gpt.h"

#define  RECOVERY_MODE_DIR "/sbin/recovery"
#define DEVICE_NAME "gpt_dev"
#define parted_cmd "/flysystem/lib/out/parted -s /dev/block/mmcblk0"

GPT_PARTITION_SIZE gpt_size = GPT_SIEZ_ORIGIN;
bool bNeedUpdate;

int gpt_open (struct inode *inode, struct file *filp)
{
    printk("Gpt dev be opened. \n");
    return 0;
}

ssize_t gpt_read (struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

ssize_t gpt_write (struct file *filp, const char __user *buf, size_t count,
                      loff_t *f_pos)
{
    char gpt_info[32];//C99 variable length array

    char *p = NULL;
    int len = count;

    lidbg("gpt_write.\n");

    if (copy_from_user( gpt_info, buf, count))
    {
        lidbg("copy_from_user ERR\n");
    }

    if((p = memchr(gpt_info, '\n', count)))
    {
        len = p - gpt_info;
        *p = '\0';
    }
    else
        gpt_info[len] =  '\0';


	if(copy_from_user(gpt_info, buf, count))
	{
		PM_ERR("copy_from_user ERR\n");
	}

    if(!strcmp(gpt_info, "1024MB"))
		gpt_size = GPT_SIEZ_1024M;
	else if(!strcmp(gpt_info, "1536MB"))
		gpt_size = GPT_SIEZ_1536M;
	else if(!strcmp(gpt_info, "2048MB"))
		gpt_size = GPT_SIEZ_2048M;
	else
		gpt_size = GPT_SIEZ_ORIGIN;

	return count;
}

static  struct file_operations gpt_fops =
{
	.owner = THIS_MODULE,
	.read = gpt_read,
	.write = gpt_write,
	.open = gpt_open,
};

static int thread_update_gpt(void *data)
{
	while(1){
		if(gpt_size != GPT_SIEZ_ORIGIN)
		{
			if(gpt_size == GPT_SIEZ_1024M){
				lidbg("********** gpt set userdata to 1G **********\n");
				lidbg_shell_cmd("umount /cache");
				//rm mmcblk0p29 and mmcblk0p30
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 rm 29");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 rm 30");
				//create userdata from 1879MB ~ 2953MB
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 mkpartfs userdata ext2 1879MB 2953MB");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 name 29 userdata");
				//create flash from 2953MB ~ 31.3GB
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 mkpartfs flash ext2 2953MB 31.3GB");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 name 30 flash");
			}else if(gpt_size == GPT_SIEZ_1536M){
				lidbg("********** gpt set userdata to 1.5G **********\n");
				lidbg_shell_cmd("umount /cache");
				//rm mmcblk0p29 and mmcblk0p30
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 rm 29");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 rm 30");
				//create userdata from 1879MB ~ 3465MB
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 mkpartfs userdata ext2 1879MB 3465MB");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 name 29 userdata");
				//create flash from 3465MB ~ 31.3GB
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 mkpartfs flash ext2 3465MB 31.3GB");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 name 30 flash");
			}else if(gpt_size == GPT_SIEZ_2048M){
				lidbg("********** gpt set userdata to 2G **********\n");
				lidbg_shell_cmd("umount /cache");
				//rm mmcblk0p29 and mmcblk0p30
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 rm 29");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 rm 30");
				//create userdata from 1879MB ~ 3977MB
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 mkpartfs userdata ext2 1879MB 3977MB");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 name 29 userdata");
				//create flash from 3977MB ~ 31.3GB
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 mkpartfs flash ext2 3977MB 31.3GB");
				lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 name 30 flash");
			}else
				lidbg("********** Undefined gpt userdata size **********\n");

		lidbg_shell_cmd("/flysystem/lib/out/parted -s /dev/block/mmcblk0 print");
		}
		gpt_size = GPT_SIEZ_ORIGIN;
		msleep(500);
	}
	return 0;
}

static int gpt_driver_init(void)
{
	int ret;

	DUMP_BUILD_TIME;

	ret = 0;
	lidbg_new_cdev(&gpt_fops, DEVICE_NAME);

    if(fs_is_file_exist(RECOVERY_MODE_DIR))
		    CREATE_KTHREAD(thread_update_gpt, NULL);

	return 0;
}

static void gpt_driver_exit(void)
{
	return;
}

module_init(gpt_driver_init);
module_exit(gpt_driver_exit);

MODULE_AUTHOR("JesonJoy");
MODULE_DESCRIPTION("Update gpt driver");
MODULE_LICENSE("GPL");
