


function build_system()
{
	cd $DBG_SYSTEM_DIR
	source build/envsetup.sh
	cd - 
}

function build_kernel()
{
	cd $DBG_SYSTEM_DIR
	source build/envsetup.sh
	cd - 
}

function make()
{
	cd $DBG_SYSTEM_DIR
	source build/envsetup.sh
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
		*)
			echo
		esac
}


