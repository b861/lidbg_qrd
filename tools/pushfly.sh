
source ../build/env_entry.sh
expect $DBG_TOOLS_PATH/root
adb  wait-for-device remount
adb shell rm /flysystem/lib/out/*.ko
adb shell sync
adb push $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out /flysystem/lib/out
adb shell chmod 777 /flysystem/lib/out/*.ko
adb shell rm -r /system/lib/modules/out
adb shell sync
adb reboot
