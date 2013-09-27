source ../../dbg_cfg.sh
LOCATE_PATH=`pwd`
cd ../../build && source env_entry.sh && ./build_cfg.sh $BOARD_VERSION $BUILD_VERSION $DBG_SOC
cd $DBG_SYSTEM_DIR/&&source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng

for each_dir in `ls -l $LOCATE_PATH | grep "^d" | awk '{print $NF}'`

do
	dir=$LOCATE_PATH/$each_dir
	mmm $dir -B
done


















