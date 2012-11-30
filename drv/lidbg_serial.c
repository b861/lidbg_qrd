
#include "lidbg.h"
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <plat/serial.h>
//#include <mach/serial.h>




//#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)  //���ճ�ʱ
//#define TIMEOUT_USEC 0

//export from serial_core.c


/*******************************************
 *  ��ö˿�����
********************************************/
char *get_ptty(int port)
{
    char *ptty;

    switch(port)
    {
    case 0:
    {
        ptty = TTY_DEV"0";
    }
    break;
    case 1:
    {
        ptty = TTY_DEV"1";
    }
    break;
    case 2:
    {
        ptty = TTY_DEV"2";
    }
    break;
    case 3:
    {
        ptty = TTY_DEV"3";
    }
    break;
    case 4:
    {
        ptty = TTY_DEV"4";
    }
    break;
    case 5:
    {
        ptty = TTY_DEV"5";
    }
    break;
    case 6:
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


int  lidbg_serial_set(int port, int baud, int parity, int bits)
{
    soc_serial_set_tty( port, baud,  parity,  bits);

}


/*******************************************
 *  Setup comm attr
 *  fdcom: �����ļ���������pportinfo: �����õĶ˿���Ϣ����ȷ�ϣ�
 *
********************************************/
int lidbg_serial_main(int argc, char **argv)
{



}

/*******************************************
 *  Open serial port
 *  tty: �˿ں� ttyS0, ttyS1, ....
 *  ����ֵΪ�����ļ�������
********************************************/
int lidbg_serial_open(int port)
{
    struct file *fdcom;   //�����ļ�������
    char *ptty;

    ptty = get_ptty(port);
    //fdcom = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
    //fdcom = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK);
    fdcom = filp_open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
    return (fdcom);
}

/*******************************************
 *  Close serial port
********************************************/
void lidbg_serial_close(struct file *fdcom)
{
    //close(fdcom);
    filp_close(fdcom, 0);
}




/********************************************
 *  send data
 *  fdcom: ������������data: ���������ݣ�datalen: ���ݳ���
 *  ����ʵ�ʷ��ͳ���
*********************************************/
int lidbg_serial_send(struct file *fdcom, char *data, int datalen)
{
#if 1
    int len = 0;
    mm_segment_t old_fs;
    BEGIN_KMEM;
    //len = write(fdcom, data, datalen);  //ʵ��д��ĳ���
    len = fdcom->f_op->write(fdcom, data, datalen, 0); //ʵ��д��ĳ���
    END_KMEM;

    if(len == datalen)
    {
        return (len);
    }
    else
    {
        //tcflush(fdcom, TCOFLUSH);
        lidbg("PortSend fail!\n");
        return -1;
    }

#endif

    return 0;
}

/*******************************************
 *  receive data
 *  ����ʵ�ʶ�����ֽ���
 *
********************************************/
int lidbg_serial_recv(struct file *fdcom, char *data, int datalen, int baudrate)
{

#if 1
    int readlen;
    mm_segment_t old_fs;

    BEGIN_KMEM;

    readlen = fdcom->f_op->read(fdcom, data, datalen, 0);
    return(readlen);


    END_KMEM;

    return (readlen);
#endif
    return (0);
}

MODULE_AUTHOR("lsw, < @gmail.com>");
MODULE_DESCRIPTION("Serial Driver");
MODULE_LICENSE("GPL");


EXPORT_SYMBOL(lidbg_serial_set);
EXPORT_SYMBOL(lidbg_serial_send);
EXPORT_SYMBOL(lidbg_serial_recv);
EXPORT_SYMBOL(lidbg_serial_main);


