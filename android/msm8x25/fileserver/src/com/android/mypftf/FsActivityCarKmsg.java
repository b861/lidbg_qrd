package com.android.mypftf;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import org.apache.http.util.EncodingUtils;
import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.util.TypedValue;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnLongClickListener;
import android.view.View.OnTouchListener;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class FsActivityCarKmsg extends Activity implements OnTouchListener,
		OnGestureListener
{
	private TextView kmsgTextView;
	private Thread kmsg_thread;
	private ScrollView kmsgScrollView;
	private FileInputStream inputStream;
	private boolean in_follow_enable = false;
	private boolean is_ts_moved;
	private boolean kmsg_thread_enable = true;
	private GestureDetector detector = new GestureDetector(this);
	private SeekBar kmsgSeekBar;
	private String TAG = "kmsg_activity";
	private String kmsg_file = "/data/lidbg/lidbg_kmsg.txt";
	private String kmsgstring;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		request_full_screen();
		setContentView(R.layout.layout_kmsg);

		kmsg_thread_enable = true;
		setTitle("fileserver.apk 获取系统KMSG模式  【machine_id:"
				+ FsActivityCarMain.machine_id + "】");
		Log.e(TAG, "[futengfei].onProgressChanged:\n");
		toast_show("长摁开启KMSG跟随");
		kmsgTextView = (TextView) findViewById(R.id.xmtvkmsg);
		kmsgScrollView = (ScrollView) findViewById(R.id.xmsvkmsg);
		kmsgSeekBar = (SeekBar) findViewById(R.id.xmkmsgSeekbar);
		kmsgTextView.setTextColor(Color.WHITE);
		kmsgTextView.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10);
		kmsgTextView.setScrollbarFadingEnabled(true);
		kmsgTextView.setBackgroundColor(Color.parseColor("#150015"));
		kmsgTextView.setOnLongClickListener(new OnLongClickListener()
		{
			@Override
			public boolean onLongClick(View v)
			{
				// TODO Auto-generated method stub
				in_follow_enable = !in_follow_enable;
				if (in_follow_enable)
				{
					toast_show("已开启kmsg跟随");
				} else
				{
					toast_show("已关闭kmsg跟随");
				}
				return false;
			}
		});
		kmsgScrollView.setScrollbarFadingEnabled(false);
		kmsgScrollView.setSmoothScrollingEnabled(true);
		kmsgScrollView.setOnTouchListener(this);
		kmsgScrollView.setLongClickable(true);
		detector.setIsLongpressEnabled(true);
		kmsgSeekBar.setOnSeekBarChangeListener(kmsgSeekBarChangeListener);
		creat_kmsg_thread();
	}

	OnSeekBarChangeListener kmsgSeekBarChangeListener = new OnSeekBarChangeListener()
	{
		@Override
		public void onStopTrackingTouch(SeekBar seekBar)
		{
			// TODO Auto-generated method stub
			is_ts_moved = false;
		}

		@Override
		public void onStartTrackingTouch(SeekBar seekBar)
		{
			// TODO Auto-generated method stub
			is_ts_moved = true;
		}

		@Override
		public void onProgressChanged(SeekBar seekBar, int progress,
				boolean fromUser)
		{
			// TODO Auto-generated method stub
			float srollto = seekBar.getProgress() * kmsgTextView.getHeight();
			kmsgScrollView.smoothScrollTo((int) srollto / seekBar.getMax(),
					(int) srollto / seekBar.getMax());
			Log.w(TAG, "[futengfei].onProgressChanged:\n" + srollto);
		}
	};

	private void request_full_screen()
	{
		// TODO Auto-generated method stub
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
				WindowManager.LayoutParams.FLAG_FULLSCREEN);
	}

	private void toast_show(CharSequence toast_string)
	{
		// TODO Auto-generated method stub
		Toast.makeText(FsActivityCarKmsg.this, toast_string, Toast.LENGTH_SHORT)
				.show();
	}

	private void creat_kmsg_thread()
	{
		// TODO Auto-generated method stub
		kmsg_thread = (Thread) new Thread(new Runnable()
		{
			private long kmsg_delay = 600;
			private int old_file_length = 0;
			private int buff_len = 512;
			private int len;
			private File kmsgFile;

			@Override
			public void run()
			{
				try
				{
					kmsgFile = new File(kmsg_file);
					inputStream = new FileInputStream(kmsgFile);
				} catch (FileNotFoundException e1)
				{
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				while (true)
				{
					runOnUiThread(new Runnable()
					{
						public void run()
						{
							if (kmsg_thread_enable)
							{
								try
								{
									len = inputStream.available();
									setTitle("fileserver.apk 获取系统KMSG模式  【machine_id:"
											+ FsActivityCarMain.machine_id
											+ "】"
											+ "                     "
											+ kmsgFile.length()
											+ "  "
											+ kmsgTextView.getHeight()
											+ "  speed:" + len);
									if (in_follow_enable)
									{
										buff_len = 128;
										kmsg_delay = 200;
									}
									if (len > buff_len)
									{
										byte[] buffer = new byte[len];
										inputStream.read(buffer);
										kmsgstring = EncodingUtils.getString(
												buffer, "UTF-8");
										if (in_follow_enable && !is_ts_moved)
										{
											kmsgScrollView
													.fullScroll(ScrollView.FOCUS_DOWN);
										}
										kmsgTextView.append(kmsgstring);
										if (in_follow_enable && !is_ts_moved)
										{
											kmsgScrollView
													.fullScroll(ScrollView.FOCUS_DOWN);
										}
									}
									if (kmsgFile.length() < old_file_length)
									{
										inputStream.close();
										inputStream = new FileInputStream(
												kmsgFile);
										kmsgTextView.setText("reset");
										// toast_show("reset");
										Log.e(TAG,
												"[futengfei].kmsgTextView:reset\n");
									}
									old_file_length = (int) kmsgFile.length();
								} catch (IOException e)
								{
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
							}
						}
					});
					// Log.e(TAG, "-------->onResume" + cunt1);
					try
					{
						Thread.sleep(kmsg_delay);
					} catch (InterruptedException e)
					{
						e.printStackTrace();
					}
				}
			}
		});
		kmsg_thread.start();
	}

	@Override
	protected void onDestroy()
	{
		// TODO Auto-generated method stub
		Log.e(TAG, "[futengfei].onDestroy\n");
		try
		{
			inputStream.close();
			kmsg_thread_enable = false;
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
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
			kmsgSeekBar
					.setProgress((int) ((kmsgScrollView.getScrollY() * 100) / kmsgTextView
							.getHeight()));
			Log.e(TAG, "[futengfei].kmsgSeekBar.setProgress\n");
			break;
		default:
			break;
		}
		// detector.onTouchEvent(event);
		return false;
	}
}
