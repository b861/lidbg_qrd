#=======================================================================================
#	FileName    : 
#	Description : Make Drv
#       Date:         2010/04/27
#=======================================================================================

source ./env_entry.sh

dir=$DBG_LIB_PATH


clear
#if [ "$DBG_SYN_XP2LI" = "y" ]; then
#echo "######### execute Syn_file_XP2Li.sh ##########"
#cd  $DBG_SHELL_PATH ; pwd
#sh ./Syn_file_XP2Li.sh
#fi

echo "######### build work ko ##########"
cd  $dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
make modules -j8 && echo "Make MyMork Successful!"


if [ "$?" = "0" ]; then # do when make successful
        echo "###########copy_rel.sh###############"
	cd  $DBG_SHELL_PATH ; pwd
	sh ./copy_rel.sh
else
        echo "Make Kernel ERROR!"
fi


#if [ "$DBG_SYN_LI2XP" = "y" ]; then
#echo "######### execute Syn_file_Li2XP.sh ##########"
#cd  $DBG_SHELL_PATH ; pwd
#sh ./Syn_file_li2XP.sh
#fi


