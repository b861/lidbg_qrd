
DBG_SOC=msm8x60
DBG_PLATFORM=su640
BUILD_VERSION=rel


case "$DBG_PLATFORM" in
    su640)
	BOARD_VERSION=v1
	DBG_SYSTEM_DIR=/home/android/su640/system;;
    *) 
	echo "exit,not find your platform:  $DBG_PLATFORM"
	exit;;
esac


DBG_KERNEL_SRC_DIR=$DBG_SYSTEM_DIR/kernel/lge/iproj
DBG_KERNEL_OBJ_DIR=$DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/KERNEL_OBJ
DBG_CROSS_COMPILE=$DBG_SYSTEM_DIR/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-

echo $DBG_SYSTEM_DIR
echo $BOARD_VERSION

export DBG_SOC
export DBG_PLATFORM
export DBG_KERNEL_SRC_DIR
export DBG_KERNEL_OBJ_DIR
export DBG_SYSTEM_DIR
export DBG_CROSS_COMPILE
export BOARD_VERSION
export BUILD_VERSION
