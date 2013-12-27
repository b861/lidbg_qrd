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
	./build/pushfly.sh
}

function release_func()
{
	echo 编译lidbg所有文件
	read -p "是否提交到二进制仓库y/n:" choice
	if [ $choice = y ];then
		read -p "输入提交到二进制仓库的说明文字：" descriptor
		./release.sh $descriptor
	else
		./release.sh
	fi
}

function push_func()
{
	echo push到原生系统
	./build/push.sh
}

function pull_func()
{
	echo git pull
	expect ./build/pull_lidbg
}

function menu_func()
{
	echo 
	echo	
	echo [1] clean.sh'                        '清除生成文件	
	echo [2] buid.sh'                         '编译模块
	echo [3] pushfly.sh'                      'push驱动模块到产品系统
	echo [4] release.sh'                      '编译lidbg所有文件
	echo [5] push.sh'                         'push驱动模块到原生系统
	echo [6] choose platform'                 '选择平台
	echo [7] pull'                            'git pull
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
		 	pull_func;;
		*) 
			echo 
			#exit;;
		esac
}

function auto_build()
{
	       	handle_func $1
		handle_func $2

	while :;do
		menu_func	
		read -p "Enter your select:" name1 name2 name3 name4 name5
	       	handle_func $name1
		handle_func $name2
		handle_func $name3
		handle_func $name4
		handle_func $name5

	done
}



	auto_build $1 $2;


