

source ./env_entry.sh

function build_dir()
{
	dir=$1
	echo -e "\033[44;37m build $1 \033[0m"
	cd  $dir
	make modules -j8

	if [ $? = 0 ]; then
		echo
	else
		echo -e "\033[41;37m +++++++++ soc error codes +++++++++ \033[0m"
		make modules 2 &> err_log
		grep -C 1 "error:" err_log
		echo -e "\033[41;37m --------- soc error codes --------- \033[0m"
		read get_key
	fi
}

clear
build_dir $DBG_SOC_PATH/$DBG_SOC
build_dir $DBG_CORE_PATH
build_dir $DBG_DRIVERS_PATH
