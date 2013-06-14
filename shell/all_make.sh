#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh

mkdir $DBG_OUT_PATH
mkdir $DBG_ANDROID_OUT_PATH
mkdir -p $DBG_OUT_RELEASE_PATH/v1/out
mkdir -p $DBG_OUT_RELEASE_PATH/v2/out

if [ "$DBG_SYN_XP2LI" = "y" ]; then
echo "######### execute Syn_file_XP2Li.sh ##########"
sh ./Syn_file_XP2Li.sh
fi

./drv_make_copy.sh  &&./work_make_copy_all.sh

if [ "$DBG_SYN_LI2XP" = "y" ]; then
echo "######### execute Syn_file_Li2XP.sh ##########"
./Syn_file_li2XP.sh
fi
