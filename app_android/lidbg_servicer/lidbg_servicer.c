#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <pthread.h>
#include <utils/Log.h>
#include "lidbg_servicer.h"

//mount yaffs2 mtd@system /system rw remount
//...
//   chmod 777 /dev/lidbg_servicer
//    chmod 777 /dev/mlidbg0
//    chmod 777 /system/bin/lidbg_servicer

//service lidbg_servicer /system/bin/logwrapper /system/bin/lidbg_servicer
//    class late_start
//    user root
//    group root


#define LOG_COUNT_MAX (16)



#define SERVICER_DONOTHING  (0)
#define LOG_DMESG  (1)
#define LOG_LOGCAT (2)
#define LOG_ALL (3)
#define LOG_CONT    (4)

#define WAKEUP_KERNEL (10)
#define SUSPEND_KERNEL (11)

#define LOG_DVD_RESET (64)
#define LOG_CAP_TS_GT811 (65)
#define LOG_CAP_TS_FT5X06 (66)
#define LOG_CAP_TS_FT5X06_SKU7 (67)
#define LOG_CAP_TS_RMI (68)
#define LOG_CAP_TS_GT801 (69)
#define  CMD_FAST_POWER_OFF (70)

#define UMOUNT_USB (80)
#define VIDEO_SET_PAL (81)
#define VIDEO_SET_NTSC (82)

#define SUSPEND_PREPARE (83)
#define RESUME_PREPARE (84)

#define VIDEO_SHOW_BLACK (85)
#define VIDEO_NORMAL_SHOW (86)

#define UBLOX_EXIST   (88)
#define CMD_ACC_OFF_PROPERTY_SET (89)


pthread_t ntid;
int fd = 0;

#include "./copyfile.c"


void log_acc_times()
{




}

void set_power_state(int state)
{

	int fd;
	const char suspendstring[] = "mem";
	const char wakeupstring[] = "on";
	const char *powerdev = "/sys/power/state";
	
	printf("set_power_state:%d\n",state);

	fd = open(powerdev, O_RDWR);
	if(fd >= 0) 
	{
	   //printf("open linux power dev ok: %s\n", powerdev);
	   if(state == 0)
	   	write(fd, suspendstring, sizeof(suspendstring) - 1);
	   else
	   	write(fd, wakeupstring, sizeof(wakeupstring) - 1);
	   
	   close(fd);
	}
	else
	{  
	   printf("open linux power dev fail: %s\n", powerdev);
	}

}


void thread_fastboot(void)
{
	printf("thread_fastboot+\n");
	
//	property_set("fly.fastboot.accoff", "1");
	system("am broadcast -a android.intent.action.FAST_BOOT_START");
	printf("thread_fastboot-\n");

    pthread_exit(0);

}


void lunch_fastboot()
{
    pthread_t id1;
    int ret;
    ret=pthread_create(&id1,NULL,(void *)thread_fastboot,NULL);
    if(ret!=0)
    {
         printf("Create pthread error!\n");
        
    }
	//pthread_join(id1,NULL); 


}



int  servicer_handler(int signum)
{

    int cmd = 0;
    static int count = 0;
    int readlen;
    printf("servicer_handler++\n");


loop_read:

    readlen = read(fd, &cmd, 4);

	if(cmd == SERVICER_DONOTHING)
	{
		printf("servicer_handler-\n");
		return SERVICER_DONOTHING;
	}
    //printf("fd=%x,readlen=%d,cmd=%d\n",fd,readlen,cmd);
    //LOGW("[futengfei]  fd=%x,readlen=%d,cmd=%d", fd, readlen, cmd);
    else

    {

        printf("cmd = %d\n", cmd);

        switch(cmd)
        {
        case LOG_DMESG:
        case LOG_LOGCAT:
        case LOG_ALL:
        case LOG_CONT:
        {
            if(count == 0)

            {
                system("date > /sdcard/log_logcat.txt");
                system("date > /sdcard/log_dmesg.txt");
                system("date > /sdcard/all_mesg.txt");
                //system("date > /sdcard/log_dmesg_lidbg.txt");

                //system("date > /sdcard/tflash/log_dmesg.txt");
            }
            count ++;
            printf("count=%d\n", count);

            if(count > LOG_COUNT_MAX)
            {
                printf("log count>%d break!\n", LOG_COUNT_MAX);

                break;
            }
            if(LOG_ALL == cmd)
            {
                system("date >> /sdcard/all_mesg.txt");
                system("logcat -f /dev/kmsg & dmesg >/sdcard/all_mesg.txt");
				break;

            }
            if(LOG_CONT == cmd)
            {

                system("logcat -f /dev/kmsg & cat /proc/kmsg >/sdcard/all_mesg.txt");
				break;

            }

            if(LOG_DMESG == cmd)
            {
                //printf("LOG_DMESG == cmd\n");

                system("date >> /sdcard/log_dmesg.txt");

                system("dmesg >> /sdcard/log_dmesg.txt");

                //system("date >> /sdcard/log_dmesg_lidbg.txt");

                //system("dmesg | grep -irn lidbg >> /sdcard/log_dmesg_lidbg.txt");

                //system("date >> /sdcard/tflash/log_dmesg.txt");

                // system("dmesg >> /sdcard/tflash/log_dmesg.txt");
				break;

            }

            else if(LOG_LOGCAT == cmd)
            {

                //printf("LOG_LOGCAT == cmd\n");
                system("date >> /sdcard/log_logcat.txt");
                system("logcat >> /sdcard/log_logcat.txt");
                //system("logcat | grep -irn lidbg >> /sdcard/log_logcat_lidbg.txt");
				break;



            }
            break;

        }
        //cap_ts
        case LOG_CAP_TS_GT811:
        case LOG_CAP_TS_FT5X06_SKU7:
        case LOG_CAP_TS_FT5X06:
        case LOG_CAP_TS_RMI:
        case LOG_CAP_TS_GT801:
        {
            if(LOG_CAP_TS_GT811 == cmd)

            {
#if 1

                system("insmod /system/lib/modules/out/gt811.ko");
                system("insmod /flysystem/lib/out/gt811.ko");
				break;



#else
                void *module;
                module = read_file("/lib/modules/out/rmi_touch.ko", &size);
                if (!module)
                    printk("read_file rmi_touch.ko fail\n");
                else
                {
                    ret = init_module(module, size, "");
                    if(ret < 0)
                        printk("init_module rmi_touch.ko fail\n");
                    free(module);
                }

#endif
            }

            else if (LOG_CAP_TS_FT5X06 == cmd)
            {
                system("insmod /system/lib/modules/out/ft5x06_ts.ko");
                system("insmod /flysystem/lib/out/ft5x06_ts.ko");
				break;

            }

            else if (LOG_CAP_TS_FT5X06_SKU7 == cmd)
            {
                system("insmod /system/lib/modules/out/ft5x06_ts_sku7.ko");
				break;

            }

            else if (LOG_CAP_TS_RMI == cmd)
            {
                system("insmod /system/lib/modules/out/rmi_touch.ko");
				break;

            }
            else if (LOG_CAP_TS_GT801 == cmd)
            {
                system("insmod /system/lib/modules/out/gt801.ko");
                system("insmod /flysystem/lib/out/gt801.ko");
				break;

            }
            //sleep(10);//delay to mount sdcard
            //system("dmesg > /sdcard/log_cap_ts_dmesg.txt");
            break;

        }
        case CMD_ACC_OFF_PROPERTY_SET :
		{
			printf("CMD_ACC_OFF_PROPERTY_SET\n");
			property_set("fly.fastboot.accoff", "1");
			break;

		}
        case CMD_FAST_POWER_OFF :
		{
			printf("CMD_FAST_POWER_OFF+++\n");
			//system("setprop fly.fastboot.accoff 1");
			if(0)//why this will block sometime ?
			{
				
				property_set("fly.fastboot.accoff", "1");
				system("am broadcast -a android.intent.action.FAST_BOOT_START");
				
			}
			else
			{
				lunch_fastboot();
			}
			printf("CMD_FAST_POWER_OFF---\n");
			
			break;
		}

        case UMOUNT_USB:
        {

            system("umount /mnt/usbdisk");
            break;

        }
        case WAKEUP_KERNEL:
        {
			printf("WAKEUP_KERNEL+\n");
			if(0)
			{
            	system("su");
            	system("echo on > /sys/power/state");
			}
			else
				set_power_state(1);
            //system("setprop fly.fastboot.accoff 0");
            property_set("fly.fastboot.accoff", "0");
            //system("echo host > /mnt/debugfs/otg/mode");
            log_acc_times();
			printf("WAKEUP_KERNEL-\n");
            break;

        }
        case SUSPEND_KERNEL:
        {
			char value;
			printf("SUSPEND_KERNEL+\n");
            //system("echo peripheral > /mnt/debugfs/otg/mode");
			
			property_get("fly.fastboot.accoff", &value, "");
			printf("fly.fastboot.accoff=%c\n",value);
			if(value == '1')
			{
	            property_set("fly.fastboot.accoff", "0");//fix bug ,enter suspend again

				
				if(0)//not safe
				{
					system("su");
	           	 	system("echo mem > /sys/power/state");
				}
				else
	            {
					set_power_state(0);
	            }
			}
			printf("SUSPEND_KERNEL-\n");
            break;

        }

	case VIDEO_SET_PAL:
        {
	   		printf("<<<<< now get QCamera set pal\n");
            property_set("tcc.fly.vin.pal", "1");// 1 is pal 0 is ntsc
            break;

        }
      case VIDEO_SET_NTSC:
        {
	    	printf("<<<<< now get QCamera set ntsc\n");
            property_set("tcc.fly.vin.pal", "0");// 1 is pal 0 is ntsc
            break;

        }

        case SUSPEND_PREPARE:
        {
			 printf("SUSPEND_PREPARE\n");
			 system("echo 1 > /sys/bus/platform/devices/fastboot/fastboot");
             break;

        }
        case RESUME_PREPARE:
        {
			printf("RESUME_PREPARE\n");
			 system("echo 0 > /sys/bus/platform/devices/fastboot/fastboot");
             break;

        }
	 case VIDEO_SHOW_BLACK:
	    {
	    	printf("<<<<< now Set Video show black.\n");
	        property_set("fly.video.show.status", "0");// 0 is black
	        break;

	    }
	case VIDEO_NORMAL_SHOW:
	    {
	    	printf("<<<<< now Set Video normal show.\n");
	        property_set("fly.video.show.status", "1");// 1 is normal
	        break;

	    }
		case UBLOX_EXIST:
	    {

			if (access("/data/gps.msm7627a.so", R_OK) == 0) break;
			printf("ublox exist,copy gps.msm7627a.so to data\n");
			//system("cp /flysystem/lib/gps.msm7627a.so /data/gps.msm7627a.so");
			copyfile("/flysystem/lib/ublox_gps.so","/data/gps.msm7627a.so");
	        break;

	    }
        }
    }
	goto loop_read;
    //printf("servicer_handler-\n");
    //return cmd;
}




int main(int argc , char **argv)
{
    int err = 0;
    int cmd = 0;
    int count = 0;
    int oflags;

    printf("lidbg_servicer start\n");
    //sleep(5);
    system("insmod /system/lib/modules/out/lidbg_share.ko");
    system("insmod /system/lib/modules/out/lidbg_ts_to_recov.ko");
    system("insmod /system/lib/modules/out/lidbg_msg.ko");
    system("insmod /system/lib/modules/out/lidbg_common.ko");
    system("insmod /system/lib/modules/out/lidbg_servicer.ko");
    system("insmod /system/lib/modules/out/lidbg_touch.ko");
    system("insmod /system/lib/modules/out/lidbg_key.ko");
    system("insmod /system/lib/modules/out/lidbg_i2c.ko");
    system("insmod /system/lib/modules/out/lidbg_soc_msm8x25.ko");
    system("insmod /system/lib/modules/out/lidbg_io.ko");
    system("insmod /system/lib/modules/out/lidbg_ad.ko");
    system("insmod /system/lib/modules/out/lidbg_main.ko");

    system("insmod /system/lib/modules/out/lidbg_fly_soc.ko");

    system("insmod /system/lib/modules/out/lidbg_fastboot.ko");
    system("insmod /system/lib/modules/out/lidbg_lpc.ko");
    system("insmod /system/lib/modules/out/lidbg_soc_devices.ko");
    system("insmod /system/lib/modules/out/lidbg_videoin.ko");
    system("insmod /system/lib/modules/out/lidbg_to_bpmsg.ko");
    system("insmod /system/lib/modules/out/lidbg_gps_driver.ko");

    //for flycar
    
    system("insmod /flysystem/lib/out/lidbg_share.ko");
    system("insmod /flysystem/lib/out/lidbg_ts_to_recov.ko");
    system("insmod /flysystem/lib/out/lidbg_msg.ko");
    system("insmod /flysystem/lib/out/lidbg_common.ko");
    system("insmod /flysystem/lib/out/lidbg_servicer.ko");
    system("insmod /flysystem/lib/out/lidbg_touch.ko");
    system("insmod /flysystem/lib/out/lidbg_key.ko");
    system("insmod /flysystem/lib/out/lidbg_i2c.ko");
    system("insmod /flysystem/lib/out/lidbg_soc_msm8x25.ko");
    system("insmod /flysystem/lib/out/lidbg_io.ko");
    system("insmod /flysystem/lib/out/lidbg_ad.ko");
    system("insmod /flysystem/lib/out/lidbg_main.ko");
	
    system("insmod /flysystem/lib/out/lidbg_fly_soc.ko");
	
    system("insmod /flysystem/lib/out/lidbg_fastboot.ko");
    system("insmod /flysystem/lib/out/lidbg_lpc.ko");
    system("insmod /flysystem/lib/out/lidbg_soc_devices.ko");
    system("insmod /flysystem/lib/out/lidbg_videoin.ko");
    system("insmod /flysystem/lib/out/lidbg_to_bpmsg.ko");
    system("insmod /flysystem/lib/out/lidbg_gps_driver.ko");



    sleep(1);
	
    system("chmod 0777 /dev/lidbg_share");
    system("chmod 0777 /dev/mlidbg0");
    system("chmod 0777 /dev/lidbg_servicer");
    system("chmod 0777 /dev/lidbg_msg");
    system("chmod 0777 /dev/ubloxgps0");
open_dev:
    fd = open("/dev/lidbg_servicer", O_RDWR);
    //printf("fd = %x\n",fd);
    if((fd == 0xfffffffe) || (fd == 0) || (fd == 0xffffffff))
    {
        printf("open lidbg_servicer fail\n");
        sleep(1);//delay wait for /dev/lidbg_servicer to creat
        //usleep(1000 * 100); //100ms
        goto open_dev;
    }

    //system("date > /sdcard/log_dmesg.txt");
    //system("date > /sdcard/log_dmesg_lidbg.txt");
    //system("date > /sdcard/log_logcat.txt");

#if 0	//block_read
    while(1)
    {
        servicer_handler(0);

    }
#else//sigal read
    signal(SIGIO, servicer_handler); //让input_handler()处理SIGIO信号
    fcntl(fd, F_SETOWN, getpid());
    oflags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oflags | FASYNC); //调用驱动的fasync_helper
    
#endif


    //clear fifo
    while(1)
    {
        if(servicer_handler(0) == SERVICER_DONOTHING)
            break;
    }

    system("insmod /system/lib/modules/out/lidbg_ts_probe.ko");
    system("insmod /flysystem/lib/out/lidbg_ts_probe.ko");
#if 1
    //for flycar
    sleep(1);
    system("insmod /flysystem/lib/modules/FlyDebug.ko");
    system("insmod /flysystem/lib/modules/FlyMmap.ko");
    system("insmod /flysystem/lib/modules/FlyPM.ko");
    system("insmod /flysystem/lib/modules/FlyPMDevice.ko");
    system("insmod /flysystem/lib/modules/FlyADC.ko");
    system("insmod /flysystem/lib/modules/FlySpeed.ko");
    system("insmod /flysystem/lib/modules/FlyBrake.ko");
    system("insmod /flysystem/lib/modules/FlyHardware.ko");
    system("insmod /flysystem/lib/modules/FlyHardwareDevice.ko");
    system("insmod /flysystem/lib/modules/FlyAudio.ko");
    system("insmod /flysystem/lib/modules/FlyAudioDevice.ko");
    system("insmod /flysystem/lib/modules/productinfo.ko");
    system("insmod /flysystem/lib/modules/vendor_flyaudio.ko");
	
    //chegnweidong
    system("insmod /flysystem/lib/mdrv/flysemdriver.ko");

    system("insmod /flysystem/lib/tcdriver/uuid.ko");


    sleep(1);
    system("chmod 0777 /dev/FlyDebug");
    system("chmod 0777 /dev/FlyMmap ");
    system("chmod 0777 /dev/FlyHardware");
    system("chmod 0777 /dev/FlyAudio");
    system("chmod 0666 /dev/FlySpeed");
    system("chmod 0666 /dev/FlyBrake");

    //chegnweidong
    system("chmod 0777 /dev/flysemdriver");


	sleep(5);
	
	
	system("chmod 777 /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");

	system("chmod 777 /sys/power/state");

	system("chmod 777 /proc/fake_suspend");
	system("chmod 777 /proc/fake_wakeup");




#endif
	sleep(30);

///////low mem kill
	if(1)
	{
		
		system("chmod 777 /sys/module/lowmemorykiller/parameters/minfree");
		// cat /sys/module/lowmemorykiller/parameters/minfree
		//system("echo 3674,4969,6264,8312,9607,11444 > /sys/module/lowmemorykiller/parameters/minfree");//origin
		sleep(1);
		printf("set minfree\n");
		system("echo 3674,4969,6264,6264,6264,6264 > /sys/module/lowmemorykiller/parameters/minfree");
	}

	
////////set cpu fre 
	if(1)
	{
			system("chmod 777 /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
			//system("echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
			sleep(1);
			system("echo ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
			//system("echo powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
			//sleep(10);
		if(0)//loop set
		{
			 int i=2;
			 while(i>0)
			 {
				 sleep(5);
				 //printf("set performance mode\n");
				 printf("set ondemand mode\n");
				 system("echo ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
				 //system("echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
				 i--;
			 }
		}
	
	}

	
	printf("enter while\n");
    while(1)
    {
    	//printf("enter while...\n");
		//system("echo 3674,4969,6264,6264,6264,6264 > /sys/module/lowmemorykiller/parameters/minfree");
		sleep(60);
    }



    return 0;
}
