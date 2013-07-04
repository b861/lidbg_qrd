
source ./env_entry.sh

release_path=$DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out

cp -rf $release_path $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/modules
cp $DBG_ANDROID_OUT_PATH/lidbg_servicer $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin
cp $DBG_ANDROID_OUT_PATH/gps.$DBG_PLATFORM.so $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/hw

