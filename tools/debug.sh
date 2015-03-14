
function debug_menu()
{
	cd $DBG_ROOT_PATH
	echo debug:
	echo [51] kmsg
	echo [52] remount
	echo [53] adb reboot bootloader
	echo [54] fastboot flash boot
	echo [55] fastboot flash system
	echo [56] fastboot flash recovery
	echo [57] fastboot reboot
}

function debug_handle()
{
	case $1 in
	51)
		expect $DBG_TOOLS_PATH/kmsg;;
	52)
		expect $DBG_TOOLS_PATH/remount;;
	53)
		adb wait-for-device reboot-bootloader;;
	54)
		soc_flash_kernel;;
	55)
		soc_flash_system;;
	56)
		soc_flash_recovery;;
	57)
		sudo fastboot reboot;;
	*)
		echo
	esac
}

