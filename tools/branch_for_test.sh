
function judge_which_combine()
{
    echo $FUNCNAME
if [[ $which_combine -eq "1" ]];then
        
    echo "打包basesystem zip 包"
    cd $RELEASE_REPOSITORY
    soc_build_all && soc_make_otapackage && depository_copy_basesystem
#for test
#    echo "baseqcom.flb" > basesystem.txt
elif [[ $which_combine -eq "2" ]];then

    echo "打包 libdg zip 包"
    lidbg_build_all && depository_copy_lidbg && depository_copy_lidbg
#for test
#    cd $RELEASE_REPOSITORY
#    echo "libdg.zip" > libdg.txt

elif [[ $which_combine -eq "3" ]];then
 
    echo "打包所有"
    soc_build_all && soc_make_otapackage && depository_copy_basesystem
    lidbg_build_all && depository_copy_lidbg && depository_copy_lidbg
#for test   
#    cd $RELEASE_REPOSITORY
#    echo "all.zip" > all.txt
else
    echo "输入错误"
    exit 
fi
}



function new_remote_test_branch()
{
    echo $FUNCNAME
    cd $RELEASE_REPOSITORY
    git checkout -b $branch_name
    git add .
    git add -f $UPDATA_BIN_DIR
    git commit -am "$commit"
    expect $DBG_TOOLS_PATH/push $branch_name $DBG_REPO_PASSWORD
}

function new_branch_remove()
{
    echo $FUNCNAME
    cd $RELEASE_REPOSITORY
    git checkout master
    git branch -D $branch_name
}


