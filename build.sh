#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
cd shell
rm version.sh
cp debug.sh version.sh
./all_make.sh
#adb wait-for-device remount
adb remount
adb push ./out /system/lib/modules/out
#adb reboot

