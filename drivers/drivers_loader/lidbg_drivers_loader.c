#include "lidbg.h"

static LIST_HEAD(lidbg_list);
static LIST_HEAD(flyaudio_list);

struct judgment
{
    char *key;
    int value;
} judgment_list[] =
{
    {"PLFID", -1},
    {"Factory", -1},
};
void judgment_list_print(void)
{
    int cunt, size;
    size = ARRAY_SIZE(judgment_list);
    for(cunt = 0; cunt < size; cunt++)
    {
        LIDBG_WARN("judgment:%d<%s,%d>\n", cunt, judgment_list[cunt].key , judgment_list[cunt].value);
    }
}
void judgment_list_init(void)
{
#ifdef PLATFORM_ID_0
    judgment_list[0].value = 0;
#endif
#ifdef PLATFORM_ID_1
    judgment_list[0].value = 1;
#endif
#ifdef PLATFORM_ID_2
    judgment_list[0].value = 2 ;
#endif
#ifdef PLATFORM_ID_3
    judgment_list[0].value = 3 ;
#endif
#ifdef PLATFORM_ID_4
    judgment_list[0].value = 4 ;
#endif
#ifdef PLATFORM_ID_5
    judgment_list[0].value = 5 ;
#endif
#ifdef PLATFORM_ID_6
    judgment_list[0].value = 6;
#endif
#ifdef PLATFORM_ID_7
    judgment_list[0].value = 7;
#endif
#ifdef PLATFORM_ID_8
    judgment_list[0].value = 8;
#endif
#ifdef PLATFORM_ID_9
    judgment_list[0].value = 9;
#endif
#ifdef PLATFORM_ID_10
    judgment_list[0].value = 10;
#endif
#ifdef PLATFORM_ID_11
    judgment_list[0].value = 11;
#endif
#ifdef PLATFORM_ID_12
    judgment_list[0].value = 12;
#endif
#ifdef PLATFORM_ID_14
    judgment_list[0].value = 14;
#endif
#ifdef PLATFORM_ID_15
    judgment_list[0].value = 15 ;
#endif
    judgment_list[1].value = is_out_updated ;

    judgment_list_print();
}
int get_judgment_list_value(char *key)
{
    int cunt, size;
    size = ARRAY_SIZE(judgment_list);
    for(cunt = 0; cunt < size; cunt++)
    {
        if(strncmp(key, judgment_list[cunt].key, strlen(judgment_list[cunt].key)) == 0)
            return  judgment_list[cunt].value;
    }
    return -2;
}
//return =-1 means not judgment cmd and should not skip  but else  ... 
int judgment_cmd(char *judgmentcmd, int *judgmenttimes)
{
    //example: ?PLFID=1||Factory=1:belowCmd=3

    // 1:if it is enabled ,just return;
    if(*judgmenttimes > 0)
    {
        LIDBG_WARN("judgment:judgmenttimes = %d <skip.%s>\n", *judgmenttimes, judgmentcmd);
        (*judgmenttimes)--;
        goto err_judgment_cmd;
    };
    // 2:check the cmd
    if(*(judgmentcmd) == '?' && strstr(judgmentcmd, ":") != NULL)
    {
        char * or , * and ;
        LIDBG_WARN("judgment:valid cmd======= <%s>============\n",  judgmentcmd);
        lidbgstrtrim(judgmentcmd);
        LIDBG_WARN("judgment:after lidbgstrtrim <%s>\n", judgmentcmd);
        judgmentcmd += 1; //skip '?'
        or = strstr(judgmentcmd, "||");
        and = strstr(judgmentcmd, "&&");
        if( ( or != NULL && and != NULL) || ( or == NULL && and == NULL))
        {
            LIDBG_WARN("judgment:or ,and == NULL <%s,%d,%d>\n",  judgmentcmd, or == NULL, and == NULL);
            goto err_judgment_cmd;
        }

        lidbg_strrpl(judgmentcmd, or != NULL ? "||" : "&&", or != NULL ? "|" : "&");
        LIDBG_WARN("judgment:after lidbg_strrpl <%s>\n",  judgmentcmd);
        //get judgmenttimes
        {
            char *arg[8] = {NULL};
            char *str1 = strstr(judgmentcmd, ":");
            if(lidbg_token_string(str1 + 1, "=", arg) == 2)
            {
                *str1 = '\0'; //del string below ':'
                *judgmenttimes = simple_strtoul(arg[1], 0, 0);
                LIDBG_WARN("judgment:set judgmenttimes<%s,%d,%d,%d>\n", judgmentcmd, *judgmenttimes, or != NULL, and != NULL);
            }
            else
            {
                LIDBG_WARN("judgment:get judgmenttimes err<%s>\n", judgmentcmd);
                goto err_judgment_cmd;
            }
        }

        //toke judgment string and have a check if it is true
        {
            int loops, tokenvalue, judgmentvalue;
            char *arg[8] = {NULL};
            char *arg2[8] = {NULL};
            int cmd_num = lidbg_token_string(judgmentcmd, or != NULL ? "|" : "&" , arg) ;
            for(loops = 0; loops < cmd_num; loops++)
            {
                LIDBG_WARN("judgment:%d/%d.start toke:<%s>\n", loops, cmd_num, arg[loops]);
                if(arg[loops] && lidbg_token_string(arg[loops], "=", arg2) == 2)
                {
                    judgmentvalue = get_judgment_list_value(arg2[0]);
                    if(judgmentvalue == -2)
                    {
                        LIDBG_WARN("judgment:error unknown judgment <%s>\n", arg2[0]);
                        goto err_judgment_cmd;
                    }
                    tokenvalue = simple_strtoul(arg2[1], 0, 0);

                    LIDBG_WARN("<judgment:%d/%d. check :[%s,%d,%d]>\n", loops, cmd_num, arg2[0], tokenvalue, judgmentvalue);

                    //check
                    if( or != NULL)
                    {
                        if( tokenvalue == judgmentvalue)
                        {
                            LIDBG_WARN("judgment:<judgment OK [||]>\n");
                            goto judgment_cmd_succes;
                        }
                    }
                    else
                    {
                        if( tokenvalue != judgmentvalue)
                        {
                            LIDBG_WARN("judgment:<judgment error [&&]>\n");
                            goto err_judgment_cmd;
                        }
                    }
                }
                else if(arg[loops] != NULL)
                {
                    LIDBG_WARN("judgment:toke judgment string  err<%s>\n", arg[loops]);
                    goto judgment_cmd_succes;
                }
            }
            goto judgment_cmd_succes;
        }
    }
    else
        goto not_judgment_cmd;

err_judgment_cmd:
    return *judgmenttimes;

not_judgment_cmd:
    *judgmenttimes = 0;
    return -1;
judgment_cmd_succes:
    *judgmenttimes = 0;
    return -2;

}

bool analyze_list_cmd(struct list_head *client_list)
{
    struct string_dev *pos;
    char *cmd[8] = {NULL};
    int cmd_num  = 0, tsleep = 0, judgmenttimes = 0;

    if (list_empty(client_list))
    {
        LIDBG_ERR("<list_is_empty>\n");
        return false;
    }

    list_for_each_entry(pos, client_list, tmp_list)
    {
        if(pos->yourkey)
        {
            if(strlen(pos->yourkey) < 3)
                goto drop;

            if(judgment_cmd(pos->yourkey, &judgmenttimes) != -1)
                continue;

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
#ifdef SOC_msm8x25
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
#endif
            else
            {
                msleep(100);
                lidbg_shell_cmd(pos->yourkey);
            }

            continue;
drop:
            LIDBG_WARN("bad cmd<%s>\n", pos->yourkey);
        }
    }

    return true;
}

static int thread_drivers_loader_analyze(void *data)
{
    char buff[50] = {0};
    judgment_list_init();

    if(gboot_mode == MD_FLYSYSTEM)
    {
        LIDBG_WARN("<==gboot_mode==MD_FLYSYSTEM==>\n");
        fs_fill_list(get_lidbg_file_path(buff, "flyaudio.init.rc.conf"), FS_CMD_FILE_LISTMODE, &flyaudio_list);
        analyze_list_cmd(&flyaudio_list);
    }
    else
    {
        LIDBG_WARN("<==gboot_mode==origin rc==>\n");
        fs_fill_list(get_lidbg_file_path(buff, "lidbg.init.rc.conf"), FS_CMD_FILE_LISTMODE, &lidbg_list);
        analyze_list_cmd(&lidbg_list);
    }

    ssleep(30);//later,exit
    return 0;
}
static int __init lidbg_drivers_loader_init(void)
{

    DUMP_FUN;
    LIDBG_WARN("<==IN==>\n");
    CREATE_KTHREAD(thread_drivers_loader_analyze, NULL);
    LIDBG_MODULE_LOG;
    LIDBG_WARN("<==OUT==>\n\n");
    return 0;
}
static void __exit lidbg_drivers_loader_exit(void)
{
}

module_init(lidbg_drivers_loader_init);
module_exit(lidbg_drivers_loader_exit);

MODULE_AUTHOR("futengfei");
MODULE_DESCRIPTION("for hal or other group to insmod their KO or something else,2014.5.12");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(analyze_list_cmd);


