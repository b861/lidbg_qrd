source ../dbg_cfg.sh
source ./config_$DBG_SOC
LOCATE_PATH=`pwd`
clear
cd ../build && source env_entry.sh && ./build_cfg.sh  $DBG_SOC $BUILD_VERSION
cd $DBG_SYSTEM_DIR/&&source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE && mmm $DBG_HAL_PATH -B

cp -u $DBG_HAL_PATH/*.conf    $DBG_OUT_PATH/
