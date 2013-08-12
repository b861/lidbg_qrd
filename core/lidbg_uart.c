

#include "lidbg.h"

static u32 delay=0;
static spinlock_t uart_lock;
unsigned long flags_uart_send;

void soc_io_uart_send1 ( u32 baud,const char *fmt, ... );


void soc_io_uart_send1 ( u32 baud,const char *fmt, ... )
{
	va_list args;
	int n;
	char printbuffer[256];

        va_start ( args, fmt );
        n = vsprintf ( printbuffer, (const char *)fmt, args );
        va_end ( args );

        soc_io_uart_send(baud,( unsigned char *)printbuffer);
}

static void Delay(u32 uDelay)
{
	udelay(uDelay);
}

void soc_io_uart_cfg(u32 baud)
{
	TX_CFG;
	TX_H;
	delay = baud;
}

void soc_io_uart_send(u32 baud, char *printChar)
{
	u8 data;
	u8 len, i;
	len = strlen(printChar);
	//printk("\n\n\n*******soc_io_uart_send******\n\n");
	soc_io_uart_cfg(baud);
	
	for(i=0; i<len; i++){
	   	data = (u8)(printChar[i]);
		soc_io_uart_send_byte(data);
	}
	
	soc_io_uart_send_byte(13);   //equal \r
	soc_io_uart_send_byte(10);   // equal \n
}

void soc_io_uart_send_byte(u8 input)
{
    u8 i=8;
	spin_lock_irqsave(&uart_lock, flags_uart_send);
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
    spin_unlock_irqrestore(&uart_lock, flags_uart_send);
}	

void lidbg_uart_main(int argc, char **argv)
{

	if(!strcmp(argv[0], "io_w"))
	{
		u8 data;
		u32 delay;
		u8 len,i;
	//len = strlen(argv[2]);
        delay = simple_strtoul(argv[1], 0, 0);
		soc_io_uart_send1(delay,"%s", argv[2]);
		/*soc_io_uart_cfg(delay);
       		for(i=0; i<len; i++){
	       		data = (u8)(argv[2][i]);
			soc_io_uart_send_byte(data);
			
       		}
		soc_io_uart_send_byte(13);   //equal \r
		soc_io_uart_send_byte(10);   // equal \n*/
	}
}

static int __init io_uart_init(void)
{
    DUMP_BUILD_TIME;
    lidbg("io_uart_init\n");	
    spin_lock_init(&uart_lock);
    return 0;
}

static void __exit io_uart_exit(void)
{
	 lidbg("io_uart_exit\n");
}

module_init(io_uart_init);
module_exit(io_uart_exit);

EXPORT_SYMBOL(soc_io_uart_cfg);
EXPORT_SYMBOL(soc_io_uart_send);
EXPORT_SYMBOL(lidbg_uart_main);
EXPORT_SYMBOL(soc_io_uart_send_byte);

