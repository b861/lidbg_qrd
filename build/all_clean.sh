#=======================================================================================
#	FileName    : 
#	Description : Make ALL Clean
#       Date:         2012/02/03
#=======================================================================================
source ./env_entry.sh

if [ "$DBG_OUT_PATH" = "" ]; then
echo "exit:  DBG_OUT_PATH=null"
exit
fi

if [ "$DBG_OUT_RELEASE_PATH " = "" ]; then
echo "exit:  DBG_OUT_PATH=null"
exit
fi

./drv_clean.sh
./work_clean.sh
rm -rf $DBG_OUT_PATH
rm -rf $DBG_OUT_RELEASE_PATH 
cd $DBG_ANDROID_PATH && rm -rf out

