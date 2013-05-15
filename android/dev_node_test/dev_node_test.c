/* notify:
 add one feature,note its item down below as the example.	2013.3.15  

1:add:dev node test






*/


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
#include "dev_node_test.h"
#define mdbg(msg...) printf("[futengfei]"msg);
typedef struct
{
    int fd;
    unsigned char err;
    char *dev_node;
} sdev_node_test;
sdev_node_test test_dev1 = { -1, -1, "/dev/uboxserver"};

int main (int argc, char **argv)
{
    mdbg("=========test start1=============\n");
    char *gps_data[255];

    test_dev1.fd = open(test_dev1.dev_node, O_RDWR);
    mdbg("%d\n", test_dev1.fd);

    while(test_dev1.fd > 0)
    {
        test_dev1.err = read(test_dev1.fd, gps_data, sizeof(gps_data));
        mdbg("%d %s\n", sizeof(gps_data), gps_data);
        sleep(1);
    }

    return 0;
}
