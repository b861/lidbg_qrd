#ifndef _LIGDBG_SERIAL__
#define _LIGDBG_SERIAL__

int  lidbg_serial_set(int port, int baud, int parity, int bits);
int lidbg_serial_send(struct file *fdcom, char *data, int datalen);
int lidbg_serial_recv(struct file *fdcom, char *data, int datalen, int baudrate);
int lidbg_serial_main(int argc, char **argv);


#endif
