

function soc_build_system()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make systemimage -j8
}

function soc_build_kernel()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make bootimage -j8
}

function soc_build_all()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make -j8
}

function soc_build_origin()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	expect $DBG_TOOLS_PATH/pull
	cd $DBG_SYSTEM_DIR
	expect $DBG_TOOLS_PATH/pull
	cp -r $RELEASE_REPOSITORY/driver/out $DBG_SYSTEM_DIR/out/target/product/msm8226/system/lib/modules/
	cp $RELEASE_REPOSITORY/driver/out/vold $DBG_SYSTEM_DIR/out/target/product/msm8226/system/bin/
	cp $RELEASE_REPOSITORY/app/FastBoot.apk $DBG_SYSTEM_DIR/out/target/product/msm8226/system/app/
	cp $RELEASE_REPOSITORY/app/FlyBootService.apk $DBG_SYSTEM_DIR/out/target/product/msm8226/system/app/
	cp $RELEASE_REPOSITORY/driver/out/lidbg_iserver $DBG_SYSTEM_DIR/out/target/product/msm8226/system/bin/
	soc_build_all
	soc_make_otapackage 
}


function soc_flash_kernel()
{
	echo $FUNCNAME
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&&fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/boot.2knand.img
	else
		echo $DBG_PLATFORM&&fastboot flash boot $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img
	fi
}

function soc_flash_system()
{
	echo $FUNCNAME
	if [ $DBG_PLATFORM = msm7627a ];then
		echo $DBG_PLATFORM&&fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/2kpagenand_images/system.2knand.img
	else
		echo $DBG_PLATFORM&&fastboot flash system $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img
	fi

}



function soc_menu()
{
	echo $DBG_SYSTEM_DIR
	echo [21] make bootimage
	echo [22] make system
	echo [23] make
	echo [24] make otapackage
	echo [25] make origin_system
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
		soc_build_origin;;
	*)
		echo
	esac
}


