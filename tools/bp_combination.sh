
#cd $BP_SOURCE_PATH
#build the modem should source ./setenv-modem.sh
#other is source ./setenv.sh

function build_mpss()
{
    echo $FUNCNAME
    cd $MPSS_BUILD_PATH && $MPSS_BUILD_CMD
}

function build_bootloader()
{
    echo $FUNCNAME
    cd $BOOT_IMAGE_PATH && $BOOTLOADER_BUILD_CMD
}

function build_adsp()
{
    echo $FUNCNAME
    cd $ADSP_PATH && $ADSP_BUILD_CMD
}

function build_debug_image()
{
     echo $FUNCNAME
     cd $DEBUG_IMAGE_PATH && $DEBUG_IMAGE_CMD
}
function build_rpm()
{
    echo $FUNCNAME
    cd $BP_SOURCE_PATH && $RPM_BUILD_CMD
}

function build_wcnss()
{
    echo $FUNCNAME
    cd $WCNSS_PATH && $WCNSS_BUILD_CMD
}

function build_trustzone_image()
{
    echo $FUNCNAME
    cd $TZ_PATH && $TZ_BUILD_CMD
}

function build_ln_get_image()
{
    echo $FUNCNAME
    cd $LINUX_android_PATH && ln -s $DBG_SYSTEM_DIR/out out
#  cp qcn
}


function build_update()
{
    echo $FUNCNAME
    cd $UPDATE_INFO && $UPDATE_BUILD_CMD
}

#生成工厂镜像
function factory_bin()
{
	echo $FUNCNAME
}
#用fastboot烧写bp的镜像
function flash_bin()
{
	echo $FUNCNAME
}

#清理bp镜像
function clean()
{
	echo $FUNCNAME
}


function bp_combination_handle()
{
    echo $FUNCNAME
    cd $BP_SOURCE_PATH
    case $1 in
    71)
 #   	case "$DBG_PLATFORM_ID" in
 #	4)
#		source setenv-modem.sh;;
	
		source setenv.sh
#	esac
        build_mpss;;
    72)
	source setenv.sh
        build_bootloader;;
    73)
	source setenv.sh
        build_adsp;;
    74)
	source setenv.sh
        build_rpm;;
    75)
	source setenv.sh
	case "$DBG_PLATFORM" in
	msm8226)
	build_debug_image;;
	msm8974)
        build_wcnss;;
	esac;;
    76)
	source setenv.sh
        build_trustzone_image;;
    77) 
	build_ln_get_image;;
	
    78)
	source setenv.sh
	build_update;;
    79)
	source setenv.sh
	build_all_handle;;
    *)
        echo
    esac
}

function build_all_handle()
{
    echo $FUNCNAME
    case "$DBG_PLATFORM" in
    msm8226)
    echo "进入编译8226"
   # insmod xx && 
    cd $BP_SOURCE_PATH && source setenv.sh && build_mpss && build_bootloader && build_adsp && build_rpm && build_trustzone_image && build_debug_image && build_ln_get_image && build_update;;
    ndefine)
    echo "进入编译8926"
   # insmod xx && 
    cd $BP_SOURCE_PATH && source setenv.sh && build_mpss && build_bootloader && build_adsp && build_rpm && build_trustzone_image && build_debug_image && build_ln_get_image && build_update;;
    msm8974)
    cd $BP_SOURCE_PATH && source $BP_SOURCE_PATH/setenv-modem.sh && build_mpss && source $BP_SOURCE_PATH/setenv.sh && build_bootloader && build_adsp && build_rpm && build_wcnss && build_trustzone_image && build_ln_get_image && build_update;;
    esac

#    cp all image to ./out
}


