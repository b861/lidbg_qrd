
function soc_prebuild()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR

	rm -rf $DBG_SYSTEM_DIR/kernel/drivers/flyaudio
	mkdir -p $DBG_SYSTEM_DIR/kernel/drivers/flyaudio
	cp -ru $DBG_DRIVERS_PATH/build_in/*	        $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_DRIVERS_PATH/inc/lidbg_interface.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/cmn_func.c   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/inc/cmn_func.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/inc/lidbg_def.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/

	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
}


function soc_build_system()
{
	echo $FUNCNAME
	soc_prebuild && make systemimage -j8
}

function soc_build_kernel()
{
	echo $FUNCNAME
	soc_prebuild && make bootimage -j8
}

function soc_build_all()
{
	echo $FUNCNAME
	soc_prebuild && make -j8
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


