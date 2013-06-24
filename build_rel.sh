#=======================================================================================
#	FileName    : 
#	Description : Make ALL
#       Date:         2012/02/03
#=======================================================================================

cd shell
source ./env_entry.sh

case "$DBG_PLATFORM" in
    msm7627a)
	./build.sh v2 rel;;
    msm8625)
	./build.sh v3 rel;;
    *) 
	echo "exit,not find your platform:  $DBG_PLATFORM"
	exit;;
esac

./copy_out_release.sh

