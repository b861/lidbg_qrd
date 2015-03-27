
function lidbg_clean()
{
	echo $FUNCNAME
	echo 清除生成文件
	cd $DBG_BUILD_PATH && ./clean.sh
}

function lidbg_build()
{
	echo $FUNCNAME
	echo 编译模块
	cd $DBG_BUILD_PATH && ./build.sh
}

function lidbg_pushfly_out()
{
	echo $FUNCNAME
	echo push驱动模块到产品系统
	cd  $DBG_TOOLS_PATH && ./pushfly.sh
}

function lidbg_push_out()
{
	echo $FUNCNAME
	echo push到原生系统
	cd  $DBG_TOOLS_PATH && ./push.sh
}

function lidbg_build_all()
{
	echo $FUNCNAME
	cd $DBG_BUILD_PATH
	./build_cfg.sh $DBG_SOC $BUILD_VERSION $DBG_PLATFORM
	cd $DBG_BUILD_PATH && ./clean.sh
	cd $DBG_HAL_PATH   && ./build_all.sh
	cd $DBG_BUILD_PATH && ./build.sh
}


function lidbg_pull()
{
	echo $FUNCNAME
	echo git pull
	expect $DBG_TOOLS_PATH/pull_lidbg
	chmod 777 $DBG_ROOT_PATH -R
	git config core.filemode false
	git gc
}

function lidbg_push()
{
	echo $FUNCNAME
	echo push lidbg_qrd到服务器
	expect $DBG_TOOLS_PATH/push_lidbg
}

function lidbg_disable()
{
	adb wait-for-devices remount && adb shell rm /system/lib/modules/out/lidbg_loader.ko && adb shell rm /flysystem/lib/out/lidbg_loader.ko
}

function lidbg_menu()
{
	echo $DBG_ROOT_PATH
	echo [1] clean'                        '清除生成文件	
	echo [2] build'                        '编译模块
	echo [3] build all'                    '编译lidbg所有文件
	echo [4] push out'                     'push驱动模块到原生系统
	echo [5] push out to fly'              'push驱动模块到产品系统
	echo [6] del lidbg loader'             '删除lidbg_loader.ko驱动
	echo [7] open dbg_cfg.sh

	echo
	soc_menu
	echo
	depository_menu
	echo
	debug_menu
	echo
	combination_menu
	echo
	bp_combine_menu
	echo
	common_menu
}

function lidbg_handle()
{
		cd $DBG_ROOT_PATH
		case $1 in
		1)	
			lidbg_clean;;
		2)
			lidbg_build;;
		3)
			lidbg_build_all;;	
		4)
			lidbg_push_out;;
		5)
			lidbg_pushfly_out;;
		6)
			lidbg_disable;;
		7)
			gedit $DBG_ROOT_PATH/dbg_cfg.sh &;;
		*)
			echo
		esac
}


function menu_do()
{
	if [[ $1 -le 20 ]] ;then
		lidbg_handle $1
	elif [[ $1 -le 40 ]] ;then
		soc_handle $1 $2 $3 $4
	elif [[ $1 -le 50 ]] ;then
		depository_handle $1
	elif [[ $1 -le 60 ]] ;then
		debug_handle $1
	elif [[ $1 -le 70 ]] ;then
		combination_handle $1
	elif [[ $1 -le 80 ]] ;then
		bp_combination_handle $1
	else
		common_handle $1
	fi
}

function auto_build()
{
	       	menu_do $1 $2 $3 $4
		menu_do $2 $3 $4 $5
		menu_do $3 $4 $5
		menu_do $4 $5
		menu_do $5
	while :;do
		cd $DBG_BUILD_PATH
		lidbg_menu
		read -p "[USERID:$USERS_ID  PLATFORMID:$DBG_PLATFORM_ID ]Enter your select:" name1 name2 name3 name4 name5
	       	menu_do $name1 $name2 $name3 $name4
		menu_do $name2 $name3 $name4 $name5
		menu_do $name3 $name4 $name5
		menu_do $name4 $name5
		menu_do $name5
	done
}

function bp_combine_menu()
{
    echo $BP_SOURCE_PATH
    echo [71] build MPSS'                         'NON-HLOS.bin_1
    echo [72] build Bootloader'                   'sbl1.mbn
    echo [73] build ADSP'                         'NON-HLOS.bin_2
    echo [74] build RPM'                          'rpm.mbn
    echo [75] build WCNSS'                        'wcnss.mbn
    echo [76] build TZ'                           'tz.mbn
    echo [77] update bp info'                     '汇总编译NON-HLOS.bin
    echo [78] build ALL'                          '编译全部
}

# 分离
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
        build_wcnss;;
    76)
	source setenv.sh
        build_trustzone_image;;
    77)
	source setenv.sh
	copy_android_image
	build_update;;
    78)
	source setenv.sh
	build_all_handle;;
    *)
        echo
    esac
}

#move
function build_all_handle()
{
    echo $FUNCNAME
    case "$DBG_PLATFORM_ID" in
    2)
    echo "进入编译8226"
   # insmod xx && 
    cd $BP_SOURCE_PATH && source setenv.sh && build_mpss && build_bootloader && build_adsp && build_rpm && build_trustzone_image && build_debug_image && copy_android_image && build_update;;
    3)
    echo "进入编译8926"
   # insmod xx && 
    cd $BP_SOURCE_PATH && source setenv.sh && build_mpss && build_bootloader && build_adsp && build_rpm && build_trustzone_image && build_debug_image && copy_android_image && build_update;;
    4)
    cd $BP_SOURCE_PATH && source $BP_SOURCE_PATH/setenv-modem.sh && build_mpss && source $BP_SOURCE_PATH/setenv.sh && build_bootloader && build_adsp && build_rpm && build_wcnss && build_trustzone_image && copy_android_image && build_update;;
    esac

#    cp all image to ./out
}


# apt-get install expect
cd build
source ./env_entry.sh
. $DBG_TOOLS_PATH/soc_$DBG_SOC.sh
. $DBG_TOOLS_PATH/depository.sh
. $DBG_TOOLS_PATH/debug.sh
. $DBG_TOOLS_PATH/combination.sh
. $DBG_TOOLS_PATH/common.sh
. $DBG_TOOLS_PATH/branch_for_test.sh
. $DBG_TOOLS_PATH/bp_combination.sh
auto_build $1 $2 $3 $4 $5;

