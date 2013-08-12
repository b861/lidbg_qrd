

#include "lidbg.h"

static u32 delay=0;

static void Delay()
{
	udelay(delay);
}

void soc_io_uart_cfg(u32 baud)
{
	TX_CFG;
	delay = baud;
}

void soc_io_uart_send(u8 input)
{
    u8 i=8;
    TX_L;
    Delay(delay);//start bit
    while(i--)
    {
        if(input&0x01)TX_H;else TX_L; //send bit
        Delay(delay);
        input=input>>1;
    }
    TX_H;
    Delay(delay*2);//stop bit
}


void lidbg_uart_main(int argc, char **argv)
{

	if(!strcmp(argv[0], "io_w"))
	{
		u8 data;
		u32 delay;
        delay = simple_strtoul(argv[1], 0, 0);
		soc_io_uart_cfg(delay);
        data = simple_strtoul(argv[2], 0, 0);
		soc_io_uart_send(data);
	}
}

EXPORT_SYMBOL(soc_io_uart_cfg);
EXPORT_SYMBOL(soc_io_uart_send);
EXPORT_SYMBOL(lidbg_uart_main);

