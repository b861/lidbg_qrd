
#include "lidbg_servicer.h"

pthread_t thread_checkStatus_id;
int fd = 0;
struct pollfd fds;

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

#define FLYCAM_FRONT_REC_IOC_MAGIC  'F'
#define FLYCAM_FRONT_ONLINE_IOC_MAGIC  'f'
#define FLYCAM_BACK_REC_IOC_MAGIC  'B'
#define FLYCAM_BACK_ONLINE_IOC_MAGIC  'b'
#define FLYCAM_STATUS_IOC_MAGIC  's'

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

//ioctl
#define READ_CAM_PROP(magic , nr) _IOR(magic, nr ,int) 
#define WRITE_CAM_PROP(magic , nr) _IOW(magic, nr ,int)

void startRec(void)
{
	int ret;
	ret = ioctl(fd,_IO(FLYCAM_FRONT_REC_IOC_MAGIC, NR_START_REC), NULL);
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

void check_status(void)
{
	unsigned char cam_status;
	fds.revents = 0;
	if (poll(&fds, 1, -1) <= 0)
	{
		lidbg("time out\n");
		return 0;
	}
	if(fds.revents&POLLIN)
	{
		read(fd, &cam_status, 1);
		//printf("[knob0]knob_val=>%x\n",knob_val);
		lidbg("====[cam]cam_status =>0x%x====\n",cam_status);
	}
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
		lidbg("====[cam]cam_status 1====\n");
		if(fds.revents&POLLIN)
		{
			read(fd, &cam_status, 1);
			//printf("[knob0]knob_val=>%x\n",knob_val);
			lidbg("====[cam]cam_status 2====\n");
			lidbg("====[cam]cam_status =>0x%x====\n",cam_status);
		}
	}
	return 0;
}
int main(int argc, char **argv)
{
	int i = 0,count = 5;
  	int ret;
	
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
	while(count--)
	{
		ret = ioctl(fd,_IO(FLYCAM_FRONT_REC_IOC_MAGIC, NR_PATH), "/storage/sdcard0/camera_rec/");
		if (ret != 0)
        {
        	lidbg("@@NR_PATH ioctl fail===%d===\n",ret);
		}
		ret = ioctl(fd,_IO(FLYCAM_FRONT_REC_IOC_MAGIC, NR_RESOLUTION), "720");
		if (ret != 0)
        {
        	lidbg("@@NR_RESOLUTION ioctl fail====%d===\n",ret);
		}
		ret = ioctl(fd,_IO(FLYCAM_FRONT_REC_IOC_MAGIC, NR_TIME), 300); 
		if (ret != 0)
        {
        	lidbg("@@NR_TIME ioctl fail====%d===\n",ret);
		}
		/*start recording*/
		lidbg("@@========START========\n");
		startRec();
		lidbg("@@ioctl => %d\n", i++);
		//check_status();
		sleep(1);
		/*stop recording*/
		if (ioctl(fd,_IO(FLYCAM_FRONT_REC_IOC_MAGIC, NR_STOP_REC), NULL) < 0)
	    {
	      	lidbg("@@NR_STOP_REC ioctl fail=======\n");
		}
		lidbg("@@========STOP========\n");
		//check_status();
		sleep(2);
	}
	lidbg("@@=======DONE=========");
	return 0;
}

