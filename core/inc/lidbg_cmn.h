#ifndef _LIGDBG_CMN__
#define _LIGDBG_CMN__





void mod_cmn_main(int argc, char **argv);

u32 GetNsCount(void);
int cmn_task_kill(char *task_name);
void cmn_launch_user( char bin_path[], char argv1[]);
#endif

