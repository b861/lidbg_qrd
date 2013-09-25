#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh
hostname > $DBG_OUT_PATH/build_time.txt
date >> $DBG_OUT_PATH/build_time.txt
./drv_make_copy.sh  &&./work_make_copy_all.sh


