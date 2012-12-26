#=======================================================================================
#	FileName    : 
#	Description : copy images to specified directory when changed
#       Date:         2010/04/27
#=======================================================================================

echo "copy_rel+"
source ./env_entry.sh

src_dir=$DBG_DRV_PATH/$DBG_SOC
dest_dir=$DBG_OUT_PATH

echo "from" $src_dir 
echo "to" $dest_dir

echo "SOC Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "SOC Modules Copy Finish!"


src_dir=$DBG_DRV_PATH
echo "from" $src_dir 
echo "to" $dest_dir

echo "Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "Modules Copy Finish!"


src_dir=$DBG_WORK_PATH/$DBG_SOC/touchscreen
echo "from" $src_dir 
echo "to" $dest_dir

echo "Work/touchscreen Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "Work/touchscreen Modules Copy Finish!"


src_dir=$DBG_WORK_PATH/$DBG_SOC/test
echo "from" $src_dir 
echo "to" $dest_dir

echo "Work/test Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "Work/test Modules Copy Finish!"


src_dir=$DBG_WORK_PATH/$DBG_SOC/videoin
echo "from" $src_dir 
echo "to" $dest_dir

echo "Work/videoin Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "Work/videoin Modules Copy Finish!"

src_dir=$DBG_WORK_PATH/$DBG_SOC/fastboot
echo "from" $src_dir 
echo "to" $dest_dir

echo "Work/fastboot Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "Work/fastboot Modules Copy Finish!"

src_dir=$DBG_DEV_PATH/$DBG_SOC
echo "from" $src_dir 
echo "to" $dest_dir

echo "Devices Modules Copy Start!"
cp -u $src_dir/*.ko               $dest_dir/
echo "Devices Modules Copy Finish!"

#src_dir=$DBG_APP_LINUX_PATH
#echo "appdbg Copy Start!"
#cp -u $src_dir/appdbg               $dest_dir/appdbg
#echo "appdbg Copy Finish!"

#copy to soc/plat/out
cp -ru $DBG_OUT_PATH/   $DBG_PLATFORM_DIR/release/
cp -u $DBG_DRV_PATH/$DBG_SOC/inc/fly_soc.h   $DBG_PLATFORM_DIR/release/



#copy to window
#cp -ru $DBG_OUT_PATH/   $DBG_COPY_DIR/
echo "copy_rel-"


