
#include "lidbg.h"
int led_en ;
void led_on(void)
{
    LED_ON;
    msleep(1000);
    LED_OFF;
    msleep(1000);
}


int thread_led(void *data)
{
    FS_REGISTER_INT(led_en, "led_en", 1, NULL);
    if(led_en)
    while(1)
    {
            led_on();
    }
    return 0;
}

