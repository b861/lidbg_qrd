
source ./env_entry.sh
echo input :"$1" "$2" "$3"
#version choose
	case "$1" in
	v1) 	
	echo "EXTRA_CFLAGS += -DBOARD_V1" > $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DBOARD_V1" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "v1";;

	v2) 
	echo "EXTRA_CFLAGS += -DBOARD_V2" > $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DBOARD_V2" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "v2";;

	v3) 
	echo "EXTRA_CFLAGS += -DBOARD_V3" > $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DBOARD_V3" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "v3";;

	v4) 
	echo "EXTRA_CFLAGS += -DBOARD_V4" > $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DBOARD_V4" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "v4";;
	*)

	echo -e "\033[41;37m  ===============support table====================\033[0m "  
	echo "v1"
	echo "v2"
	echo "v3"
	echo "v4"
	echo "dbg"
	echo "rel"
	echo "  "
	echo -e "exit,check your input: \033[41;37m  $1 $2  \033[0m"
	exit;;
	esac 

# dbg choose
	case "$2" in
	dbg) 
	echo "EXTRA_CFLAGS += -DFLY_DEBUG" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DFLY_DEBUG" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "dbg";;

	rel) 
	echo "EXTRA_CFLAGS += -DFLY_RELEASE" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DFLY_RELEASE" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "rel";;
	*)

	echo -e "\033[41;37m  ===============support table====================\033[0m "  
	echo "v1"
	echo "v2"
	echo "v3"
	echo "v4"
	echo "dbg"
	echo "rel"
	echo "  "
	echo -e "exit,check your input: \033[41;37m  $1 $2  \033[0m"
	exit;;
	esac



	case "$3" in
	msm8x25) 
	echo "EXTRA_CFLAGS += -DSOC_MSM8x25" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DSOC_MSM8x25" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "msm8x25";;

	mst786)
	echo "EXTRA_CFLAGS += -DSOC_MST786" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DSOC_MST786" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "mst786";;

	msm8x60)
	echo "EXTRA_CFLAGS += -DSOC_MSM8x60" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DSOC_MSM8x60" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "msm8x60";;
	
   msm8x26)
	echo "EXTRA_CFLAGS += -DSOC_MSM8x26" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "LOCAL_CFLAGS += -DSOC_MSM8x26" >> $DBG_ROOT_PATH/build_cfg.mk
	echo "msm8x26";;
	*)
	echo -e "\033[41;37m  ===============support table====================\033[0m "  
	echo "v1"
	echo "v2"
	echo "v3"
	echo "v4"
	echo "dbg"
	echo "rel"
	echo "  "
	echo -e "exit,check your input: \033[41;37m  $1 $2  \033[0m"
	exit;;
	esac

