#=======================================================================================
#	FileName    : 
#	Description : Make Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh
cd $DBG_DRV_PATH/$DBG_SOC && make clean
cd $DBG_DRV_PATH && make clean
#cd $DBG_WORK_PATH/$DBG_SOC && make clean
cd $DBG_DEV_PATH/$DBG_SOC && make clean
cd $DBG_OUT_PATH && rm *.ko
