
source ../build/env_entry.sh

adb push $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out/camera.msm7627a.so /flysystem/lib/hw
adb push $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out/gps.msm8625.so /flysystem/lib/hw
