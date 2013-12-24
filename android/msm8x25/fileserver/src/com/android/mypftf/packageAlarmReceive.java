package com.android.mypftf;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.widget.Toast;

public class packageAlarmReceive extends BroadcastReceiver
{

	@Override
	public void onReceive(Context context, Intent intent)
	{
		// TODO Auto-generated method stub
		FsClassCarCommen mfsFsClassCarCommen = new FsClassCarCommen(context);
		mfsFsClassCarCommen.show_toast("收到广播 ");
		mfsFsClassCarCommen.write_to_file(
				"/sys/class/leds/lcd-backlight/brightness", false,
				intent.getAction());
		// Intent mIntent = new Intent(context, changetonormal.class);
		// mIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		// context.startActivity(mIntent);
	}
}
