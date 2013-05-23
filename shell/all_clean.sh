#=======================================================================================
#	FileName    : 
#	Description : Make ALL Clean
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh
./drv_clean.sh
./work_clean.sh
cd $DBG_OUT_PATH && rm -rf *
cd $DBG_OUT_RELEASE_PATH && rm -rf *

