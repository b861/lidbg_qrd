#=======================================================================================
#	FileName    : 
#	Description : 
#       Date:         2010/04/27
#=======================================================================================

source ../dbg_cfg.sh
#source ./version.sh

DBG_SHELL_PATH=$DBG_ROOT_PATH/shell
DBG_OUT_PATH=$DBG_ROOT_PATH/out
DBG_OUT_RELEASE_PATH=$DBG_ROOT_PATH/out_release
DBG_SOC_PATH=$DBG_ROOT_PATH/soc
DBG_DRV_PATH=$DBG_ROOT_PATH/drv
DBG_DEV_PATH=$DBG_ROOT_PATH/devices
DBG_WORK_PATH=$DBG_ROOT_PATH/mywork
DBG_LIB_PATH=$DBG_ROOT_PATH/lib
DBG_TOOLS_PATH=$DBG_ROOT_PATH/tools
DBG_APP_LINUX_PATH=$DBG_ROOT_PATH/app_linux
DBG_ANDROID_PATH=$DBG_ROOT_PATH/android
DBG_ANDROID_OUT_PATH=$DBG_ANDROID_PATH/out

DBG_SOC_DIR=$DBG_SOC_PATH/$DBG_SOC
DBG_PLATFORM_DIR=$DBG_SOC_DIR/$DBG_PLATFORM



export DBG_SHELL_PATH
export DBG_OUT_PATH
export DBG_SOC_PATH
export DBG_DRV_PATH
export DBG_DEV_PATH
export DBG_WORK_PATH
export DBG_LIB_PATH
export DBG_APP_LINUX_PATH
export DBG_APP_ANDROID_PATH
export DBG_TOOLS_PATH
export DBG_OUT_RELEASE_PATH
export DBG_SOC_DIR
export DBG_PLATFORM_DIR

export DBG_CROSS_COMPILE
export DBG_ANDROID_OUT_PATH
#source $DBG_PLATFORM_DIR/config_$DBG_SOC.sh

