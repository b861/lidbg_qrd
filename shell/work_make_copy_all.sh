#=======================================================================================
#	FileName    : 
#	Description : Make Drv
#       Date:         2010/04/27
#=======================================================================================

source ./env_entry.sh

clear
dbg_soc=$DBG_WORK_PATH/$DBG_SOC/
cd $dbg_soc

for each_dir in `ls -l | grep "^d" | awk '{print $9}'`

	do
	
	dir=$dbg_soc/$each_dir


	#if [ "$DBG_SYN_XP2LI" = "y" ]; then
	#echo "######### execute Syn_file_XP2Li.sh ##########"
	#cd  $DBG_SHELL_PATH ; pwd
	#sh ./Syn_file_XP2Li.sh
	#fi

	echo -e "\n\033[44;37m ----build    $each_dir    ko---- \033[0m"
	cd  $dir ; pwd          # ";"  do when ok or not   "&&" do when first no err
	make modules -j4 > build_log

	if [ "$?" = "0" ]; then
		echo -e "\033[42;37m Make  Successful\033[0m"
		clear
		#       echo "run copy_rel.sh"
		#	cd  $DBG_SHELL_PATH ; pwd
		#	sh ./copy_rel.sh > copy_log
	else
		echo -e "\033[41;37m +++++++++ $each_dir error codes +++++++++ \033[0m"
		make modules 2 &> err_log # 错误信息ID 2
		grep -C 1 "error:" err_log
		echo -e "\033[41;37m --------- $each_dir error codes --------- \033[0m"
		read get_key
	fi
	
	#then copy to out dir 
	cp -u $dir/*.ko     $DBG_OUT_PATH/
	
	#if [ "$DBG_SYN_LI2XP" = "y" ]; then
	#echo "######### execute Syn_file_Li2XP.sh ##########"
	#cd  $DBG_SHELL_PATH ; pwd
	#sh ./Syn_file_li2XP.sh
	#fi








done #end do 

