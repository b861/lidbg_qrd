package com.android.mypftf;
import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

public class FsActivityCarTS extends Activity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		request_full_screen();
        setContentView(new PointerLocationView(this));  
        
        // Make the screen full bright for this activity.  
        WindowManager.LayoutParams lp = getWindow().getAttributes();  
        lp.screenBrightness = 1.0f;  
        getWindow().setAttributes(lp); 
	}

	private void request_full_screen()
	{
		// TODO Auto-generated method stub
		{
			requestWindowFeature(Window.FEATURE_NO_TITLE);
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
					WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}
	}

}
