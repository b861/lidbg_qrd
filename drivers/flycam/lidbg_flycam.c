
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
	isBack = SOC_IO_Input(BACK_DET, BACK_DET, GPIO_CFG_PULL_UP);
	
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
		else if(!strcmp(keyval[0], "gain") )
		{
			int gainVal;
			char temp_cmd[256];
			gainVal = simple_strtoul(keyval[1], 0, 0);
			if(gainVal > 100)
			{
		        lidbg("gain args error![0-100]");
		        return size;
		    }
			lidbg("gainVal = %d",gainVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set gain=%d ", gainVal);
			lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "sharp") )
		{
			int sharpVal;
			char temp_cmd[256];
			sharpVal = simple_strtoul(keyval[1], 0, 0);
			if(sharpVal > 6)
			{
		        lidbg("sharp args error![0-6]");
		        return size;
		    }
			lidbg("sharpVal = %d",sharpVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set sharp=%d ", sharpVal);
			lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "gamma") )
		{
			int gammaVal;
			char temp_cmd[256];
			gammaVal = simple_strtoul(keyval[1], 0, 0);
			if(gammaVal > 500)
			{
		        lidbg("gamma args error![0-500]");
		        return size;
		    }
			lidbg("gammaVal = %d",gammaVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set gamma=%d ", gammaVal);
			lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "bright") )
		{
			int brightVal;
			char temp_cmd[256];
			brightVal = simple_strtoul(keyval[1], 0, 0);
			if(brightVal > 128)
			{
		        lidbg("bright args error![0-128]");
		        return size;
		    }
			lidbg("brightVal = %d",brightVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set bright=%d ", brightVal);
			lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "vmirror") )
		{
			int vmirrorVal;
			char temp_cmd[256];
			vmirrorVal = simple_strtoul(keyval[1], 0, 0);
			if(vmirrorVal > 1)
			{
		        lidbg("vmirror args error![0|1]");
		        return size;
		    }
			lidbg("vmirrorVal = %d",vmirrorVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set vmirror=%d ", vmirrorVal);
			lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "autogain") )
		{
			int autogainVal;
			char temp_cmd[256];
			autogainVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("autogainVal = %d",autogainVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set autogain=%d ", autogainVal);
			//lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "exposure") )
		{
			int exposureVal;
			char temp_cmd[256];
			exposureVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("exposureVal = %d",exposureVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set exposure=%d ", exposureVal);
			//lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "contrast") )
		{
			int contrastVal;
			char temp_cmd[256];
			contrastVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("autogainVal = %d",contrastVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set contrast=%d ", contrastVal);
			lidbg_shell_cmd(temp_cmd);
		}

		else if(!strcmp(keyval[0], "saturation") )
		{
			int saturationVal;
			char temp_cmd[256];
			saturationVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("exposureVal = %d",saturationVal);
			sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video1 --ef-set saturation=%d ", saturationVal);
			lidbg_shell_cmd(temp_cmd);
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
#if 0
	SOC_IO_Input(BACK_DET, BACK_DET, GPIO_CFG_PULL_UP);
	SOC_IO_ISR_Add(BACK_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING , irq_back_det, NULL);
#endif
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


