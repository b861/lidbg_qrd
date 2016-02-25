
#include "lidbg.h"
//#include "LidbgCameraUsb.h"
LIDBG_DEFINE;

//for hub
#define	FRONT_NODE		"1-1.2"	
#define	BACK_NODE		"1-1.3"

static DECLARE_COMPLETION (timer_stop_rec_wait);

static wait_queue_head_t wait_queue;
char isBackChange = 0;
char isBack = 0;
unsigned int previewCnt = 0;
char isPreview = 0;
char isFirstresume = 0;
char isSuspend = 0;
char isRec = 0;
int back_charcnt = 0,front_charcnt = 0;
char back_hub_path[256],front_hub_path[256];
unsigned char oldCamStatus = 0,camStatus = 0;
static char isDVRRec,isOnlineRec;
//struct completion rec_completion;

struct semaphore sem;
wait_queue_head_t camStatus_wait_queue;
unsigned char read_status;


#define FLY_CAM_ISVALID	0x01
#define FLY_CAM_ISSONIX	0x02
#define FLY_CAM_ISDVRUSED		0x04
#define FLY_CAM_ISONLINEUSED		0x08

static struct timer_list suspend_stoprec_timer;
#define SUSPEND_STOPREC_ONLINE_TIME   (jiffies + 180*HZ)  /* 3min */
#define SUSPEND_STOPREC_ACCOFF_TIME   (jiffies + 60*HZ)  /* 1min */

typedef enum {
  NR_BITRATE,
  NR_RESOLUTION,
  NR_PATH,
  NR_TIME,
  NR_FILENUM,
  NR_TOTALSIZE,
  NR_START_REC,
  NR_STOP_REC,
  NR_STATUS,
}cam_ctrl_t;

typedef enum {
  RET_SUCCESS,
  RET_NOTVALID,
  RET_NOTSONIX,
  RET_PAR_FAIL,
  RET_IGNORE,
  RET_REPEATREQ,
}cam_ioctl_ret_t;

typedef enum {
  RET_DEFALUT,
  RET_START,
  RET_STOP,
  RET_EXCEED_UPPER_LIMIT,
  RET_DISCONNECT,
  RET_INSUFFICIENT_SPACE_CIRC,
  RET_INSUFFICIENT_SPACE_STOP,
  RET_INIT_INSUFFICIENT_SPACE_STOP,
}cam_read_ret_t;

static int f_rec_bitrate,f_rec_time,f_rec_filenum,f_rec_totalsize;
static int f_online_bitrate,f_online_time,f_online_filenum,f_online_totalsize;
char f_rec_res[100],f_rec_path[100];
char f_online_res[100],f_online_path[100];

#define FLYCAM_FRONT_REC_IOC_MAGIC  'F'
#define FLYCAM_FRONT_ONLINE_IOC_MAGIC  'f'
#define FLYCAM_BACK_REC_IOC_MAGIC  'B'
#define FLYCAM_BACK_ONLINE_IOC_MAGIC  'b'
#define FLYCAM_STATUS_IOC_MAGIC  's'

//ioctl
#define READ_CAM_PROP(magic , nr) _IOR(magic, nr ,int) 
#define WRITE_CAM_PROP(magic , nr) _IOW(magic, nr ,int)

static int lidbg_flycam_event(struct notifier_block *this,
                       unsigned long event, void *ptr)
{
    DUMP_FUN;
	lidbg("flycam event: %ld\n", event);
    switch (event)
    {
	    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, NOTIFIER_MINOR_ACC_ON):
			lidbg("flycam event:resume %ld\n", event);
			isSuspend = 0;
			del_timer(&suspend_stoprec_timer);
			isFirstresume = 1;
			break;
	    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, NOTIFIER_MINOR_ACC_OFF):
			lidbg("flycam event:suspend %ld\n", event);
			isSuspend = 1;
			mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ACCOFF_TIME);
			break;
	    default:
	        break;
    }

    return NOTIFY_DONE;
}

static struct notifier_block lidbg_notifier =
{
    .notifier_call = lidbg_flycam_event,
};

static void suspend_stoprec_timer_isr(unsigned long data)
{
	if(isRec)
	{
	    lidbg("-------[TIMER]uvccam stop_recording -----\n");
		complete(&timer_stop_rec_wait);
	}
}


static int thread_stop_rec_func(void *data)
{
	while(1)
	{
		wait_for_completion(&timer_stop_rec_wait);
		lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
		lidbg_shell_cmd("echo 'udisk_unrequest' > /dev/flydev0");
	}
	return 0;
}

struct name_list
{
    char name[33];
    struct list_head list;
};

static int readdir_build_namelist(void *arg, const char *name, int namlen,	loff_t offset, u64 ino, unsigned int d_type)
{
    if(!(name[0] == '.' || (name[0] == '.' && name[1] == '.'))) // ignore "." and ".."
    {
        struct list_head *names = arg;
        struct name_list *entry;
        entry = kzalloc(sizeof(struct name_list), GFP_KERNEL);
        if (entry == NULL)
            return -ENOMEM;
        memcpy(entry->name, name, namlen);
        entry->name[namlen] = '\0';
        list_add(&entry->list, names);
    }
    return 0;
}

static int lidbg_checkCam(void)
{
    LIST_HEAD(names);
    struct file *dir_file, *f_dir, *b_dir;
    int status;
	char insure_is_dir[50] = "/sys/bus/usb/drivers/usb/";
	char f_videocnt = 0,b_videocnt = 0;
	unsigned char retcode = 0;
#if 0
    if(!insure_is_dir )
   	{
   		lidbg("dir err!");
		return -1;
	}
#endif
    //lidbg("insure_is_dir => %s",insure_is_dir);
	//obtain hub node path
    dir_file = filp_open(insure_is_dir, O_RDONLY | O_DIRECTORY, 0);
    if (IS_ERR(dir_file))
    {
        LIDBG_ERR("open%s,%ld\n", insure_is_dir, PTR_ERR(dir_file));
        return -1;
    }
    else
    {
        struct name_list *entry;
        int count = 0;
        //LIDBG_SUC("open:<%s,%s>\n", insure_is_dir, dir_file->f_path.dentry->d_name.name);
        status = vfs_readdir(dir_file, readdir_build_namelist, &names);
        if (dir_file)
            fput(dir_file);
        while (!list_empty(&names))
        {
            entry = list_entry(names.next, struct name_list, list);
            if (!status && entry)
            {
                count++;
				//lidbg("======5===%s==1==",entry->name);
				if((!strncmp(entry->name,  BACK_NODE , 5)) && (strlen(entry->name) >= back_charcnt) )
				{
					back_charcnt = strlen(entry->name);
					sprintf(back_hub_path, "/sys/bus/usb/drivers/usb/%s/%s:1.0/video4linux/", entry->name,entry->name);//back cam
				}
				else if((!strncmp(entry->name,  FRONT_NODE , 5)) && (strlen(entry->name) >= front_charcnt))
				{
					front_charcnt = strlen(entry->name);
					sprintf(front_hub_path, "/sys/bus/usb/drivers/usb/%s/%s:1.0/video4linux/", entry->name,entry->name);//front cam
				} 
                list_del(&entry->list);
                kfree(entry);
            }
            entry = NULL;
        }
				
		//check front Cam status:isPlugIn? isSonix?
		if(front_charcnt == 0)
		{
			//lidbg("%s: can not found front cam plug in! ", __func__ );	
		}
		else
		{
			if(IS_ERR(f_dir = filp_open(front_hub_path, O_RDONLY | O_DIRECTORY, 0)))
			{
				lidbg("%s: front_hub_path access wrong! ", __func__ );
			}
			else
			{
				status = vfs_readdir(f_dir, readdir_build_namelist, &names);
				while (!list_empty(&names))
      		    {
					entry = list_entry(names.next, struct name_list, list);
					if(!strncmp(entry->name,  "video" , 5))
					{
						f_videocnt++;
						//lidbg("f_video found");
					}
					list_del(&entry->list);
               		kfree(entry);
				}
			}
		}
		//check back Cam status:isPlugIn? isSonix?
		if(back_charcnt == 0)
		{
			//lidbg("%s: can not found back cam plug in! ", __func__ );	
		}
		else
		{
			if(IS_ERR(b_dir = filp_open(back_hub_path, O_RDONLY | O_DIRECTORY, 0)))
			{
				lidbg("%s: back_hub_path access wrong! ", __func__ );
			}
			else
			{
				status = vfs_readdir(b_dir, readdir_build_namelist, &names);
				while (!list_empty(&names))
      		    {
					entry = list_entry(names.next, struct name_list, list);
					if(!strncmp(entry->name,  "video" , 5))
					{
						b_videocnt++;
						//lidbg("b_video found");
					}
					list_del(&entry->list);
               		kfree(entry);
				}
			}
		}
		if(f_videocnt == 2)
		{
			lidbg("front isSonixCam\n");
			retcode |= FLY_CAM_ISSONIX;
			retcode |= FLY_CAM_ISVALID;
		}
		else if(f_videocnt == 1)  
		{
			lidbg("front is not SonixCam\n");
			retcode &= ~FLY_CAM_ISSONIX;
			retcode |= FLY_CAM_ISVALID;
		}
		else
		{
			lidbg("front not found\n");
			retcode &= ~FLY_CAM_ISSONIX;
			retcode &= ~FLY_CAM_ISVALID;
		}
		if(b_videocnt == 2)
		{
			lidbg("back isSonixCam\n");
			retcode |= (FLY_CAM_ISSONIX << 4);
			retcode |= (FLY_CAM_ISVALID << 4);
		}
		else if(b_videocnt == 1)
		{
			lidbg("back is not SonixCam\n");
			retcode &= (~(FLY_CAM_ISSONIX << 4));
			retcode |= (FLY_CAM_ISVALID << 4);
		}
		else
		{
			lidbg("back not found\n");
			retcode &= (~(FLY_CAM_ISSONIX << 4));
			retcode &= (~(FLY_CAM_ISVALID << 4));
		}
        return retcode;
    }
}

static int usb_nb_cam_func(struct notifier_block *nb, unsigned long action, void *data)
{
	//struct usb_device *dev = data;
	switch (action)
	{
	case USB_DEVICE_ADD:
	case USB_DEVICE_REMOVE:
		oldCamStatus = camStatus;
		camStatus = lidbg_checkCam();
		lidbg("=====Camera retcode => 0x%x======\n",camStatus);
#if 0
		sprintf(temp_cmd, "setprop fly.uvccam.retcode %d ", camStatus);
		lidbg_shell_cmd(temp_cmd);	
#endif
		/*plug out process*/
		if((oldCamStatus & FLY_CAM_ISSONIX) && !(camStatus & FLY_CAM_ISSONIX))
		{
			read_status = RET_DISCONNECT;
			wake_up_interruptible(&camStatus_wait_queue);
			isDVRRec = 0;
			isOnlineRec= 0;
		}
	    break;
	}
	return NOTIFY_OK;
}

static struct notifier_block usb_nb_cam =
{
    .notifier_call = usb_nb_cam_func,
};

static int startRec(void)
{
	lidbg("-------uvccam recording -----\n");
	if(isSuspend) mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ONLINE_TIME);
	lidbg_shell_cmd("echo 'udisk_request' > /dev/flydev0");
	//fix screen blurred issue(only in preview scene)
	if(isPreview) previewCnt++;
	if(isPreview && ((previewCnt == 1) || (previewCnt && isFirstresume)))
	{
		lidbg("======fix screen blurred issue==E=====%d",previewCnt);
		lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
		lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
		msleep(3500);
		lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
		msleep(500);
		lidbg("======fix screen blurred issue==X=====");
	}
	lidbg("previewCnt => %d ",previewCnt);
	//if(previewCnt >= 20) previewCnt = 20;
	isFirstresume = 0;
    lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
    if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
    else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
	//wait_for_completion(&rec_completion);
	//return sonixAPstatus;
	return 0;
}

static int stopRec(void)
{
	lidbg("-------uvccam stop_recording -----\n");
	if(isSuspend) del_timer(&suspend_stoprec_timer);
	lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
	msleep(500);
	lidbg_shell_cmd("echo 'udisk_unrequest' > /dev/flydev0");
	//wait_for_completion(&rec_completion);
	//return sonixAPstatus;
	return 0;
}

static long flycam_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	char ret = 0;
	char temp_cmd[256];	
	//lidbg("=====camStatus => %d======\n",camStatus);
	if(_IOC_TYPE(cmd) == FLYCAM_FRONT_REC_IOC_MAGIC)//front cam recording mode
	{
		//check camera status before doing ioctl
		if(!(camStatus & FLY_CAM_ISVALID))
		{
			lidbg("%s:DVR not found,ioctl fail!\n",__func__);
			return RET_NOTVALID;
		}
		if(!(camStatus & FLY_CAM_ISSONIX))
		{
			lidbg("%s:is not SonixCam ,ioctl fail!\n",__func__);
			return RET_NOTSONIX;
		}
		switch(_IOC_NR(cmd))
		{
			case NR_BITRATE:
		        lidbg("DVR NR_REC_BITRATE\n");
				f_rec_bitrate = arg;
				lidbg("f_rec_bitrate = %d\n",f_rec_bitrate);
				sprintf(temp_cmd, "setprop fly.uvccam.recbitrate %d", f_rec_bitrate);
				lidbg_shell_cmd(temp_cmd);
		        break;
		    case NR_RESOLUTION:
		        lidbg("DVR NR_REC_RESOLUTION === %s\n",(char*)arg);
				strcpy(f_rec_res,(char*)arg);
				lidbg("f_rec_res = %s\n",f_rec_res);
				sprintf(temp_cmd, "setprop fly.uvccam.res %s", f_rec_res);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_PATH:
		        lidbg("DVR NR_REC_PATH=== %s\n",(char*)arg);
				strcpy(f_rec_path,(char*)arg);
				lidbg("f_rec_path = %s\n",f_rec_path);
				if(IS_ERR(filp_open(f_rec_path, O_RDONLY | O_DIRECTORY, 0)))
				{
					lidbg("%s: f_rec_path access wrong! %d", __func__ ,EFAULT);
					ret = RET_PAR_FAIL;
				}
				else
				{
					sprintf(temp_cmd, "setprop fly.uvccam.recpath %s", f_rec_path);
					lidbg_shell_cmd(temp_cmd);
				}
		        break;
			case NR_TIME:
		        lidbg("DVR NR_REC_TIME\n");
				f_rec_time = arg;
				lidbg("f_rec_time = %d\n",f_rec_time);
				sprintf(temp_cmd, "setprop fly.uvccam.rectime %d", f_rec_time);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_FILENUM:
		        lidbg("DVR NR_REC_FILENUM\n");
				f_rec_filenum = arg;
				lidbg("f_rec_filenum = %d\n",f_rec_filenum);
				sprintf(temp_cmd, "setprop fly.uvccam.recnum %d", f_rec_filenum);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_TOTALSIZE:
		        lidbg("DVR NR_REC_TOTALSIZE\n");
				f_rec_totalsize= arg;
				lidbg("f_rec_totalsize = %d\n",f_rec_totalsize);
				sprintf(temp_cmd, "setprop fly.uvccam.recfilesize %d", f_rec_totalsize);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_START_REC:
		        lidbg("DVR NR_START_REC\n");
				//check occupancy of camera
				if(isDVRRec)
				{
					lidbg("====DVR start cmd repeatedly====\n");
					ret = RET_REPEATREQ;
				}
				else if(isOnlineRec) 
				{
					lidbg("====DVR restart rec===E=\n");
					isDVRRec = 1;
					stopRec();
					msleep(100);
					ret = startRec();
					lidbg("====DVR restart rec : ret =>%d===X=\n",ret);
				}
				else 
				{
					lidbg("====DVR start rec====\n");
					isDVRRec = 1;
					ret = startRec();
					lidbg("====DVR start rec : ret =>%d===X=\n",ret);
				}
		        break;
			case NR_STOP_REC:
		        lidbg("NR_STOP_REC\n");
				if(isDVRRec)
				{
					lidbg("====DVR stop rec====\n");
					ret = stopRec();
					isDVRRec = 0;
					lidbg("====DVR stop rec : ret =>%d===X=\n",ret);
				}
				else if(isOnlineRec) 
				{
					lidbg("====DVR stop cmd neglected====\n");
					ret = RET_IGNORE;
				}
				else
				{
					lidbg("====DVR stop cmd repeatedly====\n");
					ret = RET_REPEATREQ;
				}
		        break;
		    default:
		        return -ENOTTY;
		}
	}

	else if(_IOC_TYPE(cmd) == FLYCAM_FRONT_ONLINE_IOC_MAGIC)//front cam online mode
	{
		//check camera status before doing ioctl
		if(!(camStatus & FLY_CAM_ISVALID))
		{
			lidbg("%s:DVR[online] not found,ioctl fail!\n",__func__);
			return RET_NOTVALID;
		}
		if(!(camStatus & FLY_CAM_ISSONIX))
		{
			lidbg("%s:is not SonixCam ,ioctl fail!\n",__func__);
			return RET_NOTSONIX;
		}
		switch(_IOC_NR(cmd))
		{
			case NR_BITRATE:
		        lidbg("Online NR_REC_BITRATE\n");
				f_online_bitrate = arg;
				sprintf(temp_cmd, "setprop fly.uvccam.recbitrate %d", f_online_bitrate);
				lidbg_shell_cmd(temp_cmd);
		        break;
		    case NR_RESOLUTION:
		        lidbg("Online NR_REC_RESOLUTION === %s\n",(char*)arg);
				strcpy(f_online_res,(char*)arg);
				sprintf(temp_cmd, "setprop fly.uvccam.res %s", f_online_res);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_PATH:
		        lidbg("Online NR_REC_PATH=== %s\n",(char*)arg);
				strcpy(f_online_path,(char*)arg);
				if(IS_ERR(filp_open(f_online_path, O_RDONLY | O_DIRECTORY, 0)))
				{
					lidbg("%s: f_online_path access wrong! ", __func__ );
					ret = RET_PAR_FAIL;
				}
				else
				{
					sprintf(temp_cmd, "setprop fly.uvccam.recpath %s", f_online_path);
					lidbg_shell_cmd(temp_cmd);
				}
		        break;
			case NR_TIME:
		        lidbg("Online NR_REC_TIME\n");
				f_online_time = arg;
				sprintf(temp_cmd, "setprop fly.uvccam.rectime %d", f_online_time);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_FILENUM:
		        lidbg("Online NR_REC_FILENUM\n");
				f_online_filenum = arg;
				sprintf(temp_cmd, "setprop fly.uvccam.recnum %d", f_online_filenum);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_TOTALSIZE:
		        lidbg("Online NR_REC_TOTALSIZE\n");
				f_online_totalsize= arg;
				sprintf(temp_cmd, "setprop fly.uvccam.recfilesize %d", f_online_totalsize);
				lidbg_shell_cmd(temp_cmd);
		        break;
			case NR_START_REC:
		        lidbg("Online NR_START_REC\n");
				if(isDVRRec)
				{
					lidbg("====Online stop cmd neglected====\n");
					ret = RET_IGNORE;
				}
				else if(isOnlineRec) 
				{
					lidbg("====Online start cmd repeatedly====\n");
					ret = RET_REPEATREQ;
				}
				else
				{
					lidbg("====Online start rec===E=\n");
					isOnlineRec = 1;
					ret = startRec();
					lidbg("====Online start rec : ret =>%d===X=\n",ret);
				}
		        break;
			case NR_STOP_REC:
		        lidbg("Online NR_STOP_REC\n");
				if(isDVRRec)
				{
					lidbg("====Online stop cmd neglected====\n");
					ret = RET_IGNORE;
				}
				else if(isOnlineRec) 
				{
					lidbg("====Online stop rec===E=\n");
					ret = stopRec();
					isOnlineRec = 0;
					lidbg("====Online stop rec : ret =>%d===X=\n",ret);
				}
				else
				{
					lidbg("====Online stop cmd repeatedly====\n");
					ret = RET_REPEATREQ;
				}
		        break;
		    default:
		        return -ENOTTY;
		}
	}
	else if(_IOC_TYPE(cmd) == FLYCAM_STATUS_IOC_MAGIC)//rec TestAp status
	{
		switch(_IOC_NR(cmd))
		{
			case NR_STATUS:
		        lidbg("NR_STATUS\n");
				read_status = arg;
				//complete(&rec_completion);
				wake_up_interruptible(&camStatus_wait_queue);
		        break;
			default:
		        return -ENOTTY;
		}
	}
	else return -EINVAL;
    return ret;
}

static unsigned int  flycam_poll(struct file *filp, struct poll_table_struct *wait)
{
    //struct fly_KeyEncoderInfo *pflycam_Info= filp->private_data;
    unsigned int mask = 0;
    pr_debug("[knob_poll]wait begin\n");
    poll_wait(filp, &camStatus_wait_queue, wait);
    pr_debug("[knob_poll]wait done\n");
    if(read_status != RET_DEFALUT)
    {
        mask |= POLLIN | POLLRDNORM;
    }
    return mask;
}

ssize_t  flycam_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
#if 0
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
#endif
	if(read_status == RET_DEFALUT)
    {
        if(wait_event_interruptible(camStatus_wait_queue, (read_status != RET_DEFALUT)))
            return -ERESTARTSYS;
    }
	lidbg("====read_status => %d=====",read_status);
	if(copy_to_user(buffer, &read_status, 1))
    {
        return -1;
    }
	read_status = RET_DEFALUT;
	return 1;
}

int flycam_open (struct inode *inode, struct file *filp)
{
	//filp->private_data = pflycam_Info;
    return 0;
}

ssize_t flycam_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	char *cmd[30] = {NULL};//cmds array
	char *keyval[2] = {NULL};//key-vals
	char cmd_num  = 0;//cmd amount
    char cmd_buf[256];
	int i;
    memset(cmd_buf, '\0', 256);
    if(copy_from_user(cmd_buf, buf, size))
    {
        lidbg("copy_from_user ERR\n");
    }
    if(cmd_buf[size - 1] == '\n')
		cmd_buf[size - 1] = '\0';
    cmd_num = lidbg_token_string(cmd_buf, " ", cmd) ;
	lidbg("-----cmd_buf------------------[%s]---\n", cmd_buf);
	lidbg("-----cmd_num------------[%d]---\n", cmd_num);
	for(i = 0;i < cmd_num; i++)
	{
		lidbg_token_string(cmd[i], "=", keyval) ;
		if(!strcmp(keyval[0], "record") )
		{
			if(!strncmp(keyval[1], "1", 1))//start
			{
			    lidbg("-------uvccam recording -----");
				isRec = 1;
				if(isSuspend) mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ONLINE_TIME);
				lidbg_shell_cmd("echo 'udisk_request' > /dev/flydev0");
				//fix screen blurred issue(only in preview scene)
				if(isPreview) previewCnt++;
				if(isPreview && ((previewCnt == 1) || (previewCnt && isFirstresume)))
				{
					lidbg("======fix screen blurred issue==E=====%d",previewCnt);
					lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
					lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
					msleep(3500);
					lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
					msleep(500);
					lidbg("======fix screen blurred issue==X=====");
				}
				lidbg("previewCnt => %d ",previewCnt);
				//if(previewCnt >= 20) previewCnt = 20;
				isFirstresume = 0;
			    lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
			    if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
			    else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
			}
			else if(!strncmp(keyval[1], "0", 1))//stop
			{
				lidbg("-------uvccam stop_recording -----");
				isRec = 0;
				if(isSuspend) del_timer(&suspend_stoprec_timer);
				lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
				msleep(500);
				lidbg_shell_cmd("echo 'udisk_unrequest' > /dev/flydev0");
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
		else if(!strcmp(keyval[0], "res") )
		{
			isPreview = 0;//defalut:DVR
			if(!strncmp(keyval[1], "1080", 4))
			{
				lidbg_shell_cmd("setprop fly.uvccam.res 1080");
			}
			else if(!strncmp(keyval[1], "720", 3))
			{
				lidbg_shell_cmd("setprop fly.uvccam.res 720");
			}
			else if(!strncmp(keyval[1], "640x360", 7))
			{
				lidbg_shell_cmd("setprop fly.uvccam.res 640x360");
				isPreview = 1;//preview
			}
			else
			{
				lidbg("-------res wrong arg:%s-----",keyval[1]);
			}
		}
		else if(!strcmp(keyval[0], "rectime") )
		{
			int rectimeVal;
			char temp_cmd[256];
			rectimeVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("rectimeVal = %d",rectimeVal);
			sprintf(temp_cmd, "setprop fly.uvccam.rectime %d", rectimeVal);
			lidbg_shell_cmd(temp_cmd);
		}
		else if(!strcmp(keyval[0], "recbitrate") )
		{
			int recbitrateVal;
			char temp_cmd[256];
			recbitrateVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recbitrateVal = %d",recbitrateVal);
			//sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video2  --xuset-br %d", recbitrateVal);
			sprintf(temp_cmd, "setprop fly.uvccam.recbitrate %d", recbitrateVal);
			lidbg_shell_cmd(temp_cmd);
		}
		#if 1
		else if(!strcmp(keyval[0], "recnum") )
		{
			int recnumVal;
			char temp_cmd[256];
			recnumVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recnumVal = %d",recnumVal);
			sprintf(temp_cmd, "setprop fly.uvccam.recnum %d", recnumVal);
			lidbg_shell_cmd(temp_cmd);
		}
		#endif
		else if(!strcmp(keyval[0], "recpath") )
		{
			//int recpathVal;
			char temp_cmd[256];
			//recpathVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recpathVal = %s",keyval[1]);
			sprintf(temp_cmd, "setprop fly.uvccam.recpath %s", keyval[1]);
			lidbg_shell_cmd(temp_cmd);
		}
		else if(!strcmp(keyval[0], "recfilesize") )
		{
			int recfilesizeVal;
			char temp_cmd[256];
			recfilesizeVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recfilesizeVal = %d",recfilesizeVal);
			sprintf(temp_cmd, "setprop fly.uvccam.recfilesize %d", recfilesizeVal);
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
    .unlocked_ioctl = flycam_ioctl,
    .poll = flycam_poll,
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


static int flycam_ops_suspend(struct device *dev)
{
    lidbg("-----------flycam_ops_suspend------------\n");
    DUMP_FUN;
	
    return 0;
}

static int flycam_ops_resume(struct device *dev)
{

    lidbg("-----------flycam_ops_resume------------\n");
    DUMP_FUN;
	//lidbg("g_var.acc_flag => %d",g_var.acc_flag);
#if 0
	if(g_var.acc_flag == FLY_ACC_ON)
	{
		isFirstresume = 1;
	}
#endif
    return 0;
}

static struct dev_pm_ops flycam_ops =
{
    .suspend	= flycam_ops_suspend,
    .resume	= flycam_ops_resume,
};

static int flycam_probe(struct platform_device *pdev)
{
    return 0;
}

static int flycam_remove(struct platform_device *pdev)
{
    return 0;
}

static struct platform_device flycam_devices =
{
    .name			= "lidbg_flycam",
    .id 			= 0,
};

static struct platform_driver flycam_driver =
{
    .probe = flycam_probe,
    .remove = flycam_remove,
    .driver = 	{
        .name = "lidbg_flycam",
        .owner = THIS_MODULE,
        .pm = &flycam_ops,
    },
};

int thread_flycam_init(void *data)
{
    lidbg_new_cdev(&flycam_nod_fops, "lidbg_flycam");
	lidbg("%s:------------start------------\n",__func__);
#if 0
	SOC_IO_Input(BACK_DET, BACK_DET, GPIO_CFG_PULL_UP);
	SOC_IO_ISR_Add(BACK_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING , irq_back_det, NULL);
#endif
/*
	pflycam_Info= (struct flycam_Info *)kmalloc( sizeof(struct flycam_Info), GFP_KERNEL );
    if (pflycam_Info == NULL)
    {
        lidbg("flycam mem err\n");
        return -ENOMEM;
    }
*/
	init_waitqueue_head(&wait_queue);
	register_lidbg_notifier(&lidbg_notifier);
	usb_register_notify(&usb_nb_cam);
	init_timer(&suspend_stoprec_timer);
    suspend_stoprec_timer.data = 0;
    suspend_stoprec_timer.expires = 0;
    suspend_stoprec_timer.function = suspend_stoprec_timer_isr;
	//init_completion(&rec_completion);
	init_waitqueue_head(&camStatus_wait_queue);
    //sema_init(&pflycam_Info->sem, 1);
    //kfifo_init(&knob_data_fifo, knob_fifo_buffer, FIFO_SIZE);
#if 0
	//CREATE_KTHREAD(thread_flycam_test, NULL);
	/*
    if((!g_var.is_fly) && (g_var.recovery_mode == 0)))
    {
       
    }
	*/
#endif
    return 0;
}

static __init int lidbg_flycam_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;

    CREATE_KTHREAD(thread_flycam_init, NULL);
	CREATE_KTHREAD(thread_stop_rec_func, NULL);
	platform_device_register(&flycam_devices);
    platform_driver_register(&flycam_driver);
    return 0;

}
static void __exit lidbg_flycam_deinit(void)
{
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_flycam_init);
module_exit(lidbg_flycam_deinit);


