#include "lidbg.h"


static LIST_HEAD(third_party_list);
void third_party_prepare(void)
{
    char buff[50] = {0};
    fs_copy_file(get_lidbg_file_path(buff, "third_party.conf"), LIDBG_LOG_DIR"third_party.txt");
    ssleep(1);
    fs_fill_list(LIDBG_LOG_DIR"third_party.txt", FS_CMD_FILE_LISTMODE, &third_party_list);
    fs_register_filename_list(LIDBG_LOG_DIR"third_party.txt", true);
}
void third_party_analyze(void)
{
    struct string_dev *pos;
    char *cmd[8] = {NULL};
    int cmd_num  = 0, tsleep = 0;

    list_for_each_entry(pos, &third_party_list, tmp_list)
    {
        if(pos->yourkey)
        {
            if(strlen(pos->yourkey) < 3)
                goto drop;

            if(strncmp(pos->yourkey, "msleep", sizeof("msleep") - 1) == 0)
            {
                cmd_num = lidbg_token_string(pos->yourkey, " ", cmd) ;
                if(cmd_num == 2)
                {
                    tsleep = simple_strtoul(cmd[1], 0, 0);
                    msleep(tsleep);
                    LIDBG_WARN("msleep<%d,%d>\n", cmd_num, tsleep);
                }
                else
                    goto drop;
            }
            else if(strncmp(pos->yourkey, "cp", sizeof("cp") - 1) == 0)
            {
                cmd_num = lidbg_token_string(pos->yourkey, " ", cmd) ;
                if(cmd_num == 3)
                {
                    LIDBG_WARN("cp <%s,%s>\n", cmd[1], cmd[2]);
                    fs_copy_file(cmd[1], cmd[2]);
                }
                else
                    goto drop;
            }
            else
                lidbg_uevent_shell(pos->yourkey);
            continue;
drop:
            LIDBG_WARN("bad cmd<%s>\n", pos->yourkey);
        }
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

