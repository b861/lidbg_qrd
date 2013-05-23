# for example
#./make.sh mywork/msm8x25/videoin/
cd shell && source env_entry.sh
echo start build $DBG_ROOT_PATH/$1
cd $DBG_ROOT_PATH/$1 && make modules
mv $DBG_ROOT_PATH/$1/*.ko $DBG_OUT_PATH/
