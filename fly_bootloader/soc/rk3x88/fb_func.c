#include "../soc.h"

struct fbcon_config *config = NULL;

void fb_config()
{
	config->width = 1024;
	config->height = 600;
	config->base = (void *) gd->fb_base;
}

void *fb_base_get()
{
	fb_config();
	return NULL;
}
