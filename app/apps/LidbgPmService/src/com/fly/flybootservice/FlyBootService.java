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
import android.os.Handler;
import android.os.IBinder;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.os.HandlerThread;
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
/*
 * ScreenOn ScreenOff DeviceOff Going2Sleep 四种状态分别表示：1.表示正常开屏状态2.表示关屏，但没关外设的状态
 * 0'~30'的阶段3.表示关屏关外设，但没到点进入深度休眠 30'~60'的阶段4.表示发出休眠请求到执行快速休眠 60'后,即进入深度休眠
 */
public class FlyBootService extends Service {

    // public static int NORMAL = 0;
    // public static int SUSPEND = 1;
    private static final String TAG = "boot";

    private final static int FBS_SCREEN_OFF = 0;
    private final static int FBS_DEVICE_DOWN = 1;
    private final static int FBS_ANDROID_DOWN = 2;
    private final static int FBS_GOTO_SLEEP = 3;
    private final static int FBS_KERNEL_DOWN = 4;
    private final static int FBS_KERNEL_UP = 5;
    private final static int FBS_ANDROID_UP = 6;
    private final static int FBS_DEVICE_UP = 7;
    private final static int FBS_SCREEN_ON = 8;
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

    private static String ACC_ON_FLYUI = "cn.flyaudio.action.ACCON";
    private static String ACC_OFF_FLYUI = "cn.flyaudio.action.ACCOFF";
    private static String SYSTEM_RESUME = "com.flyaudio.system.resume";

    private static FlyBootService mFlyBootService;

    private Handler mmHandler;
    private HandlerThread mHandlerThread;
    public PowerManager pm;
    private PowerManager fbPm = null;
    public static WakeLock mWakeLock = null;
    private ActivityManager mActivityManager = null;
    private static boolean bIsKLDRunning = true;
    private static boolean sendBroadcastDone = false;
    private static boolean firstBootFlag = false;
    private boolean booleanRemoteControl = false;
    private static int pmState = -1;

    // add launcher in protected list
    String systemLevelProcess[] = {
            "com.android.flyaudioui",
            "cn.flyaudio.android.flyaudioservice",
            "cn.flyaudio.navigation", "com.android.launcher",
            "cn.flyaudio.osd.service", "android.process.acore",
            "android.process.media", "com.android.systemui",
            "com.android.deskclock", "sys.DeviceHealth", "system",
            "com.fly.flybootservice","com.android.keyguard","android.policy","com.android.launcher3",
	        "com.example.sleeptest","com.flyaudio.proxyservice","com.goodocom.gocsdk"
    };

    @Override
    public void onCreate() {
        super.onCreate();
        LIDBG_PRINT("flybootservice onCreate......");

        mFlyBootService = this;
        acquireWakeLock();

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
							switch (pmState) {
								case FBS_SCREEN_OFF:
									LIDBG_PRINT("FlyBootService get pm state: FBS_SCREEN_OFF");
									SendBroadcastToService(KeyBootState, keyScreenOFF);
									break;
								case FBS_DEVICE_DOWN:
									LIDBG_PRINT("FlyBootService get pm state: FBS_DEVICE_DOWN");
									enterAirplaneMode();
									SendBroadcastToService(KeyBootState, keyEearlySusupendOFF);
									LIDBG_PRINT("FlyBootService sent device_down to hal");
									sendBroadcast(new Intent(ACC_OFF_FLYUI));
									break;
								case FBS_ANDROID_DOWN:
									LIDBG_PRINT("FlyBootService get pm state: FBS_ANDROID_DOWN");
									SendBroadcastToService(KeyBootState, keyFastSusupendOFF);
									start_fastboot();
									break;
								case FBS_GOTO_SLEEP:
									LIDBG_PRINT("FlyBootService get pm state: FBS_GOTO_SLEEP");
									break;
								case FBS_KERNEL_DOWN:
									LIDBG_PRINT("FlyBootService get pm state: FBS_KERNEL_DOWN");
									break;
								case FBS_KERNEL_UP:
									LIDBG_PRINT("FlyBootService get pm state: FBS_KERNEL_UP");
									break;
								case FBS_ANDROID_UP:
									LIDBG_PRINT("FlyBootService get pm state: FBS_ANDROID_UP");
									SendBroadcastToService(KeyBootState, keyFastSusupendON);
									sendBroadcast(new Intent(SYSTEM_RESUME));
									Intent intentBoot = new Intent(Intent.ACTION_BOOT_COMPLETED);
									intentBoot.putExtra("flyauduio_accon", "accon");
									sendBroadcast(intentBoot);  
									break;
								case FBS_DEVICE_UP:
									LIDBG_PRINT("FlyBootService get pm state: FBS_DEVICE_UP");
									restoreAirplaneMode(mFlyBootService);
									sendBroadcast(new Intent(ACC_ON_FLYUI));
									SendBroadcastToService(KeyBootState, keyEearlySusupendON);
									break;
								case FBS_SCREEN_ON:
									LIDBG_PRINT("FlyBootService get pm state: FBS_SCREEN_ON");
									acquireWakeLock();
									SendBroadcastToService(KeyBootState, keyScreenOn);
									system_resume();
									break;
								default:
									return;
							}
						}
					}
            }
        }.start();

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
            // mWakeLock = (WakeLock) pm.newWakeLock(
            // PowerManager.PARTIAL_WAKE_LOCK
            // | PowerManager.ACQUIRE_CAUSES_WAKEUP
            // | PowerManager.ON_AFTER_RELEASE, "flytag");

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
	/*
	final ContentResolver cr = context.getContentResolver();
        Settings.Secure.putInt(cr, Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_HIGH_ACCURACY);
        LIDBG_PRINT("ON  LOCATION_MODE: " + Settings.Secure.getInt(context.getContentResolver(), Settings.Secure.LOCATION_MODE,
                        -100));
	*/
	
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

	private void start_fastboot(){
		firstBootFlag = true;

		fbPm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		mActivityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);

		delay(8000);

		LIDBG_PRINT(" ********** start fastboot ********** ");

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
		// notify music application to pause music before kill the
		// application
		LIDBG_PRINT("powerOffSystem+");
		sendBecomingNoisyIntent();
		LIDBG_PRINT("powerOffSystem step 1");

		SystemProperties.set("ctl.start", "bootanim");
		LIDBG_PRINT("powerOffSystem step 2");

//		enterAirplaneMode();
		LIDBG_PRINT("powerOffSystem step 3");
		getLastPackage();
		// flyaudio
		bIsKLDRunning = IsKLDrunning();

		if (bIsKLDRunning) {
		// FlyPowerOnWithKLD();
		SystemProperties.set("fly.gps.run", "1");
			Log.d(TAG, "-----fly.gps.run----1----");
		} else {
			SystemProperties.set("fly.gps.run", "0");
			Log.d(TAG, "-----fly.gps.run-----0---");
		}
		LIDBG_PRINT("powerOffSystem step 4");
		KillProcess();
		LIDBG_PRINT("powerOffSystem step5");
		//goToSleep前通知内核,作用关外设

		LIDBG_PRINT("powerOffSystem step6");
		SystemClock.sleep(1000);
		LIDBG_PRINT("powerOffSystem step7");

		fbPm.goToSleep(SystemClock.uptimeMillis());
		msgTokenal("flyaudio gotosleep");
		releaseWakeLock();
		LIDBG_PRINT("powerOffSystem-");
		// Intent iFinish = new Intent("FinishActivity");
		// sendBroadcast(iFinish);
    }

    private void powerOnSystem(Context context) {
        LIDBG_PRINT("qf powerOnSystem+");
    	 //msgTokenal("bootcompleted");
        //sendBootCompleted(false);
        LIDBG_PRINT("powerOnSystem step 1");
        restoreAirplaneMode(context);
        LIDBG_PRINT("powerOnSystem step 2");
        SystemProperties.set("ctl.stop", "bootanim");
        LIDBG_PRINT("powerOnSystem-");
    }

    // send broadcast to music application to pause music
    private void sendBecomingNoisyIntent() {
        sendBroadcast(new Intent(AudioManager.ACTION_AUDIO_BECOMING_NOISY));
    }

    private void KillProcess() {
        List<ActivityManager.RunningAppProcessInfo> appProcessList = null;

        appProcessList = mActivityManager.getRunningAppProcesses();

        LIDBG_PRINT("begin to KillProcess.");
        for (ActivityManager.RunningAppProcessInfo appProcessInfo : appProcessList) {
            int pid = appProcessInfo.pid;
            int uid = appProcessInfo.uid;
            String processName = appProcessInfo.processName;
            if (isKillableProcess(processName)) {
                // mActivityManager.killBackgroundProcesses(processName);
                LIDBG_PRINT(processName +"."+pid +" will be killed");
                mActivityManager.forceStopPackage(processName);
	    /*if(processName.equals("cld.navi.c2739.mainframe")){
		msgTokenal("flyaudio kill "+pid);
		LIDBG_PRINT(pid + " cld.navi.c2739.mainframe"+" will be kill by the kenal");	
		}*/

            }
        }
    }

    private boolean isKillableProcess(String packageName) {
        for (String processName : systemLevelProcess) {
            if (processName.equals(packageName)) {
                return false;
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
                LIDBG_PRINT(" writeToFile   new fileoutputstream+ accstate:"
                        + str);
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

}

