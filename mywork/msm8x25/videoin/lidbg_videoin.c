

#include "lidbg.h"







void lidbg_video_main(int argc, char **argv)
{


}



int lidbg_video_init(void)
{
	return 0;

}

int lidbg_video_deinit(void)
{
	return 0;

}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

EXPORT_SYMBOL(lidbg_video_main);


module_init(lidbg_video_init);
module_exit(lidbg_video_deinit);



