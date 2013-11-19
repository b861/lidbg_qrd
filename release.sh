
chmod 777 -R ./*
cd build
source ./env_entry.sh

./clean.sh
cd $DBG_ANDROID_PATH/$DBG_SOC
./build_all.sh

cd -
./build.sh

