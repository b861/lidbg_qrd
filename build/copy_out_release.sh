
source ./env_entry.sh

release_path=$DBG_OUT_RELEASE_PATH/$BOARD_VERSION/out

function copy_release()
{
IFS="
"
releasefile=$1/release
while read -r line
do
    cp $1/$line $release_path
done < $releasefile
}

cat  $DBG_SOC_PATH/$DBG_SOC/release > $DBG_OUT_PATH/release
cat  $DBG_DRV_PATH/release >> $DBG_OUT_PATH/release
cat  $DBG_WORK_PATH/release_$DBG_SOC >> $DBG_OUT_PATH/release
cat  $DBG_ANDROID_PATH/$DBG_SOC/release >> $DBG_OUT_PATH/release

cat $DBG_OUT_PATH/release > $release_path/release

copy_release $DBG_OUT_PATH



