
#include "lidbg_servicer.h"

int main(int argc, char **argv)
{
    pthread_t lidbg_uevent_tid;
    int checkout=0; //checkout=1 origin ; checkout=2 flyaudio 
    DUMP_BUILD_TIME_FILE;

	if(access("/flysystem/lib/out",F_OK) == 0)
	{
		checkout = 2;
		ALOGE("lidbg_iserver: this is flyaudio system\n");
	}
	else
	{
		checkout = 1;
		ALOGE("lidbg_iserver: this is origin system\n");
	}

    system("mkdir /data/lidbg");
    system("mkdir /data/lidbg/lidbg_osd");
    system("chmod 777 /data/lidbg");
    system("chmod 777 /data/lidbg/lidbg_osd");
	system("mount -o remount /system");
	

     if(checkout == 1)
      {
		system("insmod /system/lib/modules/out/lidbg_uevent.ko");
		while(1)
		{
		   if(access("/system/lib/modules/out/lidbg_userver", X_OK) == 0)
			{
			  system("/system/lib/modules/out/lidbg_userver &");
			  ALOGE("lidbg_iserver: origin userver start\n");
			  break;
			}
			system("chmod 777 /system/lib/modules/out/lidbg_userver");
			system("chmod 777 /system/lib/modules/out/*");
			sleep(1);
		}
		sleep(1);
		system("chmod 777 /dev/lidbg_uevent");
		system("insmod /system/lib/modules/out/lidbg_loader.ko");
      }

	if(checkout == 2)
	{
		system("insmod /flysystem/lib/out/lidbg_uevent.ko");
		while(1)
		{
		   if(access("/flysystem/lib/out/lidbg_userver", X_OK) == 0)
			{
			  system("/flysystem/lib/out/lidbg_userver &");
			  ALOGE("lidbg_iserver: flyaudio userver start\n");
			  break;
			}
			system("chmod 777 /flysystem/lib/out/lidbg_userver");
			system("chmod 777 /flysystem/lib/out/*");
			sleep(1);
		}
		sleep(1);
	   	system("chmod 777 /dev/lidbg_uevent");
	   	system("insmod /flysystem/lib/out/lidbg_loader.ko");
	}
    return 0;
}

