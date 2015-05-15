
#include "lidbg_servicer.h"
#include "lidbg_insmod.h"

int main(int argc, char **argv)
{
    argc=argc;
    argv=argv;
    pthread_t lidbg_uevent_tid;
    int recovery_mode,checkout = 0; //checkout=1 origin ; checkout=2 old flyaudio;checkout=3 new flyaudio
	int ret;
	 
	system("setenforce 0");
	
    system("chmod 777 /dev/dbg_msg");

    DUMP_BUILD_TIME_FILE;
    lidbg("lidbg_iserver: iserver start\n");

#if 0
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
#endif

    if(is_file_exist("/flysystem/lib/out/lidbg_loader.ko"))
    {
    	if(is_file_exist("/system/etc/build_origin"))
    	{
			checkout = 1;
			lidbg("lidbg_iserver: this is origin system\n");
    	}
		else
		{
	        checkout = 2;
	        lidbg("lidbg_iserver: this is old flyaudio system\n");
		}
    }
    else if(is_file_exist("/system/vendor/lib/out/lidbg_loader.ko"))
    {
        checkout = 3;
        lidbg("lidbg_iserver: this is new flyaudio system\n");

	 }
	else
    {
        checkout = 1;
        lidbg("lidbg_iserver: this is origin system\n");
    }

	if(is_file_exist("/sbin/recovery")) 
	{
		recovery_mode = 1;
		checkout = 1;
		lidbg("recovery_mode=1\n=====force use origin system=====\n");
	}
	else
	{
		recovery_mode = 0;
	}

    system("mkdir /data/lidbg");
    system("mkdir /data/lidbg/lidbg_osd");
    system("chmod 777 /data/lidbg");
    system("chmod 777 /data/lidbg/lidbg_osd");

    if(checkout == 1)
    {
    	if(is_file_exist("/system/etc/build_origin"))
		{
			 system("mount -o remount /flysystem");
			 system("rm -rf /flysystem/bin");
			 system("rm -rf /flysystem/lib");
			 system("rm -rf /flysystem/app");
		}
		
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
    else if(checkout == 3)
    {
		system("mkdir -p /flysystem/lib/out");
		system("chmod 777 /flysystem");
		system("chmod 777 /flysystem/lib");
		system("chmod 777 /flysystem/lib/out");

		system("cp -rf /system/vendor/lib/out/* /flysystem/lib/out/*");
		checkout = 2;
    }

    if(checkout == 2)
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

	if(recovery_mode == 1)
	{
	    sleep(5);// wait for ts load
		system("setprop service.recovery.start 1");
		//system("/sbin/recovery &");
	}
	
	while(1){
		 sleep(10);
	}
    return 0;
}
