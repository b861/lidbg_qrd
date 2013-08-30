#ifndef _LIGDBG_UART__
#define _LIGDBG_UART__

#define lidbg_io(fmt,...) do{SOC_IO_Uart_Send(IO_UART_DELAY_245_115200,fmt,##__VA_ARGS__);}while(0)

void soc_io_uart_cfg(u32 baud);
void soc_io_uart_send(u32 baud, char *printChar);
void lidbg_uart_main(int argc, char **argv);
void soc_io_uart_send_byte(u8 input);

#endif
