function build_android()
{
	echo 编译android
	read -p "输入提交到二进制仓库的说明文字：" descriptors
	expect $DBG_TOOLS_PATH/pull
	if [[ $TARGET_PRODUCT = "" ]];then		
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make -j8
		. $DBG_TOOLS_PATH/basesystem_release.sh $descriptors
}


function build_system()
{
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make systemimage -j4
}

function build_kernel()
{
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make bootimage -j4
}

function build_all()
{
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
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

function make_package()
{
	echo make_package
	cd $UPDATA_BASESYSTEM_DIR/..
	git reset --hard
	expect $DBG_TOOLS_PATH/pull
	cp -r $DBG_OUT_PATH  $UPDATA_BIN_DIR && echo "copy lidbg"
	cp -r $DBG_SYSTEM_DIR/flyaudio/out/*  $UPDATA_BASESYSTEM_DIR && echo "copy basesystem"
	pwd && expect $DBG_TOOLS_PATH/make_package
}


function soc_menu_func()
{
	echo [21] make bootimage
	echo [22] make system
	echo [23] make
	echo [24] git pull
	echo [25] git push
	echo [26] gitk
	echo [27] git reset
	echo [30] adb reboot bootloader
	echo [31] fastboot flash boot
	echo [32] fastboot flash system
	echo [33] fastboot reboot
	echo [35] nautilus
	echo "[40] make kernel & reboot & flash"
	echo [41] build basesystem and push
	echo [42] 使用最新的ko和basesystem生成升级包
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
		git reset --hard;;
	30)
		adb reboot bootloader;;
	31)
		flash_kernel;;
	32)
		flash_system;;
	33)
		fastboot reboot;;
	35)
		nautilus $DBG_SYSTEM_DIR;;
	40)
		build_kernel && adb wait-for-devices reboot bootloader &&  flash_kernel && fastboot reboot;;
	41)
		build_android;;
	42)
		make_package;;
	*)
		echo
	esac
}


