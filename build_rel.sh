#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================

release_path=./out_release/v2/out
#./build.sh v1 rel
#cp -r ./out ./out_release/v1
./build.sh v2 rel
#cp -r ./out $release_path
cp ./out/lidbg_share.ko $release_path
cp ./out/lidbg_ts_to_recov.ko $release_path
cp ./out/lidbg_msg.ko $release_path
cp ./out/lidbg_common.ko $release_path
cp ./out/lidbg_servicer.ko $release_path
cp ./out/lidbg_touch.ko $release_path
cp ./out/lidbg_key.ko $release_path
cp ./out/lidbg_i2c.ko $release_path
cp ./out/lidbg_soc_msm8x25.ko $release_path
cp ./out/lidbg_io.ko $release_path
cp ./out/lidbg_ad.ko $release_path
cp ./out/lidbg_main.ko $release_path
cp ./out/lidbg_fly_soc.ko $release_path
cp ./out/lidbg_fastboot.ko $release_path
cp ./out/lidbg_lpc.ko $release_path
cp ./out/lidbg_soc_devices.ko $release_path
cp ./out/lidbg_videoin.ko $release_path
cp ./out/lidbg_to_bpmsg.ko $release_path
cp ./out/lidbg_gps_driver.ko $release_path
cp ./out/lidbg_ts_probe.ko $release_path
cp ./out/gt801.ko $release_path
cp ./out/gt811.ko $release_path
