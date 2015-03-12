
function soc_build_system()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	soc_prebuild && make systemimage -j16 && soc_postbuild
}

function soc_build_kernel()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	soc_prebuild && soc_build_common 'make bootimage -j16'
}


function soc_build_recovery()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	soc_prebuild && soc_build_common 'make recovery -j16'
}

function soc_build_recoveryimage()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	rm $DBG_OUT_PATH/*.apk $DBG_OUT_PATH/ES.ko $DBG_OUT_PATH/ST.ko $DBG_OUT_PATH/mkfs.exfat $DBG_OUT_PATH/GPS.ko
	rm -rf $DBG_SYSTEM_DIR/bootable/recovery/flyaudio/lib/modules/out
	mkdir -p $DBG_SYSTEM_DIR/bootable/recovery/flyaudio/lib/modules
	cp -rf $DBG_OUT_PATH  $DBG_SYSTEM_DIR/bootable/recovery/flyaudio/lib/modules/out
	soc_prebuild && soc_build_common 'make recoveryimage -j16'
}

function soc_build_common()
{
	echo $FUNCNAME $1 $2 $3
	cd $DBG_SYSTEM_DIR
	set_env && $1 $2 $3
}

function soc_build_all()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
	soc_prebuild && make -j16 && soc_postbuild
}


function soc_postbuild()
{
	echo $FUNCNAME
	hostname > $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/etc/build_time.conf
	date >> $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/etc/build_time.conf
	git log --oneline | sed -n '1,5p' >> $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/etc/build_time.conf
	echo "soc_build_all ok"
	#if [ $DBG_PLATFORM = msm8226 ];then	
	#	mmm $DBG_SYSTEM_DIR/system/core/libdiskconfig -B
	#fi
}


function set_env()
{
	echo $FUNCNAME
	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi
}

function soc_prebuild()
{
	echo $FUNCNAME
	echo $DBG_PLATFORM
	cd $DBG_SYSTEM_DIR

if [ $DBG_PLATFORM = msm8226 ];then
	rm -rf $DBG_SYSTEM_DIR/kernel/drivers/flyaudio
	mkdir -p $DBG_SYSTEM_DIR/kernel/drivers/flyaudio
	cp -ru $DBG_DRIVERS_PATH/build_in/*	        $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_DRIVERS_PATH/inc/lidbg_interface.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/cmn_func.c   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/inc/cmn_func.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/
	cp -u $DBG_CORE_PATH/inc/lidbg_def.h   $DBG_SYSTEM_DIR/kernel/drivers/flyaudio/

#	rm -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/modules/out
#	cp -r $RELEASE_REPOSITORY/driver/out $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/modules/
#	cp $RELEASE_REPOSITORY/driver/out/vold $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/
#	cp $RELEASE_REPOSITORY/app/FastBoot.apk $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/
#	cp $RELEASE_REPOSITORY/app/FlyBootService.apk $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/
#	cp $RELEASE_REPOSITORY/driver/out/lidbg_load $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/
fi

	rm -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system
	rm -rf $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/root
	rm -rf $DBG_SYSTEM_DIR/out/target/product/msm8226/obj/EXECUTABLES/vold_intermediates

	set_env
}


function soc_build_release()
{
	echo $FUNCNAME
	cd $RELEASE_REPOSITORY
	expect $DBG_TOOLS_PATH/pull $REPOSITORY_WORK_BRANCH $DBG_REPO_PASSWORD
	cd $DBG_SYSTEM_DIR
	expect $DBG_TOOLS_PATH/pull $SYSTEM_WORK_BRANCH $DBG_PASSWORD

	soc_build_all
	soc_make_otapackage
}

function soc_make_otapackage()
{
	echo $FUNCNAME
# cp lk,bp to /device/qcom/msm8226/radio
	#cp -u $RELEASE_REPOSITORY/lk/emmc_appsboot.mbn  $DBG_SYSTEM_DIR/device/qcom/msm8226/radio/
	#cp -u $RELEASE_REPOSITORY/radio/* 	        $DBG_SYSTEM_DIR/device/qcom/msm8226/radio/
	cd $DBG_SYSTEM_DIR

	if [[ $TARGET_PRODUCT = "" ]];then
		source build/envsetup.sh&&choosecombo release $DBG_PLATFORM $SYSTEM_BUILD_TYPE
	fi

	make otapackage -j16
}

function soc_build_origin_image
{
	echo $FUNCNAME
	lidbg_build_all
	soc_build_all

	cp $DBG_SOC_PATH/$DBG_SOC/init.lidbg.rc        $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/root/init.lidbg.rc
	cp $DBG_OUT_PATH/lidbg_load		       $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/lidbg_load
	cp $DBG_OUT_PATH/vold		       	       $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/bin/vold
	cp -rf $DBG_OUT_PATH                           $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/lib/modules/out
	cp -rf $DBG_SYSTEM_DIR/origin-app/priv-app/*   $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/priv-app/
	cp -rf $DBG_SYSTEM_DIR/origin-app/app/*        $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/
	#copy fastboot apk
	cd $RELEASE_REPOSITORY
	git checkout $REPOSITORY_WORK_BRANCH
	cp $DBG_OUT_PATH/FastBoot.apk        $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/FastBoot.apk
	cp $DBG_OUT_PATH/FlyBootService.apk  $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/FlyBootService.apk

	echo "build_origin" > $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/etc/build_origin

	cd $DBG_SYSTEM_DIR
	make otapackage -j16

}


. $DBG_TOOLS_PATH/soc_common.sh
