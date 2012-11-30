#=======================================================================================
#	FileName    : Syn File to WinXP
#	Description : 
#       Date:         2010/04/23
#=======================================================================================
source ./env_entry.sh

echo "Syn File Start!"
source_dir=$DBG_ROOT_PATH
dest_dir=$DBG_COPY_DIR
cp -ru $source_dir/   $dest_dir/

source_dir=$DBG_ROOT_PATH/out
dest_dir=$DBG_COPY_DIR/adb
cp -ru $source_dir/   $dest_dir/

echo "Syn File End!"

