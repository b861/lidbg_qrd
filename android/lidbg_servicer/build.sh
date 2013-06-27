LOCATE_PATH=`pwd`
source ../../dbg_cfg.sh
cd ../../shell && source env_entry.sh
cd $DBG_SYSTEM_DIR/&&source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng&&mmm $LOCATE_PATH
