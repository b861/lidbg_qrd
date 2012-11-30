#=======================================================================================
#	FileName    : 
#	Description : copy images to specified directory when changed
#       Date:         2010/04/27
#=======================================================================================

echo "backup+"
source ./env_entry.sh

src_dir=~
dest_dir=$DBG_ROOT_BACKUP_PATH/soc/$DBG_SOC

echo "from" $src_dir 
echo "to" $dest_dir

echo "bashrc Copy Start!"
cp -u $src_dir/.bashrc            $dest_dir/bashrc
echo "bashrc Copy End!"


#src_dir=$DBG_SYSTEM_DIR/external/app_android
#dest_dir=$DBG_ROOT_PATH
#cp -ru $src_dir     $dest_dir


#src_dir=$DBG_ROOT_PATH
#dest_dir=$DBG_ROOT_BACKUP_PATH
#cp -ru $src_dir     $dest_dir

echo "backup-"


