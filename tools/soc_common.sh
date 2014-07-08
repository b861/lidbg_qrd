function soc_flash_kernel()
{
	echo $FUNCNAME
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&& sudo fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/boot.2knand.img
	else
		echo $DBG_PLATFORM&& sudo fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img
	fi
}

function soc_flash_system()
{
	echo $FUNCNAME
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&& sudo fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/system.2knand.img
	else
		echo $DBG_PLATFORM&& sudo fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img
	fi

}



function soc_menu()
{
	echo $DBG_SYSTEM_DIR
	echo [21] make bootimage
	echo [22] make system
	echo [23] make
	echo [24] make otapackage
	echo [25] make release system
}

function soc_handle()
{
	cd $DBG_SYSTEM_DIR
	case $1 in
	21)	
		soc_build_kernel;;
	22)	
		soc_build_system;;
	23)
		soc_build_all;;
	24)
		soc_make_otapackage;;
	25)
		soc_build_release;;
	*)
		echo
	esac
}


