source ../../dbg_cfg.sh
cd ../../shell && source env_entry.sh
cd $DBG_SYSTEM_DIR/&&source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng&&mmm $DBG_ROOT_PATH/android/lidbg_servicer

