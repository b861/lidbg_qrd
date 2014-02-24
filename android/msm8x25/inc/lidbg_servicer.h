#ifndef _LIGDBG_SERVER_APP__
#define _LIGDBG_SERVER_APP__


//#undef printf
#define lidbg  LIDBG_PRINT
#define DUMP_BUILD_TIME    do{ lidbg( "Build Time: %s, %s, %s \n", __FUNCTION__, __DATE__, __TIME__);}while(0)
#define DUMP_BUILD_TIME_FILE do{\
	char string[256];\
	sprintf(string, "echo %s build time:%s,%s >> /dev/log/lidbg_log.txt", __FILE__,__DATE__,__TIME__);\
	system(string);\
}while(0)



#define LIDBG_CALL(cmd,buf,ret_bytes) do{\
	int fd;\
	 fd = open("/dev/mlidbg0", O_RDWR);\
	 if((fd == 0)||(fd == (int)0xfffffffe)|| (fd == (int)0xffffffff))break;\
	 write(fd, &cmd, sizeof(cmd));\
	 if((buf != NULL)&&(ret_bytes))\
	 {\
	 	read(fd, buf, ret_bytes);\
	 }\
	close(fd);\
}while(0)

#if 1

#define LOG_BYTES   (256)

#define LIDBG_PRINT(msg...) do{\
	int fd;\
	char s[LOG_BYTES];\
	sprintf(s, "lidbg_msg: " msg);\
	s[LOG_BYTES - 1] = '\0';\
	 fd = open("/dev/lidbg_msg", O_RDWR);\
	 if((fd == 0)||(fd == (int)0xfffffffe)|| (fd == (int)0xffffffff))break;\
	 write(fd, s, /*sizeof(msg)*/strlen(s)/*LOG_BYTES*/);\
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

struct lidbg_dev_smem
{
    unsigned long smemaddr;
    unsigned long smemsize;
    unsigned long valid_offset;
};


#endif
