//package com.mypftf.callmessage;

package com.fly.lidbg.LidbgCommenLogic;

import android.os.PowerManager;
import android.os.SystemClock;
import android.os.SystemProperties;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.HttpConnectionParams;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.IBinder;
import android.telephony.TelephonyManager;

public class LidbgCommenLogicService extends Service
{

	protected static final String ACCProperties = "persist.lidbg.acc.status";
	private LidbgCommenLogicService mLidbgCommenLogicService;
	private PendingIntent peration;
	private AlarmManager mAlarmManager;
	protected int loopCount = 0;
	private int mAlarmEventAction = 0;
	private int absolMinu = 5;
	private long oldTimes;
	private static int testCount = 0;
	private static int testCountError = 0;

	@Override
	public void onCreate()
	{
		super.onCreate();
		printKernelMsg("onCreate");

		mLidbgCommenLogicService = this;
		IntentFilter filter = new IntentFilter();
		filter.addAction("android.intent.action.BOOT_COMPLETED");
		filter.addAction("com.fly.lidbg.LidbgCommenLogic");
		filter.addAction(Intent.ACTION_SCREEN_OFF);
		filter.addAction(Intent.ACTION_SCREEN_ON);
		filter.setPriority(Integer.MAX_VALUE);
		mLidbgCommenLogicService.registerReceiver(myReceiver, filter);
	}

	private void addRepeatAlarm()
	{
		// TODO Auto-generated method stub
		Date mDate = new Date(System.currentTimeMillis());
		setAndaddAlarmAtTtime(1, 0, mDate.getHours(), (mDate.getMinutes()
				+ absolMinu - mDate.getMinutes() % absolMinu), 0,
				absolMinu * 60 * 1000);
	}

	private void setAndaddAlarmAtTtime(long repearAlarm, long intervalDate,
			long absolutelyHour, long absolutelyMinutes,
			long absolutelySeconds, long repeatIntervalTimeInMillis)
	{
		// TODO Auto-generated method stub
		printKernelMsg("=====add new alarm=======\nrepeatIntervalTimeInMillis:"
				+ repeatIntervalTimeInMillis / 1000 + "S\n");
		oldTimes = SystemClock.elapsedRealtime();

		Intent intent = new Intent("LidbgCommenLogicService");
		intent.putExtra("msg", "=====LidbgCommenLogicService.alarm=====\n");
		peration = PendingIntent.getBroadcast(this, 0, intent, 0);

		// get AlarmManager
		mAlarmManager = (AlarmManager) this
				.getSystemService(Context.ALARM_SERVICE);
		mAlarmManager.cancel(peration);
		if (repearAlarm == 1)
		{
			mAlarmManager.setRepeating(
					AlarmManager.ELAPSED_REALTIME_WAKEUP,
					SystemClock.elapsedRealtime()
							+ getFutureCalenderTimeInMillis(intervalDate,
									absolutelyHour, absolutelyMinutes,
									absolutelySeconds),
					repeatIntervalTimeInMillis, peration);
		} else
		{
			mAlarmManager.set(
					AlarmManager.ELAPSED_REALTIME_WAKEUP,
					SystemClock.elapsedRealtime()
							+ getFutureCalenderTimeInMillis(intervalDate,
									absolutelyHour, absolutelyMinutes,
									absolutelySeconds), peration);
		}

		registerReceiver(mAlarmBroadcast, new IntentFilter(
				"LidbgCommenLogicService"));
	}

	private BroadcastReceiver mAlarmBroadcast = new BroadcastReceiver()
	{
		private long interval = 0;

		@Override
		public void onReceive(Context arg0, Intent arg1)
		{
			// TODO Auto-generated method stub
			loopCount++;
			String msg = arg1.getStringExtra("msg");
			interval = SystemClock.elapsedRealtime() - oldTimes;
			oldTimes = SystemClock.elapsedRealtime();
			String log = msg + getCurrentTimeString() + "\nloopCount:"
					+ loopCount + "/" + "interval:" + interval / 1000 + "S\n";
			printKernelMsg(log);
			handleAlarmEvent(log);
		}
	};

	public String getHttpClientResponseCode(String msgString)
	{
		// TODO Auto-generated method stub
		String msgString1 = "err";
		HttpClient cliet = new DefaultHttpClient();
		HttpGet get = new HttpGet(msgString);
		try
		{
			HttpConnectionParams.setConnectionTimeout(cliet.getParams(), 3000);
			HttpConnectionParams.setSoTimeout(cliet.getParams(), 3000);
			HttpResponse response = cliet.execute(get);
			HttpEntity entity = response.getEntity();
			InputStream is = entity.getContent();
			BufferedReader br = new BufferedReader(new InputStreamReader(is));
			String line = null;
			StringBuffer sb = new StringBuffer();
			while ((line = br.readLine()) != null)
			{
				sb.append(line);
			}
			msgString1 = "success:getStatusCode:"
					+ response.getStatusLine().getStatusCode() + "/"
					+ msgString;
			// + sb.toString();
		} catch (ClientProtocolException e)
		{
			msgString1 = "error:" + e.getMessage();
		} catch (IOException e)
		{
			msgString1 = "error:" + e.getMessage();
		}
		return msgString1;
	}

	public String runShellCommend(String command)
	{
		String reponse = "error";
		try
		{
			Runtime runtime = Runtime.getRuntime();
			Process proc = runtime.exec(command);

			try
			{
				if (proc.waitFor() != 0)
				{
					reponse = "exit value = " + proc.exitValue();
				}
				BufferedReader in = new BufferedReader(new InputStreamReader(
						proc.getInputStream()));
				StringBuffer stringBuffer = new StringBuffer();
				String line = null;
				while ((line = in.readLine()) != null)
				{
					stringBuffer.append(line + "-");
				}
				reponse = stringBuffer.toString();

			} catch (InterruptedException e)
			{
				reponse = e.getMessage();
			}
		} catch (Exception e)
		{
			// TODO: handle exception
			reponse = e.getMessage();
		}

		return reponse;
	}

	protected boolean isNetworkResponseOk()
	{
		// TODO Auto-generated method stub
		boolean ret;
		String response1 = "response1:"
				+ getHttpClientResponseCode("https://www.baidu.com/") + "\n";
		String response2 = "response2:"
				+ getHttpClientResponseCode("https://www.1688.com/") + "\n";
		// String pingString1 = ("ping baidu:" +
		// runShellCommend("ping -c 1 202.108.22.5"))
		// + "\n";
		// String pingString2 = ("ping ali:" +
		// runShellCommend("ping -c 1 120.76.43.12"))
		// + "\n";
		ret = (response1.contains("success") || response2.contains("success"));

		String log = ("\n" + getCurrentTimeString() + "\nret:" + ret + "\n"
				+ response1 + response2);
		printKernelMsg(log);
		return ret;
	}

	public String getNetworkAviName()
	{
		// TODO Auto-generated method stub
		ConnectivityManager mConnectivityManager = (ConnectivityManager) mLidbgCommenLogicService
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		if (mConnectivityManager.getActiveNetworkInfo() != null)
		{
			return mConnectivityManager.getActiveNetworkInfo().getSubtypeName();
		}
		return null;
	}

	protected boolean isCanResetNetWork()
	{
		// TODO Auto-generated method stub
		ConnectivityManager manager = (ConnectivityManager) mLidbgCommenLogicService
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo info2 = manager
				.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
		TelephonyManager mTelephonyManager = (TelephonyManager) mLidbgCommenLogicService
				.getSystemService(Context.TELEPHONY_SERVICE);
		printKernelMsg("getSimState:" + mTelephonyManager.getSimState() + "/"
				+ (info2 != null ? info2.isConnected() : "null"));
		return ((mTelephonyManager.getSimState() == TelephonyManager.SIM_STATE_READY) && (info2 != null && info2
				.isConnected()));
	}

	protected void handleAlarmEvent(String log)
	{
		// TODO Auto-generated method stub
		switch (mAlarmEventAction)
		{
		case 0:
			new Thread(new Runnable()
			{

				@Override
				public void run()
				{
					// TODO Auto-generated method stub
					printKernelMsg("====START======" + testCount + "/"
							+ testCountError + "==========\n");
					boolean isCanResetNetWork = isCanResetNetWork();
					// 0:ACC on/1:ACC off
					int accState = SystemProperties.getInt(ACCProperties, 0);
					printKernelMsg("isCanResetNetWork:" + isCanResetNetWork
							+ "\naccState:" + accState + "\ngetNetworkAviName:"
							+ getNetworkAviName());
					if (accState == 1 && isCanResetNetWork)
					{
						testCount++;
						if (!isNetworkResponseOk())
						{
							testCountError++;
							wakeUpSystem();
							printKernelMsg("msleep(5000)");
							msleep(5000);
							boolean ret = isNetworkResponseOk();
							printKernelMsg("after wakeUpSystem:" + ret);
							if (!ret)
							{
								resetNetWork();
								printKernelMsg("msleep(10000)");
								msleep(10000);
								printKernelMsg("after reset:"
										+ isNetworkResponseOk());
							}
							//goToSleep();
						}
					}
					printKernelMsg("====STOP======" + testCount + "/"
							+ testCountError + "==========\n");
				}
			}).start();
			break;

		default:
			break;
		}
	}

	protected void msleep(int i)
	{
		// TODO Auto-generated method stub
		try
		{
			Thread.sleep(i);
		} catch (InterruptedException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	protected void wakeUpSystem()
	{
		// TODO Auto-generated method stub
		PowerManager fbPm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		printKernelMsg("wakeUpSystem");
		fbPm.wakeUp(SystemClock.uptimeMillis());
	}

	protected void goToSleep()
	{
		// TODO Auto-generated method stub
		PowerManager fbPm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		printKernelMsg("goToSleep");
		fbPm.goToSleep(SystemClock.uptimeMillis());
	}

	protected void resetNetWork()
	{
		// TODO Auto-generated method stub
		printKernelMsg("network err:reset");
		FileWrite("/dev/lidbg_misc0", false, false,
				"flyaudio:svc data disable &");
		msleep(5000);
		FileWrite("/dev/lidbg_misc0", false, false,
				"flyaudio:svc data enable &");
		printKernelMsg("data enable");
	}

	protected String getCurrentTimeString()
	{
		// TODO Auto-generated method stub
		DateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date curDate = new Date(System.currentTimeMillis());
		return df.format(curDate);
	}

	private long getFutureCalenderTimeInMillis(long intervalDate,
			long absolutelyHour, long absolutelyMinutes, long absolutelySeconds)
	{
		// TODO Auto-generated method stub
		DateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		Date curDate = new Date(System.currentTimeMillis());

		Calendar calendar = Calendar.getInstance();
		TimeZone tm = TimeZone.getTimeZone("GMT");
		calendar.setTimeZone(tm);
		calendar.clear();

		String currentTime = df.format(curDate);

		curDate.setDate(curDate.getDate() + (int) intervalDate);
		curDate.setHours((int) absolutelyHour);
		curDate.setMinutes((int) absolutelyMinutes);
		curDate.setSeconds((int) absolutelySeconds);

		calendar.setTime(curDate);
		String futureTime = df.format(curDate);
		long intervalTime = calendar.getTimeInMillis()
				- System.currentTimeMillis();

		String logString = "\ncurrentTime:" + currentTime + "\nfutureTime:"
				+ futureTime + "\nintervalTime:" + intervalTime / 1000 + "\n";
		printKernelMsg(logString);
		return intervalTime;
	}

	// am broadcast -a com.fly.lidbg.LidbgCommenLogic --ei action 0
	private BroadcastReceiver myReceiver = new BroadcastReceiver()
	{
		@Override
		public void onReceive(Context context, Intent intent)
		{
			if (intent == null)
			{
				printKernelMsg("err.return:intent == null \n");
				return;
			}
			printKernelMsg("BroadcastReceiver:[" + intent.getAction() + "]\n");
			if (intent.getAction().equals(
					"android.intent.action.BOOT_COMPLETED"))
			{
				return;
			} else if (intent.getAction().equals(Intent.ACTION_SCREEN_ON))
			{
				return;
			} else if (intent.getAction().equals(Intent.ACTION_SCREEN_OFF))
			{
				return;
			}

			if (!intent.hasExtra("action"))
			{
				printKernelMsg("return:!intent.hasExtra(\"action\")\n");
				return;
			}
			int action = intent.getExtras().getInt("action");
			printKernelMsg("action:" + action + "\n");
			switch (action)
			{
			case 0:
				printKernelMsg("mAlarmManager.cancel\n");
				mAlarmManager.cancel(peration);
				break;
			case 1:
				printKernelMsg("addRepeatAlarm\n");
				addRepeatAlarm();
				break;
			case 2:
				printKernelMsg("addRepeatAlarm,per one mins\n");
				absolMinu = 1;
				addRepeatAlarm();
				break;
			case 3:
				printKernelMsg("wakeUpSystem\n");
				wakeUpSystem();
				break;
			case 4:
				printKernelMsg("goToSleep\n");
				goToSleep();
				break;
			case 5:
				printKernelMsg("handleAlarmEvent\n");
				handleAlarmEvent(null);
				break;

			default:
				printKernelMsg("unkown:" + action + "\n");
				break;
			}
		}
	};

	@Override
	public void onDestroy()
	{
		// TODO Auto-generated method stub
		super.onDestroy();
		printKernelMsg("onDestroy");
	}

	@Override
	public IBinder onBind(Intent intent)
	{
		// TODO Auto-generated method stub
		return null;
	}

	public void printKernelMsg(String string)
	{
		FileWrite("/dev/lidbg_msg", false, false, "LidbgCommenLogic: " + string
				+ "\n");
	}

	public boolean FileWrite(String file_path, boolean creatit, boolean append,
			String write_str)
	{
		// TODO Auto-generated method stub
		if (file_path == null | write_str == null)
		{
			return false;
		}
		File mFile = new File(file_path);
		if (!mFile.exists())
		{
			if (creatit)
			{
				try
				{
					mFile.createNewFile();
				} catch (IOException e)
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else
			{
				return false;
			}
		}
		try
		{
			FileOutputStream fout = new FileOutputStream(
					mFile.getAbsolutePath(), append);
			byte[] bytes = write_str.getBytes();
			fout.write(bytes);
			fout.close();
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
	}
}
