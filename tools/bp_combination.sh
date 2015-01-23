
#cd $BP_SOURCE_PATH
#build the modem should source ./setenv-modem.sh
#other is source ./setenv.sh

function build_mpss()
{
    echo $FUNCNAME

    cd $BP_ENV_PATH && source ./setenv-modem.sh
    cd $MPSS_BUILD_PATH && $MPSS_BUILD_CMD
}

function build_bootloader()
{
    echo $FUNCNAME
    cd $BP_ENV_PATH && source ./setenv.sh
    cd $BOOT_IMAGE_PATH && $BOOTLOADER_BUILD_CMD
}

function build_adsp()
{
    echo $FUNCNAME
    cd $BP_ENV_PATH && source ./setenv.sh
    cd $ADSP_PATH && $ADSP_BUILD_CMD
}

function build_rpm()
{
    echo $FUNCNAME
    cd $BP_ENV_PATH && source ./setenv.sh
    cd $BP_SOURCE_PATH && $RPM_BUILD_CMD
}

function build_wcnss()
{
    echo $FUNCNAME
    cd $BP_ENV_PATH && source ./setenv.sh
    cd $WCNSS_PATH && $WCNSS_BUILD_CMD
}

function build_trustzone_image()
{
    echo $FUNCNAME
    cd $BP_ENV_PATH && source ./setenv.sh
    cd $TZ_PATH && $TZ_BUILD_CMD
}

function copy_android_image()
{
   mkdir $BP_SOURCE_PATH/LINUX/android/out && cd $BP_SOURCE_PATH/LINUX/android/out
   cp $DBG_SYSTEM_DIR/out/target/product/msm8974/system.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/msm8974/userdata.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/msm8974/persist.img ./
   cp $DBG_SYSTEM_DIR/out/target/product/msm8974/cache.img ./
   mkdir $BP_SOURCE_PATH/LINUX/android/out/obj && cd $BP_SOURCE_PATH/LINUX/android/out/obj
   cp -a $DBG_SYSTEM_DIR/out/target/product/msm8974/obj/KERNEL_OBJ ./
   cp -a $DBG_SYSTEM_DIR/out/target/product/msm8974/obj/EMMC_BOOTLOADER_OBJ ./
}


function build_update()
{
    echo $FUNCNAME
    cd $BP_ENV_PATH && source ./setenv.sh
    cd $UPDATE_INFO && $UPDATE_BUILD_CMD
}







