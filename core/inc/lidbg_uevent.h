#ifndef _LIGDBG_UEVENT__
#define _LIGDBG_UEVENT__

extern void lidbg_uevent_send(enum kobject_action action, char *envp_ext[]);
extern void lidbg_uevent_shell(char *shell_cmd);
extern bool lidbg_uevent_focus(char *focus, void(*callback)(char *focus,char *uevent));

#endif

