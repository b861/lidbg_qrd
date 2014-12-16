
source ./env_entry.sh
echo input :"$1" "$2" "$3"

echo EXTRA_CFLAGS += -DSOC_${1} > $DBG_BUILD_PATH/build_cfg.mk
echo LOCAL_CFLAGS += -DSOC_${1} >> $DBG_BUILD_PATH/build_cfg.mk

echo EXTRA_CFLAGS += -DBOARD_${2} >> $DBG_BUILD_PATH/build_cfg.mk
echo LOCAL_CFLAGS += -DBOARD_${2} >> $DBG_BUILD_PATH/build_cfg.mk

echo EXTRA_CFLAGS += -DPLATFORM_${3} >> $DBG_BUILD_PATH/build_cfg.mk
echo LOCAL_CFLAGS += -DPLATFORM_${3} >> $DBG_BUILD_PATH/build_cfg.mk

echo EXTRA_CFLAGS += -D${4} >> $DBG_BUILD_PATH/build_cfg.mk
echo LOCAL_CFLAGS += -D${4} >> $DBG_BUILD_PATH/build_cfg.mk
