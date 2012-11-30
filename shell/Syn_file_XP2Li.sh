#=======================================================================================
#	FileName    : Syn File to linux
#	Description : 
#       Date:         2010/04/23
#=======================================================================================
source ./env_entry.sh

echo "Syn File Start WIN2LINUX!"
source_dir=/lsw/linux_debug
dest_dir=/home

echo "from" $source_dir
echo "to" $dest_dir
cp -ru $source_dir/   $dest_dir/


echo "Syn File End!"
