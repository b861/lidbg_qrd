package com.android.mypftf99;


import com.android.mypftf99.R;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class MyGridViewAdapter extends BaseAdapter
{

	private Context _con;
	private String[] _icons_name;
	private int[] _icons;

	public MyGridViewAdapter (Context context,String[] icons_name,int[] icons)
	{
		_con =context;
		_icons_name=icons_name;
		_icons= icons;
		
	}
	@Override
	public int getCount()
	{
		// TODO Auto-generated method stub
		return _icons_name.length;
	}

	@Override
	public Object getItem(int arg0)
	{
		// TODO Auto-generated method stub
		return _icons_name[arg0];
	}

	@Override
	public long getItemId(int arg0)
	{
		// TODO Auto-generated method stub
		return arg0;
	}

	@Override
	public View getView(int position, View convertvView, ViewGroup parent)
	{
		
		LayoutInflater factoryInflater = LayoutInflater.from(_con);
		View v =factoryInflater.inflate(R.layout.grid, null);
		ImageView imageView=(ImageView) v.findViewById(R.id.mx1imageview);
		TextView tView =(TextView) v.findViewById(R.id.mx1textview);
		imageView.setImageResource(_icons[position]);
		tView.setText(_icons_name[position]);
		
		return v;
	}

}
