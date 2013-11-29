source ../../../dbg_cfg.sh
echo `pwd`
case "$DBG_PLATFORM" in
    msm7627a)
	cd 8x25 && ./build.sh;;
    msm8625)
	cd 8x25q && ./build.sh;;
    *) 
	echo "exit,not find your platform:  $DBG_PLATFORM"
	exit;;
esac



