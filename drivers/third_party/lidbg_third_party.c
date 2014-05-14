#include "lidbg.h"


static LIST_HEAD(third_party_list);
void third_party_prepare(void)
{
    char buff[50]={0};
    fs_copy_file(get_lidbg_file_path(buff,"third_party.conf"), LIDBG_LOG_DIR"third_party.txt");
    ssleep(1);
    fs_fill_list(LIDBG_LOG_DIR"third_party.txt", FS_CMD_FILE_LISTMODE, &third_party_list);
    fs_register_filename_list(LIDBG_LOG_DIR"third_party.txt", true);
}
void third_party_analyze(void)
{
    struct string_dev *pos;
    char *p = NULL;
    list_for_each_entry(pos, &third_party_list, tmp_list)
    {
        if(pos->yourkey)
        {
            p = strchr(pos->yourkey, ',');
            if(!p)
            {
                LIDBG_ERR("droped:%s\n", pos->yourkey);
                continue;
            }

            p[0] = '\0';

            pos->yourvalue = ++p;
            lidbg_insmod(pos->yourkey);
			msleep(100);

            if(pos->yourvalue && strchr(pos->yourvalue, '/'))
            {
                msleep(300);
                lidbg_chmod(pos->yourvalue);
            }

        }
        p = NULL;
    }
}


static int thread_third_party_analyze(void *data)
{
    third_party_prepare();
    third_party_analyze();
    return 0;
}
static int __init lidbg_third_party_init(void)
{

    DUMP_FUN;
    LIDBG_WARN("<==IN==>\n");
    CREATE_KTHREAD(thread_third_party_analyze, NULL);
    LIDBG_MODULE_LOG;
    LIDBG_WARN("<==OUT==>\n\n");
    return 0;
}
static void __exit lidbg_third_party_exit(void)
{
}

module_init(lidbg_third_party_init);
module_exit(lidbg_third_party_exit);

MODULE_AUTHOR("futengfei");
MODULE_DESCRIPTION("for hal or other group to insmod their KO or something else,2014.5.12");
MODULE_LICENSE("GPL");

