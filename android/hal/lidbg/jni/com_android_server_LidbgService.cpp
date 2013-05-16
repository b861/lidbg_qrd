

#define LOG_TAG "LidbgService"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include "../lidbg.h"

#include <stdio.h>

namespace android
{
/*在硬件抽象层中定义的硬件访问结构体，参考<hardware/lidbg.h>*/
struct lidbg_device_t* lidbg_device = NULL;

/*通过硬件抽象层定义的硬件访问接口设置硬件寄存器val的值*/



/*通过硬件抽象层定义的硬件模块打开接口打开硬件设备*/
static inline int lidbg_device_open( hw_module_t* module, struct lidbg_device_t** device)
{
	return module->methods->open(module, LIDBG_HARDWARE_MODULE_ID, (struct hw_device_t**)device);
}

/*通过硬件模块ID来加载指定的硬件抽象层模块并打开硬件*/
static jboolean init_native(JNIEnv* env, jclass clazz) 
{
	hw_module_t* module;
	
	LOGI("lidbg JNI: initializing......");
	if(hw_get_module(LIDBG_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) == 0) //获取了hw_module_t 结构体
	{
		LOGI("lidbg JNI: lidbg Stub found.");
		if(lidbg_device_open(module, &lidbg_device) == 0) //获取了lidbg_device_t 结构体
		{
			LOGI("lidbg JNI: lidbg device is open.");
			return 0;
		}
		LOGE("lidbg JNI: failed to open lidbg device.");
		return -1;
	}
	LOGE("lidbg JNI: failed to get lidbg stub module.");
	return -1;		
}



static int send_cmd_native(JNIEnv *env, jobject clazz, jstring cmd)
{
   const char *send_cmd = env->GetStringUTFChars(cmd, NULL);
   LOGI("LIDBG JNI: send_cmd_native %s.", send_cmd);  
   if(!lidbg_device) 
   {	
	   LOGE("LIDBG JNI: device is not open.");	
	   return -1;	
   }
   
   lidbg_device->send_cmd(lidbg_device, send_cmd); 
   return 0;

}

static JNINativeMethod method_table[] = {
    { "init_native", "()Z", (void*)init_native },
    { "send_cmd_native", "(Ljava/lang/String;)I", (void*)send_cmd_native },
};

int register_android_server_LidbgService(JNIEnv *env)
{
   // return jniRegisterNativeMethods(env, "com/android/server/LidbgService", method_table, NELEM(method_table));
   
   return jniRegisterNativeMethods(env, "com/android/mypftf99/LidbgService", method_table, NELEM(method_table));
}

};
