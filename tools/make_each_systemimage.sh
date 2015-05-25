cd ../build
source ./env_entry.sh

. $DBG_TOOLS_PATH/soc_$DBG_SOC.sh
. $DBG_TOOLS_PATH/depository.sh
. $DBG_TOOLS_PATH/debug.sh
. $DBG_TOOLS_PATH/combination.sh
. $DBG_TOOLS_PATH/common.sh
. $DBG_TOOLS_PATH/branch_for_test.sh

cd $DBG_TOOLS_PATH
DIR_LIDBG_PATH=`cd ../ && pwd`
DIR_BUILD_PATH=$DIR_LIDBG_PATH/build
DIR_TOOLS_PATH=$DIR_LIDBG_PATH/tools

main_loop_times=0

mkdir -p /home/systemimage/
cd $DBG_SYSTEM_DIR
git log --oneline  > /home/systemimage/gitoneline.txt


echo "=======$DBG_SOC $DBG_PLATFORM $DBG_PLATFORM_ID======="

while read myline
do
 echo "start:=======$main_loop_times=======:"$myline
 mkdir -p /home/systemimage/$DBG_PLATFORM"_"$DBG_PLATFORM_ID/"$myline"
 
 #STEP1:
 cd $DBG_SYSTEM_DIR
 rm /out/target/product/$DBG_PLATFORM/boot.img
 rm /out/target/product/$DBG_PLATFORM/system.img
 rm /out/target/product/$DBG_PLATFORM/emmc_appsboot.img
 commit=$myline
 git reset ${commit:0:5} --hard
 
 #STEP2:
 cd $DIR_LIDBG_PATH
 ./lidbg.sh 26 -1

  
 #STEP3:
  cp -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img /home/systemimage/$DBG_PLATFORM"_"$DBG_PLATFORM_ID/"$myline"
  cp -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img /home/systemimage/$DBG_PLATFORM"_"$DBG_PLATFORM_ID/"$myline"
  cp -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/emmc_appsboot.mbn /home/systemimage/$DBG_PLATFORM"_"$DBG_PLATFORM_ID/"$myline"

  echo "stop:=======$main_loop_times=======:"$myline
  
sleep 5
 let main_loop_times++
 
done < /home/systemimage/gitoneline.txt




