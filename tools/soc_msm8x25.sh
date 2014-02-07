

function build_system()
{
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng
	fi
		make systemimage -j4
}

function build_kernel()
{
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng
	fi
		make bootimage -j4
}

function build_all()
{
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng
	fi
		make -j4
}


function flash_kernel()
{
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&&fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/boot.2knand.img
	else
		echo $DBG_PLATFORM&&fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img
	fi
}

function flash_system()
{
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&&fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/system.2knand.img
	else
		echo $DBG_PLATFORM&&fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img
	fi

}

function soc_menu_func()
{
	echo [21] make bootimage
	echo [22] make system
	echo [23] make
	echo [24] git pull
	echo [25] git push
	echo [26] gitk
	echo [27] adb reboot bootloader
	echo [28] fastboot flash boot
	echo [29] fastboot flash system
	echo [30] fastboot reboot
	echo [31] nautilus
	echo "[40] make kernel & reboot & flash"
}

function soc_handle_func()
{
	cd $DBG_SYSTEM_DIR
	case $1 in
	21)	
		build_kernel;;
	22)	
		build_system;;
	23)
		build_all;;
	24)
		expect $DBG_TOOLS_PATH/pull;;
	25)
		expect $DBG_TOOLS_PATH/push;;
	26)
		gitk &;;
	27)
		adb reboot bootloader;;
	28)
		flash_kernel;;
	29)
		flash_system;;
	30)
		fastboot reboot;;
	31)
		nautilus $DBG_SYSTEM_DIR;;
	40)
		adb reboot bootloader && build_kernel && flash_kernel && fastboot reboot;;
	*)
		echo
	esac
}


