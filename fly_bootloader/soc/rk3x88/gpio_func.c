#include "../soc.h"

void gpio_set_direction(int pin, int dir)
{
    int group = 1;
    int val = 1;

    if(dir == GPIO_INPUT)
        GetPortInput(group, pin);
    else
        SetPortOutput(group, pin, val);
}

void gpio_set_val(int pin, int val)
{
    int group = 1;

    SetPortOutput(group, pin, val);;
}

int gpio_get_val(int pin)
{
    int group = 1;
    static key_config io_config;

    /* set key type to gpio */
    io_config.type = KEY_GPIO;
    io_config.key.gpio.valid = 1;
    io_config.key.gpio.group = 1;
    io_config.key.gpio.index = 28;
    setup_gpio(&io_config.key.gpio);

    GetPortInput(group, pin);

    return GetPortState(&io_config);
}

