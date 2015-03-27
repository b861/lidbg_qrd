
#cd $BP_SOURCE_PATH
#build the modem should source ./setenv-modem.sh
#other is source ./setenv.sh

function build_mpss()
{
    echo $FUNCNAME
    cd $MPSS_BUILD_PATH && $MPSS_BUILD_CMD
}

function build_bootloader()
{
    echo $FUNCNAME
    cd $BOOT_IMAGE_PATH && $BOOTLOADER_BUILD_CMD
}

function build_adsp()
{
    echo $FUNCNAME
    cd $ADSP_PATH && $ADSP_BUILD_CMD
}

function build_debug_image()
{
     echo $FUNCNAME
     cd $DEBUG_IMAGE_PATH && $DEBUG_IMAGE_CMD
}
function build_rpm()
{
    echo $FUNCNAME
    cd $BP_SOURCE_PATH && $RPM_BUILD_CMD
}

function build_wcnss()
{
    echo $FUNCNAME
    cd $WCNSS_PATH && $WCNSS_BUILD_CMD
}

function build_trustzone_image()
{
    echo $FUNCNAME
    cd $TZ_PATH && $TZ_BUILD_CMD
}

function copy_android_image()
{

#   ln 
   echo $FUNCNAME
   mkdir -p $BP_SOURCE_PATH/LINUX/android/out/target/product/$DBG_PLATFORM
   cd $BP_SOURCE_PATH/LINUX/android/out/target/product/$DBG_PLATFORM
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/userdata.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/persist.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/cache.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/bootloader ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/clean_steps.mk ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/dt.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/emmc_appsboot.mbn ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/emmc_appsboot.raw ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/EMMCBOOT.MBN ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/filesmap ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/kernel ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/previous_build_config.mk ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/ramdisk.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/ramdisk-recovery.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/recovery.img ./
#if exist
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/flysystem.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/flyrecovery.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/flyapdata.img ./
   cp -vu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/flash.img ./
   mkdir ./obj
   cp -avu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/KERNEL_OBJ ./obj/
   cp -avu $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/EMMC_BOOTLOADER_OBJ ./obj/

#  cp qcn
}


function build_update()
{
    echo $FUNCNAME
    cd $UPDATE_INFO && $UPDATE_BUILD_CMD
}


function factory_bin()
{
	echo $FUNCNAME
}

function flash_bin()
{
	echo $FUNCNAME
}


function clean()
{
	echo $FUNCNAME
}



