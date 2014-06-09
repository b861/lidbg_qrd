function soc_make_otapackage()
{
	echo $FUNCNAME
# cp lk,bp to /device/qcom/msm8226/radio
	cp -u $RELEASE_REPOSITORY/lk/emmc_appsboot.mbn  device/qcom/msm8226/radio/
	cp -u $RELEASE_REPOSITORY/radio/* 	       device/qcom/msm8226/radio/
	cd $DBG_SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
		make otapackage -j8
}
. $DBG_TOOLS_PATH/soc_common.sh
