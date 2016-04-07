
#include "lidbg_servicer.h"
#include "../../drivers/inc/lidbg_flycam_par.h" /*flycam parameter*/

pthread_t thread_checkStatus_id;
int fd = 0;
struct pollfd fds;

//ioctl
#define READ_CAM_PROP(magic , nr) _IOR(magic, nr ,int) 
#define WRITE_CAM_PROP(magic , nr) _IOW(magic, nr ,int)

void startRec(void)
{
	int ret;
	ret = ioctl(fd,_IO(FLYCAM_REAR_REC_IOC_MAGIC, NR_START_REC), NULL);
	if (ret == RET_NOTVALID)
      	lidbg("@@=========Camera not valid !!=======\n");
	else if (ret == RET_NOTSONIX)
		lidbg("@@=========Camera not sonix !!=======\n");
	else if (ret == RET_IGNORE)
		lidbg("@@=========cmd ignore !!=======\n");
	else
		lidbg("@@========Camera rec done ===%d====\n",ret);
	return 0;
}

void *thread_checkStatus(void *par)
{
	unsigned char cam_status;
	fds.fd     = fd;
   	fds.events = POLLIN;
	int ret;
	while(1)
	{
		fds.revents = 0;
		ret = poll(&fds, 1, -1);
		if (ret <= 0)
		{
			lidbg("time out\n");
			continue;
		}
		if(fds.revents&POLLIN)
		{
			read(fd, &cam_status, 1);
			//printf("[knob0]knob_val=>%x\n",knob_val);
			lidbg("====[cam]cam_status =>0x%x====\n",cam_status);
		}
	}
	return 0;
}
int main(int argc, char **argv)
{
	int i = 0,count = 5;
  	int ret;
	char fw_version[256];
	lidbg("@@lidbg_ioctl start\n");

open_dev:
	fd = open("/dev/lidbg_flycam0", O_RDWR);
	if((fd == 0xfffffffe) || (fd == 0) || (fd == 0xffffffff))
	{
	    lidbg("@@open lidbg_flycam0 fail\n");
	    goto open_dev;
	}

	lidbg("@@open lidbg_flycam0 ok\n");

	system("chmod 0777 /dev/lidbg_flycam0");
	sleep(1);

	fds.fd     = fd;
   	fds.events = POLLIN;
#if 1
	ret = pthread_create(&thread_checkStatus_id,NULL,thread_checkStatus,NULL);
	if(ret != 0)
	{
		lidbg( "@@Create pthread error!\n");
		return 1;
	}
#endif
	ret = ioctl(fd,_IO(FLYCAM_FW_IOC_MAGIC, NR_VERSION), fw_version);
	if (ret != 0)
      {
      	lidbg("@@NR_PATH ioctl fail===%d===\n",ret);
	}
	lidbg("@@FW Version ===> %s\n" , fw_version);

	while(count--)
	{
		ret = ioctl(fd,_IO(FLYCAM_REAR_REC_IOC_MAGIC, NR_PATH), "/storage/sdcard0/camera_rec/");
		if (ret != 0)
        {
        	lidbg("@@NR_PATH ioctl fail===%d===\n",ret);
		}
		ret = ioctl(fd,_IO(FLYCAM_REAR_REC_IOC_MAGIC, NR_RESOLUTION), "1280x720");
		if (ret != 0)
        {
        	lidbg("@@NR_RESOLUTION ioctl fail====%d===\n",ret);
		}
		ret = ioctl(fd,_IO(FLYCAM_REAR_REC_IOC_MAGIC, NR_TIME), 300); 
		if (ret != 0)
        {
        	lidbg("@@NR_TIME ioctl fail====%d===\n",ret);
		}
		
		ret = ioctl(fd,_IO(FLYCAM_REAR_REC_IOC_MAGIC, NR_SET_PAR), NULL); 
		if (ret != 0)
        {
        	lidbg("@@NR_SET_PAR ioctl fail====%d===\n",ret);
		}
		
		/*start recording*/
		lidbg("@@========START========\n");
		startRec();
		sleep(500);
		/*stop recording*/
		if (ioctl(fd,_IO(FLYCAM_REAR_REC_IOC_MAGIC, NR_STOP_REC), NULL) < 0)
	    {
	      	lidbg("@@NR_STOP_REC ioctl fail=======\n");
		}
		lidbg("@@========STOP========\n");
		//check_status();
		//sleep(1);
	}
	lidbg("@@=======DONE=========");
	return 0;
}

