
DIR_LIDBG_PATH=`cd ../ && pwd`
DIR_BUILD_PATH=$DIR_LIDBG_PATH/build
DIR_TOOLS_PATH=$DIR_LIDBG_PATH/tools

main_loop_times=0
pre_commit=('' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '' '')
commit_times=(0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 )

#$SYSTEM_DIR $SYSTEM_DIR_PASSWORD
function git_pull()
{
	cd $1
	expect $DIR_TOOLS_PATH/pull $2 
}

while true ;do

echo "============loops:$main_loop_times  status:["${commit_times[0]}"] ["${commit_times[1]}"] ["${commit_times[2]}"] ["${commit_times[3]}"] ["${commit_times[4]}"]  ==============="
	thiswhile_loop=0
	while read line
	do
	compel_info=$line
	compel_info=($compel_info)
	#echo ==${compel_info[0]} ${compel_info[1]} ${compel_info[2]} ${compel_info[3]} ${compel_info[4]} ${compel_info[5]} ${compel_info[6]} ${compel_info[7]} ${compel_info[8]}
	
	cd ${compel_info[3]}
	git log --oneline | sed -n '1,5p' > /dev/shm/${compel_info[1]}.txt && new_commitinfo=$(head /dev/shm/${compel_info[1]}.txt --lines 1)
	
	thiscommit_info=${pre_commit[$thiswhile_loop]}
	thiscommit_times=${commit_times[$thiswhile_loop]}
	echo "==$thiswhile_loop==["${compel_info[1]}"]==== "
	
	if [[ $thiscommit_info == "" ]]; then
		echo "pre_commit[$thiswhile_loop] is NULL"
	else
		if [[ "$thiscommit_info" != "$new_commitinfo" ]]; then
		let thiscommit_times++
		echo $(date) $thiscommit_times/$main_loop_times ${compel_info[1]} ["$thiscommit_info"] ["$new_commitinfo"]  >> /dev/shm/lidbg_git_history.txt
		echo "detect a new commit: "$thiscommit_times ["$thiscommit_info"] ["$new_commitinfo"]
		sleep 1
		cd $DIR_TOOLS_PATH
		./basesystem.sh ${compel_info[0]} ${compel_info[1]} ${compel_info[2]} ${compel_info[3]} ${compel_info[4]} ${compel_info[5]} ${compel_info[6]} ${compel_info[7]} ${compel_info[8]} "${new_commitinfo#* }"
		fi
	fi

	pre_commit[$thiswhile_loop]=$new_commitinfo
	commit_times[$thiswhile_loop]=$thiscommit_times
	
	git_pull ${compel_info[3]} ${compel_info[4]}
	let thiswhile_loop++
	done <$DIR_TOOLS_PATH/git_update_auto_detect.conf
let main_loop_times++
sleep 1
done
