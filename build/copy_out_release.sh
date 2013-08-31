
source ./env_entry.sh

release_path=$DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out

#cp $DBG_OUT_PATH/lidbg_share.ko $release_path
cp $DBG_OUT_PATH/lidbg_ts_to_recov.ko $release_path
cp $DBG_OUT_PATH/lidbg_msg.ko $release_path
cp $DBG_OUT_PATH/lidbg_common.ko $release_path
cp $DBG_OUT_PATH/lidbg_servicer.ko $release_path
cp $DBG_OUT_PATH/lidbg_touch.ko $release_path
cp $DBG_OUT_PATH/lidbg_key.ko $release_path
cp $DBG_OUT_PATH/lidbg_i2c.ko $release_path
cp $DBG_OUT_PATH/lidbg_soc_msm8x25.ko $release_path
cp $DBG_OUT_PATH/lidbg_io.ko $release_path
cp $DBG_OUT_PATH/lidbg_ad.ko $release_path
cp $DBG_OUT_PATH/lidbg_main.ko $release_path
cp $DBG_OUT_PATH/lidbg_fly_hal.ko $release_path
cp $DBG_OUT_PATH/lidbg_fastboot.ko $release_path
cp $DBG_OUT_PATH/lidbg_lpc.ko $release_path
cp $DBG_OUT_PATH/lidbg_devices.ko $release_path
cp $DBG_OUT_PATH/lidbg_videoin.ko $release_path
cp $DBG_OUT_PATH/lidbg_bpmsg.ko $release_path
cp $DBG_OUT_PATH/lidbg_gps.ko $release_path
cp $DBG_OUT_PATH/lidbg_ts_probe.ko $release_path
cp $DBG_OUT_PATH/gt801.ko $release_path
cp $DBG_OUT_PATH/gt811.ko $release_path
cp $DBG_OUT_PATH/gt911.ko $release_path
cp $DBG_OUT_PATH/gt80x_update.ko $release_path
cp $DBG_OUT_PATH/lidbg_loader.ko $release_path
cp $DBG_OUT_PATH/lidbg_uart.ko $release_path
cp $DBG_OUT_PATH/lidbg_i2c_gpio.ko $release_path
cp $DBG_OUT_PATH/fastboot_not_kill_list.conf $release_path
cp $DBG_OUT_PATH/core.conf $release_path
cp $DBG_OUT_PATH/drivers.conf $release_path
