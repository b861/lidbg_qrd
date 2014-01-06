source ../../../dbg_cfg.sh
echo $DBG_PLATFORM
if [ $DBG_PLATFORM = msm7627a ];then
rm ../out/vold
rm ../out/vdc
source ../build_cmn.sh
fi

