
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
	./build_cfg.sh $DBG_SOC $BUILD_VERSION 
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


function lidbg_menu()
{
	echo $DBG_ROOT_PATH
	echo [1] clean.sh'                        '清除生成文件	
	echo [2] build.sh'                        '编译模块
	echo [3] pushfly.sh'                      'push驱动模块到产品系统
	echo [4] build all'                       '编译lidbg所有文件
	echo [5] push.sh'                         'push驱动模块到原生系统
	echo [6] pull'                            'git pull服务器的libg_qrd
	echo [7] push'                            'git push服务器libg_qrd
	echo [8] gitk'                            '执行gitk
	echo [9] git log'                         '执行git log --oneline
	echo [10] git reset'                      '执行git reset --hard	
	echo [15] nautilus'                       '打开lidbg目录
	echo
	soc_menu
	echo
	depository_menu
	echo
	debug_menu
	echo
	combination_menu
	echo
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
			lidbg_pushfly_out;;
		4)
			lidbg_build_all;;
		5)
			lidbg_push_out;;
		6)       
		 	lidbg_pull;;
		7)	
			lidbg_push;;
		8)
			gitk &;;
		9)
			git log --oneline;;
		10)
			git reset --hard && chmod 777 * -R;;
		15)
			nautilus $DBG_ROOT_PATH;;
		*)
			echo
		esac
}


function menu_do()
{
	if [[ $1 -le 20 ]] ;then
		lidbg_handle $1
	elif [[ $1 -le 40 ]] ;then
		soc_handle $1
	elif [[ $1 -le 50 ]] ;then
		depository_handle $1
	elif [[ $1 -le 60 ]] ;then
		debug_handle $1
	else
		combination_handle $1
	fi
}

function auto_build()
{
	       	menu_do $1
		menu_do $2
	while :;do
		cd $DBG_BUILD_PATH
		lidbg_menu
		read -p "Enter your select:" name1 name2 name3 name4 name5
	       	menu_do $name1
		menu_do $name2
		menu_do $name3
		menu_do $name4
		menu_do $name5
	done
}

# apt-get install expect
cd build
source ./env_entry.sh
. $DBG_TOOLS_PATH/soc_$DBG_SOC.sh
. $DBG_TOOLS_PATH/depository.sh
. $DBG_TOOLS_PATH/debug.sh
. $DBG_TOOLS_PATH/combination.sh
auto_build $1 $2;

