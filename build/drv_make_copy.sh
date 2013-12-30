#=======================================================================================
#	FileName    : 
#	Description : Make Drv
#       Date:         2010/04/27
#=======================================================================================

source ./env_entry.sh

dir=$DBG_SOC_PATH

clear

echo -e "\033[44;37m ----build soc ko---- \033[0m"
cd  $dir/$DBG_SOC ; pwd          # ";"  do when ok or not   "&&" do when first no err
make modules -j16 > build_log

if [ $? = 0 ]; then
	echo "Make SocKo Successful!"
else
	echo -e "\033[41;37m +++++++++ soc error codes +++++++++ \033[0m"
	make modules 2 &> err_log # 错误信息ID 2
	grep -C 1 "error:" err_log
	echo -e "\033[41;37m --------- soc error codes --------- \033[0m"
	read get_key
fi

dir=$DBG_DRV_PATH
echo -e "\033[44;37m ----build core ko---- \033[0m"
cd  $dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
make modules -j16 > build_log

if [ "$?" = "0" ]; then
	echo -e "\033[42;37m Make  Successful\033[0m"
	cd  $DBG_SHELL_PATH ; pwd
else
	echo -e "\033[41;37m +++++++++ drv error codes +++++++++ \033[0m"
	make modules 2 &> err_log # 错误信息ID 2
	grep -C 1 "error:" err_log
	echo -e "\033[41;37m --------- drv error codes --------- \033[0m"
	read get_key
fi

sh  $DBG_SHELL_PATH/copy_rel.sh 


