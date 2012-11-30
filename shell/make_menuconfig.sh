#=======================================================================================
#	FileName    : 
#	Description : Config Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh

dir=$DBG_KERNEL_DIR


echo "Config Kernel Start!"          
cd  $dir && make menuconfig
echo "Config Kernel End!"
read

