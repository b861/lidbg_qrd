
DBG_SOC=msm8x25
#DBG_PLATFORM=msm7627a
DBG_PLATFORM=msm8625

case "$DBG_PLATFORM" in
    msm7627a)
	DBG_SYSTEM_DIR=/home/swlee/qrd_451/R8625SSNSKQLYA10145451

    msm8625)
	DBG_SYSTEM_DIR=/home/swlee/R8625QSOSKQLYA3060
    *) 
	echo "exit,not find your platform:  $DBG_PLATFORM"
	exit;;
esac

DBG_KERNEL_DIR=$DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/KERNEL_OBJ;;
DBG_CROSS_COMPILE=$DBG_SYSTEM_DIR/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-

echo $DBG_SYSTEM_DIR

export DBG_SOC
export DBG_PLATFORM
export DBG_KERNEL_DIR
export DBG_SYSTEM_DIR
export DBG_CROSS_COMPILE
