#ifndef _LIGDBG_SERVICER__
#define _LIGDBG_SERVICER__




#define SERVICER_DONOTHING  (0)
#define LOG_DMESG  (1)
#define LOG_LOGCAT (2)
#define LOG_ALL (3)
#define LOG_CONT    (4)

void lidbg_servicer_main(int argc, char **argv);

void k2u_write(int cmd);
int k2u_read(void);
int u2k_read(void);


#endif

