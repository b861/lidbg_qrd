#ifndef _LIGDBG_CMN__
#define _LIGDBG_CMN__





void mod_cmn_main(int argc, char **argv);

u32 GetNsCount(void);

//extern struct lidbg_dev *lidbg_devp; /*Éè±¸½á¹¹ÌåÖ¸Õë*/
extern void *global_lidbg_devp; /*Éè±¸½á¹¹ÌåÖ¸Õë*/
#endif

