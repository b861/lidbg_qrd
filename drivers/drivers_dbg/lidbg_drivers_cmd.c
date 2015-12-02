
#define TEMP_LOG_PATH 	 LIDBG_LOG_DIR"log_ct.txt"


int thread_log_temp(void *data)
{
    int tmp, cur_temp;
    while(1)
    {
        tmp = cpufreq_get(0);
        cur_temp = soc_temp_get(g_hw.cpu_sensor_num);
        lidbg_fs_log(TEMP_LOG_PATH,  "%d,%d\n", cur_temp, cpufreq_get(0));
        msleep(1000);
    }
}
void cb_kv_log_temp(char *key, char *value)
{
    CREATE_KTHREAD(thread_log_temp, NULL);
}



int thread_antutu_test(void *data)
{
    int cnt = 0;
    ssleep(50);
#ifdef SOC_msm8x26
    set_system_performance(1);
#endif

    while(1)
    {
        cnt++;
        lidbg_fs_log(TEMP_LOG_PATH, "antutu test start: %d\n", cnt);

        //lidbg_shell_cmd("pm uninstall com.antutu.ABenchMark");
        //lidbg_pm_install("/data/antutu.apk");
        //ssleep(5);

        lidbg_shell_cmd("am start -n com.antutu.ABenchMark/com.antutu.ABenchMark.ABenchMarkStart");
        ssleep(5);
        lidbg_shell_cmd("am start -n com.antutu.ABenchMark/com.antutu.benchmark.activity.ScoreBenchActivity");
        ssleep(60 * 5); // 4 min loop
    }

}
bool set_wifi_adb_mode(bool on)
{
    LIDBG_WARN("<%d>\n", on);
    if(on)
        lidbg_setprop("service.adb.tcp.port", "5555");
    else
        lidbg_setprop("service.adb.tcp.port", "-1");
    lidbg_stop("adbd");
    lidbg_start("adbd");
    return true;
}
static bool encode = false;
int thread_dump_log(void *data)
{
#ifdef SOC_msm8x25
    msleep(7000);
    fs_cp_data_to_udisk(true);
#else
    char shell_cmd[128] = {0}, tbuff[128] = {0};
    msleep(7000);
    lidbg_get_current_time(tbuff, NULL);
    sprintf(shell_cmd, "mkdir "USB_MOUNT_POINT"/ID-%d-%s", get_machine_id() , tbuff);
    lidbg_shell_cmd(shell_cmd);
    sprintf(shell_cmd, "cp -rf "LIDBG_LOG_DIR"* "USB_MOUNT_POINT"/ID-%d-%s", get_machine_id() , tbuff);
    lidbg_shell_cmd(shell_cmd);
    ssleep(2);
#endif
    lidbg_domineering_ack();
    return 0;
}
static bool logcat_enabled = false;
int thread_enable_logcat(void *data)
{
    if(logcat_enabled)
        goto out;
    logcat_enabled = true;
    lidbg_enable_logcat();
out:
    lidbg("logcat.skip\n");
    return 0;
}
int thread_enable_logcat2(void *data)
{
    if(logcat_enabled)
        goto out;
    logcat_enabled = true;
    lidbg_enable_logcat2();
out:
    lidbg("logcat.skip\n");
    return 0;
}

int thread_system_trace(void *data)
{
    lidbg_shell_cmd("top -t -m 15 >> /sdcard/logcat.txt &");

    while(1)
    {
	     lidbg_shell_cmd("procrank -u >> /sdcard/logcat.txt &");
	     ssleep(5);
	     lidbg_shell_cmd("dumpsys meminfo >> /sdcard/logcat.txt &");
            ssleep(5);
    }

    return 0;
}



static bool top_enabled = false;
int thread_enable_top(void *data)
{
    int size, sizeold = 0;
    if(top_enabled)
        goto out;
    top_enabled = true;
    lidbg("top+\n");
    lidbg_shell_cmd("rm /sdcard/top.txt");
    lidbg_shell_cmd("rm /sdcard/top_old.txt");
    ssleep(2);

    lidbg_shell_cmd("date >/sdcard/top.txt");
    ssleep(1);
    lidbg_shell_cmd("chmod 777 /sdcard/top.txt");
    ssleep(1);
    lidbg_shell_cmd("top -t -m 10 > /sdcard/top.txt &");
    while(1)
    {
        size = fs_get_file_size("/sdcard/top.txt") ;
        if(size >= MEM_SIZE_1_MB * 300)
        {
            lidbg("file_len over\n");
            lidbg_shell_cmd("rm /sdcard/top_old.txt");
            ssleep(1);
            lidbg_shell_cmd("cp -rf /sdcard/top.txt /sdcard/top_old.txt");
            ssleep(5);
            lidbg_shell_cmd("date > /sdcard/top.txt");
            ssleep(1);
            lidbg_shell_cmd("chmod 777 /sdcard/top.txt");
        }
        ssleep(60);
        if(size == sizeold)
        {
            lidbg_shell_cmd("top -t -m 10 > /sdcard/top.txt &");
            lidbg("run top again \n");
        }
        sizeold = size ;

    }
    lidbg("top-\n");
out:
    lidbg("top.skip\n");
    return 0;
}
static bool dmesg_enabled = false;
int thread_enable_dmesg(void *data)
{
    if(dmesg_enabled)
        goto out;
    dmesg_enabled = true;
    lidbg_enable_kmsg();
out:
    lidbg("kmsg.skip\n");
    return 0;
}

int thread_screenshot(void *data)
{
    SOC_Key_Report(KEY_POWER, KEY_PRESSED);
    SOC_Key_Report(KEY_VOLUMEDOWN, KEY_PRESSED);
    msleep(3000);
    SOC_Key_Report(KEY_POWER, KEY_RELEASED);
    SOC_Key_Report(KEY_VOLUMEDOWN, KEY_RELEASED);
    return 0;
}

int thread_kmsg_fifo_save(void *data)
{
    kmsg_fifo_save();
    return 0;
}

int thread_monkey_test(void *data)
{
    u32 loop = 0;
    lidbg("monkey test start !\n");
    while(1)
    {
        if(te_is_ts_touched())
        {
            lidbg_domineering_ack();
            lidbg("thread_monkey_test:te_is_ts_touched.pause\n");
            ssleep(60);
            continue;
        }
        lidbg("monkey loop = %d\n", loop);
        loop++;
        lidbg_shell_cmd("monkey --ignore-crashes --ignore-timeouts --throttle 300 500 &");
        msleep(60 * 1000);
    }
    lidbg("thread_monkey_test end\n");
    return 0;
}

irqreturn_t TEST_isr(int irq, void *dev_id)
{
    lidbg("TEST_isr================%d ", irq);
    return IRQ_HANDLED;
}

void callback_func_test_readdir(char *dirname, char *filename)
{
    LIDBG_WARN("%s<---%s\n", dirname, filename);
}

static bool fan_enable = false;
void parse_cmd(char *pt)
{
    int argc = 0;
    int i = 0;

    char *argv[32] = {NULL};
    argc = lidbg_token_string(pt, " ", argv);

    lidbg("cmd:");
    while(i < argc)
    {
        printk(KERN_CRIT"%s ", argv[i]);
        i++;
    }
    lidbg("\n");

    if (!strcmp(argv[0], "appcmd"))
    {
        lidbg("%s:[%s]\n", argv[0], argv[1]);

        if (!strcmp(argv[1], "*158#000"))
        {
            //*#*#158999#*#*
            lidbg_chmod("/data");

            fs_mem_log("*158#998--install third apk\n");
            fs_mem_log("*158#999--install debug apk\n");
            fs_mem_log("*158#001--LOG_LOGCAT\n");
            fs_mem_log("*158#002--LOG_DMESG\n");
            fs_mem_log("*158#003--LOG_CLEAR_LOGCAT_KMSG\n");
            fs_mem_log("*158#004--LOG_SHELL_TOP_DF_PS\n");
            fs_mem_log("*158#010--USB_ID_LOW_HOST\n");
            fs_mem_log("*158#011--USB_ID_HIGH_DEV\n");
            fs_mem_log("*158#012--lidbg_trace_msg_disable\n");
            fs_mem_log("*158#013--dump log and copy to udisk\n");
            fs_mem_log("*158#014--origin system\n");
            fs_mem_log("*158#015--flyaudio system\n");
            fs_mem_log("*158#016--enable wifi adb\n");//adb connect ip
            fs_mem_log("*158#017--disable wifi adb\n");
            fs_mem_log("*158#018--origin gps\n");
            fs_mem_log("*158#019--enable system print\n");
            fs_mem_log("*158#020--disable system print\n");
            fs_mem_log("*158#021--save fifo msg\n");
            fs_mem_log("*158#022--log2sd to save qxdm\n");
            fs_mem_log("*158#023--show cpu temp\n");
            fs_mem_log("*158#024--!fan enalbe\n");
            fs_mem_log("*158#025--LPC_CMD_ACC_SWITCH_START\n");
            fs_mem_log("*158#026--clear acc history\n");
            fs_mem_log("*158#027--antutu auto test\n");
            fs_mem_log("*158#028--delete ublox so && reboot\n");
            fs_mem_log("*158#029--log cpu temp\n");
            fs_mem_log("*158#030--cpu top performance mode\n");
            fs_mem_log("*158#031--pr_debug GPS_val\n");
            fs_mem_log("*158#032--pr_debug AD_val\n");
            fs_mem_log("*158#033--pr_debug TS_val\n");
            fs_mem_log("*158#034--pr_debug cpu_temp\n");
            fs_mem_log("*158#035--pr_debug lowmemorykillprotecter\n");
            fs_mem_log("*158#040--monkey test\n");
            fs_mem_log("*158#041--disable uart debug\n");
            fs_mem_log("*158#042--disable adb\n");
            fs_mem_log("*158#043--enable adb\n");
            fs_mem_log("*158#044--start SleepTest acc test,????,?*158#0448010\n");
            fs_mem_log("*158#045x--start RGB LED test,????,?*158#0451\n");
            fs_mem_log("*158#046--set cpu run in performance mode\n");
            fs_mem_log("*158#047--set cpu run in powersave mode\n");
            fs_mem_log("*158#048 flybootserver airplane disable\n");
            fs_mem_log("*158#049 flybootserver airplane enable\n");
            fs_mem_log("*158#050 enable top -t -m 10\n");
            fs_mem_log("*158#051--LOG_LOGCAT2\n");
            fs_mem_log("*158#052--udisk reset\n");
            fs_mem_log("*158#053--system trace\n");
	    fs_mem_log("*158#054--uvccam recording control(1 or 0)\n");

            show_password_list();
            lidbg_domineering_ack();
        }
        if (!strcmp(argv[1], "*158#998"))
        {
            char buff[50] = {0};
            lidbg_pm_install(get_lidbg_file_path(buff, "ES.ko"));
            lidbg_pm_install(get_lidbg_file_path(buff, "ST.ko"));
            lidbg_pm_install(get_lidbg_file_path(buff, "GPS.ko"));
            lidbg_pm_install(get_lidbg_file_path(buff, "camera4hal.ko"));
            lidbg_domineering_ack();
        }
        if (!strcmp(argv[1], "*158#999"))
        {
            char buff[50] = {0};
            lidbg_shell_cmd("setenforce 0");
            lidbg_shell_cmd("cp /flysystem/lib/out/lidbg_udisk_shell.conf /dev/log/");
            lidbg_shell_cmd("chmod 777 /dev/log/lidbg_udisk_shell.conf");
            lidbg_pm_install(get_lidbg_file_path(buff, "fileserver.apk"));
            lidbg_pm_install(get_lidbg_file_path(buff, "MobileRateFlow.apk"));
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#001"))
        {
            lidbg_chmod("/sdcard");
            CREATE_KTHREAD(thread_enable_logcat2, NULL);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#053"))
        {
            lidbg_chmod("/sdcard");
            CREATE_KTHREAD(thread_system_trace, NULL);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#002"))
        {
            lidbg_chmod("/data");
#ifdef USE_CALL_USERHELPER
            k2u_write(LOG_DMESG);
#else
            CREATE_KTHREAD(thread_enable_dmesg, NULL);
#endif
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#003"))
        {
#ifdef USE_CALL_USERHELPER
            k2u_write(LOG_CLEAR_LOGCAT_KMSG);
#else
            lidbg("clear+logcat*&&kmsg*\n");
#ifdef SOC_mt3360
            lidbg_shell_cmd("rm /sdcard/logcat*");
            lidbg_shell_cmd("rm /sdcard/kmsg*");
            logcat_enabled = false;
#else
            lidbg_shell_cmd("rm /data/logcat*");
            lidbg_shell_cmd("rm /data/kmsg*");
#endif
            lidbg("clear-logcat*&&kmsg*\n");
#endif
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#004"))
        {
#ifdef USE_CALL_USERHELPER
            k2u_write(LOG_SHELL_TOP_DF_PS);
#else
            lidbg("\n\nLOG_SHELL_TOP_DF_PS+\n");
            lidbg_shell_cmd("date > /data/machine.txt");
            lidbg_shell_cmd("cat /proc/cmdline >> /data/machine.txt");
            lidbg_shell_cmd("getprop fly.version.mcu >> /data/machine.txt");
            lidbg_shell_cmd("top -n 3 -t >/data/top.txt &");
            lidbg_shell_cmd("screencap -p /data/screenshot.png &");
            lidbg_shell_cmd("ps > /data/ps.txt");
            lidbg_shell_cmd("df > /data/df.txt");
            lidbg_shell_cmd("lsmod > /data/lsmod.txt");
            lidbg_shell_cmd("chmod 777 /data/*.txt");
            lidbg_shell_cmd("chmod 777 /data/*.png");
            lidbg("\n\nLOG_SHELL_TOP_DF_PS-\n");
#endif
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#010"))
        {
            lidbg("USB_ID_LOW_HOST\n");
            USB_ID_LOW_HOST;
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#011"))
        {
            lidbg("USB_ID_HIGH_DEV\n");
            USB_ID_HIGH_DEV;
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#012"))
        {
            lidbg_trace_msg_disable(1);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#013"))
        {
            encode = false;
            lidbg_chmod("/data");
            lidbg_fifo_get(glidbg_msg_fifo, LIDBG_LOG_DIR"lidbg_mem_log.txt", 0);
            CREATE_KTHREAD(thread_dump_log, NULL);
        }
        else if (!strcmp(argv[1], "*158#014"))
            lidbg_system_switch(true);
        else if (!strcmp(argv[1], "*158#015"))
            lidbg_system_switch(false);
        else if (!strcmp(argv[1], "*158#016"))
            set_wifi_adb_mode(true);
        else if (!strcmp(argv[1], "*158#017"))
            set_wifi_adb_mode(false);
        else if (!strcmp(argv[1], "*158#018"))
        {
            lidbg_shell_cmd("mount -o remount /system");
            lidbg_shell_cmd("mount -o remount /flysystem");
            lidbg_shell_cmd("rm /flysystem/lib/out/"FLY_GPS_SO);
            lidbg_shell_cmd("rm /system/lib/modules/out/"FLY_GPS_SO);
            lidbg_shell_cmd("rm /flysystem/lib/hw/"FLY_GPS_SO);
            lidbg_domineering_ack();
            msleep(3000);
            lidbg_reboot();
        }
        else if (!strcmp(argv[1], "*158#019"))
        {
            g_recovery_meg->bootParam.upName.val = 1;
            if(flyparameter_info_save(g_recovery_meg))
            {
                lidbg_domineering_ack();
                msleep(3000);
                lidbg_reboot();
            }
        }
        else if (!strcmp(argv[1], "*158#020"))
        {
            g_recovery_meg->bootParam.upName.val = 0;
            if(flyparameter_info_save(g_recovery_meg))
            {
                lidbg_domineering_ack();
                msleep(3000);
                lidbg_reboot();
            }
        }
        else if (!strcmp(argv[1], "*158#021"))
        {
            encode = false;
            lidbg_chmod("/data");
            lidbg_fifo_get(glidbg_msg_fifo, LIDBG_LOG_DIR"lidbg_mem_log.txt", 0);
            CREATE_KTHREAD(thread_kmsg_fifo_save, NULL);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#022"))
        {
            lidbg_shell_cmd("mount -o remount /system");
            lidbg_shell_cmd("mkdir /sdcard/diag_logs");
            lidbg_shell_cmd("chmod 777 /sdcard/diag_logs");
            lidbg_shell_cmd("cp /flysystem/lib/out/DIAG.conf /sdcard/diag_logs/DIAG.cfg");
            lidbg_shell_cmd("chmod 777 /sdcard/diag_logs/DIAG.cfg");
            lidbg_shell_cmd("/system/bin/diag_mdlog &");
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#023"))
        {
            // cb_kv_show_temp(NULL, NULL);
            // lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#024"))
        {
            fan_enable = !fan_enable;
            if(fan_enable)
                LPC_CMD_FAN_ON;
            else
                LPC_CMD_FAN_OFF;
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#025"))
        {
            LPC_CMD_ACC_SWITCH_START;
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#026"))
        {
            fs_file_write2("/dev/lidbg_pm0", "flyaudio acc_history");
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#027"))
        {
            CREATE_KTHREAD(thread_antutu_test, NULL);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#028"))
        {
            lidbg_shell_cmd("mount -o remount /flysystem");
            lidbg_shell_cmd("rm /flysystem/lib/out/"FLY_GPS_SO);
            lidbg_shell_cmd("mount -o remount,ro /flysystem");
            lidbg_domineering_ack();
            msleep(3000);
            lidbg_reboot();
        }
        else if (!strcmp(argv[1], "*158#029"))
        {
            cb_kv_log_temp(NULL, NULL);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#030"))
        {
#ifdef SOC_msm8x26
            set_system_performance(3);
            lidbg_domineering_ack();
#endif
        }
        else if (!strcmp(argv[1], "*158#031"))
        {
            lidbg("gps_debug\n");
            lidbg_shell_cmd("echo -n 'file lidbg_gps.c +p' > /sys/kernel/debug/dynamic_debug/control");
        }
        else if (!strcmp(argv[1], "*158#032"))
        {
            lidbg("AD_debug\n");
            lidbg_shell_cmd("echo -n 'file lidbg_ad_msm8x26.c +p' > /sys/kernel/debug/dynamic_debug/control");
        }
        else if (!strcmp(argv[1], "*158#033"))
        {
            lidbg("ts_debug\n");
            lidbg_shell_cmd("echo -n 'file lidbg_ts.c +p' > /sys/kernel/debug/dynamic_debug/control");
            lidbg_shell_cmd("echo -n 'file lidbg_ts_probe_new.c +p' > /sys/kernel/debug/dynamic_debug/control");
        }
        else if (!strcmp(argv[1], "*158#034"))
        {
            lidbg("temp_debug\n");
            lidbg_shell_cmd("echo -n 'file lidbg_temp.c +p' > /sys/kernel/debug/dynamic_debug/control");
        }
        else if (!strcmp(argv[1], "*158#035"))
        {
            lidbg("lowmemorykill debug\n");
            lidbg_shell_cmd("echo -n 'file lowmemorykillprotecter.c +p' > /sys/kernel/debug/dynamic_debug/control");
        }
        else if (!strcmp(argv[1], "*158#040"))
        {
            CREATE_KTHREAD(thread_monkey_test, NULL);
        }
        else if (!strcmp(argv[1], "*158#041"))
        {
            lidbg_shell_cmd("echo 0 > /proc/sys/kernel/printk");
        }
        else if (!strcmp(argv[1], "*158#042"))
        {
            lidbg("disable adb\n");
            lidbg_stop("adbd");
        }
        else if (!strcmp(argv[1], "*158#043"))
        {
            lidbg("enable adb\n");
            lidbg_start("adbd");
        }
        else if (!strncmp(argv[1], "*158#044", 8))
        {
            //????,?*158#0448010,??????ACC?????,???70S,?????ACC?????,????????80,10S??
            char s[100];
            int n;
            n = strlen(argv[1]);
            if(n != 12)
                strcpy(argv[1], "*158#0448010");

            lidbg("start SleepTest acc test %s\n", (argv[1] + 8));
            sprintf(s, "am start -n com.example.sleeptest/com.example.sleeptest.SleepTest --ei time %s", (argv[1] + 8));
            lidbg_shell_cmd(s);
            lidbg("cmd : %s", s);
        }
        else if (!strncmp(argv[1], "*158#045", 8))
        {
            //opt args,ex:*158#0450
            int n;
            n = strlen(argv[1]);
            if(n != 9)//wrong args
            {
                lidbg("wrong args!");
                return;
            }
            lidbg("--------RGB_LED MODE:%s-----------", argv[1] + 8);
            if(!strcmp((argv[1] + 8), "1"))
                fs_file_write2("/dev/lidbg_rgb_led0", "rgb 255 0 0");
            else if(!strcmp((argv[1] + 8), "2"))
                fs_file_write2("/dev/lidbg_rgb_led0", "init");
            else if(!strcmp((argv[1] + 8), "3"))
                fs_file_write2("/dev/lidbg_rgb_led0", "stop");
            else if(!strcmp((argv[1] + 8), "4"))
                fs_file_write2("/dev/lidbg_rgb_led0", "reset");
            else if(!strcmp((argv[1] + 8), "5"))
                fs_file_write2("/dev/lidbg_rgb_led0", "play");
        }
        else if (!strcmp(argv[1], "*158#046"))
        {
            set_cpu_governor(1);
        }
        else if (!strcmp(argv[1], "*158#047"))
        {
            set_cpu_governor(0);
        }
        else if (!strcmp(argv[1], "*158#048"))
        {
            lidbg("**********set RmtCtrlenable false**********\n");
            lidbg_shell_cmd("setprop persist.lidbg.RmtCtrlenable false");
        }
        else if (!strcmp(argv[1], "*158#049"))
        {
            lidbg("**********set RmtCtrlenable true**********\n");
            lidbg_shell_cmd("setprop persist.lidbg.RmtCtrlenable true");
        }
        else if (!strcmp(argv[1], "*158#050"))
        {
            lidbg_chmod("/sdcard");
            CREATE_KTHREAD(thread_enable_top, NULL);
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#051"))
        {
            lidbg_chmod("/data");
#ifdef USE_CALL_USERHELPER
            k2u_write(LOG_LOGCAT);
#else
            CREATE_KTHREAD(thread_enable_logcat, NULL);
#endif
            lidbg_domineering_ack();
        }
        else if (!strcmp(argv[1], "*158#052"))
        {
            lidbg("-------udisk reset -----");
            fs_file_write2("/dev/lidbg_pm0", "ws udisk_reset");
            lidbg_domineering_ack();
        }
	else if (!strncmp(argv[1], "*158#054", 8))
        {
            //opt args,ex:*158#0540
            int n;
            n = strlen(argv[1]);
            if(n != 9)//wrong args
            {
                lidbg("wrong args!");
                return;
            }
            lidbg("--------UVCCAM MODE:%s-----------", argv[1] + 8); 
            if(!strcmp((argv[1] + 8), "1"))//start recording
            {
                 lidbg("-------uvccam recording -----");
	        lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
	        if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
	        else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
            }
            else if(!strcmp((argv[1] + 8), "0"))//stop recording
	    {
		lidbg("-------uvccam stop_recording -----");
	        lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
	    }
                
        }
        else if (!strcmp(argv[1], "*168#001"))
        {
            encode = true;
            lidbg_chmod("/data");
            lidbg_fifo_get(glidbg_msg_fifo, LIDBG_LOG_DIR"lidbg_mem_log.txt", 0);
            CREATE_KTHREAD(thread_dump_log, NULL);
        }
        lidbg_domineering_ack();
    }
    else if(!strcmp(argv[0], "monkey") )
    {
        int enable, gpio, on_en, off_en, on_ms, off_ms;
        enable = simple_strtoul(argv[1], 0, 0);
        gpio = simple_strtoul(argv[2], 0, 0);
        on_en = simple_strtoul(argv[3], 0, 0);
        off_en = simple_strtoul(argv[4], 0, 0);
        on_ms = simple_strtoul(argv[5], 0, 0);
        off_ms = simple_strtoul(argv[6], 0, 0);
        monkey_run(enable);
        monkey_config(gpio, on_en, off_en, on_ms, off_ms);
    }
    else if(!strcmp(argv[0], "recordenable") )
    {
        lidbg("-------uvccam recording -----");
        lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 1");
        if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
        else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video2 -c -f H264 -r &");
    }
    else if(!strcmp(argv[0], "recorddisable") )
    {
        lidbg("-------uvccam stop_recording -----");
        lidbg_shell_cmd("setprop persist.lidbg.uvccam.recording 0");
    }
    else if(!strcmp(argv[0], "captureenable") )
    {
        lidbg("-------uvccam capture-----");
	if(g_var.is_fly) lidbg_shell_cmd("./flysystem/lib/out/lidbg_testuvccam /dev/video1 -c -f mjpg -S &");
        else lidbg_shell_cmd("./system/lib/modules/out/lidbg_testuvccam /dev/video1 -c -f mjpg -S &");
    }
    else if(!strcmp(argv[0], "flyparameter") )
    {
        int para_count = argc - 1;
        char pre = 'N';
        for(i = 0; i < para_count; i++)
        {
            pre = g_recovery_meg->hwInfo.info[i];
            g_recovery_meg->hwInfo.info[i] = (int)simple_strtoul(argv[i + 1], 0, 0) + '0';
            lidbg("flyparameter-char.info[%d]:old,now[%d,%d]", i, pre - '0', g_recovery_meg->hwInfo.info[i] - '0');
        }
        if(flyparameter_info_save(g_recovery_meg))
        {
            lidbg_domineering_ack();
            msleep(3000);
            lidbg_reboot();
        }
    }
#ifndef SOC_msm8x25
    else if(!strcmp(argv[0], "pm") )
    {
        int enable;
        enable = simple_strtoul(argv[1], 0, 0);
        SOC_PM_STEP((fly_pm_stat_step)enable, NULL);
    }
    else if(!strcmp(argv[0], "pm1") )
    {
        int enable;
        enable = simple_strtoul(argv[1], 0, 0);
        LINUX_TO_LIDBG_TRANSFER((linux_to_lidbg_transfer_t)enable, NULL);
    }
    else if(!strcmp(argv[0], "fread") )
    {
        char buff[64] = {0};
        int len = fs_file_read(argv[1], buff, 0, sizeof(buff));
        buff[len - 1] = '\0';
        lidbg("%d,%s:[%s]\n", len, argv[1], buff);
    }
    else if(!strcmp(argv[0], "fwrite") )
    {
        fs_file_write2(argv[1], argv[2]);
        lidbg("%s:[%s]\n", argv[1], argv[2]);
    }
    else if (!strcmp(argv[0], "lpc"))
    {
        int para_count = argc - 1;
        u8 lpc_buf[10] = {0};
        for(i = 0; i < para_count; i++)
        {
            lpc_buf[i] = simple_strtoul(argv[i + 1], 0, 0);
            lidbg("%d ", lpc_buf[i]);
        }
        lidbg("para_count = %d\n", para_count);
        SOC_LPC_Send(lpc_buf, para_count);
    }
    else if(!strcmp(argv[0], "irq") )
    {
        int irq;
        irq = simple_strtoul(argv[1], 0, 0);
        SOC_IO_ISR_Add(irq, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_ONESHOT, TEST_isr, NULL);
        SOC_IO_ISR_Enable(irq);
        lidbg("SOC_IO_ISR_Add[%d]\n", irq);
    }
    else if (!strcmp(argv[0], "vol"))
    {
        //   #ifndef SOC_mt3360
        //       int vol;
        //       vol = simple_strtoul(argv[1], 0, 0);
        //       SAF7741_Volume(vol);
        //   #endif
    }
    else if (!strcmp(argv[0], "screen_shot"))
    {
        CREATE_KTHREAD(thread_screenshot, NULL);
    }
    else if (!strcmp(argv[0], "readdir"))
    {
        if(argv[1])
            LIDBG_WARN("%s:file count=%d\n", argv[1], lidbg_readdir_and_dealfile(argv[1], callback_func_test_readdir));
        else
            LIDBG_ERR("err:echo readdir /storage/udisk/conf > /dev/lidbg_drivers_dbg0\n");
    }
#endif

#ifdef SOC_msm8x25
    else if(!strcmp(argv[0], "video"))
    {
        int new_argc;
        char **new_argv;
        new_argc = argc - 1;
        new_argv = argv + 1;
        lidbg_video_main(new_argc, new_argv);
    }
#endif
}
