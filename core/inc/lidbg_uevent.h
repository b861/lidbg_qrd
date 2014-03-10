#ifndef _LIGDBG_UEVENT__
#define _LIGDBG_UEVENT__

extern void lidbg_send_uevent(enum kobject_action action, char *envp_ext[]);
extern void lidbg_uevent_shell(char *shell_cmd);

#endif

