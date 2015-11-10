#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/resource.h>

#define DEBUG_PRINT_FLAG 1

#define LOG_BYTES   (512)

#define LIDBG_PRINT(msg...) do{\
	int fd;\
	char s[LOG_BYTES];\
	sprintf(s, "hal_msg: " msg);\
	s[LOG_BYTES - 1] = '\0';\
	 fd = open("/dev/lidbg_msg", O_RDWR);\
	 if((fd == 0)||(fd == (int)0xfffffffe)|| (fd == (int)0xffffffff))break;\
	 write(fd, s, /*sizeof(msg)*/strlen(s)/*LOG_BYTES*/);\
	 close(fd);\
}while(0)

struct uartConfig
{
    int fd;
    int ttyReadWrite;
    int baudRate;
    char *portName;

    int nread;	//bytes to read
    int nwrite;	//bytes to write
    int wlen;		//lenght have sent
    int rlen;		//lenght have read
};

static struct uartConfig pUartInfo;

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;

    if ( tcgetattr( fd, &oldtio) != 0)
    {
        printf("SetupSerial .\n");
        return -1;
    }

    bzero( &newtio, sizeof( newtio ) );
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    switch ( nBits )
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch ( nEvent )
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    switch ( nSpeed )
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;
    case 576000:
        cfsetispeed(&newtio, B576000);
        cfsetospeed(&newtio, B576000);
        break;
    case 921600:
        cfsetispeed(&newtio, B921600);
        cfsetospeed(&newtio, B921600);
        break;
    case 3000000:
        cfsetispeed(&newtio, B3000000);
        cfsetospeed(&newtio, B3000000);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    if ( nStop == 1 )
        newtio.c_cflag &= ~CSTOPB;
    else if ( nStop == 2 )

        newtio.c_cflag |= CSTOPB;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(fd, TCIOFLUSH);

    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        printf("com set error\n");
        return -1;
    }

    printf("set done!\n");
    return 0;
}

int open_port(int fd, char *portName)
{
    long vdisable;

    pUartInfo.fd = open( portName, O_RDWR | O_NOCTTY);

    if (-1 == pUartInfo.fd)
    {
        printf("Can't Open Serial Port\n");
        return(-1);
    }
    else
    {
        printf("open %s .....\n", portName);
    }

    if (fcntl(pUartInfo.fd, F_SETFL, 0) < 0)
        printf("fcntl failed!\n");
    else
        printf("fcntl=%d\n", fcntl(pUartInfo.fd, F_SETFL, 0));
    if (isatty(STDIN_FILENO) == 0)
        printf("standard input is not a terminal device\n");
    else
        printf("isatty success!\n");

    return pUartInfo.fd;
}

int main(int argc , char **argv)
{

    static pthread_t readTheadId;
    static pthread_t writeTheadId;
    int ret;
    int i = 0;
    char data[5] = {0};

    pUartInfo.wlen = 0;
    /*
    	setpriority(PRIO_PROCESS, getpid(), -20);
    	struct sched_param sp = {90};
    	if (sched_setscheduler(0,SCHED_FIFO, &sp) < 0)
    	{
    		printf(" Error !! \n");
    	}
    */
    if(argc < 3)
    {
        printf("uaer:%s portName baudRate data\n", argv[0]);
        printf("uaer:example: %s 0 /dev/ttyS0 115200 111\n", argv[0]);
        return -1;
    }

    pUartInfo.portName = argv[1];
    pUartInfo.baudRate = strtoul(argv[2], 0, 0);

    for(i = 0; i < 5; i++)
        data[i] = strtoul(argv[3 + i], 0, 0);

    pUartInfo.fd = -1;

    if(pUartInfo.portName == NULL)
    {
        printf("ERR:tty dev does not exist.\n");
        return -1;
    }

    printf("%s %s %d\n", argv[0], pUartInfo.portName, pUartInfo.baudRate);

    if ((pUartInfo.fd = open_port(pUartInfo.fd, pUartInfo.portName)) < 0)
    {
        printf("open_port error\n");
        return -1;
    }

    if ((i = set_opt(pUartInfo.fd, pUartInfo.baudRate, 8, 'N', 1)) < 0)
    {
        printf("set_opt error\n");
        return -1;
    }

    pUartInfo.nwrite = sizeof(data);

    if((pUartInfo.fd) > 0) //Tx
    {
        pUartInfo.wlen = write(pUartInfo.fd, data, pUartInfo.nwrite);

        //usleep(10 * 1000);
#if DEBUG_PRINT_FLAG
        LIDBG_PRINT("sent:%d  0x%x 0x%x 0x%x 0x%x 0x%x\n", pUartInfo.wlen, data[0], data[1], data[2], data[3], data[4]);
#endif
    }

    close(pUartInfo.fd);

    return 0;
}



