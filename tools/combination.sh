
function combination_menu()
{
	cd $DBG_ROOT_PATH
	echo combinations
	echo "[61] release lidbg           (1  4  44 41 45)"
	echo "[62] release basesystem      (23 44 42 45)"
	echo "[65] debug kernel            (21 53 54 54)"
}

function combination_handle()
{
	cd $RELEASE_REPOSITORY
	case $1 in
	61)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		lidbg_clean & lidbg_build_all & depository_pull & depository_copy_lidbg & depository_add_push $descriptors;;
	62)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		soc_build_all & depository_pull & depository_copy_basesystem & depository_add_push $descriptors;;
		
	65)
		soc_build_kernel & adb wait-for-devices reboot bootloader &  soc_flash_kernel & fastboot reboot;;

	*)
		echo
	esac
}

