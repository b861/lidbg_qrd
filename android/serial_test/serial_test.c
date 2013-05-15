

/*���������GPL��Լ
MyCom.c
*/


#include <stdio.h>              // printf
#include <fcntl.h>              // open
#include <string.h>             // bzero
#include <stdlib.h>             // exit
#include <sys/times.h>          // times
#include <sys/types.h>          // pid_t
#include <termios.h>            //termios, tcgetattr(), tcsetattr()
#include <unistd.h>
#include <sys/ioctl.h>          // ioctl
#include "serial_test.h"



#define TTY_DEV "/dev/tcc-uart" //�˿�·��   

#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)  //���ճ�ʱ   
#define TIMEOUT_USEC 0
/*******************************************
 *  ��ö˿�����
********************************************/
char *get_ptty(pportinfo_t pportinfo)
{
    char *ptty = NULL;

    switch(pportinfo->tty)
    {
    case '0':
    {
        ptty = TTY_DEV"0";
    }
    break;
    case '1':
    {
        ptty = TTY_DEV"1";
    }
    break;
    case '2':
    {
        ptty = TTY_DEV"2";
    }
    break;

    case '3':
    {
        ptty = TTY_DEV"3";
    }
    break;

    case '4':
    {
        ptty = TTY_DEV"4";
    }
    break;

    case '5':
    {
        ptty = TTY_DEV"5";
    }
    break;

    case '6':
    {
        ptty = TTY_DEV"6";
    }
    break;
    }

    return(ptty);
}

/*******************************************
 *  ������ת����������ȷ���Ƿ���ȷ��
********************************************/
int convbaud(unsigned long int baudrate)
{
    switch(baudrate)
    {
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    default:
        return B9600;
    }
}

/*******************************************
 *  Setup comm attr
 *  fdcom: �����ļ���������pportinfo: �����õĶ˿���Ϣ����ȷ�ϣ�
 *
********************************************/
int PortSet(int fdcom, const pportinfo_t pportinfo)
{
    struct termios termios_old, termios_new;
    int     baudrate, tmp;
    char    databit, stopbit, parity, fctl;

    bzero(&termios_old, sizeof(termios_old));
    bzero(&termios_new, sizeof(termios_new));
    cfmakeraw(&termios_new);
    tcgetattr(fdcom, &termios_old);         //get the serial port attributions
    /*------------���ö˿�����----------------*/
    //baudrates
    baudrate = convbaud(pportinfo -> baudrate);
    cfsetispeed(&termios_new, baudrate);        //���봮������˵Ĳ�����
    cfsetospeed(&termios_new, baudrate);        //���봮������˵Ĳ�����
    termios_new.c_cflag |= CLOCAL;          //����ģʽ����֤���򲻻��Ϊ�˿ڵ�ռ����
    termios_new.c_cflag |= CREAD;           //����ģʽ��ʹ�ܶ˿ڶ�ȡ���������

    // ����ģʽ��flow control
    fctl = pportinfo-> fctl;
    switch(fctl)
    {
    case '0':
    {
        termios_new.c_cflag &= ~CRTSCTS;        //no flow control
    }
    break;
    case '1':
    {
        termios_new.c_cflag |= CRTSCTS;         //hardware flow control
    }
    break;
    case '2':
    {
        termios_new.c_iflag |= IXON | IXOFF | IXANY; //software flow control
    }
    break;
    }

    //����ģʽ��data bits
    termios_new.c_cflag &= ~CSIZE;      //����ģʽ�������ַ���Сλ
    databit = pportinfo -> databit;
    switch(databit)
    {
    case '5':
        termios_new.c_cflag |= CS5;
    case '6':
        termios_new.c_cflag |= CS6;
    case '7':
        termios_new.c_cflag |= CS7;
    default:
        termios_new.c_cflag |= CS8;
    }

    //����ģʽ parity check
    parity = pportinfo -> parity;
    switch(parity)
    {
    case '0':
    {
        termios_new.c_cflag &= ~PARENB;     //no parity check
    }
    break;
    case '1':
    {
        termios_new.c_cflag |= PARENB;      //odd check
        termios_new.c_cflag &= ~PARODD;
    }
    break;
    case '2':
    {
        termios_new.c_cflag |= PARENB;      //even check
        termios_new.c_cflag |= PARODD;
    }
    break;
    }

    //����ģʽ��stop bits
    stopbit = pportinfo -> stopbit;
    if(stopbit == '2')
    {
        termios_new.c_cflag |= CSTOPB;  //2 stop bits
    }
    else
    {
        termios_new.c_cflag &= ~CSTOPB; //1 stop bits
    }

    //other attributions default
    termios_new.c_oflag &= ~OPOST;          //���ģʽ��ԭʼ�������
    termios_new.c_cc[VMIN]  = 1;            //�����ַ�, ��Ҫ��ȡ�ַ�����С����
    termios_new.c_cc[VTIME] = 1;            //�����ַ�, ��ȡ��һ���ַ��ĵȴ�ʱ��    unit: (1/10)second

    termios_new.c_lflag &= ~(ICANON | ECHO | ECHOE);//lsw ��������ֱ�ӷ���,����Ҫ�س��ͻ��з���
    tcflush(fdcom, TCIFLUSH);               //��������ݿ��Խ��գ�������
    tmp = tcsetattr(fdcom, TCSANOW, &termios_new);  //���������ԣ�TCSANOW�����иı�������Ч    tcgetattr(fdcom, &termios_old);
    return(tmp);
}

/*******************************************
 *  Open serial port
 *  tty: �˿ں� ttyS0, ttyS1, ....
 *  ����ֵΪ�����ļ�������
********************************************/
int PortOpen(pportinfo_t pportinfo)
{
    int fdcom;  //�����ļ�������
    char *ptty;

    ptty = get_ptty(pportinfo);

    /*
    O_NONBLOCK��O_NDELAY�������Ľ������ʹI/O��ɷǸ���ģʽ(non-blocking)���ڶ�ȡ�������ݻ���д�뻺��������������return����������ó�������ֱ�������ݻ�д����ɡ�


    ������Ҫע����ǣ���GNU C��O_NDELAYֻ��Ϊ����BSD�ĳ�����ݣ�ʵ������ʹ��O_NONBLOCK��Ϊ�궨�壬����O_NONBLOCK������ioctl��ʹ�ã���������openʱ�趨��

    APPENDED:
    ���û�����ݣ���ô�õ��ý�������.���ڵȴ�״̬��ֱ�����ַ����룬
        ���ߵ��˹涨��ʱ�޺ͳ��ִ���Ϊֹ,
        ͨ�����·�������ʹread�����������ء�

        fcntl(fd,F_SETFL,FNDELAY);

        FNDELAY ����ʹread�����ڶ˿�û���ַ����ڵ�����£����̷���0,
        ���Ҫ�ָ�����(����)״̬,���Ե���fcntl()��������ҪFNDELAY����,
        ������ʾ��
            fcntl(Fd,F_SETFL,0);
        ��ʹ��O_NDELAY�����򿪴��пں�ͬ����ʹ���˸ú������á�

        fcntl(fd,F_SETFL,0);

    */

    //fdcom = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
    fdcom = open(ptty, O_RDWR | O_NOCTTY/* |O_NDELAY| O_NONBLOCK*/);

    return (fdcom);
}

/*******************************************
 *  Close serial port
********************************************/
void PortClose(int fdcom)
{
    close(fdcom);
}

/********************************************
 *  send data
 *  fdcom: ������������data: ���������ݣ�datalen: ���ݳ���
 *  ����ʵ�ʷ��ͳ���
*********************************************/
int PortSend(int fdcom, char *data, int datalen)
{
    int len = 0;

    len = write(fdcom, data, datalen);  //ʵ��д��ĳ���
    if(len == datalen)
    {
        return (len);
    }
    else
    {
        tcflush(fdcom, TCOFLUSH);
        return -1;
    }
}

/*******************************************
 *  receive data
 *  ����ʵ�ʶ�����ֽ���
 *
********************************************/
int PortRecv(int fdcom, char *data, int datalen, int baudrate)
{
    int readlen, fs_sel;
    fd_set  fs_read;
    struct timeval tv_timeout;

    int flags;//lsw

    //C/C++ code
    flags = fcntl( fdcom, F_GETFL, 0 );
    fcntl( fdcom, F_SETFL, flags | O_NONBLOCK );
    FD_ZERO(&fs_read);
    FD_SET(fdcom, &fs_read);
    tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
    tv_timeout.tv_usec = TIMEOUT_USEC;

    fs_sel = select(fdcom + 1, &fs_read, NULL, NULL, &tv_timeout);
    if(fs_sel)
    {
        readlen = read(fdcom, data, datalen);
        return(readlen);
    }
    else
    {
        return(-1);
    }

    return (readlen);
}

#if 1
void read_com(int fdcom, int len)
{
    int RecvLen, i;
    char *RecvBuf = (char *)malloc(len);
    fcntl(fdcom, F_SETFL, 0); //����

    for(;;)
    {
        RecvLen = read(fdcom, RecvBuf, len);
        if(RecvLen > 0)
        {
            for(i = 0; i < RecvLen; i++)
            {

                printf("%x ", RecvBuf[i]);
            }

            printf("\n");
        }
        else
        {
            printf("Error: receive error.\n");
        }
    }
}
#endif

int main(int argc, char *argv[])
{
    int fdcom, i, SendLen, RecvLen, baud, len;
    char port;
    struct termios termios_cur;
    char RecvBuf[128];
    portinfo_t portinfo =
    {
        '0',                            // print prompt after receiving
        57600/*115200*/,                // baudrate: 9600
        '8',                            // databit: 8
        '0',                            // debug: off
        '0',                            // echo: off
        '0',                            // flow control: software
        '3',                            // default tty: COM1
        '0',                            // parity: none
        '1',                            // stopbit: 1
        0                          	// reserved
    };

    port = *argv[2];
    baud = strtoul(argv[3], 0, 0);
    printf("port = %c\n", port);
    printf("baud = %d\n", baud);

    portinfo.baudrate = baud;
    portinfo.tty = port;



    fdcom = PortOpen(&portinfo);
    if(fdcom < 0)
    {
        printf("Error: open serial port error.\n");
        exit(1);
    }

    PortSet(fdcom, &portinfo);

    if(!strcmp(argv[1] , "w"))
    {
        printf("send char = %s\n", argv[4]);
        len = strtoul(argv[5], 0, 0);
        printf("len = %d\n", len);

        SendLen = PortSend(fdcom, argv[4], len);

    }
    else
    {
        len = strtoul(argv[4], 0, 0);
        printf("len = %d\n", len);
        read_com(fdcom, len);


    }
    PortClose(fdcom);
    return 0;
}

#if 0
//*************************Test*********************************
int main(int argc, char *argv[])
{
    int fdcom, i, SendLen, RecvLen;
    struct termios termios_cur;
    char RecvBuf[128];
    portinfo_t portinfo =
    {
        '0',                            // print prompt after receiving
        57600/*115200*/,                         // baudrate: 9600
        '8',                            // databit: 8
        '0',                            // debug: off
        '0',                            // echo: off
        '0',                            // flow control: software
        '3',                            // default tty: COM1
        '0',                            // parity: none
        '1',                            // stopbit: 1
        0                          	// reserved
    };


    fdcom = PortOpen(&portinfo);
    if(fdcom < 0)
    {
        printf("Error: open serial port error.\n");
        exit(1);
    }

    PortSet(fdcom, &portinfo);

    if(atoi(argv[1]) == 0)
    {
        //send data

        for(i = 0; i < 100; i++)
        {
            SendLen = PortSend(fdcom, "1234567890", 10);
            if(SendLen > 0)
            {
                printf("No %d send %d data 1234567890.\n", i, SendLen);
            }
            else
            {
                printf("Error: send failed.\n");
            }
            sleep(1);
        }
        PortClose(fdcom);

    }
    else
    {


        for(;;)
        {
            RecvLen = PortRecv(fdcom, RecvBuf, 10, portinfo.baudrate);

            if(RecvLen > 0)
            {
                for(i = 0; i < RecvLen; i++)
                {
                    printf("Receive data No %d is %x.\n", i, RecvBuf[i]);

                }
                printf("Total frame length is %d.\n", RecvLen);

            }
            else
            {
                printf("Error: receive error.\n");
            }
            sleep(2);//2000ms
        }




    }
    return 0;
}
#endif
