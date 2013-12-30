
function soc_menu_func()
{
	echo
	echo [21] make bootimage
	echo [22] make system
	echo [23] make
	echo [24] git pull
	echo [24] git push
	echo [24] gitk
	echo [24] fastboot flash boot
	echo [25] nautilus
	echo
}



function build_system()
{
	cd $DBG_SYSTEM_DIR
	#source build/envsetup.sh
	cd - 
}

function build_kernel()
{
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		cd $DBG_SYSTEM_DIR/&&source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng
	fi
		make bootimage
	cd - 
}

function make()
{
	cd $DBG_SYSTEM_DIR
	#source build/envsetup.sh
	cd - 
}

function soc_handle_func()
{
		case $1 in
		21)	
			build_kernel;;
		22)	
			adb push $DBG_OUT_PATH /sdcard/out;;
		23)
			insmod /sdcard/out/lidbg_loader.ko;;
		*)
			echo
		esac
}


