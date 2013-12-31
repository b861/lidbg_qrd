
chmod 777 -R ./
source ./env_entry.sh
./build_cfg.sh $BOARD_VERSION $BUILD_VERSION $DBG_SOC
./clean.sh
cd $DBG_ANDROID_PATH/$DBG_SOC
./build_all.sh

cd -
./build.sh

if [ "$1" = "" ]; then
	echo "exit"
else
	apt-get install expect
	cp -u ./pull  $UPDATA_BIN_DIR/pull
	cp -u ./push  $UPDATA_BIN_DIR/push
	
	cd $UPDATA_BIN_DIR
	pwd
	expect ./pull
	expect ./pull
	cp -ru $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out  $UPDATA_BIN_DIR
	git add .
	git commit -am $1
	expect ./push
	gitk &
fi

