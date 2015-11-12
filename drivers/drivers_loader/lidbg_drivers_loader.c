#include "lidbg.h"

static LIST_HEAD(lidbg_list);
static LIST_HEAD(flyaudio_list);

struct judgment
{
    char *key;
    int value;
} judgment_list[] =
{
    {"platform_id", -1},
    {"Factory", -1},
};
void judgment_list_print(void)
{
    int cunt, size;
    size = ARRAY_SIZE(judgment_list);
    for(cunt = 0; cunt < size; cunt++)
    {
        LIDBG_WARN("%d<%s,%d>\n", cunt, judgment_list[cunt].key , judgment_list[cunt].value);
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

bool judgment_cmd(char *judgmentcmd, int *judgmenttimes)
{
    //example: ?PLFID=1||Factory=1
    //{
    //code...
    //}

    if( *(judgmentcmd) == '{' || *(judgmentcmd) == '}' || *judgmenttimes > 0)
    {
        if(*(judgmentcmd) == '}' )
            (*judgmenttimes) = 0;
        if(*(judgmentcmd) != '{' && *(judgmentcmd) != '}' )
            LIDBG_WARN("judgmenttimes = %d <skip.%s>\n", *judgmenttimes, judgmentcmd);
        goto skip_current_cmd;
    };
    // 2:check the cmd
    if(*(judgmentcmd) == '?')
    {
        char * or , * and ;
        //LIDBG_WARN("valid cmd======= <%s>============\n",  judgmentcmd);
        lidbgstrtrim(judgmentcmd);
        //LIDBG_WARN("after lidbgstrtrim <%s>\n", judgmentcmd);
        judgmentcmd += 1; //skip '?'
        or = strstr(judgmentcmd, "||");
        and = strstr(judgmentcmd, "&&");
        if( or != NULL && and != NULL)
        {
            LIDBG_WARN("or ,and == NULL <%s,%d,%d>\n",  judgmentcmd, or == NULL, and == NULL);
            goto skip_current_cmd;
        }

        lidbg_strrpl(judgmentcmd, or != NULL ? "||" : "&&", or != NULL ? "|" : "&");
        LIDBG_WARN("after lidbg_strrpl ===============<%s>\n",  judgmentcmd);

        //toke judgment string and have a check if it is true
        {
            int loops, tokenvalue, judgmentvalue;
            char *arg[8] = {NULL};
            char *arg2[8] = {NULL};
            int cmd_num = lidbg_token_string(judgmentcmd, or != NULL ? "|" : "&" , arg) ;
            for(loops = 0; loops < cmd_num; loops++)
            {
                //LIDBG_WARN("%d/%d.start toke:<%s>\n", loops, cmd_num, arg[loops]);
                if(arg[loops] && lidbg_token_string(arg[loops], "=", arg2) == 2)
                {
                    judgmentvalue = get_judgment_list_value(arg2[0]);
                    if(judgmentvalue == -2)
                    {
                        LIDBG_WARN("error unknown judgment <%s>\n", arg2[0]);
                        goto skip_current_cmd;
                    }
                    tokenvalue = simple_strtoul(arg2[1], 0, 0);

                    LIDBG_WARN("<%d/%d. check :[%s,%d,%d]>\n", loops, cmd_num, arg2[0], tokenvalue, judgmentvalue);

                    //check
                    if( or != NULL || ( or == NULL && and == NULL))
                    {
                        if( tokenvalue == judgmentvalue)
                        {
                            LIDBG_WARN("<judgment OK [||]>\n");
                            goto skip_current_cmd;
                        }
                        if(loops == cmd_num - 1)
                        {
                            LIDBG_WARN("<judgment error [||]>\n");
                            goto start_skip_below_cmd;
                        }
                    }

                    if( and != NULL)
                    {
                        if( tokenvalue != judgmentvalue)
                        {
                            LIDBG_WARN("<judgment error [&&]>\n");
                            goto start_skip_below_cmd;
                        }
                        if(loops == cmd_num - 1)
                        {
                            LIDBG_WARN("<judgment OK [&&]>\n");
                            goto skip_current_cmd;
                        }
                    }
                }
                else if(arg[loops] != NULL)
                {
                    LIDBG_WARN("toke judgment string  err<%s>\n", arg[loops]);
                    goto skip_current_cmd;
                }
            }
            goto skip_current_cmd;
        }
    }
    else
        goto not_volid_judgment_cmd;

start_skip_below_cmd:
    //initrc's cmd below current cmd will be skipped until meet the cmd '}'
    *judgmenttimes = 1;
skip_current_cmd:
    //current cmd have sothing err, do not exe it and just skip.
    return true;
not_volid_judgment_cmd:
    //current cmd is a commen cmd not a judgment cmd likes "?platform_id=11".
    return false;
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
            if(judgment_cmd(pos->yourkey, &judgmenttimes) )
                continue;
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
                //LIDBG_WARN("exe<%s>\n", pos->yourkey);
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


