#ifndef _LIGDBG_MEM_LOG__
#define _LIGDBG_MEM_LOG__

int lidbg_msg_put( const char *fmt, ... );
int lidbg_msg_get(char *to_file, int out_mode );
void mem_log_main(int argc, char **argv);


#endif
