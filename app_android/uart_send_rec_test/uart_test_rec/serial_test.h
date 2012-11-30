/*���������GPL��Լ
MyCom.h
һ��������ڵĺ���
*/

//���ڽṹ
typedef struct
{
    char	prompt;		//prompt after reciving data
    int 	baudrate;		//baudrate
    char	databit;		//data bits, 5, 6, 7, 8
    char 	debug;		//debug mode, 0: none, 1: debug
    char 	echo;			//echo mode, 0: none, 1: echo
    char	fctl;			//flow control, 0: none, 1: hardware, 2: software
    char 	tty;			//tty: 0, 1, 2, 3, 4, 5, 6, 7
    char	parity;		//parity 0: none, 1: odd, 2: even
    char	stopbit;		//stop bits, 1, 2
    const int reserved;	//reserved, must be zero
} portinfo_t;
typedef portinfo_t *pportinfo_t;
/*
 *	�򿪴��ڣ������ļ�������
 *	pportinfo: �����õĴ�����Ϣ
*/
int PortOpen(pportinfo_t pportinfo);
/*
 *	���ô���
 *	fdcom: �����ļ��������� pportinfo�� �����õĴ�����Ϣ
*/
int PortSet(int fdcom, const pportinfo_t pportinfo);
/*
 *	�رմ���
 *	fdcom�������ļ�������
*/
void PortClose(int fdcom);
/*
 *	��������
 *	fdcom�������������� data�����������ݣ� datalen�����ݳ���
 *	����ʵ�ʷ��ͳ���
*/
int PortSend(int fdcom, char *data, int datalen);
/*
 *	��������
 *	fdcom�������������� data�����ջ�����, datalen.�����ճ��ȣ� baudrate��������
 *	����ʵ�ʶ���ĳ���
*/
int PortRecv(int fdcom, char *data, int datalen, int baudrate);



int serial_main(int argc, char *argv[]);
