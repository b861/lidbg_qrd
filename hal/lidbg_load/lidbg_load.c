
#include "lidbg_servicer.h"
#if 0
static void *read_file( char *filename, ssize_t *_size)
{
	int ret, fd;
	struct stat sb;
	ssize_t size;
	void *buffer = NULL;

	/* open the file */
	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return NULL;

	/* find out how big it is */
	if (fstat(fd, &sb) < 0)
		goto bail;
	size = sb.st_size;

	/* allocate memory for it to be read into */
	buffer = malloc(size);
	if (!buffer)
		goto bail;

	/* slurp it into our buffer */
	ret = read(fd, buffer, size);
	if (ret != size)
		goto bail;

	/* let the caller know how big it is */
	*_size = size;

bail:
	close(fd);
	return buffer;
}


int module_insmod(char *file)
{
	void *module = NULL;
	ssize_t size;
	int ret;
	module = read_file(file, &size);
	if(!module)
	{
		lidbg("not found module : \"%s\" \n",file);
		return -1;
	}
	else
	{
		ret = init_module(module, size, "");
		if(ret < 0)
		{
			if (0 == memcmp("File exists",strerror(errno),sizeof(strerror(errno))))
			{
				lidbg("\nflyV2Recovery insmod -> %s File exists",file);
				free(module);
				return 0;
			}

		  	lidbg("init module \"%s\" fail!\n",file);
			free(module);
			return -1;
		  }
		else
		{
			lidbg("init module success! \"%s\" \n",file);
			free(module);
			return 0;
		}
	}
}
#endif

int main(int argc, char **argv)
{
    pthread_t lidbg_uevent_tid;
    int checkout = 0; //checkout=1 origin ; checkout=2 flyaudio

    system("chmod 777 /dev/dbg_msg");

    DUMP_BUILD_TIME_FILE;
    lidbg("lidbg_iserver: iserver start\n");
	int ret;
	
	//wait flysystem mount
    while(access("/flysystem/lib", F_OK) != 0)
    {
		static int cnt = 0;
		sleep(1);
		if(++cnt>=20)
			break;
	}
	
    if(access("/flysystem/lib/out/lidbg_loader.ko", F_OK) == 0)
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
		//ret = module_insmod("/system/lib/modules/out/lidbg_uevent.ko");
		//if(ret<0){ lidbg("module_insmod fail\n");}
        system("insmod /system/lib/modules/out/lidbg_uevent.ko");
        system("insmod /system/lib/modules/out/lidbg_loader.ko");
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
			system("mount -o remount,ro /system");
            lidbg("lidbg_iserver: waitting origin lidbg_uevent...\n");
            sleep(1);
        }
    }
    else if(checkout == 2)
    {
        system("insmod /flysystem/lib/out/lidbg_uevent.ko");
        system("insmod /flysystem/lib/out/lidbg_loader.ko");
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
    return 0;
}

