
	apt-get install expect
	cd $UPDATA_BASESYSTEM_DIR
	pwd
	git checkout master
	expect $DBG_TOOLS_PATH/pull
	expect $DBG_TOOLS_PATH/pull
	cp -ru $DBG_SYSTEM_DIR/flyaudio/out/*  $UPDATA_BASESYSTEM_DIR
	git add .
	git commit -am $1
	expect $DBG_TOOLS_PATH/push
	gitk &

