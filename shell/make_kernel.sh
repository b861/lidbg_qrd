#=======================================================================================
#	FileName    : 
#	Description : Make Kernel
#       Date:         2010/04/27
#=======================================================================================
source ./env_entry.sh

dir=$DBG_KERNEL_DIR

cp -u $DBG_PLATFORM_DIR/$DBG_MAKE_KERNEL_SHELL               $dir/$DBG_MAKE_KERNEL_SHELL

until ["$get_key" = "E"] || ["$get_key" = "e"] # for exit
do

echo "######### build kernel ##########"
cd  $dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
sh ./$DBG_MAKE_KERNEL_SHELL  && echo "Make Kernel Successful!"

cd  $DBG_SHELL_PATH ; pwd
sh ./copy_img.sh

read get_key
done

