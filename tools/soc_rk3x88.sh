
function soc_build_system()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR
         make systemimage -j16 && soc_postbuild
}

function soc_build_kernel()
{
	echo $FUNCNAME
	cd $DBG_SYSTEM_DIR/kernel
	#make $KERNEL_DEFCONFIG && make kernel.img -j16
	make kernel.img -j16
        cp $DBG_SYSTEM_DIR/kernel/arch/arm/boot/Image $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/kernel
        cd $DBG_SYSTEM_DIR
        ./mkimage.sh ota
        cp $DBG_SYSTEM_DIR/rockdev/Image-rkpx3/boot.img $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/
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

function soc_build_bootloader()
{
        echo $FUNCNAME
        cd $DBG_SYSTEM_DIR/uboot
        make rk30xx -j16
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
        soc_build_bootloader
	soc_build_kernel
	set_env && make -j16 && soc_postbuild
}


function soc_postbuild()
{
	echo $FUNCNAME
	./mkimage.sh ota
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
	cd $DBG_SYSTEM_DIR
        source build/envsetup.sh && lunch $DBG_PLATFORM-$SYSTEM_BUILD_TYPE
}

function soc_prebuild()
{
	echo $FUNCNAME
	echo $DBG_PLATFORM
	cd $DBG_SYSTEM_DIR
	
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

function soc_build_origin_image()
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
	#cd $RELEASE_REPOSITORY
	#git checkout $REPOSITORY_WORK_BRANCH
	#cp $DBG_OUT_PATH/FastBoot.apk        $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/FastBoot.apk
	cp $DBG_OUT_PATH/FlyBootService.apk  $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/app/FlyBootService.apk

	echo "build_origin" > $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system/etc/build_origin

	cd $DBG_SYSTEM_DIR
	make otapackage -j16

}


. $DBG_TOOLS_PATH/soc_common.sh
