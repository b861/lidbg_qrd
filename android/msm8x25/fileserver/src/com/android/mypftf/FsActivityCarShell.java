package com.android.mypftf;
import java.io.File;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.text.InputType;
import android.util.Log;
import android.util.TypedValue;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.WindowManager;
import android.view.View.OnTouchListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.MultiAutoCompleteTextView;
import android.widget.ScrollView;
import android.widget.TextView;

public class FsActivityCarShell extends Activity implements OnTouchListener,
		OnGestureListener
{
	private TextView shellTextView;
	private ScrollView shellScrollView;
	private GestureDetector detector = new GestureDetector(this);
	private String TAG = "shell_activity";
	private Button saveButton;
	private Button LaunchButton;
	private Context shellcontent;
	private FsClassCarCommen shellFsClassCommen;
	private String shellcmdString;
	private String lastcmdresultString;
	private AutoCompleteTextView cmdEditText;
	private String PREFE_FILE_SHELL = "fs_shell_pref";
	private SharedPreferences shellPreferences;
	private String PreferShellString = "PreferShellCmd";
	private ArrayList<String> NewShellStrings;
	private String DefaltShellString = "ls ,reboot,ps,lsmod,cat /proc/version,cat /proc/cpuinfo,cat /flysystem/lib/out/build_time.conf,rm /data/lidbg/shell*,mount -t  debugfs debugfs /sys/kernel/debug,setprop persist.sys.strictmode.disable true,getprop persist.sys.strictmode.disable,";

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		request_full_screen();
		setContentView(R.layout.layout_shell);

		getWindow().setSoftInputMode(
				WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN);
		NewShellStrings = new ArrayList<String>();

		shellcontent = getApplicationContext();

		shellPreferences = getSharedPreferences(
				getResources().getString(R.string.packageSharedPreferences), 0);

		NewShellStrings = InitShellCmdArray(shellPreferences.getString(
				PreferShellString, DefaltShellString).split(","));

		shellFsClassCommen = new FsClassCarCommen(shellcontent);
		setTitle("fileserver.apk 终端模拟器模式  【machine_id:"
				+ FsActivityCarMain.machine_id + "】");

		cmdEditText = (AutoCompleteTextView) findViewById(R.id.xmedshellcmd);
		saveButton = (Button) findViewById(R.id.xmbtsave);
		LaunchButton = (Button) findViewById(R.id.xmbtclear);

		saveButton.setOnClickListener(commenClickListener);
		LaunchButton.setOnClickListener(commenClickListener);
		cmdEditText.setOnClickListener(commenClickListener);

		saveButton.setOnLongClickListener(commenLongClickListener);
		LaunchButton.setOnLongClickListener(commenLongClickListener);
		cmdEditText.setOnLongClickListener(commenLongClickListener);

		shellScrollView = (ScrollView) findViewById(R.id.xmsvshell);
		shellTextView = (TextView) findViewById(R.id.xmtvshell);
		shellTextView.setTextColor(Color.WHITE);
		shellTextView.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10);
		shellTextView.setScrollbarFadingEnabled(true);
		shellTextView.setBackgroundColor(Color.parseColor("#150015"));

		shellScrollView.setScrollbarFadingEnabled(false);
		shellScrollView.setSmoothScrollingEnabled(true);
		shellScrollView.setOnTouchListener(this);
		shellScrollView.setLongClickable(true);
		detector.setIsLongpressEnabled(true);

		ArrayAdapter adapter = new ArrayAdapter(this,
				android.R.layout.simple_spinner_item, NewShellStrings);

		cmdEditText.setAdapter(adapter);
		shellFsClassCommen.show_toast("长按保存键可清理shell日志");
	}

	private String ArrayToString(ArrayList<String> newShellStrings2)
	{
		// TODO Auto-generated method stub
		String string = null;
		for (int i = 0; i < newShellStrings2.size(); i++)
		{
			if (string != null)
			{
				string += newShellStrings2.get(i);
			} else
			{
				string = newShellStrings2.get(i);
			}

			string += ",";
		}
		return string;
	}

	private ArrayList<String> InitShellCmdArray(String[] s)
	{
		// TODO Auto-generated method stub
		ArrayList<String> mArrayList = new ArrayList<String>();
		for (int i = 0; i < s.length; i++)
		{
			mArrayList.add(s[i]);
		}
		return mArrayList;
	}

	OnLongClickListener commenLongClickListener = new OnLongClickListener()
	{

		@Override
		public boolean onLongClick(View arg0)
		{
			// TODO Auto-generated method stub
			if (arg0.equals(saveButton))
			{
				shellFsClassCommen.runRootCommend("rm /data/lidbg/shell*");
				shellFsClassCommen.show_toast("shell日志已清理");
			}
			return true;
		}
	};
	OnClickListener commenClickListener = new OnClickListener()
	{
		@Override
		public void onClick(View arg0)
		{
			// TODO Auto-generated method stub
			if (arg0.equals(saveButton))
			{
				shellFsClassCommen.write_to_file(
						"/data/lidbg/shell-"
								+ shellcmdString.replace("/", "")
								+ shellFsClassCommen
										.GetCurrentTime("yyyyMMdd_hhmmss")
								+ ".txt", true, lastcmdresultString);
			} else if (arg0.equals(LaunchButton))
			{
				if (cmdEditText.length() > 0)
				{
					shellcmdString = cmdEditText.getText().toString();
					lastcmdresultString = shellFsClassCommen
							.runRootCommend(shellcmdString);
					shellTextView.setText(lastcmdresultString);
					if (!NewShellStrings.contains(shellcmdString))
					{
						NewShellStrings.add(shellcmdString);
					}

				}
			}

		}
	};

	@Override
	protected void onPause()
	{
		// TODO Auto-generated method stub
		String mString = ArrayToString(NewShellStrings);
		shellPreferences.edit().putString(PreferShellString, mString).commit();
		shellFsClassCommen.runRootCommend("chmod 777 /data/lidbg/shell*");
		super.onPause();
	}

	private void request_full_screen()
	{
		// TODO Auto-generated method stub
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
	}

	@Override
	protected void onDestroy()
	{
		// TODO Auto-generated method stub
		Log.e(TAG, "[futengfei].onDestroy\n");
		super.onDestroy();
	}

	@Override
	public boolean onDown(MotionEvent e)
	{
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
			float velocityY)
	{
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onLongPress(MotionEvent e)
	{
		// TODO Auto-generated method stub
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,
			float distanceY)
	{
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void onShowPress(MotionEvent e)
	{
		// TODO Auto-generated method stub
	}

	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean onTouch(View v, MotionEvent event)
	{
		// TODO Auto-generated method stub
		switch (event.getAction())
		{
		case MotionEvent.ACTION_UP:
			break;
		default:
			break;
		}
		return false;
	}
}
