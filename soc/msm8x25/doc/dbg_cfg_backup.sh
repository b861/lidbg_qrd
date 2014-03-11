#---
DBG_SOC=msm8x25
#DBG_SOC=msm8x26

#---
#DBG_PLATFORM=msm7627a
DBG_PLATFORM=msm8625
#DBG_PLATFORM=msm8226


case "$DBG_PLATFORM" in
    msm7627a)
	BOARD_VERSION=V2
	DBG_SYSTEM_DIR=/home/swlee/qrd_451/R8625SSNSKQLYA10145451
	DBG_CROSS_COMPILE=$DBG_SYSTEM_DIR/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
	SYSTEM_BUILD_TYPE=eng;;

    msm8625)
	BOARD_VERSION=V4
	DBG_SYSTEM_DIR=/home/swlee/flyaudio/R8625QSOSKQLYA3060-v2
	DBG_CROSS_COMPILE=$DBG_SYSTEM_DIR/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
	SYSTEM_BUILD_TYPE=eng
	UPDATA_BIN_DIR=/8x25q-release/driver
	UPDATA_BASESYSTEM_DIR=/8x25q-release/basesystem;;

    msm8226)
	BOARD_VERSION=V1
	DBG_SYSTEM_DIR=/home/swlee/flyaudio/msm8226-1.8
	DBG_CROSS_COMPILE=$DBG_SYSTEM_DIR/prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/arm-eabi-
	SYSTEM_BUILD_TYPE=eng;;

    *) 
	echo "exit,not find your platform:  $DBG_PLATFORM"
	exit;;
esac

DBG_KERNEL_SRC_DIR=$DBG_SYSTEM_DIR/kernel
DBG_KERNEL_OBJ_DIR=$DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/KERNEL_OBJ


export DBG_SOC
export DBG_PLATFORM
export DBG_KERNEL_SRC_DIR
export DBG_KERNEL_OBJ_DIR
export DBG_SYSTEM_DIR
export DBG_CROSS_COMPILE
export BOARD_VERSION
export BUILD_VERSION
export UPDATA_BIN_DIR

