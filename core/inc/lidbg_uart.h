#ifndef _LIGDBG_UART__
#define _LIGDBG_UART__

void soc_io_uart_cfg(u32 baud);
void soc_io_uart_send(u8 input);
void lidbg_uart_main(int argc, char **argv);


#endif
