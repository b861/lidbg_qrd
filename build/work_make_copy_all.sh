#=======================================================================================
#	FileName    : 
#	Description : 
#       Date:         2010/04/27
#=======================================================================================

source ./env_entry.sh

clear
dbg_soc=$DBG_WORK_PATH/$DBG_SOC/
cd $dbg_soc

for each_dir in `ls -l | grep "^d" | awk '{print $NF}'`

	do
	
	dir=$dbg_soc/$each_dir

if [ -s $dir/Makefile ]; then
	echo -e "\n\033[44;37m ----build    $each_dir    ko---- \033[0m"
	cd  $dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
	make modules -j4 > build_log

	if [ "$?" = "0" ]; then
		echo -e "\033[42;37m Make  Successful\033[0m"

	else
		echo -e "\033[41;37m +++++++++ $each_dir error codes +++++++++ \033[0m"
		make modules 2 &> err_log # 错误信息ID 2
		grep -C 1 "error:" err_log
		echo -e "\033[41;37m --------- $each_dir error codes --------- \033[0m"
		read get_key
	fi
	#then copy to out dir 
	cp -u $dir/*.ko     $DBG_OUT_PATH/
	cp -u $dir/*.conf     $DBG_OUT_PATH/
	clear
else
	echo -e "\033[43;37m ----build    $each_dir    ko---- \033[0m"
	echo -e "skip:invalid Makefile"
fi
done

