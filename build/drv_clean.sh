#=======================================================================================
#	FileName    : 
#	Description : Make Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh
cd $DBG_SOC_PATH/$DBG_SOC && make clean
cd $DBG_DRV_PATH && make clean
