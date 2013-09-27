
#include "lidbg.h"

/*
NOTE:
	1:ts_event=te
	2:code entry:[toucheventinit_once()]  [te_regist_password()]
*/

//zone below[tools]
#define TE_VERSION "TE.VERSION:  [20130927]"
#define PASSWORD_TE_ON "001122"
#define DEBUG_MEM_FILE "/data/fs_private.txt"
#define TE_WARN(fmt, args...) pr_info("[futengfei.te]warn.%s: " fmt,__func__,##args)
#define TE_ERR(fmt, args...) pr_info("[futengfei.te]err.%s: " fmt,__func__,##args)
#define TE_SUC(fmt, args...) pr_info("[futengfei.te]suceed.%s: " fmt,__func__,##args)
#define SCEEN_X 1024
#define SCEEN_Y  600
#define RECT_WIDE 100
#define RECT_HIGH  RECT_WIDE
#define SAFE  50
#define CMD_MAX  6

struct dev_password
{
    struct list_head tmp_list;
    char *password;
    void (*cb_password)(char *password );
};


LIST_HEAD(te_password_list);
struct tspara g_pre_tspara ;
struct tspara g_curr_tspara = {0, 0, false} ;
static struct task_struct *te_task;
static char prepare_cmd[CMD_MAX];
static int prepare_cmdpos = 0;
static int g_dubug_mem = 0;
static int g_te_dbg_en = 0;
static int g_is_te_enable = 0;
static int g_te_scandelay_ms = 100;
//zone end


//zone below [interface]
bool regist_password(char *password, void (*cb_password)(char *password ));

bool te_regist_password(char *password, void (*cb_password)(char *password ))
{
    if(password && cb_password)
        return regist_password(password, cb_password);
    else
        TE_ERR("<password||cb_password:null?>\n");
    return false;
}
//zone end

//zone below[ts_event]
struct devrect
{
    char name;
    int id;//reserve
    int wide;
    int high;
    int x0;
    int y0;
} devrect_list[] =
{
    {'0', 0, RECT_WIDE, RECT_HIGH, 0 , 0},
    {'1', 1, RECT_WIDE, RECT_HIGH, SCEEN_X - RECT_WIDE , 0},
    {'2', 2, RECT_WIDE, RECT_HIGH, 0 , SCEEN_Y - RECT_HIGH},
    {'3', 3, RECT_WIDE, RECT_HIGH, SCEEN_X - RECT_WIDE , SCEEN_Y - RECT_HIGH},
    //    {'4', 4, SCEEN_X - RECT_WIDE - SAFE, SCEEN_Y - RECT_HIGH - SAFE, RECT_WIDE + SAFE, RECT_HIGH + SAFE},
};

#define clear devrect_list[3].id

void new_password_dev(char *password, void (*cb_password)(char *password ))
{
    struct dev_password *add_new_dev;
    add_new_dev = kzalloc(sizeof(struct dev_password), GFP_KERNEL);
    if(g_dubug_mem)
        fs_string2file(DEBUG_MEM_FILE, "%s.%s=%d \n", __func__, password, sizeof(struct dev_password));

    add_new_dev->password = password;
    add_new_dev->cb_password = cb_password;
    list_add(&(add_new_dev->tmp_list), &te_password_list);
}
bool is_password_exist(char *password)//
{
    struct dev_password *pos;
    struct list_head *client_list = &te_password_list;

    if(list_empty(client_list))
        return false;
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (!strcmp(pos->password, password))
            return true;
    }
    return false;
}
bool show_password_list(void)
{
    struct dev_password *pos;
    struct list_head *client_list = &te_password_list;

    if(list_empty(client_list))
    {
        TE_ERR("<nobody_register>\n");
        return false;
    }
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->password )
            TE_WARN("<registerd_list:%s>\n", pos->password);
    }
    return true;
}
bool call_password_cb(char *password)
{
    struct dev_password *pos;
    struct list_head *client_list = &te_password_list;

    if(list_empty(client_list))
    {
        TE_ERR("<nobody_register>\n");
        return false;
    }
    list_for_each_entry(pos, client_list, tmp_list)
    {
        if (pos->password && pos->cb_password && (!strcmp(pos->password, password)) )
        {
            pos->cb_password(pos->password);
            if(g_te_dbg_en)
                TE_WARN("<have called:%s>\n", pos->password);
        }
    }
    return true;
}
bool regist_password(char *password, void (*cb_password)(char *password ))
{
    if(!is_password_exist(password))
    {
        new_password_dev(password, cb_password);
        return true;
    }
    else
        TE_ERR("<existed :%s>\n", password);
    return false;
}
int get_num(void)
{
    int cunt, size;
    size = ARRAY_SIZE(devrect_list);
    for(cunt = 0; cunt < size; cunt++)
    {
        if((g_curr_tspara.x > devrect_list[cunt].x0) && (g_curr_tspara.y > devrect_list[cunt].y0)
                && (g_curr_tspara.x < devrect_list[cunt].x0 + devrect_list[cunt].wide) && (g_curr_tspara.y < devrect_list[cunt].y0 + devrect_list[cunt].high))
            return  devrect_list[cunt].id;
    }
    return -1;
}
void reset_cmd(void)
{
    prepare_cmdpos = 0;
    memset(&prepare_cmd, '\0', CMD_MAX);
}
void launch_cmd(void)
{
    if(g_te_dbg_en)
        TE_ERR("<prepare_cmd:%s>\n", prepare_cmd);
    if(g_is_te_enable)
    {
        TE_WARN("<en>\n");
        call_password_cb(prepare_cmd);
    }
    else
    {
        if (!strcmp(prepare_cmd, PASSWORD_TE_ON))
        {
            g_is_te_enable = 1;
            lidbg_chmod( "/data");
        }
    }

    reset_cmd();
}
void getnum_andanalysis(void)
{
    int num;
    if(g_curr_tspara.press)
    {
        num = get_num();
        if(num >= 0)
        {
            if(g_te_dbg_en)
                TE_WARN("<[%d,%d,%d],[%d,%d,%d,%d]>\n", g_curr_tspara.x, g_curr_tspara.y, g_curr_tspara.press, g_te_scandelay_ms, clear,  prepare_cmdpos, num);
            while(g_curr_tspara.press )
                msleep(g_te_scandelay_ms);
            if(g_te_dbg_en)
                TE_WARN("<[%d,%d,%d],[%d,%d,%d,%d]>release\n", g_curr_tspara.x, g_curr_tspara.y, g_curr_tspara.press, g_te_scandelay_ms, clear,  prepare_cmdpos, num);

            if(clear == num)
                reset_cmd();
            else
            {
                prepare_cmd[prepare_cmdpos] = devrect_list[num].name;
                prepare_cmdpos++;
                if(prepare_cmdpos >= CMD_MAX)//prepare_cmdpos = CMD_MAX :auto launch
                    launch_cmd();
            }
        }
        else
        {
            msleep(g_te_scandelay_ms);
        }
    }
    else
    {
        msleep(g_te_scandelay_ms);
    }
}
static int thread_te_analysis(void *data)
{
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    ssleep(25);
    TE_WARN("<thread start>\n" );
    if(g_te_dbg_en)
        show_password_list();
    g_curr_tspara.press = false;
    while(!kthread_should_stop())
    {
        if(g_te_scandelay_ms)
            getnum_andanalysis();
        else
            ssleep(30);
    };

    return 1;
}
//zone end

void cb_password_chmod(char *password )
{
    if(g_te_dbg_en)
        TE_WARN("<called:%s>\n", password);
    lidbg_chmod("/system/bin/mount");
    lidbg_mount("/system");

    lidbg_chmod("/data");
}
void cb_password_upload(char *password )
{
    if(g_te_dbg_en)
        TE_WARN("<called:%s>\n", password);
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_upload_machine_log();
}
void cb_password_call_apk(char *password )
{
    if(g_te_dbg_en)
        TE_WARN("<called:%s>\n", password);
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_call_apk();
}
void cb_password_remove_apk(char *password )
{
    if(g_te_dbg_en)
        TE_WARN("<called:%s>\n", password);
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_remove_apk();
}
void cb_password_clean_all(char *password )
{
    if(g_te_dbg_en)
        TE_WARN("<called:%s>\n", password);
    fs_file_log("<called:%s>\n", __func__ );//tmp,del later
    fs_clean_all();
}
void cb_kv_password(char *key, char *value)
{
    if(g_te_dbg_en)
        TE_WARN("<%s=%s>\n", key, value);
    if ( (!strcmp(key, "te_dbg_en" ))  &&  (strcmp(value, "0" )) )
        show_password_list();
}
void  toucheventinit_once(void)
{
    TE_WARN("<==IN==>\n");

    TE_WARN("<%s>\n", TE_VERSION);
    fs_file_log("%s\n", TE_VERSION );

    FS_REGISTER_INT(g_te_dbg_en, "te_dbg_en", 0, cb_kv_password);
    FS_REGISTER_INT(g_te_scandelay_ms, "te_scandelay_ms", 100, NULL);

    te_regist_password("001100", cb_password_remove_apk);
    te_regist_password("001101", cb_password_upload);
    te_regist_password("001102", cb_password_call_apk);
    te_regist_password("001110", cb_password_clean_all);
    te_regist_password("001111", cb_password_chmod);

    fs_get_intvalue(&lidbg_core_list, "te_dbg_mem", &g_dubug_mem, NULL);

    te_task = kthread_run(thread_te_analysis, NULL, "ftf_te_task");

    TE_WARN("<==OUT==>\n");
}

//zone below [EXPORT_SYMBOL]
EXPORT_SYMBOL(g_curr_tspara);
EXPORT_SYMBOL(te_regist_password);
//zone end

