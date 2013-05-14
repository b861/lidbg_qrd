#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh

if [ "$DBG_SYN_XP2LI" = "y" ]; then
echo "######### execute Syn_file_XP2Li.sh ##########"
sh ./Syn_file_XP2Li.sh
fi

./drv_make_copy.sh  &&./work_make_copy_all.sh &&./copy_rel.sh 

if [ "$DBG_SYN_LI2XP" = "y" ]; then
echo "######### execute Syn_file_Li2XP.sh ##########"
./Syn_file_li2XP.sh
fi
