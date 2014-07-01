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
