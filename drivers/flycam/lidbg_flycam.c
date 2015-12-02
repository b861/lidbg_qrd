
#include "lidbg.h"
//#include "LidbgCameraUsb.h"
LIDBG_DEFINE;

static wait_queue_head_t wait_queue;
char isBackChange = 0;
char isBack = 0;

/*
static int get_uvc_device(char *devname)
    {
        char    temp_devname[FILENAME_LENGTH];
        int     i = 0, ret = 0, fd;
        struct  v4l2_capability     cap;

        ALOGE("%s: E", __func__);
        *devname = '\0';
        while(1)
        {
            sprintf(temp_devname, "/dev/video%d", i);
            fd = open(temp_devname, O_RDWR  | O_NONBLOCK, 0);
            if(-1 != fd)
            {
                ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
                if((0 == ret) || (ret && (ENOENT == errno)))
                {
                    ALOGD("%s: Found UVC node: %s\n", __func__, temp_devname);
                    strncpy(devname, temp_devname, FILENAME_LENGTH);
                    break;
                }
                close(fd);
            }
            else
                ALOGD("%s.%d: Probing.%s: ret: %d, errno: %d,%s", __func__, i, temp_devname, ret, errno, strerror(errno));

            if(i++ > 10)
            {
                strncpy(devname, "/dev/video1", FILENAME_LENGTH);
                ALOGD("%s.%d: Probing fail:%s \n", __func__, i, devname);
                break;
            }
        }

        ALOGE("%s: X,%s", __func__, devname);
        return 0;
    }
*/

ssize_t  flycam_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
	if(!isBackChange)
	{
	    if(wait_event_interruptible(wait_queue, isBackChange))
	        return -ERESTARTSYS;
	}
	//isBack = SOC_IO_Input(BACK_DET, BACK_DET, GPIO_CFG_PULL_UP);
	
	if (copy_to_user(buffer, &isBack,  1))
	{
		lidbg("copy_to_user ERR\n");
	}
	isBackChange = 0;
	return size;
}

int flycam_open (struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t flycam_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	char *cmd[30] = {NULL};//cmds array
	char *keyval[2] = {NULL};//key-vals
	char cmd_num  = 0;//cmd amount
    char cmd_buf[512];
	int i;
    memset(cmd_buf, '\0', 512);
    if(copy_from_user(cmd_buf, buf, size))
    {
        lidbg("copy_from_user ERR\n");
    }
    if(cmd_buf[size - 1] == '\n')
		cmd_buf[size - 1] = '\0';
    cmd_num = lidbg_token_string(cmd_buf, " ", cmd) ;
	lidbg("-----FLYSTEP------------------[%s]---\n", cmd_buf);
	lidbg("-----cmd_num------------[%d]---\n", cmd_num);
	for(i = 0;i < cmd_num; i++)
	{
		lidbg_token_string(cmd[i], "=", keyval) ;
		/*
		if(!strcmp(keyval[0], "rgb"))
		{
		    int red, green, blue;
		    lidbg("case:[%s]\n", cmd[0]);
		    if(cmd_num < 4)
		    {
		        lidbg("RGB args error!");
		        return size;
		    }
		    red = simple_strtoul(cmd[1], 0, 0);
		    green = simple_strtoul(cmd[2], 0, 0);
		    blue = simple_strtoul(cmd[3], 0, 0);
		    PCA9634_RGB_Output(red, green, blue);
		}
		*/
		if(!strcmp(keyval[0], "record") )
		{
			if(!strncmp(keyval[1], "1", 1))//start
			{
			    lidbg("-------uvccam recording -----");
			    lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
			    if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
			    else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
			}
			else if(!strncmp(keyval[1], "0", 1))//stop
			{
				lidbg("-------uvccam stop_recording -----");
			    lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
			}
			else
			{
				lidbg("-------uvccam recording wrong arg:%s-----",keyval[1]);
			}
		}
		else if(!strcmp(keyval[0], "capture") )
		{
			if(!strncmp(keyval[1], "1", 1))//start
			{
				lidbg("-------uvccam capture-----");
				if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video1 -c -f mjpg -S &");
				else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video1 -c -f mjpg -S &");
			}
		}
		else if(!strcmp(keyval[0], "framesize") )
		{
			return size;
		}

		else if(!strcmp(keyval[0], "test") )
		{
			isBackChange = 1;
			wake_up_interruptible(&wait_queue);
		}
	}
  
    return size;

}

static  struct file_operations flycam_nod_fops =
{
    .owner = THIS_MODULE,
    .write = flycam_write,
    .open = flycam_open,
    .read = flycam_read,
};

irqreturn_t irq_back_det(int irq, void *dev_id)
{
	lidbg("----%s----",__func__);
	isBackChange = 1;
	wake_up_interruptible(&wait_queue);
    return IRQ_HANDLED;
}

int thread_flycam_test(void *data)
{
    while(1)
  	{
		wake_up_interruptible(&wait_queue);
		ssleep(5);
		lidbg("-------ehossleep-------");
		if(!isBackChange)
		{
		    if(wait_event_interruptible(wait_queue, isBackChange))  return -ERESTARTSYS;
		}

		lidbg("BACK_DET--------%d",SOC_IO_Input(BACK_DET, BACK_DET, GPIO_CFG_PULL_UP));
  	}
    return 0;
}

int thread_flycam_init(void *data)
{
    lidbg_new_cdev(&flycam_nod_fops, "lidbg_flycam");
	lidbg("%s:------------start------------",__func__);
	//SOC_IO_Input(BACK_DET, BACK_DET, GPIO_CFG_PULL_UP);
	//SOC_IO_ISR_Add(BACK_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING , irq_back_det, NULL);
	init_waitqueue_head(&wait_queue);

	//CREATE_KTHREAD(thread_flycam_test, NULL);
	/*
    if((!g_var.is_fly) && (g_var.recovery_mode == 0)))
    {
       
    }
	*/
    return 0;
}

static __init int lidbg_flycam_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;

    CREATE_KTHREAD(thread_flycam_init, NULL);

    return 0;

}
static void __exit lidbg_flycam_deinit(void)
{
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_flycam_init);
module_exit(lidbg_flycam_deinit);


