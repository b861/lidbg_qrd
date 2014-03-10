

function platform_func()
{
	echo 平台选择
	if [ ! -n "$1" ] ;then
		exit	
	fi
}

function clean_func()
{
	echo 清除生成文件
	./clean.sh
}

function build_func()
{
	echo 编译模块
	./build.sh
}

function pushfly_func()
{
	echo push驱动模块到产品系统
	sh $DBG_TOOLS_PATH/pushfly.sh
}

function release_func()
{
	echo 编译lidbg所有文件
	read -p "是否提交到二进制仓库y/n:" choice
	if [[ $choice = y ]];then
		read -p "输入提交到二进制仓库的说明文字：" descriptor
		. $DBG_TOOLS_PATH/release.sh $descriptor
	else
		. $DBG_TOOLS_PATH/release.sh
	fi
}

function push_func()
{
	echo push到原生系统
	sh $DBG_TOOLS_PATH/push.sh
}

function pull_lidbg_func()
{
	echo git pull
	expect $DBG_TOOLS_PATH/pull_lidbg
	chmod 777 $DBG_ROOT_PATH -R
}

function push_lidbg_func()
{
	echo push lidbg_qrd到服务器
	expect $DBG_TOOLS_PATH/push_lidbg
}

function catch_log_func()
{
	echo catch_log_func
	mkdir -p $DBG_ROOT_PATH/log
	adb wait-for-devices shell echo "c mem_log dump" > /dev/mlidbg0
	adb shell echo "c lidbg_trace_msg disable" > /dev/mlidbg0
	adb shell logcat >> $DBG_ROOT_PATH/log/logcat.txt &
	adb shell cat /proc/kmsg >> $DBG_ROOT_PATH/log/kmsg.txt &
	sleep 5
	adb pull /data/lidbg/lidbg_mem_log.txt $DBG_ROOT_PATH/log
}

function menu_func()
{
	echo $DBG_ROOT_PATH
	echo [1] clean.sh'                        '清除生成文件	
	echo [2] buid.sh'                         '编译模块
	echo [3] pushfly.sh'                      'push驱动模块到产品系统
	echo [4] release.sh'                      '编译lidbg所有文件
	echo [5] push.sh'                         'push驱动模块到原生系统
	echo [6] choose platform'                 '选择平台
	echo [7] pull'                            'git pull服务器的libg_qrd
	echo [8] push'                            'git push服务器libg_qrd
	echo [9] gitk'                            '执行gitk
	echo [10] git log'                        '执行git log --oneline
	echo [11] git reset'                      '执行git reset --hard	
	echo [15] nautilus'                       '打开lidbg目录
	echo [16] catch log'                      '抓取调试信息
	echo [17] catch kmsg'                     '抓取kmsg调试信息
	echo [18] remount'                        'remount文件系统 
	echo 
	echo $DBG_SYSTEM_DIR
	soc_menu_func
	echo
}

function handle_func()
{
		case $1 in
		1)	
			clean_func;;
		2)
			build_func;;	
		3)
			pushfly_func;;
		4)
			release_func;;
		5)
			push_func;;
		6)
			platform_func;;
		7)       
		 	pull_lidbg_func;;
		8)	
			push_lidbg_func;;
		9)
			gitk &;;
		10)
			git log --oneline;;
		11)
			git reset --hard && chmod 777 * -R;;
		15)
			nautilus $DBG_ROOT_PATH;;
		16)
			catch_log_func;;
		17)
			expect $DBG_TOOLS_PATH/kmsg;;
		18)
			expect $DBG_TOOLS_PATH/root;;
		*)
			echo
		esac
}


function menu_do()
{
	if [[ $1 -le 20 ]] ;then
		handle_func $1
	else
		soc_handle_func $1
	fi
}

function auto_build()
{
	       	menu_do $1
		menu_do $2
	while :;do
		cd $DBG_BUILD_PATH
		menu_func
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
auto_build $1 $2;

