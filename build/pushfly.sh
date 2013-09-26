
source ./env_entry.sh
adb  wait-for-device remount
adb shell rm /flysystem/lib/out/*.ko
adb shell sync
adb push $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out /flysystem/lib/out
adb push $DBG_ANDROID_OUT_PATH/gps.$DBG_PLATFORM.so /flysystem/lib/hw/gps.$DBG_PLATFORM.so
adb shell chmod 777 /flysystem/lib/out/*.ko
adb shell rm -r /system/lib/modules/out
adb shell sync
adb reboot
