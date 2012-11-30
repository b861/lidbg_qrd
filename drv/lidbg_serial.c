
#include "lidbg.h"
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <plat/serial.h>
//#include <mach/serial.h>




//#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)  //接收超时
//#define TIMEOUT_USEC 0

//export from serial_core.c


/*******************************************
 *  获得端口名称
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
 *  波特率转换函数（请确认是否正确）
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
 *  fdcom: 串口文件描述符，pportinfo: 待设置的端口信息（请确认）
 *
********************************************/
int lidbg_serial_main(int argc, char **argv)
{



}

/*******************************************
 *  Open serial port
 *  tty: 端口号 ttyS0, ttyS1, ....
 *  返回值为串口文件描述符
********************************************/
int lidbg_serial_open(int port)
{
    struct file *fdcom;   //串口文件描述符
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
 *  fdcom: 串口描述符，data: 待发送数据，datalen: 数据长度
 *  返回实际发送长度
*********************************************/
int lidbg_serial_send(struct file *fdcom, char *data, int datalen)
{
#if 1
    int len = 0;
    mm_segment_t old_fs;
    BEGIN_KMEM;
    //len = write(fdcom, data, datalen);  //实际写入的长度
    len = fdcom->f_op->write(fdcom, data, datalen, 0); //实际写入的长度
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
 *  返回实际读入的字节数
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


