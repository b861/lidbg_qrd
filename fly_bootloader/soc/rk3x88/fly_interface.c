#include "soc.h"
#include "fly_platform.h"

#if DEVICE_TREE
BUF_DMA_ALIGN(dt_buf, 4096);
#endif

static unsigned char buf[4096]; //Equal to max-supported pagesize

int boot_flyrecovery_from_mmc()
{

}

int logo_addr_get(unsigned char **partition_add, int partition_add_len,
					unsigned char **rgb565_add, int rgb565_add_len,
					unsigned char **rgb888_add, int rgb888_add_len,  int *need_alloc_flag)
{
	unsigned n = 0;

	*need_alloc_flag = 1;
	if(*need_alloc_flag)
		dprintf(INFO,"*** Need to alloc space for logo.\n");
	else
		dprintf(INFO,"*** No need to alloc space for logo.\n");


	n = ROUND_TO_PAGE(partition_add_len, page_mask);

	*partition_add = (unsigned char*)malloc(n*4);
	if (NULL == *partition_add)
	{
		printf("Fail: malloc for partition_add !\n");
		goto err_out;
	}
	memset(*partition_add,0,partition_add_len);

	*rgb565_add = (unsigned char*)malloc(rgb565_add_len);
	if (NULL == *rgb565_add)
	{
		printf("Fail: malloc for rgb565_add !\n");
		goto err_out;
	}

	*rgb888_add = (unsigned char*)malloc(rgb888_add_len);
	if (NULL == *rgb888_add)
	{
		printf("Fail: malloc for rgb888_add !\n");
		goto err_out;
	}

	return 0;

err_out:
	if (NULL != *partition_add)
	{
		free(*partition_add);
		*partition_add = NULL;
	}

	if (NULL != *rgb565_add)
	{
		free(rgb565_add);
		*rgb565_add = NULL;
	}

	if (NULL != *rgb888_add)
	{
		free(*rgb888_add);
		*rgb888_add = NULL;
	}
	
	return -1;
}

char *dbg_msg_en(const char *system_cmd, int dbg_msg_en)
{

}
