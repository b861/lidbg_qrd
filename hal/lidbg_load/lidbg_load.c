
#include "lidbg_servicer.h"
#include "lidbg_insmod.h"

int main(int argc, char **argv)
{
    pthread_t lidbg_uevent_tid;
    int checkout = 0; //checkout=1 origin ; checkout=2 flyaudio

    system("chmod 777 /dev/dbg_msg");

    DUMP_BUILD_TIME_FILE;
    lidbg("lidbg_iserver: iserver start\n");
	int ret;

	if(is_file_exist("/system/lib/modules/out/lidbg_loader.ko"))
	{
		// system("mount -o remount /flysystem");
		// system("rm -rf /flysystem/*");

	}

	//wait flysystem mount
    while(is_file_exist("/flysystem/lib") == 0)
    {
		static int cnt = 0;
		if(is_file_exist("/sbin/recovery")) 
			{
			lidbg("lidbg_iserver: this is flyaudio recovery\n");
			break;
			}
		sleep(1);
		if(++cnt>=5)
			break;
	}

    if(is_file_exist("/flysystem/lib/out/lidbg_loader.ko"))
    {
        checkout = 2;
        lidbg("lidbg_iserver: this is flyaudio system\n");
    }
    else
    {
        checkout = 1;
        lidbg("lidbg_iserver: this is origin system\n");
    }

    system("mkdir /data/lidbg");
    system("mkdir /data/lidbg/lidbg_osd");
    system("chmod 777 /data/lidbg");
    system("chmod 777 /data/lidbg/lidbg_osd");

    if(checkout == 1)
    {
		module_insmod("/system/lib/modules/out/lidbg_uevent.ko");
		module_insmod("/system/lib/modules/out/lidbg_loader.ko");
        //system("insmod /system/lib/modules/out/lidbg_uevent.ko");
        //system("insmod /system/lib/modules/out/lidbg_loader.ko");
        while(1)
        {
            if(access("/system/lib/modules/out/lidbg_userver", X_OK) == 0)
            {
                system("/system/lib/modules/out/lidbg_userver &");
                lidbg("lidbg_iserver: origin userver start\n");
                break;
            }
			system("mount -o remount /system");
            system("chmod 777 /system/lib/modules/out/lidbg_userver");
            system("chmod 777 /system/lib/modules/out/*");
            lidbg("lidbg_iserver: waitting origin lidbg_uevent...\n");
            sleep(1);
        }
    }
    else if(checkout == 2)
    {
    	
		module_insmod("/flysystem/lib/out/lidbg_uevent.ko");
		module_insmod("/flysystem/lib/out/lidbg_loader.ko");
        //system("insmod /flysystem/lib/out/lidbg_uevent.ko");
        //system("insmod /flysystem/lib/out/lidbg_loader.ko");
        while(1)
        {
            if(access("/flysystem/lib/out/lidbg_userver", X_OK) == 0)
            {
                system("/flysystem/lib/out/lidbg_userver &");
                lidbg("lidbg_iserver: flyaudio userver start\n");
                break;
            }
            system("chmod 777 /flysystem/lib/out/lidbg_userver");
            system("chmod 777 /flysystem/lib/out/*");
            lidbg("lidbg_iserver: waitting flyaudio lidbg_uevent...\n");
            sleep(1);
        }
    }

    sleep(1);
    system("chmod 777 /dev/lidbg_uevent");
	while(1){
		 sleep(10);
	}
    return 0;
}
