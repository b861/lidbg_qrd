#ifndef _LIGDBG_UEVENT__
#define _LIGDBG_UEVENT__

#define SHELL_ERRS_FILE LIDBG_LOG_DIR"lidbg_shell_errs.txt"
void lidbg_uevent_send(enum kobject_action action, char *envp_ext[]);
void lidbg_uevent_shell(char *shell_cmd);
bool lidbg_uevent_focus(char *focus, void(*callback)(char *focus,char *uevent));
void lidbg_uevent_main(int argc, char **argv);

#endif

