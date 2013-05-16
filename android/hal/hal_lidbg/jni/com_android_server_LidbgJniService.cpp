/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "[lidbgjniServerce]"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include  "../hal_lidbg_commen.h"

#include <stdio.h>

namespace android
{

// These values must correspond with the LIGHT_ID constants in
// LightsService.java
enum {
    LIGHT_INDEX_BACKLIGHT = 0,
    LIGHT_INDEX_KEYBOARD = 1,
    LIGHT_INDEX_BUTTONS = 2,
    LIGHT_INDEX_BATTERY = 3,
    LIGHT_INDEX_NOTIFICATIONS = 4,
    LIGHT_INDEX_ATTENTION = 5,
    LIGHT_INDEX_BLUETOOTH = 6,
    LIGHT_INDEX_WIFI = 7,
    LIGHT_COUNT
};

struct Devices {
    lidbg_device_t* lights[LIGHT_COUNT];
};
Devices* g_lidbg_devices;
static lidbg_device_t* get_device(hw_module_t* module, char const* name)
{
    int err;
    hw_device_t* device;
    err = module->methods->open(module, name, &device);
    if (err == 0) {
        return (lidbg_device_t*)device;
    } else {
        return NULL;
    }
}

static jint init_native(JNIEnv *env, jobject clazz)
{
    int err;
    hw_module_t* module;
    
    g_lidbg_devices = (Devices*)malloc(sizeof(Devices));

    err = hw_get_module(LIDBG_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (err == 0) {
        g_lidbg_devices->lights[LIGHT_INDEX_BACKLIGHT]
                = get_device(module, LIGHT_ID_BACKLIGHT);
    } else {
        memset(g_lidbg_devices, 0, sizeof(Devices));
    }

    return (jint)g_lidbg_devices;
}

static void finalize_native(JNIEnv *env, jobject clazz, int ptr)
{
    if (g_lidbg_devices == NULL) {
        return;
    }

    free(g_lidbg_devices);
}

static void setLight_native(JNIEnv *env, jobject clazz, jstring cmd )
{
	struct lidbg_state_t  state;
	if ( g_lidbg_devices->lights[LIGHT_INDEX_BACKLIGHT]== NULL) 
		{
			LOGE("[futengfei]======setLight_native:not find you device by name .\n");
			return ;
		}

	memset(&state, 0, sizeof(lidbg_state_t));
	const char *setid = env->GetStringUTFChars(cmd, NULL);
	state.cmd = setid;
	LOGE("[futengfei]cominto====JNI===[%s]\n", setid);
	g_lidbg_devices->lights[LIGHT_INDEX_BACKLIGHT]->cmd2kernel( g_lidbg_devices->lights[LIGHT_INDEX_BACKLIGHT],state);
}

static JNINativeMethod method_table[] = {
    { "init_native", "()I", (void*)init_native },
    { "finalize_native", "(I)V", (void*)finalize_native },
    { "setLight_native", "(Ljava/lang/String;)V", (void*)setLight_native },
};

int register_android_server_Lidbg_Jni_Service(JNIEnv *env)
{
	LOGE("[futengfei]cominto====JNI===[%s]\n", __func__);

	return jniRegisterNativeMethods(env, "com/android/mypftf99/LidbgJniService",
            method_table, NELEM(method_table));
}

};
