
DBG_SOC=msm8x25
DBG_PLATFORM=flycar

DBG_ROOT_PATH=/home/work/flyaudio/lidbg_qrd
DBG_SYSTEM_DIR=/home/work/flyaudio/qrd_450/R8625SSNSKQLYA10045450
DBG_KERNEL_DIR=/home/work/flyaudio/qrd_450/lidbg_kernel

DBG_CROSS_COMPILE=$DBG_SYSTEM_DIR/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-

export DBG_ROOT_PATH
export DBG_SOC
export DBG_PLATFORM
export DBG_KERNEL_DIR
export DBG_SYSTEM_DIR
export DBG_CROSS_COMPILE
