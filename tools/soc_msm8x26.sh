
function soc_build_system()
{
	echo $FUNCNAME
	soc_prebuild && make systemimage -j8 && soc_postbuild
}

function soc_build_kernel()
{
	echo $FUNCNAME
	soc_prebuild && soc_build_commen 'make bootimage -j8'
}

function soc_build_common()
{
	echo $FUNCNAME $1 $2 $3
	soc_prebuild && $1 $2 $3
}

function soc_build_all()
{
	echo $FUNCNAME
	soc_prebuild && make -j8 && soc_postbuild
}


function soc_postbuild()
{
if [ $DBG_PLATFORM = msm8226 ];then
#	if [ -s $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/lidbg_load ]; then
#		echo "/system/bin/lidbg_load exist"
#	else
#		soc_prebuild && make -j8
#	fi

	if [ -s $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/libdiskconfig.so ]; then
		echo "system/lib/libdiskconfig.so exist"
	else
		mmm $DBG_SYSTEM_DIR/system/core/libdiskconfig -B
		soc_prebuild && make -j8
	fi
	echo "soc_build_all ok"
fi
}

function soc_prebuild()
{
	echo $FUNCNAME
	echo $DBG_PLATFORM
	cd $DBG_SYSTEM_DIR

if [ $DBG_PLATFORM = msm8226 ];then
	rm -rf $DBG_SYSTEM_DIR/kernel/drivers/flyaudio
	mkdir -p $DBG_SYSTEM_DIR/kernel/drivers/flyaudio
	cp -ru $DBG_DRIVERS_PATH/build_in/*	        $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_DRIVERS_PATH/inc/lidbg_interface.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/cmn_func.c   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/inc/cmn_func.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/inc/lidbg_def.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/

#	rm -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/modules/out
#	cp -r $RELEASE_REPOSITORY/driver/out $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/modules/
#	cp $RELEASE_REPOSITORY/driver/out/vold $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/
#	cp $RELEASE_REPOSITORY/app/FastBoot.apk $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/
#	cp $RELEASE_REPOSITORY/app/FlyBootService.apk $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/
#	cp $RELEASE_REPOSITORY/driver/out/lidbg_load $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/
fi

	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
}


function soc_build_release()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	expect $DBG_TOOLS_PATH/pull $DBG_REPO_PASSWORD
	cd $DBG_SYSTEM_DIR
	expect $DBG_TOOLS_PATH/pull $DBG_PASSWORD

	soc_build_all
	soc_make_otapackage
}

function soc_make_otapackage()
{
	echo $FUNCNAME
# cp lk,bp to /device/qcom/msm8226/radio
	#cp -u $RELEASE_REPOSITORY/lk/emmc_appsboot.mbn  $DBG_SYSTEM_DIR/device/qcom/msm8226/radio/
	#cp -u $RELEASE_REPOSITORY/radio/* 	        $DBG_SYSTEM_DIR/device/qcom/msm8226/radio/
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		soc_prebuild && make otapackage -j8
}
. $DBG_TOOLS_PATH/soc_common.sh
