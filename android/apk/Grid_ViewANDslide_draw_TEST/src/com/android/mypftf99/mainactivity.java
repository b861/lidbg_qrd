package com.android.mypftf99;

import com.android.mypftf99.R;

import android.R.string;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.Selection;
import android.text.Spannable;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.MultiAutoCompleteTextView;
import android.widget.SlidingDrawer;
import android.widget.Toast;

/**
 * @author 付腾飞
 * 时间：2012.12.18.22：09
 * 程序骨干简介：
 * 1：采用抽屉来呈现用户界面 （SlidingDrawer）
 * 2：采用GridView 呈现更多数据
 * 3：重写用户返回键
 * 4：扩大程序视野，让 程序获取最大显示，去掉title和系统通知栏
 *
 */
public class mainactivity extends Activity {
	
	private int[] icons={R.drawable.img1,R.drawable.img2,R.drawable.img3,R.drawable.img4,R.drawable.img5,R.drawable.img6,R.drawable.img7,R.drawable.img8,R.drawable.img9};
	private String[] icons_name={"买","不卖","买","不卖","买","不卖","买","不卖","买"};
	private ImageView mjImageView;
	private GridView mjgridviewGridView;
	private SlidingDrawer mjsliDrawer;
	public AutoCompleteTextView cmdeditext;
	public Button fasongButton;
	private Button qingkongButton;

	
	
//feature2  重写用户按返回键的方法
    @Override
	public void onBackPressed()
	{
	
		new AlertDialog.Builder(mainactivity.this)
		.setTitle("确认退出")
		.setIcon(R.drawable.leave)
		.setMessage("确认退出")
		.setPositiveButton("关闭", new DialogInterface.OnClickListener()
		{
			
			@Override
			public void onClick(DialogInterface arg0, int arg1)
			{
				// TODO Auto-generated method stub
				Toast.makeText(mainactivity.this, "",Toast.LENGTH_SHORT).show();
				finish();
			}
		})
		
		.setNegativeButton("不关闭",  new DialogInterface.OnClickListener()
		{
			
			@Override
			public void onClick(DialogInterface arg0, int arg1)
			{
				// TODO Auto-generated method stub
				Toast.makeText(mainactivity.this, "open draw",Toast.LENGTH_SHORT).show();
				mjsliDrawer.open();
			}
		})
		.show();
		
		// TODO Auto-generated method stub
		super.onBackPressed();  //要把这句话去掉，不去掉，这句话回发魔力，让程序退出；~嘎嘎！付腾飞！
	}


//feature1  主程序
	/** Called when the activity is first created. */
    @SuppressWarnings("unused")
	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
if (false)
	quanping();

        
        setContentView(R.layout.main);
        
        mjgridviewGridView = (GridView) findViewById(R.id.mycontent1);
        mjsliDrawer = (SlidingDrawer) findViewById(R.id.mxslidedraw1);
        mjImageView = (ImageView) findViewById(R.id.myimage1);
        cmdeditext =  (AutoCompleteTextView) findViewById(R.id.cmd);
        fasongButton = (Button) findViewById(R.id.fasong_cmd);
        qingkongButton = (Button) findViewById(R.id.qingkong_cmd);
        String[] str = { 
        		"c io w 27 1", 
        		"c io r 27", 
        		"c i2c r 1 0x42 0x00 35", 
        		"c i2c w 1 0x50 2 0x0 0xa",
        		"c video testYuv",
        		"c key home 2",
        		"c touch 180 207 2",
        		"c cmm user /system/bin/insmod /flysystem/lib/out/lidbg_test.ko",
        		"c cmm user /system/bin/reboot",
        		"c cmm user /system/bin/setprop fly.test.me 1",
        		"c cmm user /system/bin/cat /proc/kmsg>/data/kmsg.txt",
        	};  
  
        ArrayAdapter adapter = new ArrayAdapter(this,android.R.layout.simple_dropdown_item_1line, str);
        cmdeditext.setAdapter(adapter);  
        
{ //below is not public for the apk
        	
            //LidbgJniService.init_native();        
            LidbgService.init_native();
            cmdeditext.setText("c"); 
    		cmdeditext.setOnLongClickListener(new OnLongClickListener()
    		{
    			
    			@Override
    			public boolean onLongClick(View v)
    			{
    				//LidbgJniService.setLight_native(cmdeditext.getText().toString());
    				LidbgService.send_cmd_native(cmdeditext.getText().toString());
    				return false;
    			}
    		});
    		fasongButton.setOnClickListener(new OnClickListener()
			{
				
				@Override
				public void onClick(View v)
				{
					// TODO Auto-generated method stub
    				//LidbgJniService.setLight_native(cmdeditext.getText().toString());
    				LidbgService.send_cmd_native(cmdeditext.getText().toString());
				}
			});
    		qingkongButton.setOnClickListener(new OnClickListener()
			{
				
				@Override
				public void onClick(View v)
				{
					// TODO Auto-generated method stub
					String  txtString = "c";
					cmdeditext.setText(txtString); 
					cmdeditext.setSelection(txtString.length());
				}
			});
    }//end 

        

        MyGridViewAdapter mjgridViewadaper = new MyGridViewAdapter(this,icons_name,icons);
        mjgridviewGridView.setAdapter(mjgridViewadaper);
        
        mjsliDrawer.setOnDrawerOpenListener(new SlidingDrawer.OnDrawerOpenListener()
		{
			
			@Override
			public void onDrawerOpened()
			{
				
				mjImageView.setImageResource(R.drawable.close);
				mjsliDrawer.setBackgroundResource(R.drawable.p2);
			}
		});
        
        
        mjsliDrawer.setOnDrawerCloseListener(new SlidingDrawer.OnDrawerCloseListener()
		{
			
			@Override
			public void onDrawerClosed()
			{
				// TODO Auto-generated method stub
				mjImageView.setImageResource(R.drawable.open);
				mjsliDrawer.setBackgroundResource(R.drawable.pict1);
			}
		});
    }


	private void quanping()
	{
		{
			    //feature3:  程序获取最大显示，去掉title和系统通知栏
			    requestWindowFeature(Window.FEATURE_NO_TITLE);
		        getWindow().setFlags
		        (
		        		WindowManager.LayoutParams.FLAG_FULLSCREEN, 
		        		WindowManager.LayoutParams.FLAG_FULLSCREEN
		        );
		}
	}
}