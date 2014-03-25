function depository_pull()
{
	echo $FUNCNAME
	expect $DBG_TOOLS_PATH/pull
}

function depository_add_push()
{
	echo $FUNCNAME
	git add .
	git add -f $UPDATA_BIN_DIR
	git commit -am $1
	expect $DBG_TOOLS_PATH/push
	gitk &
}

function depository_copy_lidbg()
{
	echo $FUNCNAME
	cp -r $DBG_OUT_PATH  $UPDATA_BIN_DIR
	cp -r $DBG_OUT_PATH/*.so  $UPDATA_BIN_DIR/hw
}

function depository_copy_basesystem()
{
	echo $FUNCNAME
	cp -r $DBG_SYSTEM_DIR/flyaudio/out/*  $UPDATA_BASESYSTEM_DIR
}


function depository_make_package()
{
	echo $FUNCNAME
	expect $DBG_TOOLS_PATH/make_package
}


function depository_menu()
{
	echo $RELEASE_REPOSITORY
	echo [41] copy lidbg out
	echo [42] copy basesystem
	echo [43] make package
	echo [44] pull
	echo [45] push
	echo [46] gitk
	echo [47] git log
	echo [48] git reset
	echo [49] nautilus
}

function depository_handle()
{
	cd $RELEASE_REPOSITORY
	case $1 in
	41)	
		depository_copy_lidbg;;
	42)
		depository_copy_basesystem;;
	43)
		depository_make_package;;
	44)	
		depository_pull;;
	45)
		read -p "输入提交到二进制仓库的说明文字：" descriptors
		depository_add_push;;
	46)
		gitk &;;
	47)
		git log --oneline;;
	48)
		git reset --hard && chmod 777 * -R;;
	49)
		nautilus $DBG_ROOT_PATH;;
	*)
		echo
	esac
}


