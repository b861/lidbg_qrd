
#include "lidbg.h"

//zone below [tools]
#define DEV_NAME "lidbg_uevent"
struct miscdevice lidbg_uevent_device;
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


void lidbg_send_uevent(enum kobject_action action, char *envp_ext[])
{
    if(kobject_uevent_env(&lidbg_uevent_device.this_device->kobj, action, envp_ext) < 0)
        LIDBG_ERR("%s,%s\n", (envp_ext[0] == NULL ? "null" : envp_ext[0]), (envp_ext[1] == NULL ? "null" : envp_ext[1]));
}
void lidbg_uevent_shell(char *shell_cmd)
{
    char shellstring[256];
    char *envp[] = { "LIDBG_ACTION=shell", shellstring, NULL };
    snprintf(shellstring, 256, "LIDBG_PARAMETER=%s", shell_cmd );
    lidbg_send_uevent(KOBJ_CHANGE, envp);
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
    if(!strcmp(param[0], "uevent") )
    {
        lidbg_uevent_shell(param[1]);
    }
    if(!strcmp(param[0], "appmsg") )
    {
        printk("[appmsg]:%s\n", param[1]);
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
int thread_wait_userver_ready(void *data)
{
    msleep(500);
    lidbg_uevent_shell("insmod /flysystem/lib/out/lidbg_loader.ko");
    lidbg_uevent_shell("insmod /system/lib/modules/out/lidbg_loader.ko");
    return 0;
}

static int __init lidbg_uevent_init(void)
{
    LIDBG_WARN("<==IN==>\n");

    if (misc_register(&lidbg_uevent_device))
        LIDBG_ERR("misc_register\n");
    else
        LIDBG_SUC("misc_register\n");
    kthread_run(thread_wait_userver_ready, NULL, "ftf_userver_wait");
    LIDBG_WARN("<==OUT==>\n\n");
    return 0;
}

static void __exit lidbg_uevent_exit(void)
{
    misc_deregister(&lidbg_uevent_device);
}


EXPORT_SYMBOL(lidbg_send_uevent);
EXPORT_SYMBOL(lidbg_uevent_shell);

module_init(lidbg_uevent_init);
module_exit(lidbg_uevent_exit);

MODULE_DESCRIPTION("futengfei 2014.3.8");
MODULE_LICENSE("GPL");

