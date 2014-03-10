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
DBG_CORE_PATH=$DBG_ROOT_PATH/core
DBG_SOC_PATH=$DBG_ROOT_PATH/soc
DBG_DRIVERS_PATH=$DBG_ROOT_PATH/drivers
DBG_ANDROID_PATH=$DBG_ROOT_PATH/android
DBG_PLATFORM_DIR=$DBG_SOC_DIR/$DBG_PLATFORM

export DBG_ROOT_PATH
export DBG_TOOLS_PATH
export DBG_SHELL_PATH
export DBG_OUT_PATH
export DBG_CORE_PATH
export DBG_SOC_PATH
export DBG_DRIVERS_PATH
export DBG_PLATFORM_DIR
