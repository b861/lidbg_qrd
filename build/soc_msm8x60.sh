


function build_system()
{
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh
		breakfast su640
	fi
		brunch su640
	cd - 
}

function soc_handle_func()
{
		case $1 in
		21)	
			build_system;;
		22)	
			adb push $DBG_OUT_PATH /sdcard/out;;
		23)
			insmod /sdcard/out/lidbg_loader.ko;;
		24)
			cd $DBG_KERNEL_SRC_DIR && gitk& ;;
		*)
			echo
		esac
}


