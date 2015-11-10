#include "../soc.h"

extern void dsi83_init();

void flyaudio_hw_init(void)
{
    dprintf(INFO, "Flyaudio hardware init. \n");
    dsi83_init();
}
