
DBG_SOC=msm8x25
#DBG_PLATFORM=msm7627a
DBG_PLATFORM=msm8625
BUILD_VERSION=rel


case "$DBG_PLATFORM" in
    msm7627a)
	BOARD_VERSION=v2
	DBG_SYSTEM_DIR=/home/swlee/qrd_451/R8625SSNSKQLYA10145451
	UPDATA_BIN_DIR=/pack-for-8x25/driver;;
    msm8625)
	BOARD_VERSION=v4
	DBG_SYSTEM_DIR=/home/swlee/flyaudio/R8625QSOSKQLYA3060-v2
	UPDATA_BIN_DIR=/8x25q-release/driver;;
    *) 
	echo "exit,not find your platform:  $DBG_PLATFORM"
	exit;;
esac

DBG_KERNEL_SRC_DIR=$DBG_SYSTEM_DIR/kernel
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
export UPDATA_BIN_DIR

