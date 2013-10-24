
source ./env_entry.sh
adb  wait-for-device remount
adb shell rm /system/lib/modules/out/*.ko
adb shell sync
adb push $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out /system/lib/modules/out
adb shell chmod 777 /system/lib/modules/out/*.ko
adb shell rm -r /flysystem/lib/out
adb shell sync
adb reboot
