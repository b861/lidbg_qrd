
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
	if [ $WORK_REMOTE = 1 ];then
		rm -rf $WORK_LOCAL_PATH/out
		mkdir $WORK_LOCAL_PATH/out
		scp $WORK_REMOTE_USERNAME@192.168.9.57:$DBG_OUT_PATH $WORK_LOCAL_PATH/out
	fi
	cd  $DBG_TOOLS_PATH && ./pushfly.sh
}

function lidbg_push_out()
{
	echo $FUNCNAME
	echo push到原生系统
	if [ $WORK_REMOTE = 1 ];then
		rm -rf $WORK_LOCAL_PATH/out
		mkdir $WORK_LOCAL_PATH/out
		scp $WORK_REMOTE_USERNAME@192.168.9.57:$DBG_OUT_PATH $WORK_LOCAL_PATH/out
	fi
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
	#git gc
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
	if [ $DBG_VENDOR = VENDOR_QCOM ];then
	echo
	bp_combine_menu
	fi
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
	chmod 777 $DBG_ROOT_PATH -R
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
	   case "$DBG_VENDOR" in
		VENDOR_QCOM)
			bp_combination_handle $1;;
	   esac
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
		if [[ $2 -eq "-1" ]];then
			exit 1
		fi
	while :;do
		chmod 777 $DBG_ROOT_PATH -R
		cd $DBG_BUILD_PATH
		lidbg_menu
		read -p "[USERID:$USERS_ID  PLATFORMID:$DBG_PLATFORM_ID]Enter your select:" name1 name2 name3 name4 name5
	       	menu_do $name1 $name2 $name3 $name4
		menu_do $name2 $name3 $name4 $name5
		menu_do $name3 $name4 $name5
		menu_do $name4 $name5
		menu_do $name5
	done
}





# apt-get install expect
cd build
source env_entry.sh

git config gc.auto 0

. $DBG_TOOLS_PATH/soc_$DBG_SOC.sh
. $DBG_TOOLS_PATH/depository.sh
. $DBG_TOOLS_PATH/debug.sh
. $DBG_TOOLS_PATH/combination.sh
. $DBG_TOOLS_PATH/common.sh
. $DBG_TOOLS_PATH/branch_for_test.sh
. $DBG_TOOLS_PATH/bp_combination.sh
. $DBG_TOOLS_PATH/creat_efs.sh
auto_build $1 $2 $3 $4 $5;

