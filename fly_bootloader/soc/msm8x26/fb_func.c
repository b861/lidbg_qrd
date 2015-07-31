#include <fbcon.h>
#include "../soc.h"

extern struct fbcon_config *config;

void *fb_base_get()
{
	return config->base;
}
