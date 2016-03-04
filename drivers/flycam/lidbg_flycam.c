
#include "lidbg.h"
#include "lidbg_flycam_par.h"
//#include "LidbgCameraUsb.h"
LIDBG_DEFINE;

static void work_DVR_fixScreenBlurred(struct work_struct *work);
static void work_RearView_fixScreenBlurred(struct work_struct *work);
static int start_rec(char cam_id,char isPowerCtl);
static int stop_rec(char cam_id,char isPowerCtl);

/*camStatus mask*/
#define FLY_CAM_ISVALID	0x01
#define FLY_CAM_ISSONIX	0x02
//#define FLY_CAM_ISDVRUSED		0x04
//#define FLY_CAM_ISONLINEUSED		0x08

struct fly_UsbCamInfo
{
	unsigned char camStatus;/*Camera status(DVR&RearView)*/
	unsigned char read_status;/*Camera status for HAL to read(notify poll)*/
	wait_queue_head_t camStatus_wait_queue;/*notify wait queue*/
	struct semaphore sem;
};

struct fly_UsbCamInfo *pfly_UsbCamInfo;
static DECLARE_DELAYED_WORK(work_t_DVR_fixScreenBlurred, work_DVR_fixScreenBlurred);
static DECLARE_DELAYED_WORK(work_t_RearView_fixScreenBlurred, work_RearView_fixScreenBlurred);
static DECLARE_COMPLETION (timer_stop_rec_wait);
static DECLARE_COMPLETION (Rear_ready_wait);
static DECLARE_COMPLETION (DVR_ready_wait);

/*Camera DVR & Online recording parameters*/
static int f_rec_bitrate = 8000000,f_rec_time = 300,f_rec_filenum = 5,f_rec_totalsize = 4096;
static int f_online_bitrate = 500000,f_online_time = 60,f_online_filenum = 1,f_online_totalsize = 500;
char f_rec_res[100] = "1280x720",f_rec_path[100] = "/storage/sdcard1/camera_rec/";
char f_online_res[100] = "480x272",f_online_path[100] = "/storage/sdcard0/preview_cache/";

static struct timer_list suspend_stoprec_timer;
#define SUSPEND_STOPREC_ONLINE_TIME   (jiffies + 180*HZ)  /* 3min stop Rec after online*/
#define SUSPEND_STOPREC_ACCOFF_TIME   (jiffies + 60*HZ)  /* 1min stop Rec after accoff*/

/*bool var*/
static char isDVRRec,isOnlineRec,isDVRFirstInit,isRearViewFirstInit;
static char isSuspend,isDVRAfterFix,isRearViewAfterFix,isDVRFirstResume,isRearFirstResume;

//struct work_struct work_t_fixScreenBlurred;

#define HAL_BUF_SIZE (512)
u8 *camStatus_data_for_hal;

#define FIFO_SIZE (512)
u8 *camStatus_fifo_buffer;
static struct kfifo camStatus_data_fifo;


#if 0
static wait_queue_head_t wait_queue;
char isBackChange = 0;
char isBack = 0;
unsigned int previewCnt = 0;
char isFirstresume = 0;
char isRec = 0;

struct work_struct work_t_start_rec;
struct work_struct work_t_stop_rec;

//ioctl
#define READ_CAM_PROP(magic , nr) _IOR(magic, nr ,int) 
#define WRITE_CAM_PROP(magic , nr) _IOW(magic, nr ,int)
#endif

/******************************************************************************
 * Function: status_fifo_in
 * Description: Put status in fifo(for HAL),
 * 				For poll/read (except RET_DEFALUT,RET_START,RET_STOP , handles on our own).
 * Input parameters:
 *   status             - camera status
 * Return values:
 *      none
 * Notes: none
 *****************************************************************************/
static void status_fifo_in(unsigned char status)
{
	pfly_UsbCamInfo->read_status = status;
	if(status == RET_DEFALUT || status == RET_START || status == RET_STOP)
	{
		lidbg("%s:====receive msg => %d====\n",__func__,status);
		wake_up_interruptible(&pfly_UsbCamInfo->camStatus_wait_queue);
		return;
	}
	lidbg("%s:====fifo in => %d====\n",__func__,status);
	down(&pfly_UsbCamInfo->sem);
	//if(kfifo_is_full(&camStatus_data_fifo));
	kfifo_in(&camStatus_data_fifo, &pfly_UsbCamInfo->read_status, 1);
	up(&pfly_UsbCamInfo->sem);
	wake_up_interruptible(&pfly_UsbCamInfo->camStatus_wait_queue);
	return;
}

/******************************************************************************
 * Function: lidbg_flycam_event
 * Description: ACCON/ACCOFF event callback function
 * Input parameters:
 *   this             - notifier_block itself
 *   event          - event that triggered
 *   ptr              - private data
 * Return values:
 *      
 * Notes: none
 *****************************************************************************/
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
			isDVRFirstResume = 1;
			isRearFirstResume = 1;
			schedule_delayed_work(&work_t_RearView_fixScreenBlurred, 0);/*Rec Block mode(First ACCON)*/
			schedule_delayed_work(&work_t_DVR_fixScreenBlurred, 0);/*Rec Block mode(First ACCON)*/
			break;
	    case NOTIFIER_VALUE(NOTIFIER_MAJOR_SYSTEM_STATUS_CHANGE, NOTIFIER_MINOR_ACC_OFF):
			lidbg("flycam event:suspend %ld\n", event);
			isSuspend = 1;
			mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ACCOFF_TIME);
			isDVRFirstResume = 0;
			isRearFirstResume = 0;
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


/******************************************************************************
 * Function: suspend_stoprec_timer_isr
 * Description: Suspend timeout stop Rec ISR function
 * Input parameters:
 *   data             - private data
 * Return values:
 *      none
 * Notes: none
 *****************************************************************************/
static void suspend_stoprec_timer_isr(unsigned long data)
{
	if(isDVRRec | isOnlineRec)
	{
	    lidbg("-------[TIMER]uvccam stop_recording -----\n");
		complete(&timer_stop_rec_wait);
	}
}
/*can not block in ISR function*/
static int thread_stop_rec_func(void *data)
{
	while(1)
	{
		wait_for_completion(&timer_stop_rec_wait);
		if(stop_rec(DVR_ID,1))lidbg("%s:====return fail====\n",__func__);
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

/******************************************************************************
 * Function: lidbg_checkCam
 * Description: Check Camera status
 * Input parameters:
 *   data             - private data
 * Return values:
 *      Camera status mask value.(FLY_CAM_ISVALID & FLY_CAM_ISSONIX.)
 * Notes: none
 *****************************************************************************/
static int lidbg_checkCam(void)
{
    LIST_HEAD(names);
	int back_charcnt = 0,front_charcnt = 0;
	char back_hub_path[256],front_hub_path[256];
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
		filp_close(dir_file, 0);
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
				pr_debug("%s: front_hub_path access wrong!\n ", __func__ );
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
				filp_close(f_dir, 0);
			}
		}
		//check back Cam status:isPlugIn? isSonix?
		if(back_charcnt == 0)
		{
			pr_debug("%s: can not found back cam plug in!\n ", __func__ );	
		}
		else
		{
			if(IS_ERR(b_dir = filp_open(back_hub_path, O_RDONLY | O_DIRECTORY, 0)))
			{
				pr_debug("%s: back_hub_path access wrong! \n", __func__ );
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
				filp_close(b_dir, 0);
			}
		}
		if(f_videocnt == 2)
		{
			lidbg("%s: front isSonixCam\n", __func__ );
			retcode |= FLY_CAM_ISSONIX;
			retcode |= FLY_CAM_ISVALID;
		}
		else if(f_videocnt == 1)  
		{
			lidbg("%s: front is not SonixCam\n", __func__ );
			retcode &= ~FLY_CAM_ISSONIX;
			retcode |= FLY_CAM_ISVALID;
		}
		else
		{
			lidbg("%s: front not found\n", __func__ );
			retcode &= ~FLY_CAM_ISSONIX;
			retcode &= ~FLY_CAM_ISVALID;
		}
		if(b_videocnt == 2)
		{
			lidbg("%s: back isSonixCam\n", __func__ );
			retcode |= (FLY_CAM_ISSONIX << 4);
			retcode |= (FLY_CAM_ISVALID << 4);
		}
		else if(b_videocnt == 1)
		{
			lidbg("%s: back is not SonixCam\n", __func__ );
			retcode &= (~(FLY_CAM_ISSONIX << 4));
			retcode |= (FLY_CAM_ISVALID << 4);
		}
		else
		{
			lidbg("%s: back not found\n", __func__ );
			retcode &= (~(FLY_CAM_ISSONIX << 4));
			retcode &= (~(FLY_CAM_ISVALID << 4));
		}
        return retcode;
    }
}

/******************************************************************************
 * Function: usb_nb_cam_func
 * Description: USB event callback function
 * Input parameters:
 *   this             - notifier_block itself
 *   event          - event that triggered
 *   ptr              - private data
 * Return values:
 *      
 * Notes: none
 *****************************************************************************/
static int usb_nb_cam_func(struct notifier_block *nb, unsigned long action, void *data)
{
	unsigned char oldCamStatus = 0;
	//struct usb_device *dev = data;
	switch (action)
	{
	case USB_DEVICE_ADD:
	case USB_DEVICE_REMOVE:
		oldCamStatus = pfly_UsbCamInfo->camStatus;
		pfly_UsbCamInfo->camStatus = lidbg_checkCam();
		lidbg("=====Camera retcode => 0x%x======\n",pfly_UsbCamInfo->camStatus);
		/*usb camera plug out :notify RET_DISCONNECT*/
		if((oldCamStatus & FLY_CAM_ISVALID) && !(pfly_UsbCamInfo->camStatus & FLY_CAM_ISVALID))
		{
			status_fifo_in(RET_DISCONNECT);
			isDVRRec = 0;
			isOnlineRec= 0;
			isDVRAfterFix = 0;
		}
		if(((oldCamStatus>>4) & FLY_CAM_ISVALID) && !((pfly_UsbCamInfo->camStatus>>4) & FLY_CAM_ISVALID))
			isRearViewAfterFix = 0;
		/*
			usb camera first plug in:
			Sonix:fix ScreenBlurred issue & start exposure adaptation & notify RET_SONIX;
			Not Sonix:notify RET_NOT_SONIX.
		*/
		if(!isSuspend && (g_var.recovery_mode == 0))
		{
			/*RearView*/
			if(!((oldCamStatus>>4) & FLY_CAM_ISVALID) && ((pfly_UsbCamInfo->camStatus>>4) & FLY_CAM_ISSONIX) && !isRearViewFirstInit)
			{
				//if(isRearViewFirstInit) schedule_delayed_work(&work_t_RearView_fixScreenBlurred, 11);
				//else schedule_delayed_work(&work_t_RearView_fixScreenBlurred, 0);
#if 0
				if(isRearFirstResume) complete(&Rear_ready_wait);
				else schedule_delayed_work(&work_t_RearView_fixScreenBlurred, 0);	
#endif
				complete(&Rear_ready_wait);
				if(!isRearFirstResume) schedule_delayed_work(&work_t_RearView_fixScreenBlurred, 0);
			}	
			/*DVR*/
			if(!(oldCamStatus & FLY_CAM_ISVALID) && (pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX) && !isDVRFirstInit)
			{
				//if(isDVRFirstInit) schedule_delayed_work(&work_t_DVR_fixScreenBlurred, 10);
				//else schedule_delayed_work(&work_t_DVR_fixScreenBlurred, 0);
#if 0
				if(isDVRFirstResume) complete(&DVR_ready_wait);
  			    else schedule_delayed_work(&work_t_DVR_fixScreenBlurred, 0);
#endif
				complete(&DVR_ready_wait);
				if(!isDVRFirstResume) schedule_delayed_work(&work_t_DVR_fixScreenBlurred, 0);
			}
			else if(!(oldCamStatus & FLY_CAM_ISVALID) && !(pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX) &&(pfly_UsbCamInfo->camStatus & FLY_CAM_ISVALID) )
				status_fifo_in(RET_NOT_SONIX);
		}
		
	    break;
	}
	return NOTIFY_OK;
}

static struct notifier_block usb_nb_cam =
{
    .notifier_call = usb_nb_cam_func,
};

/******************************************************************************
 * Function: start_rec
 * Description: start sonix AP recording
 * Input parameters:
 *   	cam_id	-	camera ID (0 : RearView , 1: DVR)
 * Return values:
 *		0	-	start recording success
 *   	1	-	await sonix AP return timeout
 * Notes: none
 *****************************************************************************/
static int start_rec(char cam_id,char isPowerCtl)
{
	char temp_cmd[256];	
	char cam_id_mode = cam_id;
	lidbg("%s:====E====\n",__func__);
	pfly_UsbCamInfo->read_status = RET_DEFALUT;
	if(isSuspend) mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ONLINE_TIME);
	if(isPowerCtl) lidbg_shell_cmd("echo 'udisk_request' > /dev/flydev0");//don't control HUB power in rearview
    if(cam_id == DVR_ID)
		lidbg_shell_cmd("setprop fly.uvccam.dvr.recording 1");
	else if(cam_id == REARVIEW_ID)
		lidbg_shell_cmd("setprop fly.uvccam.rearview.recording 1");
	if(((isDVRFirstResume) && (cam_id == DVR_ID)) || ((isRearFirstResume) && (cam_id == REARVIEW_ID)))
		cam_id_mode += 2;/*Rec Block mode(First ACCON)*/
	sprintf(temp_cmd, "./flysystem/lib/out/lidbg_testuvccam /dev/video2 -b %d -c -f H264 -r &", cam_id_mode);
 	lidbg_shell_cmd(temp_cmd);
	/*don't wait status pop in Rearview*/
	if(cam_id == DVR_ID)
	{
		if(!wait_event_interruptible_timeout(pfly_UsbCamInfo->camStatus_wait_queue, (pfly_UsbCamInfo->read_status == RET_START), 6*HZ))
		{
			lidbg("%s:====read_status wait timeout => %d====\n",__func__,pfly_UsbCamInfo->read_status);
			return 1; 
		}
	}
	lidbg("%s:====X====\n",__func__);
	return 0;
}

/******************************************************************************
 * Function: stop_rec
 * Description: stop sonix AP recording
 * Input parameters:
 *  	cam_id	-	camera ID (0 : RearView , 1: DVR)
 * Return values:
 *		0	-	stop recording success
 *   	1	-	await sonix AP return timeout
 * Notes: none
 *****************************************************************************/
static int stop_rec(char cam_id,char isPowerCtl)
{
	char ret = 0;
	lidbg("%s:====E====\n",__func__);
	pfly_UsbCamInfo->read_status = RET_DEFALUT;
	if(isSuspend) del_timer(&suspend_stoprec_timer);
	if(cam_id == DVR_ID)
		lidbg_shell_cmd("setprop fly.uvccam.dvr.recording 0");
	else if(cam_id == REARVIEW_ID)
		lidbg_shell_cmd("setprop fly.uvccam.rearview.recording 0");
	//msleep(500);
	/*don't wait status pop in Rearview*/
	if(cam_id == DVR_ID)
	{
		if(!wait_event_interruptible_timeout(pfly_UsbCamInfo->camStatus_wait_queue, (pfly_UsbCamInfo->read_status == RET_STOP), 3*HZ))
		{
			lidbg("%s:====read_status wait timeout => %d====\n",__func__,pfly_UsbCamInfo->read_status);
			ret = 1; 
		}
		if(isPowerCtl) lidbg_shell_cmd("echo 'udisk_unrequest' > /dev/flydev0");//don't control HUB power in rearview
	}
	lidbg("%s:====X====\n",__func__);
	return ret;
}

/******************************************************************************
 * Function: fixScreenBlurred
 * Description: Fix ScreenBlurred issue & Start exposure adaptation when camera first plug in.
 * Input parameters:
 *   	cam_id	-	camera ID (0 : RearView , 1: DVR)
 * Return values:
 *		none
 * Notes: none
 *****************************************************************************/
static void fixScreenBlurred(char cam_id , char isOnline)
{
	if(cam_id == DVR_ID)
	{
		lidbg_shell_cmd("setprop fly.uvccam.dvr.res 640x360");
		lidbg_shell_cmd("setprop fly.uvccam.dvr.recpath /storage/sdcard0/camera_rec/");
		if(start_rec(cam_id,1))lidbg("%s:====return fail====\n",__func__);
		if(isOnline) 
		{
			msleep(3000);
			if(stop_rec(cam_id,0))lidbg("%s:====return fail====\n",__func__);
		}
		else 
		{
			msleep(3500);
			if(stop_rec(cam_id,1))lidbg("%s:====return fail====\n",__func__);
		}
		lidbg_shell_cmd("setprop fly.uvccam.dvr.res 1280x720");
		lidbg_shell_cmd("rm -f /storage/sdcard0/camera_rec/tmp*.h264&");
	}
	else if(cam_id == REARVIEW_ID)
	{
		lidbg_shell_cmd("setprop fly.uvccam.rearview.res 640x360");
		lidbg_shell_cmd("setprop fly.uvccam.rearview.recpath /storage/sdcard0/");//must different path
		if(start_rec(cam_id,1))lidbg("%s:====return fail====\n",__func__);
		if(isOnline) 
		{
			msleep(2000);
			if(stop_rec(cam_id,0))lidbg("%s:====return fail====\n",__func__);
		}
		else 
		{
			msleep(5000);
			if(stop_rec(cam_id,1))lidbg("%s:====return fail====\n",__func__);
		}
		lidbg_shell_cmd("setprop fly.uvccam.rearview.res 1280x720");
		lidbg_shell_cmd("rm -f /storage/sdcard0/tmp*.h264&");
	}
	return;
}

/******************************************************************************
 * Function: work_DVR_fixScreenBlurred
 * Description: DVR Fix ScreenBlurred issue & Start exposure adaptation when camera first plug in.
 * Input parameters:
 *   	work	-	kernel work struct
 * Return values:
 *		none
 * Notes: none
 *****************************************************************************/
static void work_DVR_fixScreenBlurred(struct work_struct *work)
{
	lidbg("%s:====E====\n",__func__);
	//if(isSuspend) mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ONLINE_TIME);
	if(isDVRFirstInit)
	{
		pfly_UsbCamInfo->camStatus = lidbg_checkCam();
		if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX))
		{
			lidbg("%s:====None camera found!====\n",__func__);
			isDVRFirstInit = 0;/*no matter what,let the next work continue*/
			isDVRAfterFix = 1;
			return;
		}
	}
	fixScreenBlurred(DVR_ID,0);
	isDVRFirstInit = 0;
	isDVRAfterFix = 1;
	isDVRFirstResume = 0;
	status_fifo_in(RET_SONIX);
	lidbg("%s:====X====\n",__func__);
	return;
}

/******************************************************************************
 * Function: work_RearView_fixScreenBlurred
 * Description: DVR Fix ScreenBlurred issue & Start exposure adaptation when camera first plug in.
 * Input parameters:
 *   	work	-	kernel work struct
 * Return values:
 *		none
 * Notes: none
 *****************************************************************************/
static void work_RearView_fixScreenBlurred(struct work_struct *work)
{
	lidbg("%s:====E====\n",__func__);
	if(isRearViewFirstInit)
	{
		pfly_UsbCamInfo->camStatus = lidbg_checkCam();
		if(!((pfly_UsbCamInfo->camStatus>>4) & FLY_CAM_ISSONIX))
		{
			lidbg("%s:====None camera found!====\n",__func__);
			isRearViewFirstInit = 0;/*no matter what,let the next work continue*/
			isRearViewAfterFix = 1;
			return;
		}
	}
	fixScreenBlurred(REARVIEW_ID,0);
	isRearViewFirstInit = 0;
	isRearViewAfterFix = 1;
	isRearFirstResume = 0;
	lidbg("%s:====X====\n",__func__);
	return;
}
#if 0
static void work_startRec(struct work_struct *work)
{
	lidbg("%s:====E====\n",__func__);
	if(isSuspend) mod_timer(&suspend_stoprec_timer,SUSPEND_STOPREC_ONLINE_TIME);
	if(start_rec(DVR_ID))lidbg("%s:====return fail====\n",__func__);
	lidbg("%s:====X====\n",__func__);
	return;
}

static void work_stopRec(struct work_struct *work)
{
	lidbg("%s:====E====\n",__func__);
	if(isSuspend) del_timer(&suspend_stoprec_timer);
	if(stop_rec())lidbg("%s:====return fail====\n",__func__);
	lidbg("%s:====X====\n",__func__);
	return;
}
#endif

/******************************************************************************
 * Function: setDVRProp
 * Description: Set DVR property for recording.
 * Input parameters:
 *   	camID	-	camera ID (0 : RearView , 1: DVR)
 * Return values:
 *		none
 * Notes: none
 *****************************************************************************/
static void setDVRProp(int camID)
{
	char temp_cmd[256];	
	if(camID == DVR_ID)
	{
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recbitrate %d", f_rec_bitrate);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.res %s", f_rec_res);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recpath %s", f_rec_path);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.rectime %d", f_rec_time);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recnum %d", f_rec_filenum);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recfilesize %d", f_rec_totalsize);
		lidbg_shell_cmd(temp_cmd);
	}
	else if(camID == REARVIEW_ID)
	{
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recbitrate %d", f_rec_bitrate);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.res %s", f_rec_res);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recpath %s", f_rec_path);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.rectime %d", f_rec_time);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recnum %d", f_rec_filenum);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recfilesize %d", f_rec_totalsize);
		lidbg_shell_cmd(temp_cmd);
	}
}

/******************************************************************************
 * Function: setOnlineProp
 * Description: Set Online property for recording.
 * Input parameters:
 *   	camID	-	camera ID (0 : RearView , 1: DVR)
 * Return values:
 *		none
 * Notes: none
 *****************************************************************************/
static void setOnlineProp(int camID)
{
	char temp_cmd[256];	
	if(camID == DVR_ID)
	{
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recbitrate %d", f_online_bitrate);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.res %s", f_online_res);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recpath %s", f_online_path);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.rectime %d", f_online_time);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recnum %d", f_online_filenum);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.dvr.recfilesize %d", f_online_totalsize);
		lidbg_shell_cmd(temp_cmd);
	}
	else if(camID == REARVIEW_ID)
	{
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recbitrate %d", f_online_bitrate);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.res %s", f_online_res);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recpath %s", f_online_path);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.rectime %d", f_online_time);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recnum %d", f_online_filenum);
		lidbg_shell_cmd(temp_cmd);
		sprintf(temp_cmd, "setprop fly.uvccam.rearview.recfilesize %d", f_online_totalsize);
		lidbg_shell_cmd(temp_cmd);
	}
}

/******************************************************************************
 * Function: checkSDCardStatus
 * Description: Check SDCard status & create file path.
 * Input parameters:
 *   	path	-	Record video files save path.
 * Return values:
 *		0	-	File path it's exist.
 *   	1	-	EMMC it's not exist.(unusual case)
 *   	2	-	SDCARD it's not exist.Change to EMMC default path.
 *   	3	-	Storage device is OK but the path you specify it's not exist,so create one.
 * Notes:  1.Check storage whether it is valid;
 *				2.Create file path if it is not exist.
 *****************************************************************************/
static int checkSDCardStatus(char *path)
{
	char temp_cmd[256];	
	int ret;
	struct file *storage_path, *file_path;
	if(!strncmp(path, "/storage/sdcard0", 16))
	{
		storage_path = filp_open("/storage/sdcard0", O_RDONLY | O_DIRECTORY, 0);
		file_path = filp_open(path, O_RDONLY | O_DIRECTORY, 0);
		if(IS_ERR(storage_path))
		{
			lidbg("%s:EMMC ERR!!\n",__func__);
			ret = 1;
		}
		else if(IS_ERR(file_path))
		{
			lidbg("%s: New Rec Dir => %s\n",__func__,path);
			sprintf(temp_cmd, "mkdir -p %s", path);
			lidbg_shell_cmd(temp_cmd);
			ret = 3;
		}
		else lidbg("%s: Check Rec Dir OK => %s\n",__func__,path);
	}
	else if(!strncmp(path, "/storage/sdcard1", 16))
	{
		storage_path = filp_open("/storage/sdcard1", O_RDONLY | O_DIRECTORY, 0);
		file_path = filp_open(path, O_RDONLY | O_DIRECTORY, 0);
		if(IS_ERR(storage_path))
		{
			lidbg("%s:SDCARD1 ERR!!Reset to /storage/sdcard0/camera_rec/\n",__func__);
			strcpy(path,"/storage/sdcard0/camera_rec/");
			ret = 2;
		}
		else if(IS_ERR(file_path))
		{
			lidbg("%s: New Rec Dir => %s\n",__func__,path);
			sprintf(temp_cmd, "mkdir -p %s", path);
			lidbg_shell_cmd(temp_cmd);
			ret = 3;
		}
		else lidbg("%s: Check Rec Dir OK => %s\n",__func__,path);
	}
	if(!IS_ERR(storage_path)) filp_close(storage_path, 0);
	if(!IS_ERR(file_path)) filp_close(file_path, 0);
	return ret;
}


/******************************************************************************
 * Function: flycam_ioctl
 * Description: Flycam ioctl function.
 * Input parameters:
 *   	filp	-	file struct
 *   	cmd	-	Please refer to  lidbg_flycam_par.h magic code.
 *   	arg	-	arguments
 * Return values:
 *		Please refer to  lidbg_flycam_par.h -> cam_ioctl_ret_t.
 * Notes: 1.Check camera whether it is valid and sonix.
 				2.Get through the judgment and save args & start/stop recording.
 *****************************************************************************/
static long flycam_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	char ret = 0;
	unsigned char ret_st = 0;
	//lidbg("=====camStatus => %d======\n",pfly_UsbCamInfo->camStatus);
	if(_IOC_TYPE(cmd) == FLYCAM_FRONT_REC_IOC_MAGIC)//front cam recording mode
	{
		/*check camera status before doing ioctl*/
		if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISVALID))
		{
			lidbg("%s:DVR not found,ioctl fail!\n",__func__);
			return RET_NOTVALID;
		}
		else if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX))
		{
			lidbg("%s:Is not SonixCam ,ioctl fail!\n",__func__);
			return RET_NOTSONIX;
		}
		else if((pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX) && !isDVRAfterFix)
		{
			lidbg("%s:Fix proc running!Please wait !\n",__func__);
			return RET_NOTVALID;
		}
		
		switch(_IOC_NR(cmd))
		{
			case NR_BITRATE:
				lidbg("%s:DVR NR_REC_BITRATE = [%ld]\n",__func__,arg);
				f_rec_bitrate = arg;
		        break;
		    case NR_RESOLUTION:
				lidbg("%s:DVR NR_REC_RESOLUTION  = [%s]\n",__func__,(char*)arg);
				strcpy(f_rec_res,(char*)arg);
		        break;
			case NR_PATH:
				lidbg("%s:DVR NR_REC_PATH  = [%s]\n",__func__,(char*)arg);
				ret_st = checkSDCardStatus((char*)arg);
				if(ret_st != 1) 
					strcpy(f_rec_path,(char*)arg);
				else
					lidbg("%s: f_rec_path access wrong! %d", __func__ ,EFAULT);//not happend
				if(ret_st > 0) ret = RET_FAIL;
		        break;
			case NR_TIME:
				lidbg("%s:DVR NR_REC_TIME = [%ld]\n",__func__,arg);
				f_rec_time = arg;
		        break;
			case NR_FILENUM:
				lidbg("%s:DVR NR_REC_FILENUM = [%ld]\n",__func__,arg);
				f_rec_filenum = arg;
		        break;
			case NR_TOTALSIZE:
				lidbg("%s:DVR NR_REC_TOTALSIZE = [%ld]\n",__func__,arg);
				f_rec_totalsize= arg;
		        break;
			case NR_START_REC:
		        lidbg("%s:DVR NR_START_REC\n",__func__);
				setDVRProp(DVR_ID);
				checkSDCardStatus(f_rec_path);
				if(isDVRRec)
				{
					lidbg("%s:====DVR start cmd repeatedly====\n",__func__);
					ret = RET_REPEATREQ;
				}
				else if(isOnlineRec) 
				{
					lidbg("%s:====DVR restart rec====\n",__func__);
					isDVRRec = 1;
					if(stop_rec(DVR_ID,1)) goto failproc;
					if(start_rec(DVR_ID,1)) goto failproc;
				}
				else 
				{
					lidbg("%s:====DVR start rec====\n",__func__);
					isDVRRec = 1;
					if(start_rec(DVR_ID,1)) goto failproc;
				}
		        break;
			case NR_STOP_REC:
		        lidbg("%s:DVR NR_STOP_REC\n",__func__);
				if(isDVRRec)
				{
					lidbg("%s:====DVR stop rec====\n",__func__);
					if(stop_rec(DVR_ID,1)) goto failproc;
					isDVRRec = 0;
				}
				else if(isOnlineRec) 
				{
					lidbg("%s:====DVR stop cmd neglected====\n",__func__);
					ret = RET_IGNORE;
				}
				else
				{
					lidbg("%s:====DVR stop cmd repeatedly====\n",__func__);
					ret = RET_REPEATREQ;
				}
		        break;
			case NR_SET_PAR:
				lidbg("%s:DVR NR_SET_PAR\n",__func__);
				if(isDVRRec)
				{
					if(stop_rec(DVR_ID,1)) goto failproc;
					setDVRProp(DVR_ID);
					if(start_rec(DVR_ID,1)) goto failproc;
				}
		        break;
		    default:
		        return -ENOTTY;
		}
	}
	else if(_IOC_TYPE(cmd) == FLYCAM_FRONT_ONLINE_IOC_MAGIC)//front cam online mode
	{
		/*check camera status before doing ioctl*/
		if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISVALID))
		{
			lidbg("%s:DVR[online] not found,ioctl fail!\n",__func__);
			return RET_NOTVALID;
		}
		if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX))
		{
			lidbg("%s:is not SonixCam ,ioctl fail!\n",__func__);
			return RET_NOTSONIX;
		}
		switch(_IOC_NR(cmd))
		{
			case NR_BITRATE:
				lidbg("%s:Online NR_REC_BITRATE = [%ld]\n",__func__,arg);
				f_online_bitrate = arg;
		        break;
		    case NR_RESOLUTION:
				lidbg("%s:Online NR_REC_RESOLUTION  = [%s]\n",__func__,(char*)arg);
				strcpy(f_online_res,(char*)arg);
		        break;
			case NR_PATH:
				lidbg("%s:Online NR_REC_PATH  = [%s]\n",__func__,(char*)arg);
				ret_st = checkSDCardStatus((char*)arg);
				if(ret_st != 1) 
					strcpy(f_online_path,(char*)arg);
				else
					lidbg("%s: f_rec_path access wrong! %d", __func__ ,EFAULT);//not happend
				if(ret_st > 0) ret = RET_FAIL;
		        break;
			case NR_TIME:
				lidbg("%s:Online NR_REC_TIME = [%ld]\n",__func__,arg);
				f_online_time = arg;
		        break;
			case NR_FILENUM:
				lidbg("%s:Online NR_REC_FILENUM = [%ld]\n",__func__,arg);
				f_online_filenum = arg;
		        break;
			case NR_TOTALSIZE:
				lidbg("%s:Online NR_REC_TOTALSIZE = [%ld]\n",__func__,arg);
				f_online_totalsize= arg;
		        break;
			case NR_START_REC:
		        lidbg("%s:Online NR_START_REC\n",__func__);
				setOnlineProp(DVR_ID);
				checkSDCardStatus(f_online_path);
				if(isDVRRec)
				{
					lidbg("%s:====Online start cmd neglected====\n",__func__);
					ret = RET_IGNORE;
				}
				else if(isOnlineRec) 
				{
					lidbg("%s:====Online start cmd repeatedly====\n",__func__);
					ret = RET_REPEATREQ;
				}
				else
				{
					lidbg("%s:====Online start rec====\n",__func__);
					isOnlineRec = 1;
					if(start_rec(DVR_ID,1)) goto failproc;
				}
		        break;
			case NR_STOP_REC:
		        lidbg("%s:Online NR_STOP_REC\n",__func__);
				if(isDVRRec)
				{
					lidbg("%s:====Online stop cmd neglected====\n",__func__);
					ret = RET_IGNORE;
				}
				else if(isOnlineRec) 
				{
					lidbg("%s:====Online stop rec====\n",__func__);
					if(stop_rec(DVR_ID,1)) goto failproc;
					isOnlineRec = 0;
				}
				else
				{
					lidbg("%s:====Online stop cmd repeatedly====\n",__func__);
					ret = RET_REPEATREQ;
				}
		        break;
		    default:
		        return -ENOTTY;
		}
	}
	else if(_IOC_TYPE(cmd) == FLYCAM_STATUS_IOC_MAGIC)//Rec TestAp status
	{
		switch(_IOC_NR(cmd))
		{
			case NR_STATUS:
		        lidbg("%s:NR_STATUS\n",__func__);
				status_fifo_in(arg);
		        break;
			case NR_ACCON_CAM_READY:/*R/W:	R[0-OK;1-Timout]	W[0-Rear;1-DVR] -> Wait for 10s*/
		        lidbg("%s:NR_ACCON_CAM_READY  E cam_id=> %ld\n",__func__,arg);
				if(arg == DVR_ID)
				{
					if(!wait_for_completion_timeout(&DVR_ready_wait , 10*HZ)) ret = 1;
				}
				else if(arg == REARVIEW_ID)
				{
					if(!wait_for_completion_timeout(&Rear_ready_wait , 10*HZ)) ret = 1;
				}
				 lidbg("%s:NR_ACCON_CAM_READY  X \n",__func__);
		        break;
			default:
		        return -ENOTTY;
		}
	}
	else if(_IOC_TYPE(cmd) == FLYCAM_FW_IOC_MAGIC)//front cam online mode
	{
		switch(_IOC_NR(cmd))
		{
			case NR_VERSION:
		        lidbg("%s:NR_VERSION\n",__func__);
				strcpy((char*)arg,"123456");
		        break;
			default:
		        return -ENOTTY;
		}
	}
	else return -EINVAL;
    return ret;
failproc:
	stop_rec(DVR_ID,1);
	isDVRRec = 0;
	isOnlineRec = 0;
	return RET_FAIL;
}

/******************************************************************************
 * Function: flycam_poll
 * Description: Flycam poll function.
 * Input parameters:
 *   	filp	-	file struct
 *   	wait	-	poll table struct.
 * Return values:
 *		
 * Notes: Wait for the status ready and wake up wait_queue.
 *****************************************************************************/
static unsigned int  flycam_poll(struct file *filp, struct poll_table_struct *wait)
{
    //struct fly_KeyEncoderInfo *pflycam_Info= filp->private_data;
    unsigned int mask = 0;
    poll_wait(filp, &pfly_UsbCamInfo->camStatus_wait_queue, wait);
    if(!kfifo_is_empty(&camStatus_data_fifo))
    {
        mask |= POLLIN | POLLRDNORM;
    }
    return mask;
}

/******************************************************************************
 * Function: flycam_read
 * Description: Flycam read function.
 * Input parameters:
 *   	filp	-	file struct
 *   	buffer	-	user space buf.
 *   	size	-	buf size.
 *   	offset	-	buf offset.
 * Return values:
 *		ssize_t	-	size
 * Notes: Read camera status.(asynchronous)
 *****************************************************************************/
ssize_t  flycam_read(struct file *filp, char __user *buffer, size_t size, loff_t *offset)
{
	unsigned char ret = 0;
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
	if(kfifo_is_empty(&camStatus_data_fifo))
    {
        if(wait_event_interruptible(pfly_UsbCamInfo->camStatus_wait_queue, !kfifo_is_empty(&camStatus_data_fifo)))
            return -ERESTARTSYS;
    }
	down(&pfly_UsbCamInfo->sem);
	ret = kfifo_out(&camStatus_data_fifo, camStatus_data_for_hal, 1);
	up(&pfly_UsbCamInfo->sem);
	lidbg("%s:====HAL read_status => %d=====\n",__func__,camStatus_data_for_hal[0]);
	if(copy_to_user(buffer, camStatus_data_for_hal, 1))
    {
        return -1;
    }
	return 1;
}

int flycam_open (struct inode *inode, struct file *filp)
{
	//filp->private_data = pflycam_Info;
    return 0;
}

/******************************************************************************
 * Function: flycam_write
 * Description: Flycam write function.
 * Input parameters:
 *   	filp	-	file struct
 *   	buffer	-	user space buf.
 *   	size	-	buf size.
 *   	ppos	-	buf ppos.
 * Return values:
 *		ssize_t	-	size
 * Notes: Old command interface -> Only for online at present(DVR use ioctl)
 *****************************************************************************/
ssize_t flycam_write (struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	char *cmd[30] = {NULL};//cmds array
	char *keyval[2] = {NULL};//key-vals
	char cmd_num  = 0;//cmd amount
    char cmd_buf[256];
	int i;
	char ret = 0;
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

	/*Do not check camera status in ACCOFF*/
	if(!isSuspend)
	{
		/*check camera status before doing ioctl*/
		if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISVALID))
		{
			lidbg("%s:DVR[online] not found,ioctl fail!\n",__func__);
			return RET_NOTVALID;
		}
		if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX) && !isDVRAfterFix)
		{
			lidbg("%s:is not SonixCam ,ioctl fail!\n",__func__);
			return RET_NOTSONIX;
		}
	}
	
	for(i = 0;i < cmd_num; i++)
	{
		lidbg_token_string(cmd[i], "=", keyval) ;
		if(!strcmp(keyval[0], "record") )
		{
			if(!strncmp(keyval[1], "1", 1))//start
			{
			    lidbg("%s:==write==Online NR_START_REC\n",__func__);
				lidbg_shell_cmd("echo 'udisk_request' > /dev/flydev0");
				//setOnlineProp(DVR_ID);
				checkSDCardStatus(f_online_path);
				if(!(pfly_UsbCamInfo->camStatus & FLY_CAM_ISVALID))/*Camera not valid*/
				{
					if( isSuspend)
					{
						lidbg("%s:==write==ACCOFF wait Camera Begin!====\n",__func__);
						if(!wait_for_completion_timeout(&DVR_ready_wait , 6*HZ)) /*ACCOFF wait for 6sec*/
						{
							lidbg("%s:==write==ACCOFF wait Camera Timeout!====\n",__func__);
							goto failproc;
						}
					}
					else 
					{
						lidbg("%s:==write==ACCON can not found Camera!Force exit!====\n",__func__);/*ACCON don't wait*/
						goto failproc;
					}
				}
				if(isDVRRec)
				{
					lidbg("%s:==write==Online start cmd neglected====\n",__func__);
					ret = RET_IGNORE;
				}
				else if(isOnlineRec) 
				{
					lidbg("%s:==write==Online start cmd repeatedly====\n",__func__);
					ret = RET_REPEATREQ;
				}
				else
				{
					lidbg("%s:==write==Online start rec====\n",__func__);
					isOnlineRec = 1;
					if(isSuspend) fixScreenBlurred(DVR_ID,1);
					setOnlineProp(DVR_ID);
					if(start_rec(DVR_ID,1)) goto failproc;
				}
			}
			else if(!strncmp(keyval[1], "0", 1))//stop
			{
				lidbg("%s:==write==Online NR_STOP_REC\n",__func__);
				if(isDVRRec)
				{
					lidbg("%s:==write==Online stop cmd neglected====\n",__func__);
					ret = RET_IGNORE;
				}
				else if(isOnlineRec) 
				{
					lidbg("%s:==write==Online stop rec====\n",__func__);
					if(stop_rec(DVR_ID,1)) goto failproc;
					isOnlineRec = 0;
				}
				else
				{
					lidbg("%s:==write==Online stop cmd repeatedly====\n",__func__);
					ret = RET_REPEATREQ;
				}
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
#if 0
			if(!strncmp(keyval[1], "1080", 4))
			{
				strcpy(f_online_path,"1920x1080");
			}
			else if(!strncmp(keyval[1], "720", 3))
			{
				strcpy(f_online_path,"1280x720");
			}
			else if(!strncmp(keyval[1], "640x360", 7))
			{
				strcpy(f_online_path,"640x360");
			}
			else
			{
				lidbg("-------res wrong arg:%s-----",keyval[1]);
			}
#endif
			if(!strncmp(keyval[1], "640x360", 7))
			{
				strcpy(f_online_res,keyval[1]);
			}
			else
			{
				lidbg("-------res wrong arg:%s-----",keyval[1]);
			}
		}
		else if(!strcmp(keyval[0], "rectime") )
		{
			int rectimeVal;
			rectimeVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("rectimeVal = %d",rectimeVal);
			f_online_time = rectimeVal;
		}
		else if(!strcmp(keyval[0], "recbitrate") )
		{
			int recbitrateVal;
			recbitrateVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recbitrateVal = %d",recbitrateVal);
			f_online_bitrate = recbitrateVal;
		}
		#if 1
		else if(!strcmp(keyval[0], "recnum") )
		{
			int recnumVal;
			recnumVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recnumVal = %d",recnumVal);
			f_online_filenum = recnumVal;
		}
		#endif
		else if(!strcmp(keyval[0], "recpath") )
		{
			char ret_st;
			lidbg("recpathVal = %s",keyval[1]);
			ret_st = checkSDCardStatus(keyval[1]);
			if(ret_st != 1) 
				strcpy(f_online_path,keyval[1]);
			else
				lidbg("%s: f_online_path access wrong! %d", __func__ ,EFAULT);//not happend
			if(ret_st > 0) ret = RET_FAIL;
		}
		else if(!strcmp(keyval[0], "recfilesize") )
		{
			int recfilesizeVal;
			recfilesizeVal = simple_strtoul(keyval[1], 0, 0);
			lidbg("recfilesizeVal = %d",recfilesizeVal);
			f_online_totalsize = recfilesizeVal;
		}
#if 0
		else if(!strcmp(keyval[0], "test") )
		{
			isBackChange = 1;
			wake_up_interruptible(&wait_queue);
		}
#endif
	}
  
    return size;
failproc:
	lidbg("=====Flycam Write operation error!======\n");
	stop_rec(DVR_ID,1);
	isDVRRec = 0;
	isOnlineRec = 0;
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

#if 0
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
#endif

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

#ifndef	FLY_USB_CAMERA_SUPPORT
	lidbg("%s:FLY_USB_CAMERA_SUPPORT not define,exit\n",__func__);
  	return 0;
#endif
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
	/*init pfly_UsbCamInfo*/
	pfly_UsbCamInfo = (struct fly_UsbCamInfo *)kmalloc( sizeof(struct fly_UsbCamInfo), GFP_KERNEL);
    if (pfly_UsbCamInfo == NULL)
    {
        lidbg("[cam]:kmalloc err\n");
        return -ENOMEM;
    }
	
	camStatus_fifo_buffer = (u8 *)kmalloc(FIFO_SIZE , GFP_KERNEL);
    camStatus_data_for_hal = (u8 *)kmalloc(HAL_BUF_SIZE , GFP_KERNEL);
	sema_init(&pfly_UsbCamInfo->sem, 1);
    kfifo_init(&camStatus_data_fifo, camStatus_fifo_buffer, FIFO_SIZE);

	//init_waitqueue_head(&wait_queue);
	init_waitqueue_head(&pfly_UsbCamInfo->camStatus_wait_queue);/*camera status wait queue*/
	
	if(g_var.recovery_mode == 0)/*do not process when in recovery mode*/
	{
		CREATE_KTHREAD(thread_stop_rec_func, NULL);
		register_lidbg_notifier(&lidbg_notifier);/*ACCON/OFF notifier*/
		/*Stop recording timer(in ACCOFF scene)*/
		init_timer(&suspend_stoprec_timer);
	    suspend_stoprec_timer.data = 0;
	    suspend_stoprec_timer.expires = 0;
	    suspend_stoprec_timer.function = suspend_stoprec_timer_isr;
#if 0
		INIT_WORK(&work_t_start_rec, work_startRec);
	    INIT_WORK(&work_t_stop_rec, work_stopRec);
#endif 
		//INIT_WORK(&work_t_fixScreenBlurred, work_DVR_fixScreenBlurred);

		/*usb camera first plug in (fix lidbgshell delay issue)*/
#if 0
		isDVRFirstInit = 1;
		pfly_UsbCamInfo->camStatus = lidbg_checkCam();
		lidbg("********camStatus => %d***********",pfly_UsbCamInfo->camStatus);
		if(pfly_UsbCamInfo->camStatus & FLY_CAM_ISSONIX)
			schedule_delayed_work(&work_t_DVR_fixScreenBlurred,10*HZ);/*at about kernel 30s, lidbgshell it's ready*/
		else isDVRFirstInit = 0;

		isRearViewFirstInit = 1;
		if((pfly_UsbCamInfo->camStatus>>4) & FLY_CAM_ISSONIX)
			schedule_delayed_work(&work_t_RearView_fixScreenBlurred,11*HZ);/*at about kernel 35s, lidbgshell it's ready*/
		else isRearViewFirstInit = 0;
#endif
		schedule_delayed_work(&work_t_DVR_fixScreenBlurred,10*HZ);/*at about kernel 30s, lidbgshell it's ready*/
		schedule_delayed_work(&work_t_RearView_fixScreenBlurred,11*HZ);/*at about kernel 31s, lidbgshell it's ready*/
		
	}
	usb_register_notify(&usb_nb_cam);/*USB notifier:must after isDVRFirstInit&isRearViewFirstInit*/
	lidbg_new_cdev(&flycam_nod_fops, "lidbg_flycam");
    return 0;
}

static __init int lidbg_flycam_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;
    CREATE_KTHREAD(thread_flycam_init, NULL);
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


