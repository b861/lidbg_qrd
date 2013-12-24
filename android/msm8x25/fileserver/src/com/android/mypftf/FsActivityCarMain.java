package com.android.mypftf;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Messenger;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.telephony.gsm.SmsManager;
import android.text.Html;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnLongClickListener;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

/**
 * @author 付腾飞
 * @serial 2220929658@qq.com
 * @category lidbg定制
 * @date 2013.11.11 @ http://dl.google.com/android/ADT-18.0.0.zip *
 */
public class FsActivityCarMain extends Activity
{
	private String lidbg_node = "/dev/mlidbg0";
	private String lidbg_conf_dir = "/data/lidbg/";
	private String state_file_path = "/dev/log/state.txt";
	private String result_file_path = "/dev/log/mobile.txt";
	private String machine_id_file = lidbg_conf_dir + "MIF.txt";
	private String osd_dir_path = "/data/lidbg_osd";
	private String prefer_osd_dir = "osd_dir";
	private String prefer_osd_size = "osd_size";
	private String prefer_osd_textcolor = "osd_textcolor";
	private String prefer_wt_phonenum = "wt_phonenum";
	private String prefer_wt_cputmp = "wt_cputmp";
	private String prefer_wt_allcount = "wt_allcount";
	private String gstatetxt = "null";
	protected String cmd_string_destroy = "c file 0 8 0";
	protected String cmd_string_upload = "c file 0 7 0";
	protected String cmd_string_chmod = "c file 0 6 0";
	protected String cmd_string_kmsg_enable = "c file 0 2 1";
	protected String cmd_string_kmsg_disable = "c file 0 2 0";
	private String mjextern_state;
	private String ats;
	private String ct;
	public String line1;
	public String FS_VERSION = "版本：[2013.11.11]  作者：付腾飞 2220929658@qq.com";
	public static String machine_id;
	private static final String TAG = "futengfei";
	private Button mjBupload_state;
	private Button mjBacc_times;
	private Button mjBcputmp;
	private int data_txtfile_list_bias;
	public static int TOOL_BAR_HIGH = 0;
	public static final int MENU_ID_OSD = Menu.FIRST;
	public static final int MENU_ID_WATCHER = Menu.FIRST + 1;
	public static final int MENU_ID_CPUINFO = Menu.FIRST + 2;
	public static final int MENU_ID_MORE = Menu.FIRST + 3;
	private boolean is_state_thread_running = true;
	private boolean is_state_toast_enable = false;
	private boolean is_upload_enable = true;
	private ArrayList<File> txtfile_list_data;
	private ArrayList<File> txtfile_list_osd;
	private IntentFilter sdintentFilter;
	private Thread mstate_file_thread;
	private ListView fslistView;
	public Messenger mService = null;
	private FsClassCarCommen mFsClassCommen;
	private Context fsContext;
	private FsClassCarOsd mFsClassOsd;
	private BroadcastReceiver sdcardlistener;
	public static SharedPreferences fsPreferences;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		request_full_screen();
		setContentView(R.layout.layout_main);
		fsPreferences = getSharedPreferences(
				getResources().getString(R.string.packageSharedPreferences), 0);
		fsContext = getApplicationContext();
		mFsClassCommen = new FsClassCarCommen(fsContext);
		mFsClassOsd = new FsClassCarOsd(fsContext);
		mjBupload_state = (Button) findViewById(R.id.mxmachine_state);
		mjBacc_times = (Button) findViewById(R.id.mxAccTimes);
		mjBcputmp = (Button) findViewById(R.id.mxCurrentT);
		machine_id = mFsClassCommen.get_machine_id(machine_id_file);
		txtfile_list_data = mFsClassCommen.get_dirfile_list(lidbg_conf_dir);
		txtfile_list_osd = mFsClassCommen.get_dirfile_list(fsPreferences
				.getString(prefer_osd_dir, osd_dir_path));
		mFsClassOsd.prepare(R.layout.layout_osd, R.id.cputext);
		mFsClassOsd.set_text_size(fsPreferences.getFloat(prefer_osd_size, 40));
		mFsClassOsd.set_text_color(fsPreferences.getString(
				prefer_osd_textcolor, "993366"));
		update_statefile_content();
		update_button_state();
		update_sd_state();
		creat_state_file_thread();
		register_sd_broadcast();
		setTitle("fileserver.apk【machine_id:" + machine_id + "】");
		showmy_listview();
		OnLongClickListener buttoncommenClickListener = new OnLongClickListener()
		{
			@Override
			public boolean onLongClick(View v)
			{
				// TODO Auto-generated method stub
				if (v.equals(mjBcputmp))
				{
					show_osd_dialog();
				} else if (v.equals(mjBacc_times))
				{
					show_hardware_platform();
				} else if (v.equals(mjBupload_state))
				{
					if (is_sim_china_mobile())
					{
						mFsClassCommen.send_msg("10086", "100");
						mFsClassCommen.send_msg("10086", "112");
						mFsClassCommen.send_msg("10086", "cxchtuk");
						mFsClassCommen.send_msg("10086", "cxqqhm");
						mFsClassCommen.send_msg("10086", "10086");
					}
				}
				return false;
			}
		};
		mjBcputmp.setOnLongClickListener(buttoncommenClickListener);
		mjBacc_times.setOnLongClickListener(buttoncommenClickListener);
		mjBupload_state.setOnLongClickListener(buttoncommenClickListener);
		// mFsClassCommen.show_toast("本机IP:" +
		// mFsClassCommen.getHostIpAddress());

	}

	private Boolean is_sim_china_mobile()
	{
		// TODO Auto-generated method stub
		// 判断是否是中国移动网络
		TelephonyManager telManager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
		String subString = telManager.getSubscriberId();
		return (subString.startsWith("46000") || subString.startsWith("46002"));
	}

	private void send_call(String string)
	{
		// TODO Auto-generated method stub
		Intent intent = new Intent();
		intent.setAction(Intent.ACTION_CALL);
		intent.setData(Uri.parse("tel://" + string));
		startActivity(intent);
	}

	private void show_osd_dialog()
	{
		LayoutInflater inflater = getLayoutInflater();
		View layout = inflater.inflate(R.layout.layout_osd_dialog, null);
		final SeekBar jvsb_osdsize = (SeekBar) layout
				.findViewById(R.id.xmsbosd_size);
		final EditText jved_osdcolor = (EditText) layout
				.findViewById(R.id.xmedosd_color);
		final EditText jved_osdtextcolor = (EditText) layout
				.findViewById(R.id.xmedosd_text_color);
		final EditText jved_osdpath = (EditText) layout
				.findViewById(R.id.xmedosd_path);
		jvsb_osdsize.setProgress((int) fsPreferences.getFloat(prefer_osd_size,
				15));
		jvsb_osdsize.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
		{
			@Override
			public void onStopTrackingTouch(SeekBar seekBar)
			{
				// TODO Auto-generated method stub
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar)
			{
				// TODO Auto-generated method stub
				if (!mFsClassOsd.is_osd_enable())
				{
					mFsClassOsd.show_toast("检测到OSD没有使能");
				}
			}

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser)
			{
				// TODO Auto-generated method stub
				if (mFsClassOsd.is_osd_enable())
				{
					mFsClassOsd.set_text_size(progress);
				}
			}
		});
		AlertDialog.Builder builder = new Builder(FsActivityCarMain.this);
		builder.setView(layout);
		builder.setPositiveButton("确认", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				String resultString = "设置结果：\n";
				if (jved_osdcolor.length() > 0)
				{
					mFsClassOsd.set_text_background(jved_osdcolor.getText()
							.toString());
					resultString += jved_osdcolor.getText().toString()
							+ ".成功\n ";
				}
				if (jved_osdtextcolor.length() > 0)
				{
					mFsClassOsd.set_text_color(jved_osdtextcolor.getText()
							.toString());
					fsPreferences
							.edit()
							.putString(prefer_osd_textcolor,
									jved_osdtextcolor.getText().toString())
							.commit();
					resultString += jved_osdtextcolor.getText().toString()
							+ ".成功\n ";
				}
				if (jved_osdpath.length() > 0)
				{
					File osdFile = new File(jved_osdpath.getText().toString());
					if (osdFile.exists() && osdFile.isDirectory())
					{
						txtfile_list_osd = mFsClassCommen
								.get_dirfile_list(jved_osdpath.getText()
										.toString());
						fsPreferences
								.edit()
								.putString(prefer_osd_dir,
										jved_osdpath.getText().toString())
								.commit();
						resultString += jved_osdpath.getText().toString()
								+ ".成功 \n";
					} else
					{
						mFsClassOsd.show_toast("OSD目录设置失败：目录不存在或不可读");
					}
				}
				if (mFsClassOsd.is_osd_enable())
				{
					mFsClassOsd.set_text_size(jvsb_osdsize.getProgress());
					fsPreferences
							.edit()
							.putFloat(prefer_osd_size,
									jvsb_osdsize.getProgress()).commit();
					resultString += jvsb_osdsize.getProgress() + ".成功 \n";
				} else
				{
					resultString += "OSD未使能,文字大小设置失败";
				}
				mFsClassOsd.show_toast(resultString);
				dialog.dismiss();
			}
		});
		builder.setNegativeButton("取消", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				dialog.dismiss();
			}
		});
		builder.create();
		builder.show();
	}

	private void show_more_dialog()
	{
		// TODO Auto-generated method stub
		final EditText ksmgpathEditText = new EditText(this);
		ksmgpathEditText.setText("reboot");
		new AlertDialog.Builder(this).setTitle("执行shell终端指令")
				.setIcon(android.R.drawable.ic_dialog_info)
				.setView(ksmgpathEditText)
				.setPositiveButton("确定", new OnClickListener()
				{
					@Override
					public void onClick(DialogInterface dialog, int which)
					{
						// TODO Auto-generated method stub
						if (ksmgpathEditText.length() > 0)
						{
							String kmsgString = ksmgpathEditText.getText()
									.toString();
							mFsClassCommen.show_toast("结果\n"
									+ mFsClassCommen.runRootCommend(kmsgString));
						}
						dialog.dismiss();
					}
				}).setNegativeButton("取消", new OnClickListener()
				{
					@Override
					public void onClick(DialogInterface dialog, int which)
					{
						// TODO Auto-generated method stub
						dialog.dismiss();
					}
				}).show();
	}

	private void show_add_watcher()
	{
		// TODO Auto-generated method stub
		LayoutInflater inflater = getLayoutInflater();
		View layout = inflater.inflate(R.layout.layout_add_watcher, null);
		final EditText jved_wtphonenumEditText = (EditText) layout
				.findViewById(R.id.xmedwt_phonenum);
		final EditText jved_wtcputmpEditText = (EditText) layout
				.findViewById(R.id.xmedwt_cputmp);
		final EditText jvsb_wtallcount = (EditText) layout
				.findViewById(R.id.xmedwt_allcount);
		// EditText显示已存储的数据，方便用户核对
		jved_wtphonenumEditText.setText(fsPreferences.getString(
				prefer_wt_phonenum, ""));
		jved_wtphonenumEditText.setSelection(jved_wtphonenumEditText.length());
		jved_wtcputmpEditText.setText(fsPreferences.getString(prefer_wt_cputmp,
				"85"));
		jvsb_wtallcount.setText(fsPreferences
				.getString(prefer_wt_allcount, "2"));
		OnTouchListener l = new OnTouchListener()
		{
			@Override
			public boolean onTouch(View v, MotionEvent event)
			{
				// TODO Auto-generated method stub
				if (v.equals(jved_wtphonenumEditText))
				{
					jved_wtphonenumEditText.setText("");
				}
				if (v.equals(jved_wtcputmpEditText))
				{
					jved_wtcputmpEditText.setText("");
				}
				if (v.equals(jvsb_wtallcount))
				{
					jvsb_wtallcount.setText("");
				}
				return false;
			}
		};
		// 获得点击事件清空EditText，一个一个删除太痛苦
		jved_wtphonenumEditText.setOnTouchListener(l);
		jved_wtcputmpEditText.setOnTouchListener(l);
		jvsb_wtallcount.setOnTouchListener(l);
		AlertDialog.Builder builder = new Builder(FsActivityCarMain.this);
		builder.setView(layout);
		builder.setPositiveButton("确认", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				String resultString = "设置结果：\n";
				if (jved_wtphonenumEditText.length() > 0)
				{
					fsPreferences
							.edit()
							.putString(
									prefer_wt_phonenum,
									jved_wtphonenumEditText.getText()
											.toString()).commit();
					resultString += jved_wtphonenumEditText.getText()
							.toString() + ".成功\n";
				}
				if (jved_wtcputmpEditText.length() > 0)
				{
					fsPreferences
							.edit()
							.putString(prefer_wt_cputmp,
									jved_wtcputmpEditText.getText().toString())
							.commit();
					resultString += jved_wtcputmpEditText.getText().toString()
							+ ".成功\n";
				}
				if (jvsb_wtallcount.length() > 0)
				{
					fsPreferences
							.edit()
							.putString(prefer_wt_allcount,
									jvsb_wtallcount.getText().toString())
							.commit();
					resultString += jvsb_wtallcount.getText().toString()
							+ ".成功\n";
				}
				mFsClassOsd.show_toast(resultString);
				dialog.dismiss();
			}
		});
		builder.setNegativeButton("取消", new OnClickListener()
		{
			@Override
			public void onClick(DialogInterface dialog, int which)
			{
				dialog.dismiss();
			}
		});
		builder.create();
		builder.show();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		// TODO Auto-generated method stub
		menu.add(0, MENU_ID_OSD, 0, R.string.menu_id_osd).setIcon(
				android.R.drawable.ic_dialog_alert);
		menu.add(0, MENU_ID_WATCHER, 0, R.string.menu_id_watcher).setIcon(
				android.R.drawable.ic_dialog_alert);
		menu.add(0, MENU_ID_CPUINFO, 0, R.string.menu_id_cpuinfo).setIcon(
				android.R.drawable.ic_dialog_info);
		menu.add(0, MENU_ID_MORE, 0, R.string.menu_id_more).setIcon(
				android.R.drawable.ic_dialog_dialer);
		// getMenuInflater().inflate(R.menu.fs_activity_main, menu);
		return true;
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item)
	{
		// TODO Auto-generated method stub
		switch (item.getItemId())
		{
		case MENU_ID_OSD:
			show_osd_dialog();
			break;
		case MENU_ID_WATCHER:
			show_add_watcher();
			break;
		case MENU_ID_CPUINFO:
			show_hardware_platform();
			break;
		case MENU_ID_MORE:
			show_more_dialog();
			break;
		default:
			break;
		}
		return super.onMenuItemSelected(featureId, item);
	}

	private void show_hardware_platform()
	{
		// TODO Auto-generated method stub
		mFsClassCommen.show_toast_longer(mFsClassCommen
				.get_string_context_line(
						mFsClassCommen.get_file_content("/proc/cpuinfo"),
						"Hardware"));
	}

	private void kmsg_disable()
	{
		// TODO Auto-generated method stub
		mFsClassCommen
				.write_to_file(lidbg_node, false, cmd_string_kmsg_disable);
	}

	private void kmsg_enable()
	{
		// TODO Auto-generated method stub
		mFsClassCommen.write_to_file(lidbg_node, false, cmd_string_kmsg_enable);
	}

	private void restart_myself()
	{
		// TODO Auto-generated method stub
		Intent i = getBaseContext().getPackageManager()
				.getLaunchIntentForPackage(getBaseContext().getPackageName());
		i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		startActivity(i);
	}

	@Override
	protected void onDestroy()
	{
		// TODO Auto-generated method stub
		kmsg_disable();
		unregisterReceiver(sdcardlistener);
		stop_state_file_thread();
		mFsClassOsd.removeview();
		super.onDestroy();
	}

	private void stop_state_file_thread()
	{
		is_state_thread_running = false;
	}

	private void revert_state_toast_enable()
	{
		if (!is_state_toast_enable)
		{
			mFsClassOsd.addview();
			ats = "OSD";
			ct = "OSD";
			update_button_state();
		} else
		{
			ats = "null";
			ct = "null";
			mFsClassOsd.removeview();
		}
		is_state_toast_enable = !is_state_toast_enable;
	}

	private void creat_state_file_thread()
	{
		// TODO Auto-generated method stub
		mstate_file_thread = (Thread) new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				while (is_state_thread_running)
				{
					runOnUiThread(new Runnable()
					{
						public void run()
						{
							gstatetxt = mFsClassCommen
									.get_file_content(state_file_path);
							if (is_state_toast_enable)
							{
								String osdString = gstatetxt.replace("\n", ".");
								for (int i = 0; i < txtfile_list_osd.size(); i++)
								{
									osdString = osdString
											+ "\n"
											+ mFsClassCommen.get_file_content(
													txtfile_list_osd.get(i)
															.getAbsolutePath())
													.replace("\n", ".");
								}
								mFsClassOsd.set_text(Html
										.fromHtml("<font color=blue>LIDBG.OSD</font>")
										+ "\n" + osdString);
							} else
							{
								update_statefile_content();
								update_button_state();
							}
						}
					});
					try
					{
						Thread.sleep(2000);
					} catch (InterruptedException e)
					{
						e.printStackTrace();
					}
				}
			}
		});
		mstate_file_thread.start();
	}

	private void update_sd_state()
	{
		// TODO Auto-generated method stub
		mjextern_state = android.os.Environment.getExternalStorageState();
	}

	private void register_sd_broadcast()
	{
		// TODO Auto-generated method stub
		sdintentFilter = new IntentFilter();
		sdintentFilter.addAction(Intent.ACTION_MEDIA_UNMOUNTED);
		sdintentFilter.addAction(Intent.ACTION_MEDIA_EJECT);
		sdintentFilter.addAction(Intent.ACTION_MEDIA_REMOVED);
		sdintentFilter.addAction(Intent.ACTION_MEDIA_BAD_REMOVAL);
		sdintentFilter.addDataScheme("file");// 测试到：如果不加这句话，无法接收到广播，具体为什么待研究。
		sdcardlistener = new BroadcastReceiver()
		{
			@Override
			public void onReceive(Context context, Intent intent)
			{
				// TODO Auto-generated method stub
				mFsClassCommen.show_toast("SD卡broadcast："
						+ intent.getAction().toString());
				update_sd_state();
				showmy_listview();
			}
		};
		registerReceiver(sdcardlistener, sdintentFilter);

	}

	private void request_full_screen()
	{
		// TODO Auto-generated method stub
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
	}

	private void update_button_state()
	{
		// TODO Auto-generated method stub
		mjBupload_state.setText("上传结果："
				+ mFsClassCommen.get_file_content(result_file_path));// txtcontext
		mjBacc_times.setText("ACC次数：" + ats + " ");
		mjBcputmp.setText("CPU温度：" + ct + " ");
	}

	private void update_statefile_content()
	{
		String[] statefileStrings = gstatetxt.split("\n");
		for (int i = 0; i < statefileStrings.length; i++)
		{
			if (statefileStrings[i].contains("="))
			{
				String[] rawstring = statefileStrings[i].split("=");
				if (i == 0)
					ct = rawstring[1];
				if (i == 1)
					ats = rawstring[1];
			}
		}
	}

	private void showmy_listview()
	{
		// TODO Auto-generated method stub
		fslistView = (ListView) findViewById(R.id.mxlistview);
		fslistView.setCacheColorHint(Color.TRANSPARENT);
		fslistView.setBackgroundResource(R.drawable.them);
		fslistView.setSmoothScrollbarEnabled(true);
		fslistView.setDividerHeight(3);
		fslistView.setOnItemClickListener(mylis_list);
		fslistView.setAdapter(new ArrayAdapter<String>(this,
				android.R.layout.simple_expandable_list_item_1, getData()));
		// setContentView(listView);
	}

	private OnItemClickListener mylis_list = new OnItemClickListener()
	{
		@Override
		public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
				long arg3)
		{
			// TODO Auto-generated method stub
			// 先查看点击的item区域是否属于文本打开
			if (arg2 >= data_txtfile_list_bias
					&& arg2 < txtfile_list_data.size() + data_txtfile_list_bias)
			{
				mFsClassCommen.intent_open_file(
						txtfile_list_data.get(arg2 - data_txtfile_list_bias)
								.getAbsolutePath(), Intent.ACTION_EDIT);
			} else
			{
				showmy_listview();
			}
			// 查看点击的item区域是否属于修改/data的权限
			if ((txtfile_list_data.size() <= 0)
					&& (arg2 == data_txtfile_list_bias))
			{
				mFsClassCommen.show_toast("正在修改，【fileserver】   4 s后重启。。。");
				mFsClassCommen.write_to_file(lidbg_node, false,
						cmd_string_chmod);
				new Thread(new Runnable()
				{
					@Override
					public void run()
					{
						// TODO Auto-generated method stub
						try
						{
							Thread.sleep(4000);
							restart_myself();// 修改权限后重启自己，再次尝试读取文件列表
						} catch (InterruptedException e)
						{
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}).start();
			}
			// 散碎功能
			switch (arg2)
			{
			case 0:
				mFsClassCommen.write_to_file(lidbg_node, false,
						cmd_string_destroy);
				break;
			case 1:
				sendBroadcast(new Intent(
						"android.intent.action.FAST_BOOT_START"));
				mFsClassCommen.show_toast("已发送广播：快速关机");
				// stop_state_file_thread();
				// upload_machine_info("/mnt/sdcard/333.txt","futengfei-mayanping");
				break;
			case 2:
				revert_state_toast_enable();
				showmy_listview();
				break;
			case 3:
				mFsClassCommen.show_toast("准备中。。。");
				kmsg_enable();
				Intent intent_kmsg = new Intent();
				intent_kmsg.setClass(FsActivityCarMain.this,
						FsActivityCarKmsg.class);
				startActivity(intent_kmsg);
				break;
			case 4:
				if (!mFsClassCommen.iswifiactivy())
				{
					mFsClassCommen.show_toast("WIFI未开启，开始引导。。。");
					startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
				} else if (is_upload_enable
						&& mFsClassCommen.is_file_exist(lidbg_node))
				{
					is_upload_enable = false;// 避免频繁上传
					mFsClassCommen.show_toast(" 上传准备中，消息消失前上传功能将暂停。。。 ");
					mFsClassCommen.write_to_file(lidbg_node, false,
							cmd_string_upload);
					mFsClassCommen.show_toast(" 上传结果请留意屏幕左上角  ");
					new Thread(new Runnable()
					{
						@Override
						public void run()
						{
							// TODO Auto-generated method stub
							try
							{
								Thread.sleep(4000);
								is_upload_enable = true;
							} catch (InterruptedException e)
							{
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						}
					}).start();
				}
				break;
			case 5:
				Intent intent_shell = new Intent();
				intent_shell.setClass(FsActivityCarMain.this,
						FsActivityCarShell.class);
				startActivity(intent_shell);
				break;
			case 6:
				Intent intent_shell1 = new Intent();
				intent_shell1.setClass(FsActivityCarMain.this,
						FsActivityCarTS.class);
				startActivity(intent_shell1);
				break;
			case 7:
				mFsClassCommen.intent_open_file(state_file_path,
						Intent.ACTION_EDIT);
				break;
			default:
				break;
			}
		}
	};

	public List<String> getData()
	{
		List<String> data = new ArrayList<String>();
		data.add("系统升级   [目前SD卡状态：" + mjextern_state + "]");
		data.add("发送广播：快速关机");
		data.add("使能OSD状态信息:" + is_state_toast_enable);
		data.add("动态捕捉系统KMSG  ");
		data.add("上传本机BUG至服务器");
		data.add("打开shell终端模拟器");
		data.add("打开触摸板多点测试软件");
		data.add("打开state.txt");

		data_txtfile_list_bias = data.size();
		if (txtfile_list_data.size() > 0)
		{
			for (int i = 0; i < txtfile_list_data.size(); i++)
			{
				data.add("打开" + txtfile_list_data.get(i).getName());
			}
		} else
		{
			data.add("点击修改/data权限");
		}
		for (int i = 0; i < 15; i++)
		{
			data.add("置顶");
		}
		data.add(FS_VERSION);
		return data;
	}
}
