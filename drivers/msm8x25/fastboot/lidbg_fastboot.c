/* Copyright (c) 2012, swlee
 *
 */

//#define SOC_COMPILE

#ifdef SOC_COMPILE
#include "lidbg.h"
#include "fly_soc.h"
#else
#include "lidbg_def.h"
#include "lidbg_enter.h"
LIDBG_DEFINE;
#endif

#include <mach/proc_comm.h>
#include <mach/clk.h>
#include <mach/socinfo.h>
#include <mach/proc_comm.h>
#include <clock.h>
#include <clock-pcom.h>


#include "lidbg_fastboot.h"


#define RUN_FASTBOOT

static LIST_HEAD(fastboot_kill_list);
static DECLARE_COMPLETION(suspend_start);
static DECLARE_COMPLETION(early_suspend_start);

static DECLARE_COMPLETION(resume_ok);
static DECLARE_COMPLETION(pwroff_start);


static DECLARE_COMPLETION(late_suspend_start);

void fastboot_set_status(LIDBG_FAST_PWROFF_STATUS status);
void fastboot_pwroff(void);
int lidbg_readwrite_file(const char *filename, char *rbuf,const char *wbuf, size_t length);


struct fastboot_data
{
    int suspend_pending;
    u32 resume_count;
    struct mutex lock;
	int kill_task_en;
	int haslock_resume_times;
	int max_wait_unlock_time;
	int clk_block_suspend;
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct wake_lock flywakelock;
    struct early_suspend early_suspend;
#endif

};

struct fastboot_data *fb_data;

static spinlock_t kill_lock;
unsigned long flags_kill;
bool is_fake_suspend = 0;

int wakelock_occur_count = 0;
#define MAX_WAIT_UNLOCK_TIME  (5)

#if (defined(BOARD_V1) || defined(BOARD_V2))
#define WAIT_LOCK_RESUME_TIMES  (3)
#else
#define WAIT_LOCK_RESUME_TIMES  (0)
#endif

#define HAS_LOCK_RESUME

static struct task_struct *pwroff_task;
static struct task_struct *suspend_task;
static struct task_struct *resume_task;
static struct task_struct *pwroff_task;
static struct task_struct *late_suspend_task;

bool ignore_wakelock = 0;
#if (defined(BOARD_V1) || defined(BOARD_V2))
char *kill_exclude_process[] =
{
    "init",
    "kthreadd",

    "khelper",
    "suspend_sys_syn",
    "suspend",
    "sync_supers",
    "bdi-default",
    "kblockd",
    "khubd",
    "l2cap",
    "a2mp",
    "modem_notifier",
    "smd_channel_clo",
    "smsm_cb_wq",
    "rpcrouter",
    "msm_adsp_probe",
    "rpcrotuer_smd_x",
    "krpcserversd",
    "rmt_storage",
    "voicememo_rpc",
    "kadspd",
    "kswapd0",
    "fsnotify_mark",
    "crypto",
    "mdp_dma_wq",
    "mdp_vsync_wq",
    "mdp_pipe_ctrl_w",

    "k_hsuart",
    "kgsl-3d0",
    "k_rmnet_mux_wor",
    "f_mtp",
    "file-storage",
    "diag_wq",
    "diag_cntl_wq",
    "krtcclntd",
    "krtcclntcbd",
    "kbatteryclntd",
    "kbatteryclntcbd",
    "battery_queue",
    "msm_adc",
    "dalrpc_rcv_DAL0",
    "iewq",
    "kinteractiveup",
    "binder",
    "koemrapiclientc",
    "krfcommd",
    "msm-cpufreq",
    "khsclntd",
    "rq_stats",
    "deferwq",
    "ueventd",
    "servicemanager",
    "vold",
    "netd",
    "debuggerd",
    "rild",
    "surfaceflinger",
    "zygote",
    "drmserver",
    "mediaserver",
    "dbus-daemon",
    "installd",
    "keystore",
    "qlogd",
    "sh",
    "cnd",
    "wiperiface",
    "mm-pp-daemon",
    "logwrapper",
    "gpu_dcvsd",
    "k_gserial",
    "lidbg_servicer",
    "k_gsmd",
    "msg_task",
    "u2k_task",
    "pwroff_task",
    "suspend_task",
    "pwroff_task",
    "lpc_task",
    "led_task",
    "dev_init_task",
    "key_task",
    "k_rmnet_work",
    "bp_msg_task",
    "adbd",
    "qmuxd",
    "netmgrd",
    "sh",
    "mkdir",
    "chmod",
    "ATFWD-daemon",
    "mpdecision",
    "audmgr_rpc",
    "sleep",
    "boadcastreceiver",
    "cfg80211",
    "ath6kl",
    "wpa_supplicant",
    "workqueue_trust",
    "logcat",
    "bootanimation",
    "fsck_msdos",
    "usb-storage",
    "sys.DeviceHealth",

    "mm-qcamera-daemon",
    "com.android.qualcomm",
    "com.qualcomm.privinit",
    "com.qualcomm.restore.airplanemode",
 
    "com.android.systemui",
    // "com.android.launcher",
    "com.qualcomm.fastboot",
    //"com.android.phone",
    "com.android.inputmethod.latin",
    //"com.qualcomm.stats",

    // name err
    "alljoyn-daemon",
    "oadcastreceiver",
    "mm-qcamera-daem",
    "putmethod.latin",

    "d.process.acore",

    "ndroid.systemui",

    "alcomm.fastboot",

    "d.process.media",

    "droid.deskclock",

    //flyaudio
    "roid.flyaudioui",
    "goodix_wq",

    "kdmflush",
    "kcryptd",
    "kcryptd_io",
    "dhcpcd",
    "RilMessageDecod",
    "getprop",
    "setprop",
    "ip",
    ":pushservice_v1",
    "fly_gps_server",

    "m.android.phone",
    "android.smspush",
    "omm.datamonitor",
    ".qualcomm.stats",
    "droid.gallery3d",
    "oid.voicedialer",
    "thermald",

    "tek.inputmethod",
    "ndroid.launcher",
    "late_suspend_ta",

    //8803
    "cn.flyaudio.android.flyaudioservice",
    "cn.flyaudio.navigation",
    "com.android.launcher",
    "cn.flyaudio.osd.service",

    "udio.navigation",
    "dio.osd.service",
    "flyaudioservice",
    "ndroid.launcher",
    "fb_late_resume_",
    "GT80x Thread",
//V3
	"msm_ipc_router",
	"krmt_storagecln",
	"uether",
	"isl29028_workqu",
	"acdb_cb_thread",
	"fmcd",
	"thundersoft.fmc",
	"qualcomm.logkit",
	"qosmgr",
	"re.airplanemode",
	"ualcomm.display",
	"alcomm.privinit",
	"m.qualcomm.cabl",
	"ndroid.settings",
	".baidu.padinput",
	"io3.widget.time",
	"com.baidu.input",
	"dio.flyaudioram",
	"encode_button_q",


    "task_kill_exclude_end",

};


char *kill_exclude_process_fake_suspend[] =
{
    "init",
    "kthreadd",

    "khelper",
    "suspend_sys_syn",
    "suspend",
    "sync_supers",
    "bdi-default",
    "kblockd",
    "khubd",
    "l2cap",
    "a2mp",
    "modem_notifier",
    "smd_channel_clo",
    "smsm_cb_wq",
    "rpcrouter",
    "msm_adsp_probe",
    "rpcrotuer_smd_x",
    "krpcserversd",
    "rmt_storage",
    "voicememo_rpc",
    "kadspd",
    "kswapd0",
    "fsnotify_mark",
    "crypto",
    "mdp_dma_wq",
    "mdp_vsync_wq",
    "mdp_pipe_ctrl_w",

    "k_hsuart",
    "kgsl-3d0",

    "k_rmnet_mux_wor",
    "f_mtp",
    "file-storage",
    "diag_wq",
    "diag_cntl_wq",
    "krtcclntd",
    "krtcclntcbd",
    "kbatteryclntd",
    "kbatteryclntcbd",
    "battery_queue",
    "msm_adc",
    "dalrpc_rcv_DAL0",
    "iewq",
    "kinteractiveup",

    "binder",
    "koemrapiclientc",

    "krfcommd",
    "msm-cpufreq",
    "khsclntd",
    "rq_stats",
    "deferwq",
    "ueventd",

    "servicemanager",
    "vold",
    "netd",
    "debuggerd",
    "rild",
    "surfaceflinger",
    "zygote",
    "drmserver",
    "mediaserver",
    "dbus-daemon",
    "installd",
    "keystore",
    "qlogd",
    "sh",
    "cnd",
    "wiperiface",
    "mm-pp-daemon",
    "logwrapper",
    "gpu_dcvsd",
    "k_gserial",
    "lidbg_servicer",
    "k_gsmd",
    "msg_task",
    "u2k_task",
    "pwroff_task",
    "suspend_task",
    "pwroff_task",
    "lpc_task",
    "led_task",
    "dev_init_task",
    "key_task",
    "k_rmnet_work",
    "bp_msg_task",
    "adbd",
    "qmuxd",
    "netmgrd",
    "sh",
    "mkdir",
    "chmod",

    "ATFWD-daemon",
    "mpdecision",

    "audmgr_rpc",
    "sleep",
    "boadcastreceiver",
    "cfg80211",
    "ath6kl",
    "wpa_supplicant",
    "workqueue_trust",
    "logcat",
    "bootanimation",
    "fsck_msdos",
    "usb-storage",
    "sys.DeviceHealth",


    "mm-qcamera-daemon",
    "com.android.qualcomm",
    "com.android.settings",
    "com.qualcomm.privinit",
    "com.qualcomm.restore.airplanemode",
    "com.qualcomm.cabl",
    "com.innopath.activecare",
    "com.android.systemui",
    "com.android.launcher",
    "com.qualcomm.fastboot",
    "com.android.phone",
    "com.android.inputmethod.latin",
    "com.qualcomm.stats",

    // name err
    "alljoyn-daemon",
    "oadcastreceiver",
    "mm-qcamera-daem",
    "putmethod.latin",
    "m.android.phone",
    "ndroid.launcher",
    "ndroid.settings",
    "d.process.acore",
    "ndroid.qualcomm",
    "re.airplanemode",
    "ndroid.systemui",
    "path.activecare",
    "m.qualcomm.cabl",
    "viders.calendar",
    ".qualcomm.stats",
    "alcomm.privinit",
    "alcomm.fastboot",
    "android.smspush",
    "ndroid.contacts",
    "d.process.media",
    "qrd.simcontacts",
    "droid.gallery3d",
    "ndroid.exchange",
    "m.android.email",
    "oid.voicedialer",
    "android.musicfx",
    "droid.deskclock",
    "com.android.mms",
    "m.android.music",
    "omm.datamonitor",
    "oid.sysinfo.pro",


    //flyaudio
    "roid.flyaudioui",
    "goodix_wq",
    "io3.widget.time",
    "lyaudio.Weather",
    ".flyaudio.media",
    "ndroid.calendar",
    "settings:remote",
    "kdmflush",
    "kcryptd",
    "kcryptd_io",
    "dhcpcd",
    "RilMessageDecod",
    "getprop",
    "setprop",
    "ip",
    ":pushservice_v1",
    "fly_gps_server",

    "m.android.phone",
    "android.smspush",
    "omm.datamonitor",
    ".qualcomm.stats",
    "droid.gallery3d",
    "oid.voicedialer",
    "thermald",

    "tek.inputmethod",
    "ndroid.launcher",

    "re-initialized>",

    //8803
    "cn.flyaudio.android.flyaudioservice",
    "cn.flyaudio.navigation",
    "com.android.launcher",
    "cn.flyaudio.osd.service",

    "udio.navigation",
    "dio.osd.service",
    "flyaudioservice",
    "ndroid.launcher",
    "fb_late_resume_",
    "GT80x Thread",

    //kld
    "c2739.mainframe",

    "task_kill_exclude_end",

};
#endif
void set_power_state(int state)
{
	char buf[8];
	if(state == 0)
		lidbg_readwrite_file("/sys/power/state", NULL, "mem", sizeof("mem")-1);
	else
		lidbg_readwrite_file("/sys/power/state", NULL, "on", sizeof("on")-1);
}


void set_cpu_governor(int state)
{
	char buf[16];
	int len;
	lidbg("set_cpu_governor:%d\n",state);
	
#if 1
	{lidbg("do nothing\n");return;}
#endif

	if(state == 0)
	{
    	len = sprintf(buf, "%s","ondemand");
	}
	else
	{
		len = sprintf(buf, "%s","performance");

	}
	lidbg_readwrite_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", NULL, buf, len);
}


static int pc_clk_is_enabled(int id)
{
	if (msm_proc_comm(PCOM_CLKCTL_RPC_ENABLED, &id, NULL))
		return 0;
	else
		return id;
}

int check_all_clk_disable(vold)
{
	int i=P_NR_CLKS-1;
	int ret = 0;
	lidbg_io("\ncheck_all_clk_disable\n");
	while(i>=0)
	{
		if (pc_clk_is_enabled(i))
		{
		 	lidbg_io("pc_clk_is_enabled:%d\n", i);		 	
			ret++;
		}
		i--;
	}
	return ret;
}

int check_clk_disable(vold)
{
	int ret = 0;
	DUMP_FUN_ENTER;
	if(pc_clk_is_enabled(P_UART1DM_CLK))
	{
		lidbg("find clk:%d\n",P_UART1DM_CLK);
		ret = 1;
	}

	if(pc_clk_is_enabled(P_MDP_CLK))
	{
		lidbg("find clk:%d\n",P_MDP_CLK);
		ret = 1;
	}
	DUMP_FUN_LEAVE;
	return ret;
	
}

int fastboot_task_kill_select(char *task_name)
{
    struct task_struct *p;
    struct task_struct *selected = NULL;
    DUMP_FUN_ENTER;

    if(ptasklist_lock != NULL)
    {
        lidbg("read_lock+\n");
        read_lock(ptasklist_lock);
    }
    else
        spin_lock_irqsave(&kill_lock, flags_kill);
	
    for_each_process(p)
    {
        struct mm_struct *mm;
        struct signal_struct *sig;

        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);

        selected = p;

        if(!strcmp(p->comm, task_name))
        {
            lidbg("find %s to kill\n", task_name);

            if (selected)
            {
                force_sig(SIGKILL, selected);
                return 1;
            }
        }
    }
	
    if(ptasklist_lock != NULL)
        read_unlock(ptasklist_lock);
    else
        spin_unlock_irqrestore(&kill_lock, flags_kill);

    DUMP_FUN_LEAVE;
    return 0;
}


static void fastboot_task_kill_exclude(char *exclude_process[])
{
    char kill_process[32][64];

    struct task_struct *p;
    struct mm_struct *mm;
    struct signal_struct *sig;
    u32 i, j = 0;
    bool safe_flag = 0;
    DUMP_FUN_ENTER;

    lidbg("-----------------------\n");

    if(ptasklist_lock != NULL)
    {
        lidbg("read_lock+\n");
        read_lock(ptasklist_lock);
    }
    else
        spin_lock_irqsave(&kill_lock, flags_kill);

    for_each_process(p)
    {
        task_lock(p);
        mm = p->mm;
        sig = p->signal;
        task_unlock(p);
        safe_flag = 0;
        i = 0;
        if(
            (strncmp(p->comm, "flush", sizeof("flush") - 1) == 0) ||
            (strncmp(p->comm, "mtdblock", sizeof("mtdblock") - 1) == 0) ||
            (strncmp(p->comm, "kworker", sizeof("kworker") - 1) == 0) ||
            (strncmp(p->comm, "yaffs", sizeof("yaffs") - 1) == 0) ||
            (strncmp(p->comm, "irq", sizeof("irq") - 1) == 0) ||
            (strncmp(p->comm, "migration", sizeof("migration") - 1) == 0) ||
            (strncmp(p->comm, "mmcqd", sizeof("mmcqd") - 1) == 0) ||
            (strncmp(p->comm, "Fly", sizeof("Fly") - 1) == 0) ||
            (strncmp(p->comm, "fly", sizeof("fly") - 1) == 0) ||
            (strncmp(p->comm, "flyaudio", sizeof("flyaudio") - 1) == 0) ||
            (strncmp(p->comm, "ksdioirqd", sizeof("ksdioirqd") - 1) == 0) ||
            (strncmp(p->comm, "jbd2", sizeof("jbd2") - 1) == 0) ||
            (strncmp(p->comm, "ext4", sizeof("ext4") - 1) == 0) ||
            (strncmp(p->comm, "scsi", sizeof("scsi") - 1) == 0) ||
            (strncmp(p->comm, "loop", sizeof("loop") - 1) == 0) ||
            (strncmp(p->comm, "ServiceHandler", sizeof("ServiceHandler") - 1) == 0) ||
            (strncmp(p->comm, "system", sizeof("system") - 1) == 0) ||
            (strncmp(p->comm, "ksoftirqd", sizeof("ksoftirqd") - 1) == 0)
        )
        {
            continue;
        }
#if (defined(BOARD_V1) || defined(BOARD_V2))
        while(1)
        {

            if(!strcmp(exclude_process[i], "task_kill_exclude_end"))
                break;

            if(strcmp(p->comm, exclude_process[i]) == 0)
            {
                safe_flag = 1;
                break;
            }

            i++;
        }

#else
	{
		struct string_dev *pos; 	
		list_for_each_entry(pos, &fastboot_kill_list, tmp_list)
		{
			if(strcmp(p->comm, pos->yourkey) == 0)
			{
                safe_flag = 1;
				//lidbg("nokill:%s\n", pos->yourkey);
                break;
            }
			else
			{
				//lidbg("kill:%s\n", pos->yourkey);
			}
		}
	}
#endif
        if(safe_flag == 0)
        {
            if (p)
            {
                sprintf(kill_process[j++] , p->comm);
                force_sig(SIGKILL, p);
            }
        }
    }//for_each_process

    if(ptasklist_lock != NULL)
        read_unlock(ptasklist_lock);
    else
        spin_unlock_irqrestore(&kill_lock, flags_kill);

    lidbg("-----------------------\n\n");


    if(j == 0)
        lidbg("find nothing to kill\n");
    else
        for(i = 0; i < j; i++)
        {
            {
                lidbg("## find %s to kill ##\n", kill_process[i]);

            }
        }
    DUMP_FUN_LEAVE;

}


u32 GetTickCount(void)
{
    struct timespec t_now;

    do_posix_clock_monotonic_gettime(&t_now);

    monotonic_to_bootbased(&t_now);

    return t_now.tv_sec * 1000 + t_now.tv_nsec / 1000000;
}


int kill_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
#if (defined(BOARD_V1) || defined(BOARD_V2))
    fastboot_task_kill_exclude(kill_exclude_process);
#else
	fastboot_task_kill_exclude(NULL);
#endif
    return 1;
}

void create_new_proc_entry()
{
    create_proc_read_entry("kill_task", 0, NULL, kill_proc, NULL);

}

int pwroff_proc(char *buf, char **start, off_t offset, int count, int *eof, void *data )
{
	DUMP_FUN_ENTER;
    if(PM_STATUS_LATE_RESUME_OK == fastboot_get_status())
        fastboot_pwroff();
    return 1;
}


void create_new_proc_entry2()
{
    create_proc_read_entry("fastboot_pwroff", 0, NULL, pwroff_proc, NULL);
}


#include "./fake_suspend.c"

static int thread_pwroff(void *data)
{
    int time_count;

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            time_count = 0;
            wait_for_completion(&early_suspend_start);
            while(1)
            {
                msleep(1000);
                time_count++;
                if(fastboot_get_status() == PM_STATUS_READY_TO_PWROFF)
                {

                    if(time_count >= 5 * 2)
                    {
                        lidbgerr("thread_pwroff wait early suspend timeout!\n");
#if (defined(BOARD_V1) || defined(BOARD_V2))
                        SOC_Write_Servicer(SUSPEND_KERNEL);
#else
					    set_power_state(0);
#endif
                        break;
                    }
                }
                else
                {
                    lidbg("thread_pwroff wait time_count=%d\n", time_count);
                    break;

                }
            }
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


static int thread_fastboot_pwroff(void *data)
{
    while(1)
    {

        wait_for_completion(&pwroff_start);
		DUMP_FUN_ENTER;
        fastboot_pwroff();
    }
    return 0;
}


static int thread_fastboot_suspend(void *data)
{
    int time_count;
    static u32 lock_resume_last_time = 0;
    u32 lock_resume_interval;
    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            time_count = 0;
            wait_for_completion(&suspend_start);
            while(1)
            {
                msleep(1000);
                time_count++;
                if(fastboot_get_status() == PM_STATUS_EARLY_SUSPEND_PENDING)
                {
					if(fb_data->clk_block_suspend)
					{
						lidbg("some clk block suspend!\n");
						set_power_state(1);
						break;
					}
					
#ifdef HAS_LOCK_RESUME
                    if(time_count >= /*MAX_WAIT_UNLOCK_TIME*/fb_data->max_wait_unlock_time)
#else
                    if(time_count >= 10)
#endif
                    {
                        lidbgerr("thread_fastboot_suspend wait suspend timeout!\n");

#ifdef HAS_LOCK_RESUME
                        lock_resume_interval = GetTickCount() - lock_resume_last_time;
                        lock_resume_last_time = GetTickCount();
                        lidbg("wakelock_occur_interval=%d,GetTickCount=%d\n", lock_resume_interval, GetTickCount());
                        if(lock_resume_interval > 600 * 1000)//10min
                        {
                            lidbg("reset wakelock_occur_count!!\n");
                            wakelock_occur_count = 0;
                        }
                        wakelock_occur_count++;
                        lidbg("wakelock_occur_count=%d\n", wakelock_occur_count);
                        if(wakelock_occur_count <= /*WAIT_LOCK_RESUME_TIMES*/fb_data->haslock_resume_times)
                        {
#if (defined(BOARD_V1) || defined(BOARD_V2))
                        SOC_Write_Servicer(WAKEUP_KERNEL);
#else
						set_power_state(1);
#endif

                        }
                        else
#endif
                        {
#ifdef HAS_LOCK_RESUME
                            lidbg("$+\n");
                            msleep((10 - MAX_WAIT_UNLOCK_TIME) * 1000);
                            lidbg("$-\n");
#endif
							wakelock_occur_count = 0;
                            if(fastboot_get_status() == PM_STATUS_EARLY_SUSPEND_PENDING)
                            {
                                lidbg("start force suspend...\n");
                                ignore_wakelock = 1;
                                wake_lock(&(fb_data->flywakelock));
                                wake_unlock(&(fb_data->flywakelock));
                            }
                            else
                            {
								lidbg("thread_fastboot_suspend wait time_count=%d\n", time_count);
								complete(&late_suspend_start);
							}
                        }
                        break;
                    }
                }
                else
                {
                    lidbg("thread_fastboot_suspend wait time_count=%d\n", time_count);
                    complete(&late_suspend_start);
#ifdef HAS_LOCK_RESUME
                    wakelock_occur_count = 0;
#endif
                    break;

                }
            }
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


static int thread_late_suspend(void *data)
{
    int time_count;

    while(1)
    {
        set_current_state(TASK_UNINTERRUPTIBLE);
        if(kthread_should_stop()) break;
        if(1)
        {
            time_count = 0;
            wait_for_completion(&late_suspend_start);
            while(1)
            {
                msleep(1000);
                time_count++;
                if(fastboot_get_status() == PM_STATUS_SUSPEND_PENDING)
                {

                    if(time_count >= 10)
                    {
                        lidbgerr("late suspend wait early suspend timeout!\n");
                        lidbg("start force suspend...\n");
                        ignore_wakelock = 1;
                        wake_lock(&(fb_data->flywakelock));
                        wake_unlock(&(fb_data->flywakelock));
                        break;
                    }
                }
                else
                {
                    lidbg("late suspend wait time_count=%d\n", time_count);
                    break;

                }
            }
        }
        else
        {
            schedule_timeout(HZ);
        }
    }
    return 0;
}


void fastboot_set_status(LIDBG_FAST_PWROFF_STATUS status);

static int thread_fastboot_resume(void *data)
{
    while(1)
    {
        wait_for_completion(&resume_ok);
        DUMP_FUN_ENTER;
        msleep(3000);
#if (defined(BOARD_V1) || defined(BOARD_V2))
        SOC_Write_Servicer(WAKEUP_KERNEL);
#else
		set_power_state(1);
#endif
        //SOC_Key_Report(KEY_HOME, KEY_PRESSED_RELEASED);
        //SOC_Key_Report(KEY_BACK, KEY_PRESSED_RELEASED);

        msleep(1000);
        fastboot_set_status(PM_STATUS_LATE_RESUME_OK);
		
	//log acc off times
		if(fb_data->resume_count  % 5 == 0)
		{
			char tmp[32];
			sprintf(tmp, "acc_off_times=%d\n",fb_data->resume_count);
			fs_file_log(tmp);
		}
        DUMP_FUN_LEAVE;
    }
    return 0;
}


int fastboot_get_status(void)
{
    return fb_data->suspend_pending;
}

void fastboot_set_status(LIDBG_FAST_PWROFF_STATUS status)
{
    lidbg("fastboot_set_status:%d\n", status);
    fb_data->suspend_pending = status;
}

void fastboot_pwroff(void)
{
    u32 err_count = 0;
    DUMP_FUN_ENTER;

    while((PM_STATUS_LATE_RESUME_OK != fastboot_get_status()) && (PM_STATUS_READY_TO_FAKE_PWROFF != fastboot_get_status()))
    {
        lidbgerr("Call SOC_PWR_ShutDown when suspend_pending != PM_STATUS_LATE_RESUME_OK :%d\n", fastboot_get_status());
        err_count++;

        if(err_count > 50)//10s
        {
            lidbgerr("err_count > 50,force fastboot_pwroff!\n");
            break;
        }
        msleep(200);

    }
    fastboot_set_status(PM_STATUS_READY_TO_PWROFF);

    SOC_Dev_Suspend_Prepare();
	
	set_cpu_governor(1);//performance

    //avoid mem leak
    //    fastboot_task_kill_select("mediaserver");
    //    fastboot_task_kill_select("vold");

#if 1//def FLY_DEBUG
    msleep(1000);

#ifdef RUN_FASTBOOT
#if (defined(BOARD_V1) || defined(BOARD_V2))
    SOC_Write_Servicer(CMD_ACC_OFF_PROPERTY_SET);
    SOC_Write_Servicer(CMD_FAST_POWER_OFF);
#endif
#else
    SOC_Key_Report(KEY_POWER, KEY_PRESSED_RELEASED);
#endif

#endif

    complete(&early_suspend_start);

}


bool fastboot_is_ignore_wakelock(void)
{
    return ignore_wakelock;
}

void fastboot_go_pwroff(void)
{
    DUMP_FUN_ENTER;
    complete(&pwroff_start);
}


#ifndef SOC_COMPILE

static void set_func_tbl(void)
{
#ifdef FLY_DEBUG
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_go_pwroff;
#else
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_ShutDown = fastboot_pwroff;
#endif
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_GetStatus = fastboot_get_status;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_SetStatus = fastboot_set_status;
    plidbg_dev->soc_func_tbl.pfnSOC_PWR_Ignore_Wakelock = fastboot_is_ignore_wakelock;
    plidbg_dev->soc_func_tbl.pfnSOC_Fake_Register_Early_Suspend = fake_register_early_suspend;
}
#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void fastboot_early_suspend(struct early_suspend *h)
{

    lidbg("fastboot_early_suspend:%d\n", fb_data->resume_count);
    if(PM_STATUS_EARLY_SUSPEND_PENDING != fastboot_get_status())
    {
        lidbgerr("Call devices_early_suspend when suspend_pending != PM_STATUS_READY_TO_PWROFF\n");
    }

	if(fb_data->kill_task_en)
	{
#if (defined(BOARD_V1) || defined(BOARD_V2))
	    	fastboot_task_kill_exclude(kill_exclude_process);
#else
			fastboot_task_kill_exclude(NULL);
#endif
	    	msleep(1000);
	}
	
#if 0 //for test
    ignore_wakelock = 1;
#endif

   check_all_clk_disable();

   if(check_clk_disable())
   {
	   fb_data->clk_block_suspend = 1;
   }
   else
   {
	   fb_data->clk_block_suspend = 0;
	   wake_unlock(&(fb_data->flywakelock));
   }
   //wake_lock(&(fb_data->flywakelock));//to test force suspend
    complete(&suspend_start);

}

static void fastboot_late_resume(struct early_suspend *h)
{
    DUMP_FUN;

#ifdef HAS_LOCK_RESUME
    if((wakelock_occur_count != 0)||(fb_data->clk_block_suspend == 1))
    {
        lidbg("quick late resume\n");
		if(fb_data->clk_block_suspend == 0)
        	wake_lock(&(fb_data->flywakelock));
        fastboot_set_status(PM_STATUS_LATE_RESUME_OK);
    }
#endif

	set_cpu_governor(0);//ondemand
}
#endif

static int  fastboot_probe(struct platform_device *pdev)
{
    int ret;
    DUMP_FUN_ENTER;

    fb_data = kmalloc(sizeof(struct fastboot_data), GFP_KERNEL);
    if (!fb_data)
    {
        ret = -ENODEV;
        goto fail_mem;
    }
    mutex_init(&fb_data->lock);
    wake_lock_init(&(fb_data->flywakelock), WAKE_LOCK_SUSPEND, "lidbg_wake_lock");

    fastboot_set_status(PM_STATUS_LATE_RESUME_OK);
    fb_data->resume_count = 0;
	fb_data->clk_block_suspend = 0;

#ifdef CONFIG_HAS_EARLYSUSPEND
    fb_data->early_suspend.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 5; //the later the better
    fb_data->early_suspend.suspend = fastboot_early_suspend;
    fb_data->early_suspend.resume = fastboot_late_resume;
    register_early_suspend(&fb_data->early_suspend);
    wake_lock(&(fb_data->flywakelock));
#endif


{
	int ret = 0;char *string;

	fb_data->kill_task_en = 1;
	ret = fs_get_value(&lidbg_drivers_list, "kill_task_en",&string);
	if(ret>0) fb_data->kill_task_en = simple_strtoul(string, 0, 0);
	lidbg("config:kill_task_en=%d\n",fb_data->kill_task_en);
	
	fb_data->haslock_resume_times = 0;
	ret = fs_get_value(&lidbg_drivers_list, "haslock_resume_times",&string);
	if(ret>0) fb_data->haslock_resume_times = simple_strtoul(string, 0, 0);
	lidbg("config:haslock_resume_times=%d\n",fb_data->haslock_resume_times);

	fb_data->max_wait_unlock_time = 5;
	ret = fs_get_value(&lidbg_drivers_list, "max_wait_unlock_time",&string);
	if(ret>0) fb_data->max_wait_unlock_time = simple_strtoul(string, 0, 0);
	lidbg("config:max_wait_unlock_time=%d\n",fb_data->max_wait_unlock_time);
}


    INIT_COMPLETION(early_suspend_start);
    pwroff_task = kthread_create(thread_pwroff, NULL, "pwroff_task");
    if(IS_ERR(pwroff_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(pwroff_task);

    INIT_COMPLETION(pwroff_start);
    pwroff_task = kthread_create(thread_fastboot_pwroff, NULL, "pwroff_task");
    if(IS_ERR(pwroff_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(pwroff_task);


    INIT_COMPLETION(suspend_start);
    suspend_task = kthread_create(thread_fastboot_suspend, NULL, "suspend_task");
    if(IS_ERR(suspend_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(suspend_task);
	

    INIT_COMPLETION(resume_ok);
    resume_task = kthread_create(thread_fastboot_resume, NULL, "pwroff_task");
    if(IS_ERR(resume_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(resume_task);


    INIT_COMPLETION(late_suspend_start);
    late_suspend_task = kthread_create(thread_late_suspend, NULL, "late_suspend_task");
    if(IS_ERR(late_suspend_task))
    {
        lidbg("Unable to start kernel thread.\n");

    }
    else wake_up_process(late_suspend_task);

    spin_lock_init(&kill_lock);
    create_new_proc_entry();
    create_new_proc_entry2();
    create_proc_entry_fake_suspend();
    create_proc_entry_fake_wakeup();

	if(fs_fill_list("/flysystem/lib/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &fastboot_kill_list)<0)
		fs_fill_list("/system/lib/modules/out/fastboot_not_kill_list.conf", FS_CMD_FILE_LISTMODE, &fastboot_kill_list);
    DUMP_FUN_LEAVE;

    return 0;

fail_mem:
    return ret;
}

static int  fastboot_remove(struct platform_device *pdev)
{

#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&fb_data->early_suspend);
#endif

    kfree(fb_data);

    return 0;
}

#ifdef CONFIG_PM
static int fastboot_suspend(struct device *dev)
{
    DUMP_FUN;
    if(PM_STATUS_SUSPEND_PENDING != fastboot_get_status())
    {
        lidbgerr("Call fastboot_suspend when suspend_pending != PM_STATUS_SUSPEND_PENDING\n");
    }
#if 0
   if(check_all_clk_disable())
   {
	  // lidbg_io("wake up\n");
	  // set_power_state(1);//fail
   }
#endif
    return 0;
}

static int fastboot_resume(struct device *dev)
{
    DUMP_FUN;
    lidbg("fastboot_resume:%d\n", ++fb_data->resume_count);

    fastboot_set_status(PM_STATUS_RESUME_OK);
    wake_lock(&(fb_data->flywakelock));
    complete(&resume_ok);

    ignore_wakelock = 0;
    wakelock_occur_count = 0;

    return 0;
}

static struct dev_pm_ops fastboot_pm_ops =
{
    .suspend	= fastboot_suspend,
    .resume		= fastboot_resume,
};
#endif

static struct platform_driver fastboot_driver =
{
    .probe		= fastboot_probe,
    .remove     = fastboot_remove,
    .driver         = {
        .name = "lidbg_fastboot",
        .owner = THIS_MODULE,
#ifdef CONFIG_PM
        .pm = &fastboot_pm_ops,
#endif
    },
};

static struct platform_device lidbg_fastboot_device =
{
    .name               = "lidbg_fastboot",
    .id                 = -1,
};

static int __init fastboot_init(void)
{
    DUMP_BUILD_TIME;

#ifndef SOC_COMPILE
    LIDBG_GET;
    set_func_tbl();
#endif

    platform_device_register(&lidbg_fastboot_device);
    return platform_driver_register(&fastboot_driver);
}

static void __exit fastboot_exit(void)
{
    platform_driver_unregister(&fastboot_driver);
}


module_init(fastboot_init);
module_exit(fastboot_exit);


MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("lidbg fastboot driver");

