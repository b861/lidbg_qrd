
#include "lidbg.h"


LIDBG_DEFINE;

int test_nod_open (struct inode *inode, struct file *filp)
{
    return 0;
}
ssize_t test_nod_write (struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    int pos = 0;
    char *tmp, *tmp_back, *param[20];
    tmp = memdup_user(buf, count);
    if (IS_ERR(tmp))
    {
        FS_ERR("<memdup_user>\n");
        return PTR_ERR(tmp);
    }
    tmp[count-1]='\0';
    tmp_back = tmp;
    FS_WARN("%s\n", tmp_back);

    pos = lidbg_token_string(tmp," ", param);

    if(pos < 8)
    {
        FS_ERR("echo \"c monkey 1 123 0 1 1000 500\" > /dev/test_node0\n");
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
        monkey_enable(enable, gpio, on_en, off_en, on_ms, off_ms);
    }
    //zone end
out:
    kfree(tmp_back);
    return count;
}

static  struct file_operations test_nod_fops =
{
    .owner = THIS_MODULE,
    .write = test_nod_write,
    .open = test_nod_open,
};
int thread_test(void *data)
{
    lidbg_new_cdev(&test_nod_fops, "test_node");
    return 0;
}



int lidbg_test_init(void)
{
    DUMP_BUILD_TIME;
    LIDBG_GET;

    CREATE_KTHREAD(thread_test, NULL);

    return 0;

}
void lidbg_test_deinit(void)
{


}



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Flyaudad Inc.");

module_init(lidbg_test_init);
module_exit(lidbg_test_deinit);

