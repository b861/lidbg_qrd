#=======================================================================================
#	FileName    : 
#	Description : copy images to specified directory when changed
#       Date:         2010/04/27
#=======================================================================================

source ./env_entry.sh



echo "copy_bootloader+"
src_dir=$DBG_BOOTLOADER_IMG_DIR
dest_dir=$DBG_COPY_DIR
bootloader_name=$DBG_BOOTLOADER_IMG_NAME

echo "from" $src_dir 
echo "to" $dest_dir 
echo "name" $bootloader_name

echo "Images Copy Start!"
cp -u $src_dir/$bootloader_name               $dest_dir/$bootloader_name
echo "Images Copy Finish!"
echo "copy_bootloader-"



echo "copy_kernel+"
src_dir=$DBG_KERNEL_IMG_DIR
dest_dir=$DBG_COPY_DIR
kernel_name=$DBG_KERNEL_IMG_NAME

echo "from" $src_dir 
echo "to" $dest_dir 
echo "name" $kernel_name

echo "Images Copy Start!"
cp -u $src_dir/$kernel_name               $dest_dir/$kernel_name
echo "Images Copy Finish!"
echo "copy_kernel-"

read

