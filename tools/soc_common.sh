function soc_flash_kernel()
{
	echo $FUNCNAME
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&& sudo fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/boot.2knand.img
	elif [ $DBG_VENDOR = VENDOR_ROCKCHIP ];then
		echo $DBG_PLATFORM && sudo $DBG_SYSTEM_DIR/RKTools/linux/Linux_Upgrade_Tool_v1.16/upgrade_tool di boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img
	else
		echo $DBG_PLATFORM&& sudo fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img
	fi
}

function soc_flash_system()
{
	echo $FUNCNAME
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&& sudo fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/system.2knand.img
	elif [ $DBG_VENDOR = VENDOR_ROCKCHIP ];then
		echo $DBG_PLATFORM && sudo $DBG_SYSTEM_DIR/RKTools/linux/Linux_Upgrade_Tool_v1.16/upgrade_tool di system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img
	else
		echo $DBG_PLATFORM&& sudo fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img
	fi

}

function soc_flash_recovery()
{
	echo $FUNCNAME
	if [ $DBG_VENDOR = VENDOR_ROCKCHIP ];then
		echo $DBG_PLATFORM && sudo $DBG_SYSTEM_DIR/RKTools/linux/Linux_Upgrade_Tool_v1.16/upgrade_tool di recovery $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/recovery.img
	else
		echo $DBG_PLATFORM&& sudo fastboot flash recovery $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/recovery.img
	fi
}

function soc_flash_bootloader()
{
	echo $FUNCNAME
	echo $DBG_PLATFORM&& sudo fastboot flash aboot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/emmc_appsboot.mbn

}

function soc_menu()
{
	echo $DBG_SYSTEM_DIR
	echo [21] make bootimage
	echo [22] make system
	echo [23] make
	echo [24] make otapackage
	echo [25] make release basesystem
	echo [26] make origin system image
	echo [27] make recovery
	echo [28] make recoveryimage
	echo [29] make bootloader
	echo [30] common
	echo [31] make git commit image
}

function soc_build_git_commit_image()
{
	echo $DBG_SYSTEM_DIR
	cd $DBG_SYSTEM_DIR

}

function soc_handle()
{
	echo $DBG_SYSTEM_DIR $1 $2 $3 $4
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
	26)
		soc_build_origin_image;;
	27)
		soc_build_recovery;;
	28)
		soc_build_recoveryimage;;
	29)
		soc_build_bootloader;;
	30)
		soc_build_common "$2 $3 $4";;
	31)
		soc_build_git_commit_image;;
	*)
		echo
	esac
}


