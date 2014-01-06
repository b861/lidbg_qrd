#=======================================================================================
#	FileName    : 
#	Description : 
#       Date:         2010/04/27
#=======================================================================================

source ../dbg_cfg.sh

DBG_ROOT_PATH=`cd ../ && pwd`
DBG_SHELL_PATH=$DBG_ROOT_PATH/build
DBG_TOOLS_PATH=$DBG_ROOT_PATH/tools
DBG_OUT_PATH=$DBG_ROOT_PATH/out
DBG_OUT_RELEASE_PATH=$DBG_ROOT_PATH/out_release
DBG_DRV_PATH=$DBG_ROOT_PATH/core
DBG_SOC_PATH=$DBG_ROOT_PATH/soc
DBG_WORK_PATH=$DBG_ROOT_PATH/drivers
DBG_ANDROID_PATH=$DBG_ROOT_PATH/android
DBG_ANDROID_OUT_PATH=$DBG_ANDROID_PATH/$DBG_SOC/out

DBG_PLATFORM_DIR=$DBG_SOC_DIR/$DBG_PLATFORM

export DBG_ROOT_PATH
export DBG_TOOLS_PATH
export DBG_SHELL_PATH
export DBG_OUT_PATH
export DBG_DRV_PATH
export DBG_SOC_PATH
export DBG_WORK_PATH
export DBG_OUT_RELEASE_PATH
export DBG_PLATFORM_DIR
export DBG_ANDROID_OUT_PATH
