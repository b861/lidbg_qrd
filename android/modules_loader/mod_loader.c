#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

//mount yaffs2 mtd@system /system rw remount
//...
//chmod 777 /system/lib/modules/out/mod_loader
//service mod_loader /system/lib/modules/out/mod_loader
//chmod 777 /dev/mlidbg0
//chmod 777 /dev/lidbg_log

int main(int argc , char **argv)
{


    printf("mod_loader+\n");
    printf("insmod+\n");
    //system("su");

    system("insmod /system/lib/modules/out/mlidbg_cmn.ko");
    system("insmod /system/lib/modules/out/lidbg_log.ko");
    system("insmod /system/lib/modules/out/lidbg_touch.ko");
    system("insmod /system/lib/modules/out/lidbg_key.ko");
    system("insmod /system/lib/modules/out/lidbg_i2c.ko");
    system("insmod /system/lib/modules/out/soc_tcc8803.ko");
    system("insmod /system/lib/modules/out/lidbg_io.ko");
    system("insmod /system/lib/modules/out/lidbg_ad.ko");
    system("insmod /system/lib/modules/out/fly_soc.ko");
    system("insmod /system/lib/modules/out/soc_devices.ko");
    system("insmod /system/lib/modules/out/cap_ts.ko");
    // system("insmod /system/lib/modules/out/fly_goodix.ko");
    system("insmod /system/lib/modules/out/gt811.ko");
    system("insmod /system/lib/modules/out/mlidbg.ko");
    //system("chmod 777 /dev/mlidbg0");
    printf("insmod-\n");
    //system("lsmod");

    printf("mod_loader-\n");

    return 0;
}
