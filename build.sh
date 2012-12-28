#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
cd shell
rm version.sh
cp debug.sh version.sh
cd ..
selfpath=$(cd "$(dirname "$0")"; pwd) 
if [ $1 == "dbin" ];then #下载tcc8803的out/.../bin到msm 
	echo 下载bin到手机（等待手机中）。。。
	adb wait-for-devices remount
	adb push $selfpath/bin/grep /system/bin
adb push $selfpath/bin/cat /system/bin
adb push $selfpath/bin/cp /system/bin
adb push $selfpath/bin/date /system/bin
else
	
	if [ $1 == "d" ];then
		echo -e "\033[44;33m Down paragram! \033[0m"
		echo 等待设备。。。。
		adb wait-for-devices 
		adb remount
		adb wait-for-devices devices
		echo "这在下载$selfpath/out 到 /system/lib/modules/out"
		adb push $selfpath/out /system/lib/modules/out
		echo 下载lidbg_server
		echo 在/home/keen/451/R8625SSNSKQLYA10145451/out/target/product/msm7627a/symbols/system/bin/lidbg_servicer
		adb push /home/keen/451/R8625SSNSKQLYA10145451/out/target/product/msm7627a/symbols/system/bin/lidbg_servicer system/bin
		cd $selfpath/shell
		chmod 777 push-kejing.sh
		./push-kejing.sh
		cd .. 
		#echo 复位手机中。。。。
		#adb reboot 
		echo 等待设备。。。。
		adb wait-for-devices
		adb remount
		echo 修改控制脚本权限
		adb shell chmod 777 /system/lib/modules/key_hkjing.sh
	elif [ $1 == "dr" ];then
		echo -e "\033[44;33m Down paragram! \033[0m"
		echo 等待设备。。。。
		adb wait-for-devices 
		adb remount
		adb wait-for-devices devices
		echo "这在下载$selfpath/out 到 /system/lib/modules/out"
		adb push $selfpath/out /system/lib/modules/out
		echo 下载lidbg_server
		echo 在/home/keen/451/R8625SSNSKQLYA10145451/out/target/product/msm7627a/symbols/system/bin/lidbg_servicer
		adb push /home/keen/451/R8625SSNSKQLYA10145451/out/target/product/msm7627a/symbols/system/bin/lidbg_servicer system/bin
		cd $selfpath/shell
		chmod 777 push-kejing.sh
		./push-kejing.sh
		cd .. 
		echo 复位手机中。。。。
		adb reboot 
		echo 等待设备。。。。
		adb wait-for-devices
		adb remount
		echo 修改控制脚本权限
		adb shell chmod 777 /system/lib/modules/key_hkjing.sh
	elif [ $1 == "sh" ];then
		cd $selfpath/shell
		chmod 777 push-kejing.sh
		./push-kejing.sh
		cd .. 
		echo 等待设备。。。。
		adb wait-for-devices
		adb remount
		echo 修改控制脚本权限
		adb shell chmod 777 /system/lib/modules/key_hkjing.sh
		adb shell 
	else
		cd shell
		./all_make.sh
		#adb wait-for-device remount
	fi
fi

#git push -u origin master
#adb wait-for-device remount
#adb remount
#adb push /home/swlee/work/flyaudio/lidbg_qrd/out /system/lib/modules/out

