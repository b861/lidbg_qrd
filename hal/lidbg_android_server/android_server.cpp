
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

#define TAG "sound:"
using namespace android;

bool dbg = false;
bool playing_old = false;
int loop_count = 0;

static sp<IAudioPolicyService> gAudioPolicyService;

void GetAudioPolicyService(void)
{
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder;
    lidbg( TAG"GetAudioPolicyService.in");

    do
    {
        binder = sm->getService(String16("media.audio_policy"));
        if (binder != 0)
            break;
        lidbg(TAG" waiting...");
        usleep(500000);
    }
    while (true);
    gAudioPolicyService = interface_cast<IAudioPolicyService>(binder);
    if(gAudioPolicyService == 0)
        lidbg(TAG "GetAudioPolicyService.fail");
    else
        lidbg( TAG"GetAudioPolicyService.succes");
}

int main(int argc, char **argv)
{
    bool playing = false;

    lidbg( "lidbg_android_server:main");

    GetAudioPolicyService();

    sp<IAudioPolicyService> &aps = gAudioPolicyService;
    while(1)
    {
        playing = aps->isStreamActive((audio_stream_type_t)3, 0) |
                  aps->isStreamActive((audio_stream_type_t)0, 0) |
                  aps->isStreamActive((audio_stream_type_t)1, 0) |
                  aps->isStreamActive((audio_stream_type_t)2, 0) |
                  aps->isStreamActive((audio_stream_type_t)4, 0) |
                  aps->isStreamActive((audio_stream_type_t)5, 0);
        if(dbg)
            lidbg(TAG"playing=%d\n", playing);
        if(playing != playing_old)
        {
            char cmd[16];
            playing_old = playing;

            sprintf(cmd, "sound %d", playing);
            lidbg(TAG"write.[%d,%s]\n", playing, cmd);
            LIDBG_WRITE("/dev/flydev0", cmd);

        }
        loop_count++;
        if(loop_count > 100)
        {
            char value[PROPERTY_VALUE_MAX];
            property_get("persist.lidbg.sound.dbg", value, "0");
            if (value[0] == '1')
                dbg = 1;
            else
                dbg = 0;
            loop_count = 0;
        }
        usleep(200000);
    }

}
