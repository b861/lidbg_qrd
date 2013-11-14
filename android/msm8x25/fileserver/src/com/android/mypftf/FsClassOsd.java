package com.android.mypftf;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.view.WindowManager.LayoutParams;
import android.widget.TextView;
import android.widget.Toast;

public class FsClassOsd
{
	public WindowManager wm = null;
	public static WindowManager.LayoutParams params = new WindowManager.LayoutParams();
	public View mcpuaccOSD = null;
	private TextView cpuaccTextView;
	private boolean is_view_added = false;
	private Context gcontext;

	private String TAG = "futengfei";

	/**
	 * 
	 */
	public FsClassOsd(Context context)
	{
		// TODO Auto-generated constructor stub
		gcontext = context;
	}

	public void show_toast(String toast_string)
	{
		// TODO Auto-generated method stub
		Toast.makeText(gcontext, toast_string, Toast.LENGTH_SHORT).show();
	}

	public boolean is_osd_enable()
	{
		// TODO Auto-generated method stub
		return is_view_added;
	}

	public void prepare(int layoutid, int textviewid)
	{
		// TODO Auto-generated method stub
		if (mcpuaccOSD == null)
		{
			wm = (WindowManager) gcontext
					.getSystemService(Context.WINDOW_SERVICE);
			params.flags = LayoutParams.FLAG_NOT_FOCUSABLE
					| LayoutParams.FLAG_NOT_TOUCHABLE
					| LayoutParams.FLAG_NOT_TOUCH_MODAL
					| LayoutParams.FLAG_FULLSCREEN;
			params.type = LayoutParams.TYPE_SYSTEM_ALERT;
			params.width = WindowManager.LayoutParams.MATCH_PARENT;
			params.height = WindowManager.LayoutParams.FILL_PARENT;
			params.format = PixelFormat.RGBA_8888;
			params.alpha = 1;
			params.gravity = Gravity.TOP | Gravity.CENTER;
			LayoutInflater inflater = (LayoutInflater) gcontext
					.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

			mcpuaccOSD = (View) inflater.inflate(layoutid, null);
			cpuaccTextView = (TextView) mcpuaccOSD.findViewById(textviewid);

		}
	}

	public void addview()
	{
		// TODO Auto-generated method stub
		if (mcpuaccOSD.getParent() == null)
		{
			wm.addView(mcpuaccOSD, params);
			is_view_added = true;
		}
		Log.e(TAG, "log:" + is_view_added);
	}

	public void removeview()
	{
		// TODO Auto-generated method stub
		if (is_view_added)
			wm.removeView(mcpuaccOSD);
		is_view_added = false;
	}

	public void set_text(CharSequence text)
	{
		// TODO Auto-generated method stub
		cpuaccTextView.setText(text);
	}

	public void set_text_size(float size)
	{
		// TODO Auto-generated method stub
		if (size < 1)
		{
			size = 1;
		}
		cpuaccTextView.setTextSize(TypedValue.COMPLEX_UNIT_PX, size);
	}

	public void set_text_background(String colorString)
	{
		// TODO Auto-generated method stub
		cpuaccTextView.setBackgroundColor(Color.parseColor("#" + colorString));
	}

	public TextView get_osd_textView()
	{
		// TODO Auto-generated method stub
		return cpuaccTextView;
	}

	public void set_text_color(String string)
	{
		// TODO Auto-generated method stub
		cpuaccTextView.setTextColor(Color.parseColor("#" + string));
	}

}
