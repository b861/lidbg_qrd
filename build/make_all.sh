#=======================================================================================
#	FileName    : 
#	Description : Make Drv
#       Date:         2010/04/27
#=======================================================================================

source ./env_entry.sh

function build_dir()
{
dir=$1
echo -e "\033[44;37m ----build $1---- \033[0m"
cd  $dir/$DBG_SOC ; pwd          # ";"  do when ok or not   "&&" do when first no err
make modules -j4 > build_log

if [ $? = 0 ]; then
	echo "Make $1 Successful!"
else
	echo -e "\033[41;37m +++++++++ soc error codes +++++++++ \033[0m"
	make modules 2 &> err_log # 错误信息ID 2
	grep -C 1 "error:" err_log
	echo -e "\033[41;37m --------- soc error codes --------- \033[0m"
	read get_key
fi
}


build_dir $DBG_SOC_PATH
build_dir $DBG_CORE_PATH
build_dir $DBG_DRIVERS_PATH
