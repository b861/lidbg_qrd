
source ../build/env_entry.sh
expect $DBG_TOOLS_PATH/remount
adb  wait-for-device root
adb  wait-for-device remount
adb shell sync
adb shell rm -rf /data/lidbg
adb push $DBG_OUT_PATH /flysystem/lib/out
adb push $DBG_OUT_PATH/vold /system/bin/
adb push $DBG_OUT_PATH/lidbg_load /system/bin/
adb shell chmod 777 /system/bin/vold
adb shell chmod 777 /system/bin/lidbg_load
. $DBG_TOOLS_PATH/pushfly_$DBG_SOC.sh
adb shell chmod 777 /flysystem/lib/out/*.ko
adb shell sync
adb reboot
