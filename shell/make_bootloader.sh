#=======================================================================================
#	FileName    : 
#	Description : Make Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh

dir=$DBG_BOOTLOADER_DIR
cp -u $DBG_PLATFORM_DIR/$DBG_MAKE_BOOTLOADER_SHELL             $dir/$DBG_MAKE_BOOTLOADER_SHELL

until ["$get_key" = "E"] || ["$get_key" = "e"] # for exit
do

echo "######### build bootloader ##########"
cd  $dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
sh ./$DBG_MAKE_BOOTLOADER_SHELL  && echo "Make Bootloader Successful!"

cd  $DBG_SHELL_PATH ; pwd
sh ./copy_img.sh

read get_key
done

