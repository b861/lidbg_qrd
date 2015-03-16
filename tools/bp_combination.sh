
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
   echo $FUNCNAME
   mkdir -p $BP_SOURCE_PATH/LINUX/android/out/target/product/$DBG_PLATFORM
   cd $BP_SOURCE_PATH/LINUX/android/out/target/product/$DBG_PLATFORM
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/system.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/userdata.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/persist.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/cache.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/boot.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/bootloader ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/clean_steps.mk ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/dt.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/emmc_appsboot.mbn ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/emmc_appsboot.raw ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/EMMCBOOT.MBN ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/filesmap ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/kernel ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/previous_build_config.mk ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/ramdisk.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/ramdisk-recovery.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/recovery.img ./
   mkdir ./obj
   cp -a $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/KERNEL_OBJ ./obj/
   cp -a $DBG_SYSTEM_DIR/out/target/product/$DBG_PLATFORM/obj/EMMC_BOOTLOADER_OBJ ./obj/
}


function build_update()
{
    echo $FUNCNAME
    cd $UPDATE_INFO && $UPDATE_BUILD_CMD
}







