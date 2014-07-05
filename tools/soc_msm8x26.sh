
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


	cp -r $RELEASE_REPOSITORY/driver/out $DBG_SYSTEM_DIR/out/target/product/msm8226/system/lib/modules/
	cp $RELEASE_REPOSITORY/driver/out/vold $DBG_SYSTEM_DIR/out/target/product/msm8226/system/bin/
	cp $RELEASE_REPOSITORY/app/FastBoot.apk $DBG_SYSTEM_DIR/out/target/product/msm8226/system/app/
	cp $RELEASE_REPOSITORY/app/FlyBootService.apk $DBG_SYSTEM_DIR/out/target/product/msm8226/system/app/
	cp $RELEASE_REPOSITORY/driver/out/lidbg_load $DBG_SYSTEM_DIR/out/target/product/msm8226/system/bin/


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
}

function soc_make_otapackage()
{
	echo $FUNCNAME
# cp lk,bp to /device/qcom/msm8226/radio
	cp -u $RELEASE_REPOSITORY/lk/emmc_appsboot.mbn  $DBG_SYSTEM_DIR/device/qcom/msm8226/radio/
	cp -u $RELEASE_REPOSITORY/radio/* 	        $DBG_SYSTEM_DIR/device/qcom/msm8226/radio/
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make otapackage -j8
}
. $DBG_TOOLS_PATH/soc_common.sh
