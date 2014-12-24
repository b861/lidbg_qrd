
function combination_menu()
{
	cd $DBG_ROOT_PATH
	echo 组合选项:
	echo "[61] 完整编译lidbg并提交到产品二进制仓库           (1  3  81(1) 41 82(3))"
	echo "[62] 编译basesystem并提交到产品二进制仓库          (81(2) 25 81(3) 42 82(3))"
	echo "[63] 拷贝lidbg和basesystem到二进制仓库后生成升级包 (81(3) 41 42 43)"
	echo "[65] 编译烧写boot.img后重启                        (21 53 54 56)"
    echo "[66] 编译拷贝lidbg或basesystem并生成升级包提交到测试分支"
}

function combination_handle()
{
	case $1 in
	61)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		lidbg_build_all && depository_clean && depository_pull && rm -rf $UPDATA_BIN_DIR/out && rm -rf $UPDATA_BIN_DIR/hw/* && depository_copy_lidbg  && depository_add_push "$descriptors";;
	62)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		cd $DBG_SYSTEM_DIR && expect $DBG_TOOLS_PATH/pull master $DBG_PASSWORD  && depository_clean && depository_pull && soc_build_release && depository_pull && depository_copy_basesystem && depository_add_push "$descriptors";;
	63)
	    depository_pull && rm -rf $UPDATA_BIN_DIR/out && rm -rf $UPDATA_BIN_DIR/hw/* && depository_copy_lidbg && depository_copy_basesystem && depository_make_package && gitk&;;
	65)
		soc_build_kernel && adb wait-for-devices reboot bootloader && soc_flash_kernel && sudo fastboot reboot;;
    66)
        echo "  编译选项如下"
        echo "  (1) 编译basesystem并拷贝到二进制仓库"
        echo "  (2) 编译lidbg并拷贝到二进制仓库"
        echo "  (3) 编译basesystem和lidbg并拷贝到二进制仓库"

        read -p "  输入编译选项：" which_combine
        read -p "  自定义提交到二进制仓库测试分支的名字：" branch_name
        read -p "  提交说明：" commit
        judge_which_combine && depository_make_package && new_remote_test_branch && new_branch_remove;; 
        #for test
        #judge_which_combine && new_remote_test_branch && new_branch_remove;; 
	*)
		echo
	esac
}

