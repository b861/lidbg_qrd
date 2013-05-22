package com.android.mypftf99;

public class LidbgService
{

	static {
		System.load("/system/lib/hw/liblidbg_servers.so");
	}
	
	
	public static native boolean init_native();
	public static native int send_cmd_native(String cmd);
	
}
