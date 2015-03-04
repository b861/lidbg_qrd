package com.fly.flybootservice;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
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
import android.provider.Settings;
import android.provider.Settings.SettingNotFoundException;
import android.util.Log;

/*
 * ScreenOn ScreenOff DeviceOff Going2Sleep 四种状态分别表示：1.表示正常开屏状态2.表示关屏，但没关外设的状态
 * 0'~30'的阶段3.表示关屏关外设，但没到点进入深度休眠 30'~60'的阶段4.表示发出休眠请求到执行快速休眠 60'后,即进入深度休眠
 */
public class FlyBootService extends Service {

    // public static int NORMAL = 0;
    // public static int SUSPEND = 1;
    private static final String TAG = "boot";
    AlarmManager alarmManager;
    PendingIntent pendingIntent;
    public Handler handler = new Handler();
    public PowerManager pm;
    public WakeLock mWakeLock = null;
    private Handler mHandler = null;
    public WakeLock mFullWakeLock = null;
    private static String ACTON_FINAL_SLEEP = "fly.alarm.timeup";
    // private int mBootState = NORMAL;

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
    private static String SCREEN_ON = "flyaudio screen_on";// 开屏
    private static String SCREEN_OFF = "flyaudio screen_off";// 关屏

    private static String DEVICES_ON = "flyaudio devices_up";// 开外设
    private static String DEVICES_DOWN = "flyaudio devices_down";// 关外设

    private static String ACC_ON_FLYUI = "cn.flyaudio.action.ACCON";
    private static String ACC_OFF_FLYUI = "cn.flyaudio.action.ACCOFF";

    private static String FLYFASTBOOTSTART = "com.flyaudio.ap.broadcast";

    private PowerBroadcastReceiver powerBroadcastReceiver = null;
    private final int DEBUG_VALUE = 1;
    private static int ALERM_TIME = 20;
    private int EALYSUSPEND_TIME = 30;
    private ContentResolver resolver = null;
    private boolean DEBUG_ONCE = true;
    private Handler sleppHandler;
    private String SYSTEM_RESUME = "com.flyaudio.system.resume";

    private enum emState {
        Init, ScreenOn, ScreenOff, DeviceOff, Going2Sleep, Sleep
    };

    private emState mState = emState.Init;

    @Override
    public void onCreate() {
        super.onCreate();
        resolver = this.getContentResolver();
        if (1 == Settings.System.getInt(resolver, "flyaudio_debug", 0)) {
            EALYSUSPEND_TIME = 10;
        }
        LIDBG_PRINT("flybootservice onCreate......");
        mHandler = new Handler();
        sleppHandler = new Handler();
        IntentFilter filter = new IntentFilter();
        filter.addAction(FLYFASTBOOTSTART);
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        filter.addAction(ACTON_FINAL_SLEEP);

        IntentFilter filter_secret = new IntentFilter();
        // filter_secret.addAction(SECRET_CODE_ACTION);
        filter_secret.addAction("android.provider.Telephony.SECRET_CODE");

        filter_secret.addDataScheme("android_secret_code");
        filter.setPriority(Integer.MAX_VALUE);
        powerBroadcastReceiver = new PowerBroadcastReceiver();
        registerReceiver(powerBroadcastReceiver, filter);
        registerReceiver(powerBroadcastReceiver, filter_secret);

        acquireWakeLock();
        delay(5000);
        //setAndroidState(true);

    }

    @Override
    public void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();
        unregisterReceiver(powerBroadcastReceiver);
        LIDBG_PRINT("flybootservice destory...");
    }

    Runnable runnable = new Runnable() {
        @Override
        public void run() {
            LIDBG_PRINT("final sleep timeup...");
            // TODO Auto-generated method stub
            sendBroadcast(new Intent("fly.alarm.timeup"));
        }
    };

    /*
     * public void acquireFullWakeLock() { if (mFullWakeLock == null) {
     * LIDBG_PRINT("acquireFullWakeLock "); pm = (PowerManager)
     * getSystemService(Context.POWER_SERVICE); mFullWakeLock = (WakeLock)
     * pm.newWakeLock( PowerManager.FULL_WAKE_LOCK, "FlyBootService"); if
     * (mFullWakeLock != null && !mFullWakeLock.isHeld())
     * mFullWakeLock.acquire(); } }
     * 
     * public void releaseFullWakeLock() { if (mFullWakeLock != null &&
     * mFullWakeLock.isHeld()) { LIDBG_PRINT("releaseFullWakeLock ");
     * mFullWakeLock.release(); mFullWakeLock = null; } }
     */
    public void acquireWakeLock() {
        if (mWakeLock == null) {
            LIDBG_PRINT("acquirePartialWakeLock  ");
            pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
            // mWakeLock = (WakeLock) pm.newWakeLock(
            // PowerManager.PARTIAL_WAKE_LOCK
            // | PowerManager.ACQUIRE_CAUSES_WAKEUP
            // | PowerManager.ON_AFTER_RELEASE, "flytag");

            mWakeLock = (WakeLock) pm.newWakeLock(
                    PowerManager.PARTIAL_WAKE_LOCK, "flytag");
            if (mWakeLock != null && !mWakeLock.isHeld())
                mWakeLock.acquire();
        }
    }

    public void releaseWakeLock() {
        LIDBG_PRINT("---releaseWakeLock ---");
        if (mWakeLock != null && mWakeLock.isHeld()) {
            LIDBG_PRINT(" releaseWakeLock ");
            mWakeLock.release();
            mWakeLock = null;
        }
    }

    Runnable ealysuspend = new Runnable() {
        @Override
        public void run() {
            // 发送广播
            SendBroadcastToService(KeyBootState, keyEearlySusupendOFF);
            LIDBG_PRINT("Device Off");
            sendBroadcast(new Intent(ACC_OFF_FLYUI));
            writeToFile(file, DEVICES_DOWN);
            delay(500);
            sleppHandler.postDelayed(runnable, ALERM_TIME * 1000);
            mState = emState.DeviceOff;
        }
    };

    public void SendBroadcastToService(String key, String value) {
        // Log.d(TAG, " PowerBundle :  " + value);
        Intent intent = new Intent(action);
        Bundle bundle = new Bundle();
        bundle.putString(key, value);
        intent.putExtra(PowerBundle, bundle);
        sendBroadcast(intent);
    }

    private void procScreenOn() {
        if (mState == emState.Init) {
            mState = emState.ScreenOn;
            acquireWakeLock();
        } else if (mState == emState.ScreenOff) {
            // setAndroidState(true);
            mHandler.removeCallbacks(ealysuspend);
            sleppHandler.removeCallbacks(runnable);
            acquireWakeLock();

            writeToFile(file, SCREEN_ON);// 正常ACC，开背光
            LIDBG_PRINT("writeToFile Screen On");
            SendBroadcastToService(KeyBootState, keyScreenOn);
            mState = emState.ScreenOn;
        } else if (mState == emState.DeviceOff) {
            mHandler.removeCallbacks(ealysuspend);
            sleppHandler.removeCallbacks(runnable);

            writeToFile(file, DEVICES_ON);// 唤醒
            writeToFile(file, SCREEN_ON);// 开背光
            LIDBG_PRINT("writeToFile Device On");
            sendBroadcast(new Intent(ACC_ON_FLYUI));
            SendBroadcastToService(KeyBootState, keyEearlySusupendON);
            SendBroadcastToService(KeyBootState, keyScreenOn);
            mState = emState.ScreenOn;
        } else if (mState == emState.Going2Sleep) {
            // 不予处理
        } else if (mState == emState.Sleep) {
            SendBroadcastToService(KeyBootState, keyScreenOn);
	    SendBroadcastToService(KeyBootState, keyFastSusupendON);
            sendBroadcast(new Intent(SYSTEM_RESUME));
            setAndroidState(true);
            writeToFile(file, DEVICES_ON);// 唤醒
            writeToFile(file, SCREEN_ON);// 开背光

            acquireWakeLock();
            mState = emState.ScreenOn;

           Intent intentBoot = new Intent(Intent.ACTION_BOOT_COMPLETED);
           intentBoot.putExtra("flyauduio_accon", "accon");
           sendBroadcast(intentBoot);  
	
		
        }
    }

    private void procScreenOff() {
        if (mState == emState.ScreenOn) {
            LIDBG_PRINT("Screen off");
            writeToFile(file, SCREEN_OFF);// 关背光
            SendBroadcastToService(KeyBootState, keyScreenOFF);
            mHandler.postDelayed(ealysuspend, EALYSUSPEND_TIME * 1000);
            mState = emState.ScreenOff;
        }
    }

    // 动态注册广播 ，接收screen_of及screen_on.
    class PowerBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            // TODO Auto-generated method stub
            String action = intent.getAction();
            LIDBG_PRINT("get action:" + action + " state:" + mState);
/*            if (action.equals(FLYFASTBOOTSTART)) {
                LIDBG_PRINT(" start fastpower on ");
//                sendBroadcast(new Intent(
//                        "android.intent.action.FAST_BOOT_START"));// 启动FastBootPowerOn
					LIDBG_PRINT("********** FlyBootService start FastBoot app **********");
//					Intent mIntent = new Intent("android.intent.action.MAIN");
					Intent mIntent = new Intent();
					ComponentName comp = new ComponentName(  
					        "com.qualcomm.fastboot",  
					        "com.qualcomm.fastboot.FastBoot");
					mIntent.setComponent(comp);   
					mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
					mIntent.addCategory("android.intent.category.LAUNCHER");   
					startActivity(mIntent);

                LIDBG_PRINT(" FLYFASTBOOTSTART-");
                mState = emState.Sleep;

            } else */if (action.equals(Intent.ACTION_SCREEN_ON)) {
                procScreenOn();
            } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                procScreenOff();
            } else if (action.equals(ACTON_FINAL_SLEEP)) {
                SendBroadcastToService(KeyBootState, keyFastSusupendOFF);
                setAndroidState(false);// 向底层请求FAST_BOOT_START广播

    				LIDBG_PRINT("********** FlyBootService start FastBoot app **********");
    				Intent mIntent = new Intent();
    				ComponentName comp = new ComponentName(  
    				        "com.qualcomm.fastboot",  
    				        "com.qualcomm.fastboot.FastBoot");
    				mIntent.setComponent(comp);   
    				mIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
    				mIntent.addCategory("android.intent.category.LAUNCHER");   
    				startActivity(mIntent);

                LIDBG_PRINT(" FLYFASTBOOTSTART-");

                delay(2000);
                mState = emState.Sleep;
                releaseWakeLock();
                LIDBG_PRINT("going to sleep");
            } else if (action.equals("android.provider.Telephony.SECRET_CODE")) {
                procSecretCode(intent);
            }
        }
    }

    private void procSecretCode(Intent intent) {
        Log.d(TAG, "  SECRET_CODE_ACTION  " + intent.getData().getHost());
        String secretHost = intent.getData().getHost();
        if (secretHost.equals("4633")) {
            LIDBG_PRINT(" Return FLYAUDIO PRODUCE  MODE!!!! ");

            Settings.System.putInt(resolver, "flyaudio_debug", 0);
            Settings.System.putInt(resolver, "EALYSUSPEND_TIME", 60);
            EALYSUSPEND_TIME = Settings.System.getInt(resolver,
                    "EALYSUSPEND_TIME", 60);

        } else if (secretHost.equals("4634")) {
            LIDBG_PRINT(" Enter FLYAUDIO DEBUG MODE!!!! ");

            Settings.System.putInt(resolver, "flyaudio_debug", 1);
            Settings.System.putInt(resolver, "EALYSUSPEND_TIME", 10);
            EALYSUSPEND_TIME = Settings.System.getInt(resolver,
                    "EALYSUSPEND_TIME", 10);

        } else if (secretHost.equals("4632")) {
            LIDBG_PRINT(" Enter flyaudio  oncetimes debug mode!!!! ");

            Settings.System.putInt(resolver, "flyaudio_debug", 0);

            Settings.System.putInt(resolver, "EALYSUSPEND_TIME", 60);
            EALYSUSPEND_TIME = 10;
            DEBUG_ONCE = false;
        }else if (secretHost.equals("4631")) {
				LIDBG_PRINT(" Enter flyaudio  oncetimes debug mode!!!! ");

				Settings.System.putInt(resolver, "flyaudio_debug", 0);

				Settings.System.putInt(resolver, "EALYSUSPEND_TIME", 60);
				ALERM_TIME = 10;
				EALYSUSPEND_TIME = 10;
				DEBUG_ONCE = false;
        }else if (secretHost.equals("4630")) {
				LIDBG_PRINT(" Enter flyaudio  oncetimes debug mode!!!! ");

				Settings.System.putInt(resolver, "flyaudio_debug", 0);

				Settings.System.putInt(resolver, "EALYSUSPEND_TIME", 60);
				ALERM_TIME = 5;
				EALYSUSPEND_TIME = 5;
				DEBUG_ONCE = false;
        } else if (secretHost.equals("4600")) {
            LIDBG_PRINT(" Enter flyaudio  oncetimes debug fast mode!!!! ");
            ALERM_TIME = 0;
            EALYSUSPEND_TIME = 0;
            DEBUG_ONCE = false;
        }
        Log.d(TAG,
                " value :"
                        + Settings.System.getInt(resolver, "flyaudio_debug", 0)
                        + " ALERM_TIME:" + ALERM_TIME + " EALYSUSPEND_TIME:"
                        + EALYSUSPEND_TIME);

    }

    private void setAndroidState(boolean bUpDown) {
        // FileOutputStream stateOutputStream;
        try {
            LIDBG_PRINT(" shareFastBootState " + bUpDown);
            if (bUpDown) {
                writeToFile(file, "flyaudio android_up");
            } else {
                writeToFile(file, "flyaudio android_down");
            }
        } catch (Exception e) {
            Log.d(TAG, e.getMessage());
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

    private void LIDBG_PRINT(String msg) {
        Log.d(TAG, msg);
        //
        // if (0 == Settings.System.getInt(resolver, "flyaudio_debug", 0)) {
        // if (DEBUG_ONCE) {
        // return;
        // }
        // }

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

