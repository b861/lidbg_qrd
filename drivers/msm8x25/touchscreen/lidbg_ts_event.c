
#include "lidbg.h"

/*
NOTE:
	1:ts_event=te
	2:add one password in func: launch_cmd()

*/

//zone below[#define]
#define TE_VERSION "TE.VERSION:  [20130909 V1.0]"
#define TE_WARN(fmt, args...) pr_info("[futengfei.te]warn.%s: " fmt,__func__,##args)
#define TE_ERR(fmt, args...) pr_info("[futengfei.te]err.%s: " fmt,__func__,##args)
#define TE_SUC(fmt, args...) pr_info("[futengfei.te]suceed.%s: " fmt,__func__,##args)
#define SCEEN_X 1024
#define SCEEN_Y  600
#define RECT_WIDE 100
#define RECT_HIGH  RECT_WIDE
#define SAFE  50
#define CMD_MAX  6
//zone end


//zone below[tools]
struct tspara g_pre_tspara ;
struct tspara g_curr_tspara = {0, 0, false} ;
static struct task_struct *te_task;
static char prepare_cmd[CMD_MAX];
static int prepare_cmdpos = 0;
static int g_te_dbg_en = 0;
static int g_te_scandelay_ms = 100;
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
    TE_WARN("<prepare_cmd:[%s][%d,%d]>\n", prepare_cmd, g_te_scandelay_ms, clear );

    if (!strcmp(prepare_cmd, "001122"))
        lidbg_launch_user(CHMOD_PATH, "777", "/data");

    reset_cmd();
}
static int thread_te_test(void *data)
{
    int num;
    allow_signal(SIGKILL);
    allow_signal(SIGSTOP);
    //set_freezable();
    ssleep(25);
    g_curr_tspara.press = false;
    while(!kthread_should_stop())
    {
        if(g_te_scandelay_ms)
        {
            if(g_curr_tspara.press)
            {
                num = get_num();
                if(num >= 0)
                {
                    if(g_te_dbg_en)
                        TE_WARN("<[%d,%d,%d],[%d,%d,%d,%d]>\n", g_curr_tspara.x, g_curr_tspara.y, g_curr_tspara.press, g_te_scandelay_ms, clear, num, prepare_cmdpos );
                    while(g_curr_tspara.press )
                        msleep(g_te_scandelay_ms);
                    if(g_te_dbg_en)
                        TE_WARN("<[%d,%d,%d],[%d,%d,%d,%d]>release\n", g_curr_tspara.x, g_curr_tspara.y, g_curr_tspara.press, g_te_scandelay_ms, clear, num, prepare_cmdpos );

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
            }
            else
                msleep(g_te_scandelay_ms);
        }
        else
            ssleep(30);
    };
    return 1;
}
//zone end

static int __init ts_event_init(void)
{
    TE_WARN("<==IN==>\n");

    TE_WARN("<%s>\n", TE_VERSION);
    fs_file_log("%s\n", TE_VERSION );

    FS_REGISTER_INT(g_te_dbg_en, "te_dbg_en", 0, NULL);
    FS_REGISTER_INT(g_te_scandelay_ms, "te_scandelay_ms", 100, NULL);
    te_task = kthread_run(thread_te_test, NULL, "ftf_te_task");

    TE_WARN("<==OUT==>\n");
    return 0;
}
static void __exit ts_event_exit(void)
{
    if (te_task)
        kthread_stop(te_task);
}


//zone below [EXPORT_SYMBOL]
EXPORT_SYMBOL(g_curr_tspara);
//zone end

module_init(ts_event_init);
module_exit(ts_event_exit);

MODULE_AUTHOR("futengfei");
MODULE_LICENSE("GPL");


