package com.android.mypftf;
import java.io.File;
import java.util.ArrayList;
import java.util.Calendar;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.LayoutInflater;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AnalogClock;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.SlidingDrawer;
import android.widget.TimePicker;

/**
 * @author 付腾飞 时间：2012.12.18.22：09 程序骨干简介： 1：采用抽屉来呈现用户界面 （SlidingDrawer）
 *         2：采用GridView 呈现更多数据 3：重写用户返回键 4：扩大程序视野，让 程序获取最大显示，去掉title和系统通知栏
 * 
 */
public class PackageActivityMain extends Activity
{
	private int[] carmode_resourse =
	{ R.drawable.anim_voice_recording_1, R.drawable.anim_voice_recording_2,
			R.drawable.anim_voice_recording_3,
			R.drawable.anim_voice_recording_4,
			R.drawable.anim_voice_recording_5,
			R.drawable.anim_voice_recording_6,
			R.drawable.anim_voice_recording_7 };
	private int[] icons =
	{ R.drawable.image_light_sount, R.drawable.image_light_sount,
			R.drawable.image_light_sount, R.drawable.image_light_sount,
			R.drawable.image_shell };
	private String[] icons_name =
	{ "智能灯光助手", "智能情景模式助手", "亲友团助手", "触摸板测试", "shell终端模拟器" };

	private boolean is_show_enable;
	private int i;
	private ImageView mjImageView;
	private ListView mjgridviewGridView;
	private SlidingDrawer mjsliDrawer;
	private ImageView carModeImageView;
	private Thread mImageViewThread;
	private ImageView mCarModeImageView;
	private AnalogClock mAnalogClock;
	private Context gcontext;
	private FsClassCarCommen mFsClassCarCommen;
	private AlarmManager[] am = new AlarmManager[6];
	private String prefer_bl_history = "bl_history";
	private SharedPreferences mfsPreferences;
	private PackageActivityMainGridViewAdapter mjgridViewadaper;
	private String bl_history_string;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		request_full_screen();
		setContentView(R.layout.slide_main);
		mfsPreferences = getSharedPreferences(
				getResources().getString(R.string.packageSharedPreferences), 0);

		gcontext = getApplicationContext();
		mFsClassCarCommen = new FsClassCarCommen(gcontext);
		bl_history_string = mfsPreferences.getString(prefer_bl_history,
				"9-0-200 13-0-1 14-0-200 20-0-1");
		// parse_timer_string(bl_history_string, false);
		mFsClassCarCommen.show_toast_longer("点击右下角广播进入车机模式");
		mCarModeImageView = (ImageView) findViewById(R.id.xmcarmode);
		mAnalogClock = (AnalogClock) findViewById(R.id.analogClock1);
		mjgridviewGridView = (ListView) findViewById(R.id.mycontent1);
		mjsliDrawer = (SlidingDrawer) findViewById(R.id.mxslidedraw1);
		mjImageView = (ImageView) findViewById(R.id.myimage1);
		carModeImageView = (ImageView) findViewById(R.id.xmcarmode);

		mjgridviewGridView.setCacheColorHint(Color.TRANSPARENT);
		mjgridviewGridView.setSmoothScrollbarEnabled(true);
		// mjgridviewGridView.setDividerHeight(3);

		creat_animation_thread();
		chmod_777_all();

		mjgridViewadaper = new PackageActivityMainGridViewAdapter(this,
				icons_name, icons);
		mjgridviewGridView.setAdapter(mjgridViewadaper);

		mjsliDrawer.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View arg0)
			{
				// TODO Auto-generated method stub
				is_show_enable = !is_show_enable;
				page_show(is_show_enable);
			}
		});
		mjsliDrawer
				.setOnDrawerOpenListener(new SlidingDrawer.OnDrawerOpenListener()
				{

					@Override
					public void onDrawerOpened()
					{
						mjImageView.setImageResource(R.drawable.close);
						mjsliDrawer.setBackgroundResource(R.drawable.p2);
						page_show(false);
					}
				});

		mjsliDrawer
				.setOnDrawerCloseListener(new SlidingDrawer.OnDrawerCloseListener()
				{
					@Override
					public void onDrawerClosed()
					{
						// TODO Auto-generated method stub
						mjImageView.setImageResource(R.drawable.open);
						mjsliDrawer.setBackgroundResource(R.drawable.p1);
						page_show(true);
					}
				});

		mjgridviewGridView.setOnItemClickListener(new OnItemClickListener()
		{
			@Override
			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3)
			{
				// TODO Auto-generated method stub
				switch (arg2)
				{

				case 1:
					ShowModifyBacklightDialog();
					break;
				case 3:
					Intent intent_shell1 = new Intent();
					intent_shell1.setClass(PackageActivityMain.this,
							FsActivityCarTS.class);
					startActivity(intent_shell1);
					break;
				case 4:
					Intent intent_shell = new Intent();
					intent_shell.setClass(PackageActivityMain.this,
							FsActivityCarShell.class);
					startActivity(intent_shell);
					break;
				default:
					break;
				}
			}

		});

		carModeImageView.setOnClickListener(new OnClickListener()
		{

			@Override
			public void onClick(View arg0)
			{
				// TODO Auto-generated method stub
				Intent intent_kmsg = new Intent();
				intent_kmsg.setClass(PackageActivityMain.this,
						FsActivityCarMain.class);
				startActivity(intent_kmsg);
			}
		});

	}

	@Override
	public void onBackPressed()
	{
		// new AlertDialog.Builder(PackageActivityMain.this).setTitle("确认退出")
		// .setIcon(R.drawable.leave).setMessage("确认退出")
		// .setPositiveButton("关闭", new DialogInterface.OnClickListener()
		// {
		//
		// @Override
		// public void onClick(DialogInterface arg0, int arg1)
		// {
		// // TODO Auto-generated method stub
		// mFsClassCarCommen.show_toast("close draw");
		// finish();
		// }
		// })
		//
		// .setNegativeButton("不关闭", new DialogInterface.OnClickListener()
		// {
		//
		// @Override
		// public void onClick(DialogInterface arg0, int arg1)
		// {
		// // TODO Auto-generated method stub
		// mFsClassCarCommen.show_toast("open draw");
		// mjsliDrawer.open();
		// }
		// }).show();

		super.onBackPressed();
	}

	@Override
	protected void onDestroy()
	{
		// TODO Auto-generated method stub
		mFsClassCarCommen.show_toast("已退出");
		super.onDestroy();
	}

	private void chmod_777_all()
	{
		// TODO Auto-generated method stub
		mFsClassCarCommen
				.runRootCommend("chmod 777 /sys/class/leds/lcd-backlight/brightness");
	}

	private void ShowModifyBacklightDialog()
	{
		LayoutInflater inflater = getLayoutInflater();
		View layout = inflater.inflate(R.layout.layout_pack_backlight_dialog,
				null);

		final EditText jved_result_bl = (EditText) layout
				.findViewById(R.id.xmedresult_bl);
		final TimePicker jvtp_bl = (TimePicker) layout
				.findViewById(R.id.xm_timePicker_bl);
		jvtp_bl.setIs24HourView(true);

		jved_result_bl.setText(bl_history_string);
		new AlertDialog.Builder(PackageActivityMain.this).setView(layout)
				.setPositiveButton("确定", new DialogInterface.OnClickListener()
				{
					@Override
					public void onClick(DialogInterface arg0, int arg1)
					{
						// TODO Auto-generated method stub
						if (jved_result_bl.length() > 0)
						{
							String resultString = jved_result_bl.getText()
									.toString();
							mfsPreferences.edit()
									.putString(prefer_bl_history, resultString)
									.commit();
							// parse_timer_string(resultString, true);
						}

					}

				})

				.setNegativeButton("取消", new DialogInterface.OnClickListener()
				{

					@Override
					public void onClick(DialogInterface arg0, int arg1)
					{
						// TODO Auto-generated method stub

					}
				}).show();
	}

	private void parse_timer_string(String resultString, boolean canselen)
	{
		// TODO Auto-generated method stub
		String[] statefileStrings = resultString.split(" ");

		for (int i = 0; i < statefileStrings.length; i++)
		{
			String[] timeStrings = statefileStrings[i].split("-");
			if (timeStrings.length == 3)
			{
				init_alarm(canselen, i, Integer.parseInt(timeStrings[0]),
						Integer.parseInt(timeStrings[1]), timeStrings[2]);

			} else
			{
				mFsClassCarCommen.show_toast("格式不对：" + timeStrings.length);
			}

		}
	}

	private void page_show(boolean is_show_enable2)
	{
		// TODO Auto-generated method stub
		if (is_show_enable2)
		{
			mAnalogClock.setVisibility(ImageView.VISIBLE);
			mCarModeImageView.setVisibility(ImageView.VISIBLE);
		} else
		{
			mAnalogClock.setVisibility(ImageView.INVISIBLE);
			mCarModeImageView.setVisibility(ImageView.INVISIBLE);
		}
	}

	private void creat_animation_thread()
	{
		// TODO Auto-generated method stub

		mImageViewThread = (Thread) new Thread(new Runnable()
		{
			@Override
			public void run()
			{

				while (true)
				{
					runOnUiThread(new Runnable()
					{

						public void run()
						{
							if (i < carmode_resourse.length)
							{
								mCarModeImageView
										.setImageResource(carmode_resourse[i++]);
							} else
							{
								i = 0;
							}
						}
					});
					try
					{
						Thread.sleep(200);
					} catch (InterruptedException e)
					{
						e.printStackTrace();
					}
				}
			}
		});
		mImageViewThread.start();
	}

	private void request_full_screen()
	{
		{
			requestWindowFeature(Window.FEATURE_NO_TITLE);
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
					WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}
	}

	private void init_alarm(boolean canselen, int pos, int hour, int minute,
			String action)
	{
		// TODO Auto-generated method stub
		// mFsClassCarCommen.show_toast(pos + "空格分割：" + hour + minute);
		Calendar calendar = Calendar.getInstance();
		calendar.setTimeInMillis(System.currentTimeMillis());
		calendar.set(Calendar.HOUR_OF_DAY, hour);
		calendar.set(Calendar.MINUTE, minute);
		calendar.set(Calendar.SECOND, 0);
		calendar.set(Calendar.MILLISECOND, 0);

		long starttime = calendar.getTimeInMillis();
		long repeattime = 24 * 60 * 60 * 1000;

		Intent intent1 = new Intent(PackageActivityMain.this,
				packageAlarmReceive.class);
		intent1.setAction(action);

		PendingIntent sender = PendingIntent.getBroadcast(
				PackageActivityMain.this, pos, intent1, 0);// 第二个参数要唯一，否者申请多个闹钟只有最后一个可用
		if (canselen == true)
		{
			am[pos].cancel(sender);
		}
		am[pos] = (AlarmManager) getSystemService(ALARM_SERVICE);
		am[pos].setRepeating(AlarmManager.RTC_WAKEUP, starttime, repeattime,
				sender);
	}
}
