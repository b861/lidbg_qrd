#include "../soc.h"

void gpio_set_direction(int pin, int dir)
{
    int group = 1;
    int val = 1;
	if(pin ==6 || pin ==7) group = 0;
    if(dir == GPIO_INPUT)
        GetPortInput(group, pin);
    else
        SetPortOutput(group, pin, val);
}

void gpio_set_val(int pin, int val)
{
    int group = 1;
	if(pin ==6 || pin ==7) group = 0;
    SetPortOutput(group, pin, val);
}

int gpio_get_val(int pin)
{
    int group = 1;
    static key_config io_config;

	if(pin ==6 || pin ==7) group = 0;
    /* set key type to gpio */
    io_config.type = KEY_GPIO;
    io_config.key.gpio.valid = 1;
    io_config.key.gpio.group = group;
    io_config.key.gpio.index = pin;
    setup_gpio(&io_config.key.gpio);

    GetPortInput(group, pin);

    return GetPortState(&io_config);
}

