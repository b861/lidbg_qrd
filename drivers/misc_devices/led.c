
#include "lidbg.h"

void led_on(void)
{
    LED_ON;
    msleep(1000);
    LED_OFF;
    msleep(1000);
}


int thread_led(void *data)
{
    while(1)
    {
            led_on();
    }
    return 0;
}

