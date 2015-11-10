
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
    int led_en ;
    FS_REGISTER_INT(led_en, "led_en", 1, NULL);
    if(led_en)
    {
        while(1)
        {
            led_on();
        }
    }
    return 0;
}
void led_resume(void)
{
    IO_CONFIG_OUTPUT(0, LED_GPIO);
}

