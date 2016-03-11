
#include "lidbg_servicer.h"

#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <binder/IInterface.h>
#include <media/AudioSystem.h>
#include <media/IAudioFlinger.h>
#include <media/IAudioPolicyService.h>
#include <math.h>
#include <system/audio.h>

#define TAG "android_server:"
using namespace android;

bool dbg = false;
bool playing_old = false;
int loop_count = 0;

static sp<IAudioPolicyService> gAudioPolicyService = 0;

void GetAudioPolicyService(bool dbg)
{
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder;
    if(dbg)
        lidbg( TAG"GetAudioPolicyService.in\n");

    do
    {
        binder = sm->getService(String16("media.audio_policy"));
        if (binder != 0)
            break;
        lidbg(TAG" waiting...\n");
        usleep(1000000);
    }
    while (true);
    gAudioPolicyService = interface_cast<IAudioPolicyService>(binder);
    if(gAudioPolicyService == 0)
        lidbg(TAG "GetAudioPolicyService.fail1\n");
    else if(dbg)
        lidbg( TAG"GetAudioPolicyService.succes1\n");
}

bool playing = false;
static void *thread_check_boot_complete(void *data)
{
	while(1)
	{
	        char value[PROPERTY_VALUE_MAX];
	        property_get("sys.boot_completed", value, "0");
	        if (value[0] == '1')
	        {
	        	lidbg( TAG" send message  :boot_completed = %c,delay 2S \n",value[0]);
		sleep(2);
	        	LIDBG_WRITE("/dev/lidbg_interface", "BOOT_COMPLETED");
		break;
	        }
		sleep(1);
	}
    return ((void *) 0);
}
int main(int argc, char **argv)
{
    pthread_t ntid;
    argc = argc;
    argv = argv;
    lidbg( TAG"lidbg_android_server:main\n");

    sleep(20);
    GetAudioPolicyService(true);
    pthread_create(&ntid,NULL,thread_check_boot_complete,NULL);
    while(1)
    {
        if(gAudioPolicyService != 0)
        {
            sp<IAudioPolicyService> &aps = gAudioPolicyService;
            playing = aps->isStreamActive((audio_stream_type_t)3, 0) |
                      aps->isStreamActive((audio_stream_type_t)0, 0) |
                      aps->isStreamActive((audio_stream_type_t)2, 0) |
                      aps->isStreamActive((audio_stream_type_t)1, 0) |
                      aps->isStreamActive((audio_stream_type_t)5, 0);
            if(dbg)
                lidbg(TAG"playing=%d\n", playing);
        }
        else
        {
            lidbg( TAG"gAudioPolicyService == 0\n");
            GetAudioPolicyService(true);
        }

        if(playing != playing_old)
        {
            char cmd[16];
            playing_old = playing;

            sprintf(cmd, "sound %d", playing);
            lidbg(TAG"write.[%d,%s]\n", playing, cmd);
            LIDBG_WRITE("/dev/fly_sound0", cmd);

        }
        loop_count++;
        if(loop_count > 200)
        {
            char value[PROPERTY_VALUE_MAX];
            GetAudioPolicyService(false);
            property_get("persist.lidbg.sound.dbg", value, "0");
            if (value[0] == '1')
                dbg = 1;
            else
                dbg = 0;
            loop_count = 0;
        }

        usleep(100000);//100ms
    }

}
