#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh
hostname > $DBG_DRV_PATH/build_time.conf
date >> $DBG_DRV_PATH/build_time.conf
./drv_make_copy.sh  &&./work_make_copy_all.sh


