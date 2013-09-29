#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================
chmod 777 -R ./*
cd build
source ./env_entry.sh
./build_cfg.sh $BOARD_VERSION $BUILD_VERSION $DBG_SOC && ./all_make.sh
./copy_out_release.sh

