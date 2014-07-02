function soc_prebuild()
{
	echo $FUNCNAME
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
}


function soc_build_release()
{
	echo $FUNCNAME
	soc_build_all 
}


function soc_make_otapackage()
{
	echo $FUNCNAME
}

. $DBG_TOOLS_PATH/soc_common.sh
