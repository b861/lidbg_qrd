
source ./env_entry.sh
./build_cfg.sh $BOARD_VERSION $BUILD_VERSION $DBG_SOC

hostname > $DBG_CORE_PATH/build_time.conf
date >> $DBG_CORE_PATH/build_time.conf
echo $DBG_PLATFORM >> $DBG_CORE_PATH/build_time.conf
echo $BOARD_VERSION >> $DBG_CORE_PATH/build_time.conf
echo $BUILD_VERSION >> $DBG_CORE_PATH/build_time.conf
git log --oneline | sed -n '1,10p' >> $DBG_CORE_PATH/build_time.conf
find ../ -name "Module.symvers" -exec rm -rf {} \;

mkdir -p $DBG_OUT_PATH
./make_all.sh  && . $DBG_SHELL_PATH/copy2out.sh 


