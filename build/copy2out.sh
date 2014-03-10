#=======================================================================================
#	FileName    : 
#	Description : copy images to specified directory when changed
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh

src_dir=$DBG_SOC_PATH/$DBG_SOC
dest_dir=$DBG_OUT_PATH
cp -u $src_dir/*.ko               $dest_dir/

src_dir=$DBG_CORE_PATH
cp -u $src_dir/*.ko               $dest_dir/
cp -u $src_dir/*.conf               $dest_dir/

cd $DBG_DRIVERS_PATH
for each_dir in `ls -l | grep "^d" | awk '{print $NF}'`

	do
	dir=$dbg_soc/$each_dir

	cp -u $dir/*.ko     $DBG_OUT_PATH/
	cp -u $dir/*.conf   $DBG_OUT_PATH/
done
