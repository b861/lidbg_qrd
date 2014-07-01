
function soc_build_release()
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
	cp $RELEASE_REPOSITORY/driver/out/lidbg_load $DBG_SYSTEM_DIR/out/target/product/msm8226/system/bin/
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
