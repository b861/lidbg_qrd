#include "lidbg_servicer.h"

#include <stdio.h>
#include <utils/Log.h>
#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>

#include "jni.h"
#include "hal_lidbg_commen.h"

#define DEBG_TAG "jni_futengfei."

typedef union
{
    JNIEnv *env;
    void *venv;
} UnionJNIEnvToVoid;
struct Devices
{
    lidbg_device_t *dev;
};
Devices *g_lidbg_devices;

static lidbg_device_t *get_device(hw_module_t *module, char const *name)
{
    int err;
    hw_device_t *device;
    err = module->methods->open(module, name, &device);
    if (err == 0)
    {
        lidbg(DEBG_TAG"[%s].open.suc\n", __FUNCTION__);
        return (lidbg_device_t *)device;
    }
    else
    {
        lidbg(DEBG_TAG"[%s].open.fail\n", __FUNCTION__);
        return NULL;
    }
}
static jlong native_init(JNIEnv * /*env*/, jobject /*clazz*/)
{
    int err;
    hw_module_t *module;
    g_lidbg_devices = (Devices *)malloc(sizeof(Devices));
    lidbg(DEBG_TAG"[%s].in,%s\n", __FUNCTION__, LIDBG_HARDWARE_MODULE_ID);
    err = hw_get_module(LIDBG_HARDWARE_MODULE_ID, (hw_module_t const **)&module);
    if (err)
    {
        lidbg(DEBG_TAG"[%s].hw_get_module.fail.%s\n", __FUNCTION__, strerror(-err));
        memset(g_lidbg_devices, 0, sizeof(Devices));
    }
    else
    {
        lidbg(DEBG_TAG"[%s].hw_get_module.suc\n", __FUNCTION__);
        g_lidbg_devices->dev = get_device(module, NULL);
    }
    return (jlong)g_lidbg_devices;
}

static jint native_destroy(JNIEnv * /*env*/, jobject /*clazz*/, jlong /*ptr*/)
{
    lidbg(DEBG_TAG"[%s].in\n", __FUNCTION__);

    if (g_lidbg_devices == NULL)
    {
        return -1;
    }
    free(g_lidbg_devices);
    return 0;
}

static jint Camera_setPath(JNIEnv *env, jobject /*clazz*/, jstring path )
{
    struct lidbg_state_t  state;
    if ( g_lidbg_devices->dev == NULL)
    {
        lidbg(DEBG_TAG"[%s].g_lidbg_devices->dev == NULL\n", __FUNCTION__);
        return -1;
    }
    memset(&state, 0, sizeof(lidbg_state_t));
    char *mpath = (char *)env->GetStringUTFChars(path, NULL);
    state.cmd_type = cmd2kernel_set_path;
    state.data = mpath;
    lidbg(DEBG_TAG"[%s].mpath=%s\n", __FUNCTION__, mpath);
    return g_lidbg_devices->dev->cmd2kernel( g_lidbg_devices->dev, state);
}
static jint Camera_start_record(JNIEnv * /*env*/, jobject /*clazz*/, jint /*num*/)
{
    struct lidbg_state_t  state;
    if ( g_lidbg_devices->dev == NULL)
    {
        lidbg(DEBG_TAG"[%s].g_lidbg_devices->dev == NULL\n", __FUNCTION__);
        return -1;
    }
    memset(&state, 0, sizeof(lidbg_state_t));
    state.cmd_type = cmd2kernel_start_record;
    lidbg(DEBG_TAG"[%s].\n", __FUNCTION__);
    return g_lidbg_devices->dev->cmd2kernel( g_lidbg_devices->dev, state);
}
static jint Camera_stop_record(JNIEnv * /*env*/, jobject /*clazz*/, jint /*num*/)
{
    struct lidbg_state_t  state;
    if ( g_lidbg_devices->dev == NULL)
    {
        lidbg(DEBG_TAG"[%s].g_lidbg_devices->dev == NULL\n", __FUNCTION__);
        return -1;
    }
    memset(&state, 0, sizeof(lidbg_state_t));
    state.cmd_type = cmd2kernel_stop_record;
    lidbg(DEBG_TAG"[%s].\n", __FUNCTION__);
    return g_lidbg_devices->dev->cmd2kernel( g_lidbg_devices->dev, state);
}


static JNINativeMethod methods[] =
{
    { "nativeInit", "()J", (void *)native_init },
    { "nativeDestroy", "(J)I", (void *)native_destroy },
    { "CameraSetPath", "(Ljava/lang/String;)I", (void *)Camera_setPath },
    { "CameraStartRecord", "(I)I", (void *)Camera_start_record },
    { "CameraStopRecord", "(I)I", (void *)Camera_stop_record },
};
//add up below:
static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods)
{
    jclass clazz;
    lidbg(DEBG_TAG"[%s].\n", __FUNCTION__);
    clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        lidbg(DEBG_TAG"Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
    {
        lidbg(DEBG_TAG"RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static int registerNatives(JNIEnv *env)
{
    if (!registerNativeMethods(env, "com/android/mypftf99/app4hal/LidbgJniNative", methods, sizeof(methods) / sizeof(methods[0])))
    {
        lidbg(DEBG_TAG"[%s].fail\n", __FUNCTION__);
        return JNI_FALSE;
    }
    lidbg(DEBG_TAG"[%s].suc\n", __FUNCTION__);
    return JNI_TRUE;
}
jint JNI_OnLoad(JavaVM *vm, void * /*reserved*/)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv *env = NULL;

    lidbg(DEBG_TAG"[%s].\n", __FUNCTION__);

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK)
    {
        lidbg(DEBG_TAG"ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE)
    {
        lidbg(DEBG_TAG"ERROR: registerNatives failed");
        goto bail;
    }
    result = JNI_VERSION_1_4;
bail:
    return result;
}

