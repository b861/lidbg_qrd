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

//DBG ON OFF
#define DBG_USBDISK
int main(int argc , char **argv)
{
    int err = 0;
    int usb_success_count = 0, retry = 0, usb_test_count = 0;
    char path[50];
    memset(path, 0, 50);
    LIDBG_PRINT("[futengfei]  =================================TEST USB START=========================\n");

    sleep(10);
    while(1)
    {
        LIDBG_PRINT("[futengfei]  =====alltimes=====%d==========\n", usb_test_count);

        sleep(3);
        usb_test_count++;
        if(retry < 3)
        {
#ifdef DBG_USBDISK
            err = access("/mnt/usbdisk/text1" , F_OK);
            if ( err != 0 )
            {
                LIDBG_PRINT("[futengfei]  /mnt/usbdisk/text1   not exit 1");
                mkdir("/mnt/usbdisk/text1", 0777);
            }
            else
            {
                LIDBG_PRINT("[futengfei]  /mnt/usbdisk/text1    have exit 2");
            }

            sprintf(path, "%s", "/mnt/usbdisk/text1");
            err = access (path, R_OK);
#endif

            if(err == 0 )//have find
            {
                LIDBG_PRINT("S[access] read file ok!sucess:====111111111111111111111=usb_test_count[%d]========[%d]times ", usb_test_count, usb_success_count);
                //usb disable
                LIDBG_PRINT("=====usb disable====");
                LIDBG_CALL("c io w 35 1", NULL, sizeof(int));
                LIDBG_CALL("c io w 15 0", NULL, sizeof(int));
                LIDBG_CALL("c io w 81 0", NULL, sizeof(int));
                sleep(10);
                //usb enable
                LIDBG_PRINT("=====usb enable====");
                LIDBG_CALL("c io w 81 0", NULL, sizeof(int));
                LIDBG_CALL("c io w 15 1", NULL, sizeof(int));
                LIDBG_CALL("c io w 35 1", NULL, sizeof(int));
                sleep(1);
                LIDBG_CALL("c io w 35 0", NULL, sizeof(int));
                sleep(15);

                retry = 0;
                usb_success_count++;
            }
            else
            {
                //usb disable
                LIDBG_PRINT("=====usb disable====");
                LIDBG_CALL("c io w 35 1", NULL, sizeof(int));
                LIDBG_CALL("c io w 15 0", NULL, sizeof(int));
                LIDBG_CALL("c io w 81 0", NULL, sizeof(int));
                sleep(10);
                //usb enable
                LIDBG_PRINT("=====usb enable====");
                LIDBG_CALL("c io w 81 0", NULL, sizeof(int));
                LIDBG_CALL("c io w 15 1", NULL, sizeof(int));
                LIDBG_CALL("c io w 35 1", NULL, sizeof(int));
                sleep(1);
                LIDBG_CALL("c io w 35 0", NULL, sizeof(int));
                sleep(15);
                retry++;
                LIDBG_PRINT("=====22222====retry=[%d] err=[%d],", retry, err);
            }


        }
        else
        {

            LIDBG_PRINT("F[futengfei]==stop ==222222222222222222==usb_test_count[%d]=====sucess:[%d]times ", usb_test_count, usb_success_count);
        }
    }



    return 0;
}
