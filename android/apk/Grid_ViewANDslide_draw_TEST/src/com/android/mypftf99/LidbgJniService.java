package com.android.mypftf99;

import android.R.string;

public class LidbgJniService
{
	static {
		System.loadLibrary("_lidbg_servers");
		//System.load("/system/lib_lidbg_servers.so");
	}
	
	
	public static native int init_native();
	public static native void finalize_native(int ptr);
	public static native void setLight_native(String cmd);
	
}
