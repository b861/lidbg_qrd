

#define LOG_TAG "LidbgService"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include "../Lidbg.h"

#include <stdio.h>

namespace android
{
/*在硬件抽象层中定义的硬件访问结构体，参考<hardware/lidbg.h>*/
struct lidbg_device_t* lidbg_device = NULL;

/*通过硬件抽象层定义的硬件访问接口设置硬件寄存器val的值*/



/*通过硬件抽象层定义的硬件模块打开接口打开硬件设备*/
static inline int lidbg_device_open(const hw_module_t* module, struct lidbg_device_t** device)
{
	return module->methods->open(module, HELLO_HARDWARE_MODULE_ID, (struct hw_device_t**)device);
}

/*通过硬件模块ID来加载指定的硬件抽象层模块并打开硬件*/
static jboolean lidbg_init(JNIEnv* env, jclass clazz) 
{
	hw_module_methods_t* module;
	
	LOGI("lidbg JNI: initializing......");
	if(hw_get_module(HELLO_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) == 0) //获取了hw_module_t 结构体
	{
		LOGI("lidbg JNI: lidbg Stub found.");
		if(lidbg_device_open(&(module->common), &lidbg_device) == 0) //获取了lidbg_device_t 结构体
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

static void finalize_native(JNIEnv *env, jobject clazz, int ptr)
{
    Devices* devices = (Devices*)ptr;
    if (devices == NULL) {
        return;
    }

    free(devices);
}

static void setLight_native(JNIEnv *env, jobject clazz, int ptr,
        int light, int colorARGB, int flashMode, int onMS, int offMS, int brightnessMode)
{
    Devices* devices = (Devices*)ptr;
    light_state_t state;

    if (light < 0 || light >= LIGHT_COUNT || devices->Lidbg[light] == NULL) {
        return ;
    }

    memset(&state, 0, sizeof(light_state_t));
    state.color = colorARGB;
    state.flashMode = flashMode;
    state.flashOnMS = onMS;
    state.flashOffMS = offMS;
    state.brightnessMode = brightnessMode;

    devices->Lidbg[light]->set_light(devices->Lidbg[light], &state);
}

static JNINativeMethod method_table[] = {
    { "init_native", "()I", (void*)init_native },
    { "finalize_native", "(I)V", (void*)finalize_native },
    { "setLight_native", "(IIIIIII)V", (void*)setLight_native },
};

int register_android_server_LidbgService(JNIEnv *env)
{
    return jniRegisterNativeMethods(env, "com/android/server/LidbgService",
            method_table, NELEM(method_table));
}

};
