function depository_clean()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	git reset --hard
}

function depository_pull()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	expect $DBG_TOOLS_PATH/pull $DBG_REPO_PASSWORD
	expect $DBG_TOOLS_PATH/pull $DBG_REPO_PASSWORD
}

function depository_add_push()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	git add .
	git add -f $UPDATA_BIN_DIR
	git commit -am $1
	expect $DBG_TOOLS_PATH/push
	expect $DBG_TOOLS_PATH/push
	gitk &
}

function depository_copy_lidbg()
{
	echo $FUNCNAME
	cp -r $DBG_OUT_PATH  $UPDATA_BIN_DIR
	#cp -r $DBG_OUT_PATH/*.so  $UPDATA_BIN_DIR/hw/
}

function depository_copy_basesystem()
{
	echo $FUNCNAME
	cp -r $DBG_SYSTEM_DIR/flyaudio/out/*  $UPDATA_BASESYSTEM_DIR/
}


function depository_make_package()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	rm -r $RELEASE_REPOSITORY/out/*
	expect $DBG_TOOLS_PATH/make_package
	nautilus $RELEASE_REPOSITORY/out
}


function push_flyFastBoot_apk
{
	expect $DBG_TOOLS_PATH/remount
	adb push $RELEASE_REPOSITORY/app/FastBoot.apk /system/app/
	adb push $RELEASE_REPOSITORY/app/FlyBootService.apk /system/app/
	adb shell chmod 777 /system/app/FastBoot.apk
	adb shell chmod 777 /system/app/FlyBootService.apk
}

function depository_menu()
{
if [[ $RELEASE_REPOSITORY != "" ]];then
	echo $RELEASE_REPOSITORY
	echo [41] copy lidbg out to depository
	echo [42] copy basesystem to depository
	echo [43] make update package
	echo [44] push flyFastBoot apk
fi
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
		push_flyFastBoot_apk;;
	*)
		echo
	esac
}


