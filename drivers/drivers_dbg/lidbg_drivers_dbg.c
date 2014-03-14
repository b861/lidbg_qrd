
#include "lidbg.h"

void lidbg_video_main(int argc, char **argv);

LIDBG_DEFINE;

int drivers_dbg_open (struct inode *inode, struct file *filp)
{
    return 0;
}
ssize_t drivers_dbg_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int pos = 0;
    char *tmp, *tmp_back, *param[20];
    tmp = memdup_user(buf, count);
    if (IS_ERR(tmp))
    {
        FS_ERR("<memdup_user>\n");
        return PTR_ERR(tmp);
    }
    tmp[count - 1] = '\0';
    tmp_back = tmp;
    LIDBG_WARN("%s\n", tmp_back);

    pos = lidbg_token_string(tmp, " ", param);

    if(pos < 2)
    {
        FS_ERR("echo \"c monkey 1 123 0 1 1000 500\" > /dev/lidbg_drivers_dbg0\n");
        goto out;
    }

    //zone start [dosomething]
    if(!strcmp(param[1], "monkey") )
    {
        int enable, gpio, on_en, off_en, on_ms, off_ms;
        enable = simple_strtoul(param[2], 0, 0);
        gpio = simple_strtoul(param[3], 0, 0);
        on_en = simple_strtoul(param[4], 0, 0);
        off_en = simple_strtoul(param[5], 0, 0);
        on_ms = simple_strtoul(param[6], 0, 0);
        off_ms = simple_strtoul(param[7], 0, 0);
        monkey_run(enable);
        monkey_config(gpio, on_en, off_en, on_ms, off_ms);
    }
    else if(!strcmp(param[1], "video"))
    {
        lidbg_video_main(pos - 2, &param[2]);
    }
    //zone end

out:
    kfree(tmp_back);
    return count;
}

static  struct file_operations drivers_dbg_nod_fops =
{
    .owner = THIS_MODULE,
    .write = drivers_dbg_write,
    .open = drivers_dbg_open,
};
int thread_drivers_dbg_init(void *data)
{
    lidbg_new_cdev(&drivers_dbg_nod_fops, "lidbg_drivers_dbg");
    return 0;
}

int lidbg_drivers_dbg_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;

    CREATE_KTHREAD(thread_drivers_dbg_init, NULL);

    return 0;

}
void lidbg_drivers_dbg_deinit(void)
{
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_drivers_dbg_init);
module_exit(lidbg_drivers_dbg_deinit);


