
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
	cp -u $DBG_TOOLS_PATH/pull  $UPDATA_BIN_DIR/pull
	cp -u $DBG_TOOLS_PATH/push  $UPDATA_BIN_DIR/push
	
	cd $UPDATA_BIN_DIR
	pwd
	expect $DBG_TOOLS_PATH/pull
	expect $DBG_TOOLS_PATH/pull
	cp -ru $DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out  $UPDATA_BIN_DIR
	git add .
	git add -f ./
	git commit -am $1
	expect $DBG_TOOLS_PATH/push
	gitk &
fi

