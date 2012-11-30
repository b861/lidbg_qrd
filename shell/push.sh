#=======================================================================================
#	FileName    : 
#	Description : adb push
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh

$DBG_TOOLS_PATH/adb push $DBG_OUT_PATH /system/lib/modules/out
$DBG_TOOLS_PATH/adb push $DBG_ROOT_PATH/tmp/cap_ts.ko  /system/lib/modules/out
echo "reboot..."
$DBG_TOOLS_PATH/adb shell reboot
