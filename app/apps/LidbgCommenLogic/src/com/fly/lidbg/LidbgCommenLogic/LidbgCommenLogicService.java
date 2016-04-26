package com.fly.lidbg.LidbgCommenLogic;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;

public class LidbgCommenLogicService extends Service
{

	private LidbgCommenLogicService mLidbgCommenLogicService;

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
				break;
			default:
				printKernelMsg("unkown" + action + "\n");
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

