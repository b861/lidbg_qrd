
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
#include <cutils/uevent.h>
#include <sys/poll.h>
#include <time.h>

#include "lidbg_servicer.h"

#define LIDBG_UEVENT_MSG_LEN  1024
#define LIDBG_UEVENT_NODE_NAME "lidbg_uevent"
#define ARRAY_SIZE(ar) (sizeof(ar)/sizeof(ar[0]))





struct parse_action_table
{
    char *action;
    void (*progress_action)(char *action_para);
};

struct uevent
{
    char *action;
    char *devpath;
    char *subsystem;
    char *devname;
    char *lidbg_action;
    char *lidbg_parameter;
};


void progress_action_shell(char *action_para)
{
    system(action_para);
}
//add a new item here!
struct parse_action_table lidbg_parse_action[] =
{
    { "shell", progress_action_shell },
};

static void process_uevent( struct uevent *uevent)
{
    int loop;
    if (!strcmp(uevent->devname, LIDBG_UEVENT_NODE_NAME))
    {
        for(loop = 0; loop < ARRAY_SIZE(lidbg_parse_action); loop++)
        {
            if (!strcmp(lidbg_parse_action[loop].action, uevent->lidbg_action))
            {
                lidbg_parse_action[loop].progress_action(uevent->lidbg_parameter);
                LIDBG_PRINT2("succeed action:['%s','%s']\n", uevent->lidbg_action, uevent->lidbg_parameter);
                return ;
            }
        }
        LIDBG_PRINT2("err action, check: { '%s', '%s', '%s', '%s', '%s', '%s' }%d\n", uevent->action, uevent->devpath, uevent->subsystem,
                     uevent->devname, uevent->lidbg_action, uevent->lidbg_parameter, (uevent->lidbg_parameter != NULL));
        return ;
    }
}

static void parse_uevent(char *msg, struct uevent *uevent)
{
    uevent->action = "null";
    uevent->devpath = "null";
    uevent->subsystem = "null";
    uevent->devname = "null";
    uevent->lidbg_action = "null";
    uevent->lidbg_parameter = NULL;

    while (*msg)
    {
        //LIDBG_PRINT2("uevent str: %s\n", msg);
        if (!strncmp(msg, "ACTION=", 7))
        {
            msg += 7;
            uevent->action = msg;
        }
        else if (!strncmp(msg, "DEVPATH=", 8))
        {
            msg += 8;
            uevent->devpath = msg;
        }
        else if (!strncmp(msg, "SUBSYSTEM=", 10))
        {
            msg += 10;
            uevent->subsystem = msg;
        }
        else if (!strncmp(msg, "DEVNAME=", 8))
        {
            msg += 8;
            uevent->devname = msg;
        }
        else if (!strncmp(msg, "LIDBG_ACTION=", 13))
        {
            msg += 13;
            uevent->lidbg_action = msg;
        }
        else if (!strncmp(msg, "LIDBG_PARAMETER=", 16))
        {
            msg += 16;
            uevent->lidbg_parameter = msg;
        }
        while (*msg++)
            ;
    }
}

static bool lidbg_uevent_callback(int fd)
{
    char msg[LIDBG_UEVENT_MSG_LEN + 2];
    struct uevent uevent;
    char *p = NULL;
    int n;

    if (fd < 0 )
        return -1;

    n = uevent_kernel_multicast_recv(fd, msg, LIDBG_UEVENT_MSG_LEN);
    if (n >= LIDBG_UEVENT_MSG_LEN)
    {
        LIDBG_PRINT2("ERR--------------- overflow -- discard ---------------\n");
    }
    msg[n] = '\0';
    msg[n + 1] = '\0';

    parse_uevent(msg, &uevent);
    process_uevent(&uevent);

    return 0;
}
static void lidbg_uevent_poll(bool (*uevent_callback)(int fd))
{
    struct pollfd ufd;
    int ret, fd, i, nr;

    LIDBG_PRINT2("SUC--------------- FUTENGFEI UEVENT INIT ---------------\n");

    fd = uevent_open_socket(64 * 1024, true);
    if (fd >= 0)
    {
        LIDBG_PRINT2("SUC--------------- uevent_open_socket ---------------\n");
        fcntl(fd, F_SETFL, O_NONBLOCK);
        ufd.events = POLLIN;
        ufd.fd = fd;
        while(1)
        {
            ufd.revents = 0;
            nr = poll(&ufd, 1, -1);
            if (nr <= 0)
                continue;
            if (ufd.revents == POLLIN && uevent_callback)
                uevent_callback(ufd.fd);
        }
    }
    else
        LIDBG_PRINT2("err--------------- uevent_open_socket ---------------\n");

}
int main(int argc, char **argv)
{
    system("insmod /flysystem/lib/out/lidbg_uevent.ko");
    system("insmod /system/lib/modules/out/lidbg_uevent.ko");
    usleep(50 * 1000);
    lidbg_uevent_poll(lidbg_uevent_callback);
    return 0;
}

