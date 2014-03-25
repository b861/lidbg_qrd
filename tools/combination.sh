
function combination_menu()
{
	cd $DBG_ROOT_PATH
	echo 组合选项:
	echo "[61] 完整编译lidbg并提交到产品二进制仓库           (1  3  81 41 82)"
	echo "[62] 编译basesystem并提交到产品二进制仓库          (23 81 42 82)"
	echo "[63] 拷贝lidbg和basesystem到二进制仓库后生成升级包 (81 41 42 43)"
	echo "[65] 编译烧写内核后重启                            (21 53 54 56)"
}

function combination_handle()
{
	cd $RELEASE_REPOSITORY
	case $1 in
	61)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		lidbg_clean && lidbg_build_all && depository_pull && depository_copy_lidbg && depository_add_push $descriptors;;
	62)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		soc_build_all && depository_pull && depository_copy_basesystem && depository_add_push $descriptors;;
	63)
	        depository_pull && depository_copy_lidbg && depository_copy_basesystem && depository_make_package;;

	65)
		soc_build_kernel && adb wait-for-devices reboot bootloader &&  soc_flash_kernel & fastboot reboot;;

	*)
		echo
	esac
}

