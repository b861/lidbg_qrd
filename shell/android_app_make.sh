#=======================================================================================
#	FileName    : 
#	Description : copy images to specified directory when changed
#       Date:         2010/04/27
#=======================================================================================

Android_dir=/home/RK2818/rkeclair_v1.02_sdkdemo
App_dir=/home/RK2818/rkeclair_v1.02_sdkdemo/external/android_lidbg

until ["$get_key" = "E"] || ["$get_key" = "e"] # for exit ??
do

cd  $App_dir ; pwd 
sh ./Syn_file_XP2Li.sh

cd  $Android_dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
make android_lidbg  && echo "Make android_lidbg Successful!"

cd  $App_dir ; pwd 
sh ./copy_rel.sh

read get_key
done


