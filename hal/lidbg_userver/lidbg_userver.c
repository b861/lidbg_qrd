
#include "lidbg_servicer.h"

#define LIDBG_UEVENT_MSG_LEN  (512)
#define LIDBG_UEVENT_NODE_NAME "lidbg_uevent"

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
static char uevent_ignore[256];


//zone start [add a new item]
void progress_action_shell(char *action_para)
{
    system(action_para);
}
struct parse_action_table lidbg_parse_action[] =
{
    { "shell", progress_action_shell },
};
//zone end

//@func: replace 'des't to 'replace' between drr to drr+len.
static void memreplace(char *drr, char dest, char replace, int len)
{
    int loop = 0;
    if(drr)
    {
        while((loop < len))
        	{
				if((*(drr + loop) == dest))
					*(drr + loop) = replace;
				loop++;
			}
    }
}

static void lidbg_uevent_process( struct uevent *uevent)
{
    unsigned int loop;
    for(loop = 0; loop < SIZE_OF_ARRAY(lidbg_parse_action); loop++)
    {
        if (uevent->lidbg_action && uevent->lidbg_parameter && !strcmp(lidbg_parse_action[loop].action, uevent->lidbg_action))
        {
            lidbg_parse_action[loop].progress_action(uevent->lidbg_parameter);
            return ;
        }
    }
    lidbg( "err action, check: { '%s', '%s'}%d\n", uevent->lidbg_action, uevent->lidbg_parameter, (uevent->lidbg_parameter != NULL));
}
static void system_uevent_transfer(char *msg)
{
    LIDBG_UEVENT(msg);
}

static void parse_uevent(char *msg, struct uevent *uevent)
{
	uevent->action = strstr(msg,"ACTION=");
	uevent->devpath = strstr(msg,"DEVPATH=");
	uevent->subsystem = strstr(msg,"SUBSYSTEM=");
	uevent->devname = strstr(msg,"DEVNAME=");
	uevent->lidbg_action = strstr(msg,"LIDBG_ACTION=");
	uevent->lidbg_parameter = strstr(msg,"LIDBG_PARAMETER=");
}

static bool lidbg_uevent_callback(int fd)
{
    static char msg[LIDBG_UEVENT_MSG_LEN + 1];
    struct uevent uevent;
    char *p = NULL;
    int n;

    if (fd < 0 )
        return -1;

    n = uevent_kernel_multicast_recv(fd, msg, LIDBG_UEVENT_MSG_LEN);
    if (n > LIDBG_UEVENT_MSG_LEN )
    {
        lidbg( "ERR-overflow-%d,%s\n",n,msg);
		return -1;
    }
    msg[n] = '\0';
	
	memreplace(msg, '\0', ' ', n - 1 );

    parse_uevent(msg, &uevent);

    property_get("lidbg.uevent.ignore", uevent_ignore, "null");

    if (uevent.devname && !strcmp(uevent.devname, LIDBG_UEVENT_NODE_NAME))
        lidbg_uevent_process(&uevent);
    else if (uevent.subsystem && strstr(uevent_ignore, uevent.subsystem) == NULL)
        system_uevent_transfer(msg);

    return 0;
}

static void lidbg_uevent_poll(bool (*uevent_callback)(int fd))
{
    struct pollfd ufd;
    int fd, nr;

    lidbg( "SUC,FUTENGFEI UEVENT INIT\n");

    fd = uevent_open_socket(64 * 1024, true);
    if (fd >= 0)
    {
        lidbg( "suc,uevent_open_socket\n");
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
        lidbg( "err,uevent_open_socket\n");

}

void *thread_wait_userver(void *arg)
{
    sleep(1);
    pthread_detach(pthread_self());
    system("insmod /flysystem/lib/out/lidbg_loader.ko");
    system("insmod /system/lib/modules/out/lidbg_loader.ko");
    pthread_exit(0);
    return NULL;
}
int main(int argc, char **argv)
{
    pthread_t lidbg_uevent_tid;
	DUMP_BUILD_TIME_FILE;
    system("mkdir /data/lidbg");
    system("mkdir /data/lidbg_osd");
    system("chmod 777 /data/lidbg");
    system("chmod 777 /data/lidbg_osd");
	
    system("insmod /system/lib/modules/out/lidbg_uevent.ko");
    system("insmod /flysystem/lib/out/lidbg_uevent.ko");
	sleep(1);
	system("chmod 777 /dev/lidbg_uevent");
    usleep(50 * 1000);
    pthread_create(&lidbg_uevent_tid, NULL, thread_wait_userver, NULL);
    lidbg_uevent_poll(lidbg_uevent_callback);
    return 0;
}

