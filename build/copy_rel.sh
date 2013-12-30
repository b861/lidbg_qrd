#=======================================================================================
#	FileName    : 
#	Description : copy images to specified directory when changed
#       Date:         2010/04/27
#=======================================================================================

echo "copy_rel+"
source ./env_entry.sh

src_dir=$DBG_SOC_PATH/$DBG_SOC
dest_dir=$DBG_OUT_PATH
echo "from" $src_dir 
echo "to" $dest_dir
cp -u $src_dir/*.ko               $dest_dir/

src_dir=$DBG_DRV_PATH
echo "from" $src_dir 
echo "to" $dest_dir
cp -u $src_dir/*.ko               $dest_dir/
cp -u $src_dir/*.conf               $dest_dir/
echo "copy_rel-"

