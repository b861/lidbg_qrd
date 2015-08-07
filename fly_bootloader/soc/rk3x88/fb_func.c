#include "../soc.h"

void fb_config()
{
	config->width = 1024;
	config->height = 600;
	config->bpp = 24;
	config->base = (void *) gd->fb_base;
}

void *fb_base_get()
{
	fb_config();
	return gd->fb_base;
}
