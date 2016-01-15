package com.fly.flybootservice;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.WallpaperInfo;
import android.app.WallpaperManager;
import android.app.PendingIntent;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.Service;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.WallpaperInfo;
import android.app.WallpaperManager;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.IntentFilter.MalformedMimeTypeException;
import android.content.SharedPreferences;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Bundle;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.os.Message;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.media.AudioManager;
import android.util.Log;
import android.text.TextUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.DataInputStream;
import java.io.FileNotFoundException;
import java.util.List;
import java.io.IOException;

import java.util.List;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import java.util.ArrayList;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
/*
 * ScreenOn ScreenOff DeviceOff Going2Sleep 四种状态分别表示：1.表示正常开屏状态2.表示关屏，但没关外设的状态
 * 0'~30'的阶段3.表示关屏关外设，但没到点进入深度休眠 30'~60'的阶段4.表示发出休眠请求到执行快速休眠 60'后,即进入深度休眠
 */
public class FlyBootService extends Service {
    private static final String TAG = "boot";

    private static int FBS_SCREEN_OFF = 0;
    private static int FBS_DEVICE_DOWN = 1;
    private static int FBS_FASTBOOT_REQUEST = 2;
    private static int FBS_ANDROID_DOWN = 3;
    private static int FBS_GOTO_SLEEP = 4;
    private static int FBS_KERNEL_DOWN = 5;
    private static int FBS_KERNEL_UP = 6;
    private static int FBS_ANDROID_UP = 7;
    private static int FBS_DEVICE_UP = 8;
    private static int FBS_SCREEN_ON = 9;
    private static int FBS_SLEEP_TIMEOUT = 10;
    public static String action = "com.flyaudio.power";
    public static String PowerBundle = "POWERBUNDLE";
    public static String keyScreenOn = "KEY_SCREEN_ON";
    public static String keyScreenOFF = "KEY_SCREEN_OFF";
    public static String keyEearlySusupendON = "KEY_EARLY_SUSUPEND_ON";
    public static String keyEearlySusupendOFF = "KEY_EARLY_SUSUPEND_OFF";
    public static String keyFastSusupendON = "KEY_FAST_SUSUPEND_ON";
    public static String keyFastSusupendOFF = "KEY_FAST_SUSUPEND_OFF";
    public static String KeyBootState = "KEYBOOTSTATE";
    private static String file = "/dev/lidbg_pm0";
    private static String pmFile = "/dev/flyaudio_pm0";

    private static String SYSTEM_RESUME = "com.flyaudio.system.resume";

    private static FlyBootService mFlyBootService;

    public PowerManager pm;
    private PowerManager fbPm = null;
    public static WakeLock mWakeLock = null;
    private ActivityManager mActivityManager = null;
    private PackageManager mPackageManager = null;
    private static boolean bIsKLDRunning = true;
    private static boolean sendBroadcastDone = false;
    private static boolean firstBootFlag = false;
    private boolean booleanRemoteControl = false;
    private boolean booleanAccWakedupState = false;
    private static int pmState = -1;
    private int intPlatformId = 0;
    private boolean blSuspendUnairplaneFlag = false;

    //do not force-stop apps in list
    private String[] mWhiteList = null;
    //list who can access Internet
    private String[] mInternelWhiteList = null;
    //list about all apps'uid who request Internet permission	
    private List<Integer> mInternelAllAppListUID= new ArrayList<Integer>();
    private List<Integer> mInternelWhiteAppListUID= new ArrayList<Integer>();
    private boolean dbgMode = true;
    private boolean mInterneWhiteListAppProtectEn = true;
    private boolean mFlyaudioInternetActionEn = true;
    private boolean mKillProcessEn = true;
    private boolean mInterneDisableAll = false;

    String mInternelBlackList[] = {
            "com.qti.cbwidget"
    };
    // add launcher in protected list
    String systemLevelProcess[] = {
            "com.android.flyaudioui",
            "cn.flyaudio.android.flyaudioservice",
            "cn.flyaudio.navigation", "com.android.launcher",
            "cn.flyaudio.osd.service", "android.process.acore",
            "android.process.media", "com.android.systemui",
            "com.android.deskclock", "sys.DeviceHealth", "system",
            "com.fly.flybootservice","com.android.keyguard","android.policy","com.android.launcher3",
	        "com.example.sleeptest","com.flyaudio.proxyservice","com.goodocom.gocsdk","cn.flyaudio.assistant","cn.flyaudio.handleservice"
    };

    @Override
    public void onCreate() {
        super.onCreate();
        LIDBG_PRINT("flybootservice onCreate......");
        writeToFile("/dev/lidbg_pm0","flyaudio PmServiceStar");

	mFlyBootService = this;
	acquireWakeLock();
	mPackageManager = this.getPackageManager();
	mActivityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
	mWhiteList = FileReadList("/flysystem/lib/out/appProtectList.conf","\n");
	mInternelWhiteList = FileReadList("/flysystem/lib/out/appInternetProtectList.conf","\n");
	DUMP();
        LIDBG_PRINT("flybootservice start [getInternelAllAppUids]");
	getInternelAllAppUids(mInternelAllAppListUID);
        LIDBG_PRINT("flybootservice stop [getInternelAllAppUids]");
        FlyaudioBlackListInternetControl(false);
	IntentFilter filter = new IntentFilter();
	filter.addAction("android.intent.action.BOOT_COMPLETED");
	filter.addAction("com.lidbg.flybootserver.action");
	filter.setPriority(Integer.MAX_VALUE);
	registerReceiver(myReceiver, filter);

	intPlatformId = SystemProperties.getInt("persist.lidbg.intPlatformId", 0);
	switch (intPlatformId) {
		case 0:	//msm7627a
		case 1:	//msm8625
		case 2:	//msm8226 Android_4.4.2
		case 3:	//msm8926 Android_4.4.4
		case 4:	//msm8974 Android_4.4.4
		case 5:	//mt3360  Android_4.2
		case 6:	//msm8226 M8626AAAAANLYD1431 Android_5.0
		case 7:	//msm8974 M8974AAAAANLYD4275 Android_5.1
		case 8:	//rk3188 Radxa Rock Pro 4.4.2
		case 9:	//rk3188 PX3 Pro 4.4.4
		case 10:	//msm8226 Android_5.1.1
		case 12:	//msm8226 Android_4.4.4
		case 13:	//A80 Android_4.4
			blSuspendUnairplaneFlag = false;
			break;
		case 11:	//msm8909 Android_5.1.1
			blSuspendUnairplaneFlag = true;
			reSetPmState();
			break;
		default:
			break;
	}
	LIDBG_PRINT("flybootservice get platform_id: " + intPlatformId + ", SuspendUnairplane: " + blSuspendUnairplaneFlag);

        new Thread() {
            @Override
            public void run() {
					while(true){
						pmState = readFromFile(pmFile);
						if(pmState < 0)
						{
							LIDBG_PRINT("FlyBootService get pm state failed.");
							delay(500);
						}
						else{
							if(pmState == FBS_SCREEN_OFF){
								LIDBG_PRINT("FlyBootService get pm state: FBS_SCREEN_OFF");
								SendBroadcastToService(KeyBootState, keyScreenOFF);
								FlyaudioInternetDisable();
							}else if(pmState == FBS_DEVICE_DOWN){
								LIDBG_PRINT("FlyBootService get pm state: FBS_DEVICE_DOWN");
								final boolean booleanRemoteControl = SystemProperties.getBoolean("persist.lidbg.RmtCtrlenable",false);
								if((booleanRemoteControl) || (!blSuspendUnairplaneFlag)){
									LIDBG_PRINT("FlyBootService device down enable AirplaneMode");
									enterAirplaneMode();
								}else
									LIDBG_PRINT("FlyBootService device down disable AirplaneMode");
								SendBroadcastToService(KeyBootState, keyEearlySusupendOFF);
								LIDBG_PRINT("FlyBootService sent device_down to hal");
							}else if(pmState == FBS_FASTBOOT_REQUEST){
								LIDBG_PRINT("FlyBootService get pm state: FBS_FASTBOOT_REQUEST");
							}else if(pmState == FBS_ANDROID_DOWN){
								LIDBG_PRINT("FlyBootService get pm state: FBS_ANDROID_DOWN");
								SendBroadcastToService(KeyBootState, keyFastSusupendOFF);
								start_fastboot();
							}else if(pmState == FBS_GOTO_SLEEP){
								LIDBG_PRINT("FlyBootService get pm state: FBS_GOTO_SLEEP");
								//if(blSuspendUnairplaneFlag)
								//	KillProcess();
								system_gotosleep();
							}else if(pmState == FBS_KERNEL_DOWN){
								LIDBG_PRINT("FlyBootService get pm state: FBS_KERNEL_DOWN");
							}else if(pmState == FBS_KERNEL_UP){
								LIDBG_PRINT("FlyBootService get pm state: FBS_KERNEL_UP");
								if(blSuspendUnairplaneFlag){
									fbPm = (PowerManager) getSystemService(Context.POWER_SERVICE);
									fbPm.wakeUp(SystemClock.uptimeMillis());
								}
							}else if(pmState == FBS_ANDROID_UP){
								LIDBG_PRINT("FlyBootService get pm state: FBS_ANDROID_UP");
								SendBroadcastToService(KeyBootState, keyFastSusupendON);
								sendBroadcast(new Intent(SYSTEM_RESUME));
								Intent intentBoot = new Intent(Intent.ACTION_BOOT_COMPLETED);
								intentBoot.putExtra("flyauduio_accon", "accon");
								sendBroadcast(intentBoot);
							}else if(pmState == FBS_DEVICE_UP){
								LIDBG_PRINT("FlyBootService get pm state: FBS_DEVICE_UP");
								if(!blSuspendUnairplaneFlag)
									restoreAirplaneMode(mFlyBootService);
								SendBroadcastToService(KeyBootState, keyEearlySusupendON);
							}else if(pmState == FBS_SCREEN_ON){
								LIDBG_PRINT("FlyBootService get pm state: FBS_SCREEN_ON");
								acquireWakeLock();
								//FlyaudioWhiteListInternetControl(true);
								InternetEnable();
								FlyaudioInternetEnable();
								SendBroadcastToService(KeyBootState, keyScreenOn);
								system_resume();
							}else if(pmState == FBS_SLEEP_TIMEOUT){
								LIDBG_PRINT("FlyBootService get pm state: FBS_SLEEP_TIMEOUT");
								if(blSuspendUnairplaneFlag)
									KillProcess(true);
								//FlyaudioWhiteListInternetControl(false);
								InternetDisable();
							}else
								LIDBG_PRINT("FlyBootService undefined pm state: " + pmState);
						}
					}
            }
        }.start();

    }

	public void reSetPmState () {
		LIDBG_PRINT("flybootservice reset PM state.");
		FBS_SCREEN_OFF = 0;
		FBS_GOTO_SLEEP = 1;
		FBS_DEVICE_DOWN = 2;
		FBS_FASTBOOT_REQUEST = 3;
		FBS_ANDROID_DOWN = 4;
		FBS_SLEEP_TIMEOUT = 5;
		FBS_KERNEL_DOWN = 6;
		FBS_KERNEL_UP = 7;
		FBS_ANDROID_UP = 8;
		FBS_DEVICE_UP = 9;
		FBS_SCREEN_ON = 10;
	}

	//am broadcast -a com.lidbg.flybootserver.action --ei action 0
	private BroadcastReceiver myReceiver = new BroadcastReceiver()
	{
		@Override
		public void onReceive(Context context, Intent intent)
		{
			if ( intent == null)
			{
				LIDBG_PRINT("err.return:intent == null \n");
				return;
			}

			LIDBG_PRINT("flybootserver.BroadcastReceiver:"+intent.getAction()+"\n");
			if (intent.getAction().equals("android.intent.action.BOOT_COMPLETED"))
			{
				writeToFile("/dev/lidbg_interface", "BOOT_COMPLETED");
				return;
			}
			if ( !intent.hasExtra("action"))
			{
				LIDBG_PRINT("err.return:!intent.hasExtra(\"action\")\n");
				return;
			}
			int action = intent.getExtras().getInt("action");
			LIDBG_PRINT("BroadcastReceiver.action:"+action+"\n");
			switch (action)
			{
			case 0:
				FlyaudioInternetDisable();
			break;
			case 1:
				FlyaudioInternetEnable();
			break;
			case 2:
				dbgMode=!dbgMode;
				LIDBG_PRINT("dbgMode->"+ dbgMode+"\n");
			break;
			case 3:
				DUMP();
			break;
			case 4:
				mInterneWhiteListAppProtectEn=false;
				LIDBG_PRINT("mInterneWhiteListAppProtectEn->"+ mInterneWhiteListAppProtectEn+"\n");
			break;
			case 5:
				mInterneWhiteListAppProtectEn=true;
				LIDBG_PRINT("mInterneWhiteListAppProtectEn->"+ mInterneWhiteListAppProtectEn+"\n");
			break;
			case 6:
				mFlyaudioInternetActionEn=false;
				LIDBG_PRINT("mFlyaudioInternetActionEn->"+ mFlyaudioInternetActionEn+"\n");
			break;
			case 7:
				mFlyaudioInternetActionEn=true;
				LIDBG_PRINT("mFlyaudioInternetActionEn->"+ mFlyaudioInternetActionEn+"\n");
			break;
			case 8:
				mKillProcessEn=false;
				LIDBG_PRINT("mKillProcessEn->"+ mKillProcessEn+"\n");
			break;
			case 9:
				mInterneDisableAll=true;
				LIDBG_PRINT("mInterneDisableAll->"+ mInterneDisableAll+"\n");
			break;
			case 10:
				mInterneDisableAll=false;
				LIDBG_PRINT("mInterneDisableAll->"+ mInterneDisableAll+"\n");
			break;
			case 11:
				FlyaudioWhiteListInternetControl(false);
			break;
			case 12:
				FlyaudioWhiteListInternetControl(true);
			break;
			case 13:
				FlyaudioBlackListInternetControl(true);
			break;

			default:
			LIDBG_PRINT("BroadcastReceiver.action:unkown"+action+"\n");
			break;
			}
			// TODO Auto-generated method stub
		}

	};
	public String[] FileReadList(String fileName, String split)
	{
		// TODO Auto-generated method stub
		String[] mList = null;
		String tempString = FileRead(fileName);
		if (tempString != null && tempString.length() > 2)
		{
			mList = tempString.trim().split(split);
		}
		return mList;
	}
	public String FileRead(String fileName)
	{
		String res = null;
		File mFile = new File(fileName);
		if (!mFile.exists() || !mFile.canRead())
		{
			return res;
		}

		try
		{
			FileInputStream inputStream = new FileInputStream(mFile);
			int len = inputStream.available();
			byte[] buffer = new byte[len];
			inputStream.read(buffer);
			res = new String(buffer, "UTF-8");
			// toast_show("resString="+fineString);
			inputStream.close();
		} catch (FileNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		return res;
	}
    @Override
    public void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        LIDBG_PRINT("flybootservice destory...");
    }

    public void acquireWakeLock() {
        if (mWakeLock == null) {
            LIDBG_PRINT(" +++++ acquire flybootservice wakelock +++++ ");
            pm = (PowerManager) getSystemService(Context.POWER_SERVICE);

            mWakeLock = (WakeLock) pm.newWakeLock(
                    PowerManager.PARTIAL_WAKE_LOCK, "flytag");
            if (mWakeLock != null && !mWakeLock.isHeld())
                mWakeLock.acquire();
            else
				  LIDBG_PRINT(" Error: new flybootservice wakelock failed !");
        }
    }

    public static void releaseWakeLock() {
        LIDBG_PRINT(" ----- release flybootservice wakelock ----- ");
        if (mWakeLock != null && mWakeLock.isHeld()) {
            mWakeLock.release();
            if(mWakeLock.isHeld())
                LIDBG_PRINT(" Error: release flybootservice wakelock failed !");
            mWakeLock = null;
        }
    }


    public static void restoreAirplaneMode(Context context) {
        LIDBG_PRINT("restoreAirplaneMode+");
        if (Settings.Global.getInt(context.getContentResolver(), "fastboot_airplane_mode", -1) != 0) {
            return;
        }

        boolean b = Settings.Global.putInt(context.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0);

        LIDBG_PRINT("restoreAirplane isSet:"+b);
        
        Intent intentAirplane = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        intentAirplane.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
        intentAirplane.putExtra("state", false);

        context.sendBroadcastAsUser(intentAirplane, UserHandle.ALL);
        Settings.Global.putInt(context.getContentResolver(), "fastboot_airplane_mode", -1);

        LIDBG_PRINT("restoreAirplaneMode end");
    }

    public void SendBroadcastToService(String key, String value) {
        // Log.d(TAG, " PowerBundle :  " + value);
        Intent intent = new Intent(action);
        Bundle bundle = new Bundle();
        bundle.putString(key, value);
        intent.putExtra(PowerBundle, bundle);
        sendBroadcast(intent);
    }

	private void system_gotosleep(){

		LIDBG_PRINT(" ********** system gotosleep ********** ");
		if(blSuspendUnairplaneFlag){
			fbPm = (PowerManager) getSystemService(Context.POWER_SERVICE);
			msgTokenal("flyaudio gotosleep");
			fbPm.goToSleep(SystemClock.uptimeMillis());
		}else{
			fbPm.goToSleep(SystemClock.uptimeMillis());
			msgTokenal("flyaudio gotosleep");
		}
		releaseWakeLock();
	}

	private void start_fastboot(){
		firstBootFlag = true;

		LIDBG_PRINT(" ********** start fastboot ********** ");
		fbPm = (PowerManager) getSystemService(Context.POWER_SERVICE);

		powerOffSystem();
	}

	private void system_resume(){
		if(firstBootFlag){
			LIDBG_PRINT("FlyBootService system resume...");
			enableShowLogo(true);
			SystemClock.sleep(3000);
			powerOnSystem(mFlyBootService);
		}
	}

    BroadcastReceiver sendBroadcasResult = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
                Log.d(TAG, "Send Broadcast finish in " + SystemClock.elapsedRealtime());
                LIDBG_PRINT("o Send Broadcast finish");
                sendBroadcastDone = true;
        }
    };

    public boolean IsKLDrunning() {
        ActivityManager am = (ActivityManager) this
                .getSystemService(Context.ACTIVITY_SERVICE);
        List<RunningTaskInfo> list = am.getRunningTasks(200);

        for (RunningTaskInfo info : list) {
            if (info.topActivity.getPackageName().equals(
                    "cld.navi.c2739.mainframe")
                    && info.baseActivity.getPackageName().equals(
                            "cld.navi.c2739.mainframe")
                    || info.topActivity.getPackageName().equals(
                            "com.autonavi.xmgd.navigator")
                    && info.baseActivity.getPackageName().equals(
                            "com.autonavi.xmgd.navigator")
                    || info.topActivity.getPackageName().equals(
                            "com.baidu.BaiduMap")
                    && info.baseActivity.getPackageName().equals(
                            "com.baidu.BaiduMap")
		|| info.topActivity.getPackageName().equals(
                            "com.waze")
                    && info.baseActivity.getPackageName().equals(
                            "com.waze")) {
                return true;
            }
        }
        return false;
    }

    private boolean isFlyApp(String packageName) {
        return ("com.android.flyaudioui").equals(packageName)
                || ("cn.flyaudio.media").equals(packageName)
                || ("com.qualcomm.fastboot").equals(packageName);
    }

    private void getLastPackage() {
        ActivityManager mAManager = (ActivityManager) this
                .getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RecentTaskInfo> list = mAManager
                .getRecentTasks(20, 0);

        for (ActivityManager.RecentTaskInfo item : list) {
            String packageName = item.baseIntent.getComponent()
                    .getPackageName();
            Log.d(TAG, "@@" + packageName);
            if (!isFlyApp(packageName)) {
                SystemProperties.set("fly.third.LastPageName", packageName);
                SystemProperties.set("fly.third.LastClassName",
                        item.baseIntent.getComponent().getClassName());
                Log.d(TAG, "LastPageName--->" + packageName);
                Log.d(TAG, "LastClassName--->"
                        + item.baseIntent.getComponent().getClassName());
                return;
            }
        }

    }

    private void powerOffSystem() {
		LIDBG_PRINT("powerOffSystem+");
		sendBecomingNoisyIntent();
		LIDBG_PRINT("powerOffSystem step 1");

		SystemProperties.set("ctl.start", "bootanim");
		LIDBG_PRINT("powerOffSystem step 2");

		LIDBG_PRINT("powerOffSystem step 3");
		getLastPackage();
		bIsKLDRunning = IsKLDrunning();

		if (bIsKLDRunning) {
		SystemProperties.set("fly.gps.run", "1");
			Log.d(TAG, "-----fly.gps.run----1----");
		} else {
			SystemProperties.set("fly.gps.run", "0");
			Log.d(TAG, "-----fly.gps.run-----0---");
		}
		LIDBG_PRINT("powerOffSystem step 4");
		//if(!blSuspendUnairplaneFlag)
			KillProcess(false);
		msgTokenal("flyaudio pre_gotosleep");
		LIDBG_PRINT("powerOffSystem-");
    }

    private void powerOnSystem(Context context) {
		LIDBG_PRINT("powerOnSystem-");
		if(!blSuspendUnairplaneFlag)
			restoreAirplaneMode(context);
		SystemProperties.set("ctl.stop", "bootanim");
		LIDBG_PRINT("powerOnSystem-");
    }

    // send broadcast to music application to pause music
    private void sendBecomingNoisyIntent() {
        sendBroadcast(new Intent(AudioManager.ACTION_AUDIO_BECOMING_NOISY));
    }

    private void KillProcess(boolean mWhiteListKillEn) {
	if(!mKillProcessEn)
	{
		LIDBG_PRINT("skip KillProcess.mKillProcessEn=false\n");
		return;
	}
	acquireWakeLock();
        List<ActivityManager.RunningAppProcessInfo> appProcessList = null;

        appProcessList = mActivityManager.getRunningAppProcesses();

        LIDBG_PRINT("begin to KillProcess."+(mWhiteList == null)+"/"+mWhiteListKillEn+"\n");
        for (ActivityManager.RunningAppProcessInfo appProcessInfo : appProcessList) {
            int pid = appProcessInfo.pid;
            int uid = appProcessInfo.uid;
            String processName = appProcessInfo.processName;
            if(blSuspendUnairplaneFlag){
				delay(200);
	            booleanAccWakedupState = SystemProperties.getBoolean("persist.lidbg.AccWakedupState",false);
	            if(booleanAccWakedupState){
	                LIDBG_PRINT("Prop AccWakedupState be set:" + booleanAccWakedupState + ", stop kill process.");
	                break;
	            }
            }
            if (isKillableProcess(processName)) {
                LIDBG_PRINT(processName +"."+pid +" will be killed\n");
                mActivityManager.forceStopPackage(processName);
            }
	else if (mWhiteListKillEn&&processName.contains("flyaudio")) 
		{
			LIDBG_PRINT(processName +"."+pid +" will be killed in white list\n");
			mActivityManager.forceStopPackage(processName);
		}
        }
    releaseWakeLock();
    }

    private boolean isKillableProcess(String packageName) {
	if (mWhiteList != null)
	{
	        for (String processName : mWhiteList) {
	            if (processName.equals(packageName)) {
	                return false;
	            }
	        }
	}
	else
	{
	        for (String processName : systemLevelProcess) {
	            if (processName.equals(packageName)) {
	                return false;
	            }
	        }
	}

        String currentProcess = getApplicationInfo().processName;
        if (currentProcess.equals(packageName)) {
            return false;
        }

        // couldn't kill the live wallpaper process, if kill it, the system
        // will set the wallpaper as the default.
        WallpaperInfo info = WallpaperManager.getInstance(this)
                .getWallpaperInfo();
        if (info != null && !TextUtils.isEmpty(packageName)
                && packageName.equals(info.getPackageName())) {
            return false;
        }

        // couldn't kill the IME process.
        String currentInputMethod = Settings.Secure.getString(
                getContentResolver(), Settings.Secure.DEFAULT_INPUT_METHOD);
        if (!TextUtils.isEmpty(currentInputMethod)
                && currentInputMethod.startsWith(packageName)) {
            return false;
        }
        return true;
    }

    public static boolean isAirplaneModeOn(Context context) {
        return Settings.Global.getInt(context.getContentResolver(),
                Settings.Global.AIRPLANE_MODE_ON, 0) != 0;
    }
    
    private void enterAirplaneMode() {
		booleanRemoteControl = SystemProperties.getBoolean("persist.lidbg.RmtCtrlenable",false);

		if(booleanRemoteControl == true){
			LIDBG_PRINT("Flyaudio Remote-Control enabled, booleanRemoteControl:::"+booleanRemoteControl);
			if(isAirplaneModeOn(this)){
				LIDBG_PRINT("AirplaneMode is: ON, restore it.");
				restoreAirplaneMode(mFlyBootService);
			}else
				LIDBG_PRINT("AirplaneMode is: OFF.");
			return;
		}else{
			LIDBG_PRINT("Flyaudio Remote-Control disabled, booleanRemoteControl:::"+booleanRemoteControl);
			if (isAirplaneModeOn(this)) {
				LIDBG_PRINT("isAirplaneModeOn return.");
				return;
			}
			Settings.Global.putInt(getContentResolver(), "fastboot_airplane_mode", 0);

			// Change the system setting
			Settings.Global.putInt(getContentResolver(), Settings.Global.AIRPLANE_MODE_ON,1);

			// Update the UI to reflect system setting
			// Post the intent
			Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
			intent.putExtra("state", true);
			sendBroadcastAsUser(intent, UserHandle.ALL);
		}
    }


    private void enableShowLogo(boolean on) {
        String disableStr = (on ? "1" : "0");
        SystemProperties.set("hw.showlogo.enable", disableStr);
    }

	public int readFromFile(String fileName)
	{
		try
		{
			int temp = -1;
			DataInputStream dis = new DataInputStream(new FileInputStream(
					new File(fileName)));
			temp = dis.readUnsignedByte();
			dis.close();
			return temp;
		} catch (FileNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
			return -1;
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
			return -1;
		}
	}

    public void writeToFile(String filePath, String str) {
        File mFile = new File(filePath);
        if (mFile.exists()) {
            try {
                FileOutputStream fout = new FileOutputStream(
                        mFile.getAbsolutePath());
                byte[] bytes = str.getBytes();
                fout.write(bytes);
                fout.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                LIDBG_PRINT(" writeToFile  IOException " + e.getMessage());
                e.printStackTrace();
            }
        } else {
            LIDBG_PRINT("file not exists!!!");
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return null;
    }

    private static void msgTokenal(String msg) {
        // TODO Auto-generated method stub
            File mFile = new File("/dev/lidbg_pm0");
            String str = msg;
            if (mFile.exists()) {
                try {
                    LIDBG_PRINT(" msgTokenal");
                    FileOutputStream fout = new FileOutputStream(
                            mFile.getAbsolutePath());
                    byte[] bytes = str.getBytes();
                    fout.write(bytes);
                    fout.close();
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    LIDBG_PRINT(" writeToFile  IOException ");
                    e.printStackTrace();
                }
            } else {
                LIDBG_PRINT("file not exists!!!");
            }
        }

    private static void LIDBG_PRINT(String msg) {
        Log.d(TAG, msg);

        String newmsg = "lidbg_msg: " + msg;
        File mFile = new File("/dev/lidbg_msg");
        if (mFile.exists()) {
            try {
                FileOutputStream fout = new FileOutputStream(
                        mFile.getAbsolutePath());
                byte[] bytes = newmsg.getBytes();
                fout.write(bytes);
                fout.close();
            } catch (Exception e) {
                Log.e(TAG, "Failed to lidbg_printk");
            }

        } else {
            Log.e(TAG, "/dev/lidbg_msg not exist");
        }

    }

    private void delay(int ms) {
        try {
            Thread.sleep(ms);
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
	public void FlyaudioInternetEnable()
	{
	    LIDBG_PRINT("FlyaudioInternetEnable:"+mFlyaudioInternetActionEn+"/"+mInterneDisableAll+"\n");
	    if(!mFlyaudioInternetActionEn)
	    	    return;
	    if(mInterneDisableAll)
	    	    InternetEnable();
	    else
	    	    appInternetControl(true);
	}
	public void FlyaudioInternetDisable()
	{
	    LIDBG_PRINT("FlyaudioInternetDisable:"+mFlyaudioInternetActionEn+"/"+mInterneDisableAll+"\n");
	    if(!mFlyaudioInternetActionEn)
	    	    return;
	    if(mInterneDisableAll)
	    	    InternetDisable();
	    else
	    	    appInternetControl(false);
	}
	public void FlyaudioWhiteListInternetControl(boolean enable)
	{
	    LIDBG_PRINT("FlyaudioWhiteListInternetControl:"+enable+"\n");
		if (mInternelWhiteAppListUID!= null)
		{
			for (int i = 0; i < mInternelWhiteAppListUID.size(); i++)
			{
				Integer uid = mInternelWhiteAppListUID.get(i);	            
				LIDBG_PRINT("appInternetControl.exe.whitelist:" +(enable ? "enable/" : "disable/")+ i + "-->" + uid + "/" +  mPackageManager.getNameForUid(uid) + "\n");
				// -o rmnet+
				InternetControlUID(enable,uid);
			}
		}
	}
	public void FlyaudioBlackListInternetControl(boolean enable)
	{
	    LIDBG_PRINT("FlyaudioBlackListInternetControl:"+enable+"\n");
		if (mInternelBlackList!= null)
		{
			for (int i = 0; i < mInternelBlackList.length; i++)
			{
			ApplicationInfo info =getApplicationInfo(mInternelBlackList[i]);
				if (info!=null)
				{
					LIDBG_PRINT("appInternetControl.exe.BlackList:" +(enable ? "enable/" : "disable/")+ i + "-->" + info.uid + "/" +  info.packageName + "\n");
					InternetControlUID(false,info.uid);
				}
			}
		}
	}
	public void InternetDisable()
	{
		LIDBG_PRINT("InternetDisable\n");
		writeToFile("/dev/lidbg_misc0","flyaudio:iptables -t filter -P OUTPUT DROP");
		writeToFile("/dev/lidbg_misc0","flyaudio:iptables -t filter -P INPUT DROP");
		writeToFile("/dev/lidbg_misc0","flyaudio:iptables -t filter -P FORWARD DROP");
	}

	public void InternetEnable()
	{
		LIDBG_PRINT("InternetEnable\n");
		writeToFile("/dev/lidbg_misc0", "flyaudio:iptables -t filter -P OUTPUT ACCEPT");
		writeToFile("/dev/lidbg_misc0","flyaudio:iptables -t filter -P INPUT ACCEPT");
		writeToFile("/dev/lidbg_misc0","flyaudio:iptables -t filter -P FORWARD ACCEPT");
	}

	public void InternetControlUID(boolean enable,Integer uid )
	{
		writeToFile("/dev/lidbg_misc0", "flyaudio:iptables " + (enable ? "-D" : "-I") + " FORWARD  -m owner --uid-owner " + uid	+ " -j REJECT");
		writeToFile("/dev/lidbg_misc0", "flyaudio:iptables " + (enable ? "-D" : "-I") + " OUTPUT  -m owner --uid-owner " + uid + " -j REJECT");
		writeToFile("/dev/lidbg_misc0", "flyaudio:iptables " + (enable ? "-D" : "-I") + " INPUT  -m owner --uid-owner " + uid + " -j REJECT");
	}
	public List<Integer> getInternelAllAppUids(List<Integer> mlist)
	{
	    List<PackageInfo> packinfos = mPackageManager.getInstalledPackages(0);
	    int i = 0,j=0;
	for (PackageInfo info : packinfos)
	    {
	    	if ((info.applicationInfo.packageName.contains("flyaudio"))||((info.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) <= 0))
	                {
	                    int uid = info.applicationInfo.uid;
	                    if (isPackageInWhiteList(info.applicationInfo.packageName))
	                    {
				i++;
				LIDBG_PRINT("appInternetControl.protect:" + i + "-->" + uid + "/" +  info.applicationInfo.packageName + "\n");
				if (mInternelWhiteAppListUID!= null && !mInternelWhiteAppListUID.contains(uid))
				{
					mInternelWhiteAppListUID.add(uid);
				}
				continue;
	                    }
	                    j++;
	           	 //if (dbgMode)
	           	 //LIDBG_PRINT("appInternetControl.prepare:" + j + "-->" + uid + "/" +  info.applicationInfo.packageName + "\n");
	                    if (mlist != null && !mlist.contains(uid))
	                    {
	                        mlist.add(uid);
	                    }
	                }
	    }
	    return mlist;
	}
	
	public Boolean isPackageInWhiteList(String pkg)
	{
		if (mInternelWhiteList != null)
		{
			for (int i = 0; i < mInternelWhiteList.length; i++)
			{
				if (pkg.equals(mInternelWhiteList[i]))
				{
					return true;
				}
			}
		}
		return false;
	}
	public void appInternetControl(boolean enable)
	{
	    // TODO Auto-generated method stub
	    mInternelAllAppListUID = getInternelAllAppUids(mInternelAllAppListUID);

	    LIDBG_PRINT("appInternetControl:" + mInternelAllAppListUID.size()  +  "|" + mInterneWhiteListAppProtectEn+"\n");
	    if (mInternelAllAppListUID != null && mInternelAllAppListUID.size() > 0)
	    {
	        for (int i = 0; i < mInternelAllAppListUID.size(); i++)
	        {
	            Integer uid = mInternelAllAppListUID.get(i);	            
	            if (uid==1000)
		   continue;
	            if (dbgMode)
	            LIDBG_PRINT("appInternetControl.exe:" + i + "-->" + uid + "/" +  mPackageManager.getNameForUid(uid) + "\n");
	            // -o rmnet+
	            InternetControlUID(enable,uid);
	        }

	    }
	    else
	        LIDBG_PRINT("mInternelAllAppList == null || mInternelAllAppList.size() < 0");
	}
	private ApplicationInfo getApplicationInfo(String pkg)
	{
		ApplicationInfo ai = null;
		// TODO Auto-generated method stub
		try
		{
			ai = mPackageManager.getApplicationInfo(pkg, 0);
		} catch (NameNotFoundException e)
		{
			e.printStackTrace();
		}
		return ai;
	}
	public void DUMP()
	{
		if (mWhiteList != null)
		{
			for (int i = 0; i < mWhiteList.length; i++)
			{
				LIDBG_PRINT(i +"->"+ mWhiteList[i]+"\n");
			}
		}
		else
			LIDBG_PRINT("mWhiteList = null\n");
		if (mInternelWhiteList != null)
		{
			for (int i = 0; i < mInternelWhiteList.length; i++)
			{
				LIDBG_PRINT(i +"->"+ mInternelWhiteList[i]+"\n");
			}
		}
		else
			LIDBG_PRINT("mInternelWhiteList = null\n");
	}
}

