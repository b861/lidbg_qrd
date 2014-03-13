# msm8x25
# msm8x26
# msm8x60
DBG_SOC=msm8x25

# msm7627a
# msm8625
# msm8226
DBG_PLATFORM=msm8625


case "$DBG_PLATFORM" in
    msm7627a)
	DBG_SYSTEM_DIR=/home/flyaudio/R8625SSNSKQLYA10145451;;
    msm8625)
	DBG_SYSTEM_DIR=/home/flyaudio/R8625QSOSKQLYA3060-v2
	UPDATA_BIN_DIR=/home/flyaudio/8x25q-release/driver
	UPDATA_BASESYSTEM_DIR=/home/flyaudio/8x25q-release/basesystem;;
    msm8226)
	DBG_SYSTEM_DIR=/home/flyaudio/msm8226-1.8;;
esac

