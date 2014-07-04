# msm7627a
#Q平台 msm8625
# msm8226
DBG_PLATFORM=msm8625


case "$DBG_PLATFORM" in
    msm7627a)
	DBG_SYSTEM_DIR=/home/flyaudio/R8625SSNSKQLYA10145451;;
    msm8625)
	DBG_SYSTEM_DIR=/home/flyaudio/R8625QSOSKQLYA3060-v2
	RELEASE_REPOSITORY=/home/flyaudio/8x25q-release;;
    msm8226)
	DBG_SYSTEM_DIR=/home/flyaudio/msm8226-1.8
	RELEASE_REPOSITORY=/home/flyaudio/8x26-release;;
    msm8974)
	DBG_SYSTEM_DIR=/home/hongbo/code/msm8974
	RELEASE_REPOSITORY=/home/hongbo/code/msm8974;;
esac

