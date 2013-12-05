LOCATE_PATH=`pwd`
echo $LOCATE_PATH
source ../../../dbg_cfg.sh
cd ../../../build && source env_entry.sh && ./build_cfg.sh $BOARD_VERSION $BUILD_VERSION $DBG_SOC
cd $DBG_SYSTEM_DIR/&&source build/envsetup.sh&&choosecombo release $DBG_PLATFORM eng
mkdir -p $DBG_ANDROID_OUT_PATH
cp -u $DBG_ANDROID_PATH/$DBG_SOC/*.conf    $DBG_ANDROID_OUT_PATH/
mmm $LOCATE_PATH -B  && cp -u $DBG_ANDROID_OUT_PATH/*  $DBG_OUT_PATH/
while :;do		
	echo
	echo -e "\033[41;20m 请输入字母 b ，再次编译 \033[0m"
	read Parameter0
	if [ $Parameter0 = b ];then
		mmm $LOCATE_PATH && cp -u $DBG_ANDROID_OUT_PATH/*  $DBG_OUT_PATH/
	else 
	   exit
	fi
done
