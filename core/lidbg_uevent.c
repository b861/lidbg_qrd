
#include "lidbg.h"

//zone below [tools]
#define DEV_NAME "lidbg_uevent"
struct uevent_dev
{
    struct list_head tmp_list;
    char *focus;
    void (*callback)(char *focus, char *uevent);
};

LIST_HEAD(uevent_list);
static struct completion uevent_wait;
struct miscdevice lidbg_uevent_device;
static char last_uevent[1024] = {0};
static int uevent_dbg = 0;
//zone end

/*
input:  c io w 27 1
output:  token[0]=c  token[4]=1  return:5
*/
static int token_string(char *buf, char *separator, char **token)
{
    char *token_tmp;
    int pos = 0;
    if(!buf || !separator)
    {
        LIDBG_ERR("buf||separator NULL?\n");
        return pos;
    }
    while((token_tmp = strsep(&buf, separator)) != NULL )
    {
        *token = token_tmp;
        token++;
        pos++;
    }
    return pos;
}
bool uevent_focus(char *focus, void(*callback)(char *focus, char *uevent))
{
    struct uevent_dev *add_new_dev=NULL;
    add_new_dev = kzalloc(sizeof(struct uevent_dev), GFP_KERNEL);
    if (add_new_dev != NULL && focus && callback)
    {
        add_new_dev->focus = focus;
        add_new_dev->callback = callback;
        list_add(&(add_new_dev->tmp_list), &uevent_list);
        LIDBG_SUC("%s\n", focus);
        return true;
    }
    LIDBG_ERR("add_new_dev != NULL && focus && callback?\n");
    return false;
}
void uevent_send(enum kobject_action action, char *envp_ext[])
{
    if(kobject_uevent_env(&lidbg_uevent_device.this_device->kobj, action, envp_ext) < 0)
        LIDBG_ERR("%s,%s\n", (envp_ext[0] == NULL ? "null" : envp_ext[0]), (envp_ext[1] == NULL ? "null" : envp_ext[1]));
}
void uevent_shell(char *shell_cmd)
{
    char shellstring[256];
    char *envp[] = { "LIDBG_ACTION=shell", shellstring, NULL };
    snprintf(shellstring, 256, "LIDBG_PARAMETER=%s", shell_cmd );
    lidbg_uevent_send(KOBJ_CHANGE, envp);
}

static int thread_check_uevent_focus(void *data)
{
    struct list_head *client_list = &uevent_list;
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    while(!kthread_should_stop())
    {
        if( !wait_for_completion_interruptible(&uevent_wait) && !list_empty(client_list))
        {
            struct uevent_dev *pos;
            list_for_each_entry(pos, client_list, tmp_list)
            {
                if(uevent_dbg)
                    LIDBG_WARN("INFO: %s  %ps\n", pos->focus, pos->callback);
                if (pos->focus && pos->callback && strstr(last_uevent, pos->focus))
                    pos->callback(pos->focus, last_uevent);
            }
        }
    }
    return 1;
}


int lidbg_uevent_open(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t  lidbg_uevent_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
    int pos = 0;
    char *tmp, *tmp_back, *param[20];
    tmp = memdup_user(buf, count);
    if (IS_ERR(tmp))
    {
        LIDBG_ERR("<memdup_user>\n");
        return PTR_ERR(tmp);
    }
    tmp[count - 1] = '\0';
    tmp_back = tmp;

    pos = token_string(tmp, "*", param);

    if(pos < 2)
    {
        LIDBG_ERR("echo uevent*setprop futengfei.debug 1 > /dev/lidbg_uevent\n");
        LIDBG_ERR("%s\n", tmp_back);
        goto out;
    }

    //zone start [dosomething]
    if(!strcmp(param[0], "dbg") )
        uevent_dbg = !uevent_dbg;
    else if(!strcmp(param[0], "uevent") )
        lidbg_uevent_shell(param[1]);
    else if(!strcmp(param[0], "appmsg") )
    {
        printk("[appmsg]:%s\n", param[1]);
    }
    else if(!strcmp(param[0], "systemuevent") )
    {
        strcpy(last_uevent, param[1]);
        if(uevent_dbg)
            printk("==========%s\n", last_uevent);
        complete(&uevent_wait);
    }
    //zone end
out:
    kfree(tmp_back);
    return count;
}

static const struct file_operations lidbg_uevent_fops =
{
    .owner = THIS_MODULE,
    .open = lidbg_uevent_open,
    .write = lidbg_uevent_write,
};

struct miscdevice lidbg_uevent_device =
{
    .minor = 255,
    .name = DEV_NAME,
    .fops = &lidbg_uevent_fops,
};

static int __init lidbg_uevent_init(void)
{
    LIDBG_WARN("<==IN==>\n");

    init_completion(&uevent_wait);

    if (misc_register(&lidbg_uevent_device))
        LIDBG_ERR("misc_register\n");
    else
        LIDBG_SUC("misc_register\n");

    kthread_run(thread_check_uevent_focus, NULL, "ftf_uevent");

    LIDBG_WARN("<==OUT==>\n\n");
    return 0;
}

static void __exit lidbg_uevent_exit(void)
{
    misc_deregister(&lidbg_uevent_device);
}


//zone below [interface]
bool lidbg_uevent_focus(char *focus, void(*callback)(char *focus, char *uevent))
{
    return uevent_focus(focus, callback);
}
void lidbg_uevent_send(enum kobject_action action, char *envp_ext[])
{
    uevent_send(action, envp_ext);
}
void lidbg_uevent_shell(char *shell_cmd)
{
    uevent_shell(shell_cmd);
}
//zone end

EXPORT_SYMBOL(lidbg_uevent_focus);
EXPORT_SYMBOL(lidbg_uevent_send);
EXPORT_SYMBOL(lidbg_uevent_shell);

module_init(lidbg_uevent_init);
module_exit(lidbg_uevent_exit);

MODULE_DESCRIPTION("futengfei 2014.3.8");
MODULE_LICENSE("GPL");

