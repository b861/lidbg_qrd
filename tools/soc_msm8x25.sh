

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
	echo [27] fastboot flash boot
	echo [28] fastboot flash system
	echo [29] nautilus
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
		flash_kernel;;
	28)
		flash_system;;
	29)
		nautilus $DBG_SYSTEM_DIR;;
	*)
		echo
	esac
}


