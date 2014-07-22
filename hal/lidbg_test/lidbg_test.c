
#include "lidbg_servicer.h"

//"cat /sys/power/autosleep"

int main(int argc, char **argv)
{
	
	system("su");
	system("chmod 777 /sys/class/leds/button-backlight/brightness");
	system("chmod 777 /sys/power/wake_lock");
	system("chmod 777 /sys/power/wake_unlock");
	while(1)
	{
	    system("echo lidbg > /sys/power/wake_lock");
	    system("echo 1 > /sys/class/leds/button-backlight/brightness");
	    sleep(1);


	    system("echo 0 > /sys/class/leds/button-backlight/brightness");
	    system("echo lidbg > /sys/power/wake_unlock");
	    sleep(2);
	}
   	 return 0;
}

