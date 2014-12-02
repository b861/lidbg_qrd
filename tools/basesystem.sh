#========注意:您给的参数需按照如下参数给出==========
#1 SYSTEM_PLATFORM SYSTEM_BUILD_TYPE SYSTEM_DIR SYSTEM_DIR_PASSWORD BIN_DIR BIN_DIR_PASSWORD BASESYSTEM_DIR_IN_BIN_DIR BIN_GIT_COMMIT_DESCRIPTION
#========注意:如下是一个例子==========
#./basesystem.sh 1 msm8974 userdebug /home/swlee/flyaudio/M8974AAAAANLYD4120 git /home/swlee/flyaudio/8x26-release git /home/swlee/flyaudio/8x26-release/others/8974/basesystem 一个例子

DIR_LIDBG_PATH=`cd ../ && pwd`
DIR_BUILD_PATH=$DIR_LIDBG_PATH/build
DIR_TOOLS_PATH=$DIR_LIDBG_PATH/tools

SYSTEM_PLATFORM="null"
SYSTEM_BUILD_TYPE="null"
SYSTEM_DIR="null"
SYSTEM_DIR_PASSWORD="null" 
BIN_DIR="null"
BIN_DIR_PASSWORD="null"
BASESYSTEM_DIR_IN_BIN_DIR="null"
BIN_GIT_COMMIT_DESCRIPTION="NULL"


#$1-$BIN_DIR $2-$BIN_GIT_COMMIT_DESCRIPTION
function git_add_push()
{
	echo ====IN=====$FUNCNAME $1 $2
	cd $1
	git add .
	git commit -am $2
#	expect $DIR_TOOLS_PATH/push
#	expect $DIR_TOOLS_PATH/push
	gitk &
}

function copy_basesystem_system_to_bin_dir()
{
	echo ====IN=====$FUNCNAME $SYSTEM_DIR $SYSTEM_PLATFORM $BASESYSTEM_DIR_IN_BIN_DIR
	#cp -r $SYSTEM_DIR/flyaudio/out/*  $BASESYSTEM_DIR_IN_BIN_DIR/
	cp $SYSTEM_DIR/out/target/product/$SYSTEM_PLATFORM/*.zip $BASESYSTEM_DIR_IN_BIN_DIR/baseqcom.flb
}

function system_dir_build()
{
	echo ====IN.1pull_bin_dir=====$FUNCNAME
	git_pull $BIN_DIR $BIN_DIR_PASSWORD
	
	echo ====IN.2pull_system_dir=====$FUNCNAME
	git_pull $SYSTEM_DIR $SYSTEM_DIR_PASSWORD

	echo ====IN.3choosecombo=====$FUNCNAME
	cd $SYSTEM_DIR
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $SYSTEM_PLATFORM $SYSTEM_BUILD_TYPE
	fi
	echo ====IN.4make-j8=====$FUNCNAME
	make -j8
	echo ====IN5make otapackage -j8=====$FUNCNAME
	make otapackage -j8
}

function git_reset_hard()
{
	echo ====IN=====$FUNCNAME $1
	cd $1
	git reset --hard
}

#$SYSTEM_DIR $SYSTEM_DIR_PASSWORD
function git_pull()
{
	echo ====IN=====$FUNCNAME $1 $2
	cd $1
	expect $DIR_TOOLS_PATH/pull $2
}

function show_env()
{
	echo ===============show_env====================
	echo $SYSTEM_PLATFORM
	echo $SYSTEM_BUILD_TYPE
	echo $SYSTEM_DIR
	echo $SYSTEM_DIR_PASSWORD
	echo $BIN_DIR
	echo $BIN_DIR_PASSWORD
	echo $BASESYSTEM_DIR_IN_BIN_DIR
	echo $BIN_GIT_COMMIT_DESCRIPTION
	echo ===============show_env====================
}

function basesystem_launch()
{
	echo $1 $2 $3 $4 $5 $6 $7 $8 $9
	SYSTEM_PLATFORM=$2
	SYSTEM_BUILD_TYPE=$3
	SYSTEM_DIR=$4
	SYSTEM_DIR_PASSWORD=$5
	BIN_DIR=$6
	BIN_DIR_PASSWORD=$7
	BASESYSTEM_DIR_IN_BIN_DIR=$8
	BIN_GIT_COMMIT_DESCRIPTION=$9
	
	show_env
	case $1 in
	1)
		git_pull $SYSTEM_DIR $SYSTEM_DIR_PASSWORD && git_reset_hard $BIN_DIR&& git_pull $BIN_DIR $BIN_DIR_PASSWORD&&git_pull $BIN_DIR $BIN_DIR_PASSWORD&&
		system_dir_build && git_pull $BIN_DIR $BIN_DIR_PASSWORD && 
		copy_basesystem_system_to_bin_dir && git_add_push $BIN_DIR $BIN_GIT_COMMIT_DESCRIPTION;;
	*)
		echo =========stop.mind your input:$1=============
	esac
}

basesystem_launch $1 $2 $3 $4 $5 $6 $7 $8 $9

