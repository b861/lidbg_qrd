#=======================================================================================
#	FileName    : 
#	Description : Make Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh

dbg_soc=$DBG_WORK_PATH/
cd $dbg_soc

for each_dir in `ls -l | grep "^d" | awk '{print $NF}'`

do
	cd $dbg_soc/$each_dir && make clean
	
done

