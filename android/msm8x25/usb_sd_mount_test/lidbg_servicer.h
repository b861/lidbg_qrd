#ifndef _LIGDBG_SERVER_APP__
#define _LIGDBG_SERVER_APP__


#undef printf
#define printf  LIDBG_PRINT


#define LIDBG_CALL(cmd,buf,ret_bytes) do{\
	int fd;\
	 fd = open("/dev/mlidbg0", O_RDWR);\
	 if((fd == 0)||(fd == (int)0xfffffffe)|| (fd == (int)0xffffffff))break;\
	 write(fd, cmd, sizeof(cmd));\
	 if((buf != NULL)&&(ret_bytes))\
	 {\
	 	read(fd, buf, ret_bytes);\
	 }\
	close(fd);\
}while(0)

#if 1
#define LIDBG_PRINT(msg...) do{\
	int fd;\
	char s[64];\
	sprintf(s, "lidbg_msg: " msg);\
	 fd = open("/dev/lidbg_msg", O_RDWR);\
	 if((fd == 0)||(fd == (int)0xfffffffe)|| (fd == (int)0xffffffff))break;\
	 write(fd, s, 64);\
	 close(fd);\
}while(0)
#endif


//java
/*
import java.io.FileOutputStream;

private void LIDBG_PRINT(String msg)
{
	final String LOG_E = "LIDBG_PRINT";
	msg = "JAVA:" + msg;
	byte b[] = msg.getBytes();
	FileOutputStream stateOutputMsg;
	try {
	stateOutputMsg = new FileOutputStream("/dev/lidbg_msg", true);
	stateOutputMsg.write(b);
	} catch (Exception e ) {
		Log.e(LOG_E, "Failed to set the fastboot state");
	}
}
LIDBG_PRINT("hello world\n");


*/

#endif
