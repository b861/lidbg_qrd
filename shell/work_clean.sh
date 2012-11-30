#=======================================================================================
#	FileName    : 
#	Description : Make Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh
cd $DBG_WORK_PATH/$DBG_SOC/test && make clean
cd $DBG_WORK_PATH/$DBG_SOC/touchscreen && make clean
cd $DBG_WORK_PATH/$DBG_SOC/videoin && make clean
