
source ./env_entry.sh

release_path=$DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out

cp $DBG_OUT_PATH/lidbg_common.ko $release_path
cp $DBG_OUT_PATH/lidbg_servicer.ko $release_path
cp $DBG_OUT_PATH/lidbg_touch.ko $release_path
cp $DBG_OUT_PATH/lidbg_key.ko $release_path
cp $DBG_OUT_PATH/lidbg_i2c.ko $release_path
cp $DBG_OUT_PATH/lidbg_soc_$DBG_SOC.ko $release_path
cp $DBG_OUT_PATH/lidbg_io.ko $release_path
cp $DBG_OUT_PATH/lidbg_ad.ko $release_path
cp $DBG_OUT_PATH/lidbg_main.ko $release_path
cp $DBG_OUT_PATH/lidbg_loader.ko $release_path
cp $DBG_OUT_PATH/lidbg_uart.ko $release_path
cp $DBG_OUT_PATH/suspend_monitor.ko $release_path
cp $DBG_OUT_PATH/core.conf $release_path
cp $DBG_OUT_PATH/drivers.conf $release_path
cp $DBG_OUT_PATH/cmd.conf $release_path
cp $DBG_OUT_PATH/state.conf $release_path
